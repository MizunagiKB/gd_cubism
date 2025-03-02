#ifndef GD_CUBISM_MOTION_IMPORTER
#define GD_CUBISM_MOTION_IMPORTER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismMotionImporter : public EditorImportPlugin {
    GDCLASS(GDCubismMotionImporter, EditorImportPlugin);

protected:
    static void _bind_methods() {}

public:
    String _get_importer_name() const override { return "gdcubism.motion"; }
    String _get_visible_name() const override { return "Animation (Cubism Motion)"; }
    int32_t _get_preset_count() const override { return 0; }
    String _get_preset_name(int32_t p_preset_index) const override { return "default"; }
    PackedStringArray _get_recognized_extensions() const override { 
        PackedStringArray exts;
        exts.append("json");
        return exts; 
    }

    TypedArray<Dictionary> _get_import_options(const String &p_path, int32_t p_preset_index) const override {
        return TypedArray<Dictionary>();
    }
    bool _get_option_visibility(const String &p_path, const StringName &p_option_name, const Dictionary &p_options) const override {
        return true;
    }
    
    String _get_save_extension() const { return "anim"; }
    String _get_resource_type() const { return "Animation"; }
    double _get_priority() const { return 1.0; }
    int32_t _get_import_order() const { return 0; }

    Error _import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const override {
        ERR_FAIL_COND_V(!p_source_file.ends_with(".motion3.json"), Error::ERR_FILE_BAD_PATH);

        // Motion Importer needs no options, so we may just use direct reading through ResourceLoader
        Ref<Animation> anim = ResourceLoader::get_singleton()->load(p_source_file, "Animation");
        
        String filename = p_save_path + String(".") + this->_get_save_extension();
        return ResourceSaver::get_singleton()->save(anim, filename);
    }
};

#endif // GD_CUBISM_MOTION_IMPORTER
