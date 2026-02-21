// SPDX-License-Identifier: MIT
#include <loaders/gd_cubism_motion_loader.hpp>

#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

Array walk_files(String dir, String extension) {
	Array files;

	// pick files
	{
		Array dir_files = DirAccess::get_files_at(dir);
		for (int i = 0; i < dir_files.size(); i++) {
			String f = dir_files[i];
			if (f.ends_with(extension)) {
				files.append(dir.path_join(f));
			}
		}
	}

	// walk subdirectories
	{
		Array sub_dirs = DirAccess::get_directories_at(dir);
		for (int i = 0; i < sub_dirs.size(); i++) {
			String f = sub_dirs[i];
			files.append_array(walk_files(dir.path_join(f), extension));
		}
	}

	return files;
}

Ref<AnimationLibrary> GDCubismMotionLoader::load_motion_library(GDCubismUserModel *model) {
    String assets = model->get_scene_file_path();

    Ref<AnimationLibrary> animations;
    animations.instantiate();
        
    // create reset track for deterministic playback
    {
        Ref<Animation> reset_anim;
        reset_anim.instantiate();

        // reset parameters
        {
            Dictionary params = model->get_parameters();
            Array keys = params.keys();
            for (int i = 0; i < params.size(); i++) {
                String p_name = keys[i];
                int track_idx = reset_anim->add_track(Animation::TYPE_BEZIER);
                Dictionary p = params[p_name];
                reset_anim->track_set_path(track_idx, NodePath(".:" + p_name));
                reset_anim->bezier_track_insert_key(track_idx, 0, (float)p["default"]);
            }
        }

        // reset part opacities
        {
            Dictionary parts = model->get_parts();
            Array keys = parts.keys();
            for (int i = 0; i < parts.size(); i++) {
                String p_name = keys[i];
                int track_idx = reset_anim->add_track(Animation::TYPE_BEZIER);
                Dictionary p = parts[p_name];
                reset_anim->track_set_path(track_idx, NodePath(".:" + p_name));
                reset_anim->bezier_track_insert_key(track_idx, 0, (float)p["default"]);
            }
        }
        reset_anim->set_length(0.001);

        animations->add_animation("RESET", reset_anim);
    }
    
    ResourceLoader *res_loader = ResourceLoader::get_singleton();
    Array motion_files = walk_files(assets.get_base_dir(), MOTION_FILE_EXTENSION);
    for (int i = 0; i < motion_files.size(); i++) {
        String motion = motion_files[i];
        Ref<Animation> anim = res_loader->load(motion, "Animation");
        if (anim.is_valid()) {
            animations->add_animation(motion.get_file(), anim);
        }
    }

    return animations;
}

Variant GDCubismMotionLoader::_load(const String& p_path, const String& p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
    String buffer = FileAccess::get_file_as_string(p_original_path);
    ERR_FAIL_COND_V_MSG(buffer.is_empty(), "File contents are empty", Error::FAILED);

    Dictionary motion = JSON::parse_string(buffer);
    Dictionary meta = motion["Meta"];

    Ref<Animation> anim;
    anim.instantiate();

    bool loop = meta.get("Loop", false);
    anim->set_loop_mode(
        loop
        ? Animation::LOOP_LINEAR
        : Animation::LOOP_NONE);
    double fps = meta.get("FPS", 60.0);
    anim->set_step(1.0 / fps);

    double last_frame = 0.0;

    // parse motion curves
    Array curves = motion["Curves"];
    for (uint32_t c_idx = 0; c_idx < curves.size(); c_idx++) {
        Dictionary curve = curves[c_idx];

        String property = curve["Id"];
        Array segments = curve["Segments"];

        ERR_FAIL_COND_V_MSG(segments.size() < 2, Error::FAILED, "Invalid Segment, must have at least one point");

        int32_t track = anim->add_track(Animation::TYPE_BEZIER);
        anim->track_set_path(track, NodePath(".:" + property));
        anim->track_set_interpolation_loop_wrap(track, false);

        // first key is always the starting time and value
        anim->bezier_track_insert_key(track, segments[0], segments[1]);

        // parsing techniques adapted from Unity Importer
        // https://github.com/Live2D/CubismUnityComponents/blob/08815c83738ecdd8fe689f4222280540ff8ea9c8/Assets/Live2D/Cubism/Framework/Json/CubismMotion3Json.cs#L401
        for (uint32_t s_idx = 2, last_key = anim->track_get_key_count(track) - 1; s_idx < segments.size();) {

            int32_t type = segments[s_idx];
            ERR_FAIL_COND_V_MSG(type < 0 || type > 3, Error::FAILED, "Invalid Motion Segment Type");

            real_t p0_t = segments[s_idx - 2];
            real_t p0_v = segments[s_idx - 1];

            if (type == 0) {
                // LINEAR
                real_t p1_t = segments[s_idx + 1];
                real_t p1_v = segments[s_idx + 2];

                // tangents
                Vector2 out_t = Vector2(p1_t - p0_t, p1_v - p0_v);
                Vector2 in_t = out_t * Vector2(-1, 1);

                anim->bezier_track_set_key_out_handle(track, last_key, out_t);

                last_key = anim->bezier_track_insert_key(
                    track,
                    p1_t, p1_v,
                    in_t,
                    Vector2(0, 0));

                s_idx += 3;
            }
            else if (type == 1) {
                // CUBIC BEZIER
                real_t p1_t = segments[s_idx + 1];
                real_t p1_v = segments[s_idx + 2];
                real_t p2_t = segments[s_idx + 3];
                real_t p2_v = segments[s_idx + 4];
                real_t p3_t = segments[s_idx + 5];
                real_t p3_v = segments[s_idx + 6];

                real_t tangent_len = Math::absf(p0_t - p3_t) * 0.33333f;
                Vector2 out_t = Vector2(tangent_len, p1_v - p0_v);
                Vector2 in_t = Vector2(-tangent_len, p3_v - p2_v);

                anim->bezier_track_set_key_out_handle(track, last_key, out_t);

                last_key = anim->bezier_track_insert_key(
                    track, p3_t, p3_v,
                    in_t,
                    Vector2(0, 0));

                s_idx += 7;
            }
            else if (type == 2) {
                // Stepped
                real_t p1_t = segments[s_idx + 1];
                real_t p1_v = segments[s_idx + 2];

                last_key = anim->bezier_track_insert_key(
                    track, p1_t, p1_v,
                    Vector2(0, INFINITY));
                s_idx += 3;
            }
            else if (type == 3) {
                // Inverse Stepped
                real_t p1_t = segments[s_idx + 1];
                real_t p1_v = segments[s_idx + 2];

                Vector2 out_t = Vector2(p1_t - p0_t, p1_v - p0_v);

                anim->bezier_track_set_key_out_handle(track, last_key, out_t);

                anim->bezier_track_insert_key(
                    track, p0_t + 0.01f, p1_v,
                    out_t,
                    Vector2(0, 0));

                last_key = anim->bezier_track_insert_key(
                    track, p1_t, p1_v,
                    Vector2(0, 0),
                    Vector2(0, 0));

                s_idx += 3;
            }

            last_frame = Math::max(
                last_frame,
                anim->track_get_key_time(track, anim->track_get_key_count(track) - 1)
            );
        }
    }

    double duration = meta.get("Duration", 1.0);
    
    anim->set_length(Math::max(last_frame, duration));
    anim->set_path(p_path);

    return anim;
}