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

#include <CubismFramework.hpp>
#include <Math/CubismVector2.hpp>
#include <Motion/ACubismMotion.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>

#include <gd_cubism_motion_entry.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;
class GDCubismEffect;


// ------------------------------------------------------------------- class(s)
struct anim_expression {
public:
    String expression_id;

public:
    anim_expression() {}
    anim_expression(const Csm::csmChar* c_expression_id)
        : expression_id(String(c_expression_id)) {}

    String to_string() const {
        return String(this->expression_id);
    }
};


class anim_motion {
public:
    String group;
    int32_t no;

public:
    anim_motion() {}
    anim_motion(const Csm::csmChar* c_group, const int32_t c_no)
        : group(String(c_group))
        , no(c_no) {}

    String to_string() const {
        Array ary;
        ary.append(this->group);
        ary.append(this->no);

        return String("{0}_{1}").format(ary);
    }
};


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

    Array ary_meshes;
    Dictionary dict_mesh;

    Array ary_parameter;
    Array ary_part_opacity;
    Array hit_areas;

    Csm::csmMap<String,anim_expression> dict_anim_expression;
    String curr_anim_expression_key;

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
        ClassDB::bind_method(D_METHOD("set_parameters"), &GDCubismUserModel::set_parameters);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "parameters"), "set_parameters", "get_parameters");

        // PartOpacity
        ClassDB::bind_method(D_METHOD("get_part_opacities"), &GDCubismUserModel::get_part_opacities);

        // Meshes
        ClassDB::bind_method(D_METHOD("get_meshes"), &GDCubismUserModel::get_meshes);
        ClassDB::bind_method(D_METHOD("get_mesh_dictionary"), &GDCubismUserModel::get_mesh_dict);

        // Animations
        ClassDB::bind_method(D_METHOD("get_animations"), &GDCubismUserModel::get_animations);
        ClassDB::bind_method(D_METHOD("set_animations"), &GDCubismUserModel::set_animations);
        ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "animations", PROPERTY_HINT_RESOURCE_TYPE, "AnimationLibrary"), "set_animations", "get_animations");

        ClassDB::bind_method(D_METHOD("advance", "delta"), &GDCubismUserModel::advance);

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

    Array get_parameters() const { return this->ary_parameter; };
    void set_parameters(const Array parameters) { this->ary_parameter = parameters; }
    
    AnimationLibrary* get_animations() const { return this->ani_lib; };
    void set_animations(AnimationLibrary* library) { this->ani_lib = library; }
    
    Array get_part_opacities() const {
        return this->ary_part_opacity;
    }
    void set_part_opacities(const Array opacities) { this->ary_part_opacity = opacities; }

    Dictionary get_mesh_dict() const;

    Array get_meshes() const;
    
    void _update(const float delta);

    void advance(const float delta);

    bool check_cubism_effect_dirty() const;
    void cubism_effect_dirty_reset();

    // Properties
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    void _get_property_list(List<godot::PropertyInfo> *p_list);

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
