// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_USER_MODEL_H
#define GD_CUBISM_USER_MODEL_H


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/canvas_group.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/animation_library.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <CubismFramework.hpp>
#include <Math/CubismVector2.hpp>
#include <Motion/ACubismMotion.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>

#include <gd_cubism_motion_entry.hpp>
#include <gd_cubism_expression_controller.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
const static char* MESHES_NODE = "Meshes";
const static char* PARAMETERS_NODE = "Parameters";
const static char* EXPRESSION_CONTROLLER_NODE = "ExpressionController";
const static char* MOTION_CONTROLLER_NODE = "MotionController";

// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;
class GDCubismEffect;


// ------------------------------------------------------------------- class(s)

class GDCubismUserModel : public Node2D {
    GDCLASS(GDCubismUserModel, Node2D);

public:
    GDCubismUserModel();
    ~GDCubismUserModel();

public:
    enum moc3FileFormatVersion {
        CSM_MOC_VERSION_UNKNOWN = Live2D::Cubism::Core::csmMocVersion_Unknown,
        CSM_MOC_VERSION_30 = Live2D::Cubism::Core::csmMocVersion_30,
        CSM_MOC_VERSION_33 = Live2D::Cubism::Core::csmMocVersion_33,
        CSM_MOC_VERSION_40 = Live2D::Cubism::Core::csmMocVersion_40,
        CSM_MOC_VERSION_42 = Live2D::Cubism::Core::csmMocVersion_42,
        CSM_MOC_VERSION_50 = Live2D::Cubism::Core::csmMocVersion_50
    };

    enum Priority {
        PRIORITY_NONE = 0,
        PRIORITY_IDLE = 1,
        PRIORITY_NORMAL = 2,
        PRIORITY_FORCE = 3
    };

    InternalCubismUserModel *internal_model;

    AnimationLibrary *ani_lib;
    bool physics_evaluate;
    bool pose_update;
    Array ary_meshes;
    Dictionary dict_mesh;
    Array hit_areas;

    int32_t mask_viewport_size = 0;

    Csm::csmVector<GDCubismEffect*> _list_cubism_effect;
    bool cubism_effect_dirty;

protected:
    static void _bind_methods() {
        // csm
        ClassDB::bind_method(D_METHOD("csm_get_version"), &GDCubismUserModel::csm_get_version);

        ClassDB::bind_method(D_METHOD("get_canvas_info"), &GDCubismUserModel::get_canvas_info);

        // HitArea
        ClassDB::bind_method(D_METHOD("get_hit_areas"), &GDCubismUserModel::get_hit_areas);

        // Parameter
        ClassDB::bind_method(D_METHOD("get_parameters"), &GDCubismUserModel::get_parameters);

        // PartOpacity
        ClassDB::bind_method(D_METHOD("get_part_opacities"), &GDCubismUserModel::get_part_opacities);

        // Meshes
        ClassDB::bind_method(D_METHOD("get_meshes"), &GDCubismUserModel::get_meshes);
        ClassDB::bind_method(D_METHOD("get_mesh_dictionary"), &GDCubismUserModel::get_mesh_dict);

        // Animations
        ClassDB::bind_method(D_METHOD("get_animations"), &GDCubismUserModel::get_animations);

        ClassDB::bind_method(D_METHOD("get_animation_player"), &GDCubismUserModel::get_animation_player);

        ClassDB::bind_method(D_METHOD("advance", "delta"), &GDCubismUserModel::advance);

        // Expressions
        ClassDB::bind_method(D_METHOD("get_expressions"), &GDCubismUserModel::get_expressions);
        ClassDB::bind_method(D_METHOD("get_expression_controller"), &GDCubismUserModel::get_expression_controller);
    
        // Model properties
        ClassDB::bind_method(D_METHOD("set_physics_evaluate", "enable"), &GDCubismUserModel::set_physics_evaluate);
        ClassDB::bind_method(D_METHOD("get_physics_evaluate"), &GDCubismUserModel::get_physics_evaluate);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "physics_evaluate"), "set_physics_evaluate", "get_physics_evaluate");

        ClassDB::bind_method(D_METHOD("set_pose_update", "enable"), &GDCubismUserModel::set_pose_update);
        ClassDB::bind_method(D_METHOD("get_pose_update"), &GDCubismUserModel::get_pose_update);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pose_update"), "set_pose_update", "get_pose_update");

        ClassDB::bind_method(D_METHOD("set_mask_viewport_size", "value"), &GDCubismUserModel::set_mask_viewport_size);
        ClassDB::bind_method(D_METHOD("get_mask_viewport_size"), &GDCubismUserModel::get_mask_viewport_size);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "mask_viewport_size", PROPERTY_HINT_RANGE, "0, 4096"), "set_mask_viewport_size", "get_mask_viewport_size");

        // moc3FileFormatVersion
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_UNKNOWN);
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_30);
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_33);
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_40);
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_42);
        BIND_ENUM_CONSTANT(CSM_MOC_VERSION_50);

        // Priority
        BIND_ENUM_CONSTANT(PRIORITY_NONE);
        BIND_ENUM_CONSTANT(PRIORITY_IDLE);
        BIND_ENUM_CONSTANT(PRIORITY_NORMAL);
        BIND_ENUM_CONSTANT(PRIORITY_FORCE);
    }
    void _notification(int p_what);

