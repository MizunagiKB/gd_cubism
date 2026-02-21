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
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismMoc.hpp>
#include <Model/CubismModel.hpp>
#include <Math/CubismVector2.hpp>
#include <ICubismModelSetting.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
const static char* MESHES_NODE = "Meshes";
const static char* MASKS_NODE = "Masks";

// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
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

    bool physics_evaluate;
    bool pose_update;
    Array ary_meshes;
    Array ary_masks;
    Dictionary dict_mesh;
    Dictionary user_data;

    int32_t mask_viewport_size = 0;

    Array _list_cubism_effect;
    bool cubism_effect_dirty;

    Vector2i size;
    Vector2i origin;
    float pp_unit;

    void load_model();
    void cleanup_csm();
private:
    enum EFFECT_CALL {
        EFFECT_CALL_PROLOGUE,
        EFFECT_CALL_PROCESS,
        EFFECT_CALL_EPILOGUE
    };

    CubismMoc *_moc;
    CubismModel *internal_model;
    ICubismModelSetting *model_settings;

    Dictionary parameter_values;
    Dictionary part_opacity_values;

    Dictionary parts;
    Array ary_parts;
    Dictionary parameters;
    Array ary_parameters;

    void effect_init();
    void effect_term();
    void effect_batch(const double delta, const EFFECT_CALL efx_call);

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("is_initialized"), &GDCubismUserModel::is_initialized);

        // csm
        ClassDB::bind_method(D_METHOD("csm_get_version"), &GDCubismUserModel::csm_get_version);

        ClassDB::bind_method(D_METHOD("get_size"), &GDCubismUserModel::get_size);
        ClassDB::bind_method(D_METHOD("set_size"), &GDCubismUserModel::set_size);
        ClassDB::bind_method(D_METHOD("get_origin"), &GDCubismUserModel::get_origin);
        ClassDB::bind_method(D_METHOD("set_origin"), &GDCubismUserModel::set_origin);
        ClassDB::bind_method(D_METHOD("get_pp_unit"), &GDCubismUserModel::get_pp_unit);
        ClassDB::bind_method(D_METHOD("set_pp_unit"), &GDCubismUserModel::set_pp_unit);
        ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "size"), "set_size", "get_size");
        ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "origin"), "set_origin", "get_origin");
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pp_unit"), "set_pp_unit", "get_pp_unit");
        ClassDB::bind_method(D_METHOD("get_user_data"), &GDCubismUserModel::get_user_data);

        // Parameter
        ClassDB::bind_method(D_METHOD("get_parameters"), &GDCubismUserModel::get_parameters);
        ClassDB::bind_method(D_METHOD("set_parameters", "parameters"), &GDCubismUserModel::set_parameters);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parameters", PROPERTY_HINT_DICTIONARY_TYPE, "StringName,Dictionary"), "set_parameters", "get_parameters");

        // PartOpacity
        ClassDB::bind_method(D_METHOD("get_parts"), &GDCubismUserModel::get_parts);
        ClassDB::bind_method(D_METHOD("set_parts", "parts"), &GDCubismUserModel::set_parts);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "parts"), "set_parts", "get_parts");

        // Meshes
        ClassDB::bind_method(D_METHOD("get_meshes"), &GDCubismUserModel::get_meshes);
        ClassDB::bind_method(D_METHOD("get_mesh_dictionary"), &GDCubismUserModel::get_mesh_dict);

        // Model properties
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

    void set_size(Vector2i v) {
        this->size = v;
    }

    Vector2i get_size() const {
        return this->size;
    }

    void set_origin(Vector2i v) {
        this->origin = v;
    }

    Vector2i get_origin() const {
        return this->origin;
    }

    void set_pp_unit(float v) {
        this->pp_unit = v;
    }

    float get_pp_unit() const {
        return this->pp_unit;
    }

    Dictionary get_user_data() const {
        return this->user_data;
    }

    bool is_initialized() const;

    Dictionary get_mesh_dict() const;

    Array get_meshes() const;

    void advance(const double delta);

    bool check_cubism_effect_dirty() const;
    void cubism_effect_dirty_reset();

    // Properties
    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    void _validate_property(PropertyInfo &p_property) const;
    void _get_property_list(List<godot::PropertyInfo> *p_list);

    void set_parameters(const Dictionary v) { 
        this->parameters = v;
        this->ary_parameters = v.values();
    }
    Dictionary get_parameters() const {
        return this->parameters; 
    }

    ICubismModelSetting * get_model_settings() const {
        return this->model_settings;
    }

    void set_parts(const Dictionary v) {
        this->parts = v; 
        this->ary_parts = v.values();
    }
    Dictionary get_parts() const { return this->parts; }

    void set_mask_viewport_size(const int32_t size) { this->mask_viewport_size = size; }
    int32_t get_mask_viewport_size() const { return this->mask_viewport_size; }

    void prepare();

    void _on_append_child_act(GDCubismEffect* node);
    void _on_remove_child_act(GDCubismEffect* node);

    CubismModel* get_internal_model() {
        return this->internal_model;
    }
};

VARIANT_ENUM_CAST(GDCubismUserModel::moc3FileFormatVersion);
VARIANT_ENUM_CAST(GDCubismUserModel::Priority);

// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_USER_MODEL_H

