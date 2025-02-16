// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Motion/CubismMotion.hpp>

#include <private/internal_cubism_user_model.hpp>
#include <gd_cubism_effect_eye_blink.hpp>
#include <gd_cubism_motion_entry.hpp>
#include <gd_cubism_value_parameter.hpp>
#include <gd_cubism_value_part_opacity.hpp>
#include <gd_cubism_user_model.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
GDCubismUserModel::GDCubismUserModel()
    : internal_model(nullptr)
    , ani_lib(nullptr)
    , cubism_effect_dirty(false) {
}


GDCubismUserModel::~GDCubismUserModel() {}


void GDCubismUserModel::_notification(int p_what) {
    if (p_what == NOTIFICATION_PREDELETE) {
        if(this->internal_model == nullptr) {
            return;
        }

        this->internal_model->clear();
        CSM_DELETE(this->internal_model);
        this->internal_model = nullptr;
    }
}


Dictionary GDCubismUserModel::csm_get_version() {
    Dictionary dict_version;
    const Live2D::Cubism::Core::csmVersion version = Live2D::Cubism::Core::csmGetVersion();

    dict_version["version"] = version;
    dict_version["major"] = (version >> 24) & 0xFF;
    dict_version["minor"] = (version >> 16) & 0xFF;
    dict_version["patch"] = version & 0xFFFF;

    return dict_version;
}


Dictionary GDCubismUserModel::get_canvas_info() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Dictionary());

    Dictionary result;
    Live2D::Cubism::Core::csmVector2 vct_pixel_size;
    Live2D::Cubism::Core::csmVector2 vct_pixel_origin;
    Csm::csmFloat32 pixel_per_unit;

    Live2D::Cubism::Core::csmReadCanvasInfo(this->internal_model->GetModel()->GetModel(), &vct_pixel_size, &vct_pixel_origin, &pixel_per_unit);

    result["size_in_pixels"] = Vector2(vct_pixel_size.X, vct_pixel_size.Y);
    result["origin_in_pixels"] = Vector2(vct_pixel_origin.X, vct_pixel_origin.Y);
    result["pixels_per_unit"] = pixel_per_unit;

    return result;
}


bool GDCubismUserModel::is_initialized() const {
    if(this->internal_model == nullptr) return false;
    return this->internal_model->IsInitialized();
}

Array GDCubismUserModel::get_meshes() const {
    return this->ary_meshes;
}

Dictionary GDCubismUserModel::get_masks() const {
    return this->dict_mask;
}

Dictionary GDCubismUserModel::get_mesh_dict() const { 
    return this->dict_mesh; 
}

void GDCubismUserModel::_update(const float delta) {
    this->internal_model->pro_update(delta);

    this->internal_model->efx_update(delta);

    for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++ ) {
        GDCubismParameter *param = Object::cast_to<GDCubismParameter>(this->ary_parameter[index]);
        if(param != nullptr) {
            param->sync(this->internal_model->GetModel());
        }
    }

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++ ) {
        GDCubismPartOpacity *param = Object::cast_to<GDCubismPartOpacity>(this->ary_part_opacity[index]);
        if(param != nullptr) {
            param->sync(this->internal_model->GetModel());
        }
    }

    this->internal_model->epi_update(delta);

    this->internal_model->update_node();
}


void GDCubismUserModel::advance(const float delta) {
    ERR_FAIL_COND(this->is_initialized() == false);
    
    this->_update(delta);
}


bool GDCubismUserModel::check_cubism_effect_dirty() const {
    return this->cubism_effect_dirty;
}


void GDCubismUserModel::cubism_effect_dirty_reset() {
    this->cubism_effect_dirty = false;
}

bool GDCubismUserModel::_set(const StringName &p_name, const Variant &p_value) {
    for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++) {
        Ref<GDCubismParameter> p = Object::cast_to<GDCubismParameter>(this->ary_parameter[index]);
        if(p != nullptr && p->id == p_name) {
            p->value = p_value;
            return true;
        }
    }

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++) {
        Ref<GDCubismPartOpacity> p = Object::cast_to<GDCubismPartOpacity>(this->ary_part_opacity[index]);
        if(p != nullptr && p->id == p_name) {
            p->value = p_value;
            return true;
        }
    }
    
    return false;
}


