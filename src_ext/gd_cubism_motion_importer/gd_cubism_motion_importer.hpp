#ifndef GD_CUBISM_MOTION_IMPORTER
#define GD_CUBISM_MOTION_IMPORTER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/animation.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismMotionImporter : public EditorImportPlugin {
    GDCLASS(GDCubismMotionImporter, EditorImportPlugin);

protected:
    static void _bind_methods();

public:
    static Ref<Animation> parse_motion(const String &p_source_file);

    String _get_importer_name() const override { return "gdcubism.motion"; }
    String _get_visible_name() const override { return "Live2D Motion"; }
    int32_t _get_preset_count() const override { return 0; }
    String _get_preset_name(int32_t p_preset_index) const override { return "default"; }
    PackedStringArray _get_recognized_extensions() const override { 
        PackedStringArray exts;
        exts.append("json");
        return exts; 
    }
    TypedArray<Dictionary> _get_import_options(const String &p_path, int32_t p_preset_index) const override { 
        TypedArray<Dictionary> options;
        return options; 
    }
    String _get_save_extension() const override { return "anim"; }
    String _get_resource_type() const override { return "Animation"; }
    double _get_priority() const override { return 1.0; }
    int32_t _get_import_order() const override { return 0; }

    Error _import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const override;
};

#endif // GD_CUBISM_MOTION_IMPORTER
