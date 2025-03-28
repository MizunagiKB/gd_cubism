#ifndef GD_CUBISM_MODEL_IMPORTER
#define GD_CUBISM_MODEL_IMPORTER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <gd_cubism_user_model.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismModelImporter : public EditorImportPlugin {
    GDCLASS(GDCubismModelImporter, EditorImportPlugin);

protected:
    static void _bind_methods() {
    }

public:
    String _get_importer_name() const { return "gdcubism.model"; }
    String _get_visible_name() const { return "Cubism Model (2D)"; }
    int32_t _get_preset_count() const { return 0; }
    String _get_preset_name(int32_t p_preset_index) const { return "default"; }
    PackedStringArray _get_recognized_extensions() const { 
        PackedStringArray exts;
        exts.append("json");
        return exts; 
    }
    TypedArray<Dictionary> _get_import_options(const String &p_path, int32_t p_preset_index) const override;
    bool _get_option_visibility(const String &p_path, const StringName &p_option_name, const Dictionary &p_options) const override {
        return true;
    }
    String _get_save_extension() const { return "tscn"; }
    String _get_resource_type() const { return "PackedScene"; }
    float _get_priority() const { return 1.0f; }
    int32_t _get_import_order() const { return 0; }

    Error _import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const override;
};

#endif // GD_CUBISM_MODEL_IMPORTER
