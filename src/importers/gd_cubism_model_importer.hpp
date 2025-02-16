#ifndef GD_CUBISM_MODEL_IMPORTER
#define GD_CUBISM_MODEL_IMPORTER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_import_plugin.hpp>
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <gd_cubism_user_model.hpp>
#include <private/internal_cubism_user_model.hpp>
#include <private/internal_cubism_renderer_2d.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismModelImporter : public EditorImportPlugin {
    GDCLASS(GDCubismModelImporter, EditorImportPlugin);

protected:
    static void _bind_methods() {
        ClassDB::bind_static_method("GDCubismModelImporter", D_METHOD("load_model", "model_file", "include_expressions", "include_motions", "shaders"), &GDCubismModelImporter::load_model);
    }

private:
    static MeshInstance2D* request_mesh_instance();
    static ShaderMaterial* request_shader_material(const Csm::CubismModel *model, const Csm::csmInt32 index, Array shaders);
    static ShaderMaterial* request_mask_material(Array shaders);
    static void build_model(InternalCubismRenderer2D* renderer, GDCubismUserModel* target_node, Array textures, Array shaders);

public:
    static GDCubismUserModel* load_model(const String &p_source_file, const bool include_expressions, const bool include_motions, Array shaders);

    String _get_importer_name() const { return "gdcubism.model"; }
    String _get_visible_name() const { return "Cubism Model (2D)"; }
    int32_t _get_preset_count() const { return 0; }
    String _get_preset_name(int32_t p_preset_index) const { return "default"; }
    PackedStringArray _get_recognized_extensions() const { 
        PackedStringArray exts;
        exts.append("json");
        return exts; 
    }
    TypedArray<Dictionary> _get_import_options(const String &p_path, int32_t p_preset_index) const { 
        TypedArray<Dictionary> options;

        Dictionary include_expressions;
		include_expressions["name"] = "include_expressions";
		include_expressions["default_value"] = true;
		options.append(include_expressions);

		Dictionary include_motions;
		include_expressions["name"] = "include_motions";
		include_expressions["default_value"] = true;
		options.append(include_motions);
        
        return options; 
    }
    String _get_save_extension() const { return "tscn"; }
    String _get_resource_type() const { return "PackedScene"; }
    double _get_priority() const { return 1.0; }
    int32_t _get_import_order() const { return 0; }

    Error _import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const override;
};

#endif // GD_CUBISM_MODEL_IMPORTER
