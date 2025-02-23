// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Motion/CubismMotion.hpp>

#include <private/internal_cubism_user_model.hpp>
#include <gd_cubism_effect_eye_blink.hpp>
#include <gd_cubism_motion_entry.hpp>
#include <gd_cubism_value_parameter.hpp>
#include <gd_cubism_value_part_opacity.hpp>
#include <gd_cubism_user_model.hpp>
#include <gd_cubism_expression.hpp>

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

Dictionary GDCubismUserModel::get_mesh_dict() const { 
    return this->dict_mesh; 
}

void GDCubismUserModel::_update(const float delta) {
    if (this->get_animation_player() != nullptr) {
        this->get_animation_player()->advance(delta);
    }
    
    Array ary_parameter = this->get_parameters();
    for(Csm::csmInt32 index = 0; index < ary_parameter.size(); index++ ) {
        GDCubismParameter *param = Object::cast_to<GDCubismParameter>(ary_parameter[index]);
        if(param != nullptr) {
            param->apply(this->internal_model->GetModel());
        }
    }

    this->internal_model->pro_update(delta);

    this->internal_model->efx_update(delta);

    Array ary_part_opacity = this->get_part_opacities();
    for(Csm::csmInt32 index = 0; index < ary_part_opacity.size(); index++ ) {
        GDCubismPartOpacity *param = Object::cast_to<GDCubismPartOpacity>(ary_part_opacity[index]);
        if(param != nullptr) {
            param->apply(this->internal_model->GetModel());
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

void GDCubismUserModel::set_active_expression(const String exp) {
    this->curr_anim_expression_key = exp;
    if (!this->is_initialized()) return;
    if (exp == "") {
        this->internal_model->set_expression(nullptr);
        return;
    }

    Ref<GDCubismExpression> r = Object::cast_to<GDCubismExpression>(this->dict_anim_expression[exp]);
    if (r.is_valid()) {
        if (r->internal_expression == nullptr) { r->initialize(); }
        if (r->internal_expression == nullptr) { return; }
        UtilityFunctions::print("activating expression ", exp, r);
        this->internal_model->set_expression(r->internal_expression);
    }
};

bool GDCubismUserModel::_set(const StringName &p_name, const Variant &p_value) {
    Array ary_parameter = this->get_parameters();
    for(Csm::csmInt32 index = 0; index < ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(ary_parameter[index]);
        if(p != nullptr && p->get_name() == p_name) {
            p->set_value(p_value);
            return true;
        }
    }

    Array ary_part_opacity = this->get_part_opacities();
    for(Csm::csmInt32 index = 0; index < ary_part_opacity.size(); index++) {
        GDCubismPartOpacity *p = Object::cast_to<GDCubismPartOpacity>(ary_part_opacity[index]);
        if(p != nullptr && p->get_name() == p_name) {
            p->set_value(p_value);
            return true;
        }
    }
    
    return false;
}


bool GDCubismUserModel::_get(const StringName &p_name, Variant &r_ret) const {
    Array ary_parameter = this->get_parameters();
    for(int64_t index = 0; index < ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(ary_parameter[index]);
        if (p != nullptr && p->get_name() == p_name) {
            r_ret = p->get_value();
            return true;
        }
    }

    Array ary_part_opacity = this->get_part_opacities();
    for(int64_t index = 0; index < ary_part_opacity.size(); index++) {
        GDCubismPartOpacity *p = Object::cast_to<GDCubismPartOpacity>(ary_part_opacity[index]);
        if (p != nullptr && p->get_name() == p_name) {
            r_ret = p->get_value();
            return true;
        }
    }

    return false;
}


bool GDCubismUserModel::_property_can_revert(const StringName &p_name) const {
    Array ary_parameter = this->get_parameters();
    for(int64_t index = 0; index < ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(ary_parameter[index]);
        if (p->get_name() == p_name) return true;
    }

    return false;
}


bool GDCubismUserModel::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    Array ary_parameter = this->get_parameters();
    for(int64_t index = 0; index < ary_parameter.size(); index++) {
        GDCubismParameter *p = Object::cast_to<GDCubismParameter>(ary_parameter[index]);
        if (p->get_name() == p_name) {
            r_property = p->get_default_value();
            return true;
        }
    }

    if (p_name == StringName("active_expression")) {
        r_property = "";
        return true;
    }

    return false;
}

void GDCubismUserModel::_validate_property(PropertyInfo &p_property) const {
	String name = p_property.name;
	// Test hiding the "mouse_filter" property from the editor.
	if (name == "parameters") {
		p_property.usage = PROPERTY_USAGE_NO_EDITOR;
	}
    if (name == "active_expression") {
        p_property.hint = PROPERTY_HINT_ENUM;
        Array choices = this->get_expressions().keys().duplicate();
        p_property.hint_string = String(",").join(choices);
    }
    if (name == "expressions") {
        p_property.usage = PROPERTY_USAGE_NO_EDITOR;
    }
}

void GDCubismUserModel::_get_property_list(List<godot::PropertyInfo> *p_list) {
    // Expressions
    {
        Array expressions = this->dict_anim_expression.keys().duplicate();
        expressions.insert(0, "");
        PropertyInfo pinfo(
            Variant::STRING,
            "active_expression",
            PROPERTY_HINT_ENUM,
            String(",").join(expressions),
            PROPERTY_USAGE_DEFAULT
        );
    }

    // Property - Parameter
    p_list->push_back(PropertyInfo(Variant::STRING, PROP_PARAMETER_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    Array ary_parameter = this->get_parameters();
    for(int64_t index = 0; index < ary_parameter.size(); index++) {
        GDCubismParameter *param = Object::cast_to<GDCubismParameter>(ary_parameter[index]);

        Array ary_value;
        ary_value.append(param->get_minimum_value());
        ary_value.append(param->get_maximum_value());

        PropertyInfo pinfo(
            Variant::FLOAT,
            param->get_name(),
            PROPERTY_HINT_RANGE,
            String("{0},{1}").format(ary_value),
            PROPERTY_USAGE_DEFAULT
        );

        p_list->push_back(pinfo);
    }

    // Property - PartOpacity
    p_list->push_back(PropertyInfo(Variant::STRING, PROP_PART_OPACITY_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    Array ary_part_opacity = this->get_part_opacities();
    for(int64_t index = 0; index < ary_part_opacity.size(); index++) {
        GDCubismPartOpacity *param = Object::cast_to<GDCubismPartOpacity>(ary_part_opacity[index]);

        Array ary_value;
        ary_value.append(0.0);
        ary_value.append(1.0);

        PropertyInfo pinfo(
            Variant::FLOAT,
            param->get_name(),
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
    
    Node *meshes = this->get_node_or_null(NodePath("Meshes"));
    if (meshes != nullptr) {
        for (int i = 0; i < meshes->get_child_count(); i++) {
            MeshInstance2D *mesh = Object::cast_to<MeshInstance2D>(meshes->get_child(i));
            if (mesh == nullptr) continue;

            this->ary_meshes.append(mesh);
            this->dict_mesh[mesh->get_name()] = mesh;

            // reconnect masks to their viewports, which have a tendency to not save properly in packecscenes
            if (!mesh->has_meta("viewport")) continue;

            NodePath viewport_path = mesh->get_meta("viewport");
            SubViewport *viewport = Object::cast_to<SubViewport>(mesh->get_node_or_null(viewport_path));
            if (viewport == nullptr) continue;

            Ref<ShaderMaterial> mat = mesh->get_material();
            mat->set_shader_parameter("tex_mask", viewport->get_texture());
        }   
    }

    Array ary_parameter = this->get_parameters();
    for(int i = 0; i < ary_parameter.size(); i++ ) {
        GDCubismParameter *param = Object::cast_to<GDCubismParameter>(ary_parameter[i]);
        if(param != nullptr) {
            param->apply(this->internal_model->GetModel());
        }
    }

    Array ary_expressions = this->get_expressions().values();
    UtilityFunctions::print(ary_expressions);
    for (int i = 0; i < ary_expressions.size(); i++) {
        GDCubismExpression *exp = Object::cast_to<GDCubismExpression>(ary_expressions[i]);
        if (exp != nullptr) {
            exp->initialize();
        }
    }
    if (!this->get_active_expression().is_empty()) {
        this->set_active_expression(this->get_active_expression());
    }
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
