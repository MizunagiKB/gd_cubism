#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <importers/gd_cubism_model_importer.hpp>
#include <loaders/gd_cubism_model_loader.hpp>
#include <loaders/gd_cubism_motion_loader.hpp>
#include <gd_cubism_effect_expression.hpp>
#include <gd_cubism_effect_motion.hpp>
#include <gd_cubism_effect_pose.hpp>
#include <gd_cubism_effect_physics.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

static const char* MOTION_CONTROLLER_NODE = "MotionController";
static const char* EXPRESSION_CONTROLLER_NODE = "ExpressionController";
static const char* POSE_NODE = "PoseController";
static const char* PHYSICS_NODE = "PhysicsEffect";

Error GDCubismModelImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V_MSG(!p_source_file.ends_with(MODEL_FILE_EXTENSION), Error::FAILED, "Live2D Model file must end with model3.json");

	ResourceLoader* res_loader = ResourceLoader::get_singleton();
	Array shaders;
    shaders.resize(GD_CUBISM_SHADER_MAX);

    shaders[GD_CUBISM_SHADER_NORM_ADD] = p_options["shader_add"];
    shaders[GD_CUBISM_SHADER_NORM_MIX] = p_options["shader_mix"];
    shaders[GD_CUBISM_SHADER_NORM_MUL] = p_options["shader_mul"];

    shaders[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    Ref<GDCubismModelLoader> loader;
    loader.instantiate();

    GDCubismUserModel *model = loader->load_model(
        p_source_file,
        shaders
    );
    loader.unref();
	ERR_FAIL_COND_V(model == nullptr, Error::FAILED);

    GDCubismMotionLoader::MotionManagerType include_motions = static_cast<GDCubismMotionLoader::MotionManagerType>((int)(p_options["include_motions"]));

    // Load Native Animations
    if (include_motions == GDCubismMotionLoader::MOTION_NATIVE) {
        GDCubismEffectMotion *motionManager = memnew(GDCubismEffectMotion);
        model->add_child(motionManager);
        motionManager->set_name(MOTION_CONTROLLER_NODE);
        motionManager->set_owner(model);
    }

	// Load Godot Animations
    if (include_motions == GDCubismMotionLoader::MOTION_GODOT)
    {
        AnimationPlayer *anim_player = memnew(AnimationPlayer);
        Ref<AnimationLibrary> animations = GDCubismMotionLoader::load_motion_library(model);
        anim_player->add_animation_library("", animations);
        model->add_child(anim_player);
        anim_player->set_owner(model);
        anim_player->set_name(MOTION_CONTROLLER_NODE);
        
        anim_player->set_root_node("../");

        anim_player->set_active(true);
        anim_player->stop();
    }

    // Load Expressions
    if (p_options["include_expressions"])
    {
        GDCubismEffectExpression *expressions = memnew(GDCubismEffectExpression);
        model->add_child(expressions);
        expressions->set_name(EXPRESSION_CONTROLLER_NODE);
        expressions->set_owner(model);
    }

    // Preload physics effects
    if (p_options["include_physics"])
    {
        GDCubismEffectPhysics *physics = memnew(GDCubismEffectPhysics);
        model->add_child(physics);
        physics->set_name(PHYSICS_NODE);
        physics->set_owner(model);
    }

    // Pose effect
    {
        GDCubismEffectPose *pose = memnew(GDCubismEffectPose);
        model->add_child(pose);
        pose->set_name(POSE_NODE);
        pose->set_owner(model);
    }

	Ref<PackedScene> p;
	p.instantiate();
    if (p->pack(model) != OK) {
        p.unref();
        memdelete(model);
        return Error::FAILED;
    }

    String filename = p_save_path + String(".") + this->_get_save_extension();
    auto result = ResourceSaver::get_singleton()->save(p, filename);
    memdelete(model);
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
    include_motions["default_value"] = GDCubismMotionLoader::MOTION_GODOT;
    include_motions["property_hint"] = PropertyHint::PROPERTY_HINT_ENUM;
    include_motions["hint_string"] = "None,Godot,Native";
    options.append(include_motions);

    // physics
    {
        Dictionary option;
        option["name"] = "include_physics";
        option["default_value"] = true;
        options.append(option);
    }

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
    
    return options; 
}