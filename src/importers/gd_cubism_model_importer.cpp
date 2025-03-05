#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <importers/gd_cubism_model_importer.hpp>
#include <loaders/gd_cubism_model_loader.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;


Error GDCubismModelImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V_MSG(!p_source_file.ends_with(MODEL_FILE_EXTENSION), Error::FAILED, "Live2D Model file must end with model3.json");

	ResourceLoader* res_loader = ResourceLoader::get_singleton();
	Array shaders;
    shaders.resize(GD_CUBISM_SHADER_MAX);

    shaders[GD_CUBISM_SHADER_NORM_ADD] = p_options["shader_add"];
    shaders[GD_CUBISM_SHADER_NORM_MIX] = p_options["shader_mix"];
    shaders[GD_CUBISM_SHADER_NORM_MUL] = p_options["shader_mul"];

    shaders[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    shaders[GD_CUBISM_SHADER_MASK_ADD] = p_options["shader_mask_add"];
    shaders[GD_CUBISM_SHADER_MASK_ADD_INV] = p_options["shader_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MIX] = p_options["shader_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MIX_INV] = p_options["shader_inv_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MUL] = p_options["shader_mask_mul"];
    shaders[GD_CUBISM_SHADER_MASK_MUL_INV] = p_options["shader_inv_mask_mul"];

    Ref<GDCubismModelLoader> loader;
    loader.instantiate();

    GDCubismUserModel *m = loader->load_model(
        p_source_file,
        shaders,
        p_options["include_expressions"],
        static_cast<GDCubismModelLoader::MotionManagerType>((int)(p_options["include_motions"]))
    );
    loader.unref();
	ERR_FAIL_COND_V(m == nullptr, Error::FAILED);

	Ref<PackedScene> p;
	p.instantiate();
    if (p->pack(m) != OK) {
        p.unref();
        memdelete(m);
        return Error::FAILED;
    }

    String filename = p_save_path + String(".") + this->_get_save_extension();
    auto result = ResourceSaver::get_singleton()->save(p, filename);
    memdelete(m);
    return result;
}

TypedArray<Dictionary> GDCubismModelImporter::_get_import_options(const String &p_path, int32_t p_preset_index) const { 
    TypedArray<Dictionary> options;

    ResourceLoader *res_loader = ResourceLoader::get_singleton();

    Dictionary include_expressions;
    include_expressions["name"] = "include_expressions";
    include_expressions["default_value"] = true;
    options.append(include_expressions);

    Dictionary include_motions;
    include_motions["name"] = "include_motions";
    include_motions["default_value"] = GDCubismModelLoader::MOTION_GODOT;
    include_motions["property_hint"] = PropertyHint::PROPERTY_HINT_ENUM;
    include_motions["hint_string"] = "None,Godot,Native";
    options.append(include_motions);

    {
        Dictionary shader;
        shader["name"] = "shader_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_add.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mul.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    
    return options; 
}