bool GDCubismUserModel::_get(const StringName &p_name, Variant &r_ret) const {
    for(int64_t index = 0; index < this->ary_parameter.size(); index++) {
        Ref<GDCubismParameter> p = Object::cast_to<GDCubismParameter>(this->ary_parameter[index]);
        if(p != nullptr && p->id == p_name) {
            r_ret = p->value;
            return true;
        }
    }

    for(int64_t index = 0; index < this->ary_part_opacity.size(); index++) {
        Ref<GDCubismPartOpacity> p = Object::cast_to<GDCubismPartOpacity>(this->ary_part_opacity[index]);
        if(p != nullptr && p->id == p_name) {
            r_ret = p->value;
            return true;
        }
    }

    return false;
}


bool GDCubismUserModel::_property_can_revert(const StringName &p_name) const {
    for(int64_t index = 0; index < this->ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(this->ary_parameter[index]);
        if(p->id == p_name) return true;
    }

    return false;
}


bool GDCubismUserModel::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    for(int64_t index = 0; index < this->ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(this->ary_parameter[index]);
        if(p->id == p_name) {
            r_property = p->default_value;
            return true;
        }
    }

    return false;
}


void GDCubismUserModel::_get_property_list(List<godot::PropertyInfo> *p_list) {
    // Property - Parameter
    p_list->push_back(PropertyInfo(Variant::STRING, PROP_PARAMETER_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    for(int64_t index = 0; index < this->ary_parameter.size(); index++) {
        Ref<GDCubismParameter> param = this->ary_parameter[index];

        Array ary_value;
        ary_value.append(param->minimum_value);
        ary_value.append(param->maximum_value);

        PropertyInfo pinfo(
            Variant::FLOAT,
            param->id,
            PROPERTY_HINT_RANGE,
            String("{0},{1}").format(ary_value),
            PROPERTY_USAGE_DEFAULT
        );

        p_list->push_back(pinfo);
    }

    // Property - PartOpacity
    p_list->push_back(PropertyInfo(Variant::STRING, PROP_PART_OPACITY_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    for(int64_t index = 0; index < this->ary_part_opacity.size(); index++) {
        Ref<GDCubismPartOpacity> param = this->ary_part_opacity[index];

        Array ary_value;
        ary_value.append(0.0);
        ary_value.append(1.0);

        PropertyInfo pinfo(
            Variant::FLOAT,
            param->id,
            PROPERTY_HINT_RANGE,
            String("{0},{1}").format(ary_value),
            PROPERTY_USAGE_DEFAULT
        );

        p_list->push_back(pinfo);
    }
}

void GDCubismUserModel::_enter_tree() {

}

void GDCubismUserModel::_exit_tree() {
    
}

void GDCubismUserModel::_ready() {
    this->cubism_effect_dirty = true;

    // reattach GDCubismModel
    {
        InternalCubismUserModel *internal_model = CSM_NEW InternalCubismUserModel(this);
        internal_model->model_bind();
        this->internal_model = internal_model;
    }
    
    for (int i = 0; i < this->get_child_count(); i++) {
        SubViewport *v = Object::cast_to<SubViewport>(this->get_child(i));
        if (v != nullptr) {
            this->dict_mask[v->get_name()] = v->get_children();
        }
        MeshInstance2D *mesh = Object::cast_to<MeshInstance2D>(this->get_child(i));
        if (mesh != nullptr) {
            this->ary_meshes.append(mesh);
            this->dict_mesh[mesh->get_name()] = mesh;
        }
    }
    
    this->notify_property_list_changed();
}


void GDCubismUserModel::_process(double delta) {
    if(this->is_initialized() == false) return;

    this->_update(delta);
}


void GDCubismUserModel::_on_append_child_act(GDCubismEffect* node) {
    this->_list_cubism_effect.PushBack(node);
    this->cubism_effect_dirty = true;
}


void GDCubismUserModel::_on_remove_child_act(GDCubismEffect* node) {
    for(Csm::csmVector<GDCubismEffect*>::iterator i = this->_list_cubism_effect.Begin(); i != this->_list_cubism_effect.End(); i++) {
        if(*i == node) { this->_list_cubism_effect.Erase(i); break; }
    }
    this->cubism_effect_dirty = true;
}


// ------------------------------------------------------------------ method(s)
