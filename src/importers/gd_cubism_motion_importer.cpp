#include <importers/gd_cubism_motion_importer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)

void GDCubismMotionImporter::_bind_methods() {
    ClassDB::bind_static_method("GDCubismMotionImporter", D_METHOD("parse_motion", "motion_file"), &GDCubismMotionImporter::parse_motion);
}

Ref<Animation> GDCubismMotionImporter::parse_motion(const String &p_source_file) {
    Dictionary motion = JSON::parse_string(FileAccess::get_file_as_string(p_source_file));
    Dictionary meta = motion["Meta"];

    Ref<Animation> anim = memnew(Animation);

    bool loop = meta.get("Loop", false);
    anim->set_loop_mode(
        loop
        ? Animation::LOOP_LINEAR
        : Animation::LOOP_NONE
    );
    double fps = meta.get("FPS", 60.0);
    anim->set_step(1.0 / fps);

    // parse motion curves
    uint32_t curveCount = meta.get("CurveCount", 0);
    Array curves = motion["Curves"];
    for (uint32_t c_idx = 0; c_idx < curveCount; c_idx++) {
        Dictionary curve = curves[c_idx];
    
        // TODO only support parameter type curves for now
        if (curve["Target"] != "Parameter") {
            continue;
        }

        String property = curve["Id"];
        Array segments = curve["Segments"];

        ERR_FAIL_COND_V_MSG(segments.size() < 2, nullptr, "Invalid Segment, must have at least one point");

        int32_t track = anim->add_track(Animation::TYPE_BEZIER);
        anim->track_set_path(track, NodePath(".:" + property));

        // first key is always the starting time and value
        anim->bezier_track_insert_key(track, segments[0], segments[1]);

        uint32_t s_idx = 2;

        while (s_idx < segments.size()) {
    
            int32_t type = segments[s_idx];
            ERR_FAIL_COND_V_MSG(type < 0 || type > 3, nullptr, "Invalid Motion Segment Type");
            
            if (type == 0) {
                // LINEAR
                real_t p_t = segments[s_idx+1];
                real_t p_v = segments[s_idx+2];

                anim->bezier_track_insert_key(track, p_t, p_v);

                s_idx += 3;
            }
            else if (type == 1) {
                // CUBIC BEZIER
                real_t p0_t = segments[s_idx - 2];
                real_t p0_v = segments[s_idx - 1];
                real_t p1_t = segments[s_idx + 1];
                real_t p1_v = segments[s_idx + 2];
                real_t p2_t = segments[s_idx + 3];
                real_t p2_v = segments[s_idx + 4];
                real_t p3_t = segments[s_idx + 5];
                real_t p3_v = segments[s_idx + 6];

                anim->bezier_track_insert_key(
                    track, p3_t, p3_v,
                    Vector2(p1_t - p0_t, p1_v - p0_v),
                    Vector2(p3_t - p2_t, p3_v - p2_v)
                );

                s_idx += 7;
            }
            else if (type == 2) {
                // Stepped
                s_idx += 3;
            }
            else if (type == 3) {
                // Inverse Stepped
                s_idx += 3;
            }
        }
    }
    
    double duration = meta.get("Duration", 1.0);
    anim->set_length(duration);

    return anim;
}

Error GDCubismMotionImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V(!p_source_file.ends_with(".motion3.json"), Error::FAILED);

    Ref<Animation> anim = this->parse_motion(p_source_file);
    ERR_FAIL_COND_V(anim == nullptr, Error::FAILED);
    
    String filename = p_save_path + String(".") + this->_get_save_extension();
    return ResourceSaver::get_singleton()->save(anim, filename);
}

// ------------------------------------------------------------------ method(s)