public:
    Dictionary csm_get_version();

    Dictionary get_canvas_info() const;

    bool is_initialized() const;

    Array get_hit_areas() const { return this->hit_areas; }

    Array get_parameters() const { 
        Node* parameters = this->get_node_or_null(NodePath(PARAMETERS_NODE));
        if (parameters == nullptr) return Array();
        return parameters->get_children();
    };
    
    Ref<AnimationLibrary> get_animations() const {
        return this->get_animation_player()->get_animation_library("");
    }

    AnimationPlayer* get_animation_player() const {
        return Object::cast_to<AnimationPlayer>(this->get_node_or_null(MOTION_CONTROLLER_NODE)); 
    }

    Array get_expressions() const {
        GDCubismExpressionController* controller = this->get_expression_controller();
        if (controller == nullptr) return Array();
        return controller->get_expression_library().keys();
    }

    GDCubismExpressionController* get_expression_controller() const {
        return Object::cast_to<GDCubismExpressionController>(this->get_node_or_null(EXPRESSION_CONTROLLER_NODE));
    }
    
    Array get_part_opacities() const {
        Node* parameters = this->get_node_or_null(NodePath("Parts"));
        if (parameters == nullptr) return Array();
        return parameters->get_children();
    }

    Dictionary get_mesh_dict() const;

    Array get_meshes() const;
    
    void set_physics_evaluate(const bool enable) { this->physics_evaluate = enable; }
    bool get_physics_evaluate() const { return this->physics_evaluate; }

    void set_pose_update(const bool enable) { this->pose_update = enable; }
    bool get_pose_update() const { return this->pose_update; }

    void _update(const float delta);

    void advance(const float delta);

    bool check_cubism_effect_dirty() const;
    void cubism_effect_dirty_reset();

    // Properties
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    void _validate_property(PropertyInfo &p_property) const;
    void _get_property_list(List<godot::PropertyInfo> *p_list);

    void set_mask_viewport_size(const int32_t size) { this->mask_viewport_size = size; }
    int32_t get_mask_viewport_size() const { return this->mask_viewport_size; }

    void _ready() override;
    void _enter_tree() override;
    void _exit_tree() override;
    void _process(double delta) override;

    void _on_append_child_act(GDCubismEffect* node);
    void _on_remove_child_act(GDCubismEffect* node);
};

VARIANT_ENUM_CAST(GDCubismUserModel::moc3FileFormatVersion);
VARIANT_ENUM_CAST(GDCubismUserModel::Priority);

// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_USER_MODEL_H
