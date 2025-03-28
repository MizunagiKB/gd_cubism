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
#include <private/internal_cubism_renderer_2d.hpp>
#include <gd_cubism_effect_eye_blink.hpp>
#include <gd_cubism_motion_entry.hpp>
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
    , mask_viewport_size(0)
    , physics_evaluate(true)
    , pose_update(true)
    , cubism_effect_dirty(false) {
}


GDCubismUserModel::~GDCubismUserModel() {}


void GDCubismUserModel::_notification(int p_what) {
    if(this->internal_model == nullptr) {
        return;
    }

    // make sure to clear the SDK model when this node is deleted
    if (p_what == NOTIFICATION_PREDELETE) {
        this->effect_term();
        CSM_DELETE(this->internal_model);
        this->internal_model = nullptr;
    }

    // apply mutated parameter values to the model
    if (p_what == NOTIFICATION_PROCESS) {
        
    }
    // reset parameters set by nested animation players and emotions back to their 
    if (p_what == NOTIFICATION_INTERNAL_PROCESS) {

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

void GDCubismUserModel::_update(const double delta) {
    this->effect_batch(delta, EFFECT_CALL_PROLOGUE);

    this->internal_model->GetModel()->GetModelOpacity();

    // update parameters
    {
        for (int i = 0; i < this->ary_parameters.size(); i++) {
            Dictionary param = this->ary_parameters[i];
            String p_name = param["name"];
            int p_id = param["id"];
            float value = this->parameter_values[p_name];

            this->internal_model->GetModel()->SetParameterValue(p_id, value);
        }
    }

    // update opacities
    {
        for (int i = 0; i < this->ary_parts.size(); i++) {
            Dictionary part = this->ary_parts[i];
            String p_name = part["name"];
            int p_id = part["id"];
            float value = this->part_opacity_values[p_name];

            this->internal_model->GetModel()->SetPartOpacity(p_id, value);
        }
    }

    if(this->check_cubism_effect_dirty() == true) {
        this->effect_term();
        this->effect_init();
        this->cubism_effect_dirty_reset();
    }

    this->effect_batch(delta, EFFECT_CALL_PROCESS);

    this->internal_model->GetModel()->Update();

    this->effect_batch(delta, EFFECT_CALL_EPILOGUE);

    InternalCubismRenderer2D::update(
        this->internal_model->GetModel(),
        this->ary_meshes,
        this->ary_masks,
        this->mask_viewport_size
    );
}

void GDCubismUserModel::advance(const double delta) {
    ERR_FAIL_COND(this->is_initialized() == false);
    
    this->_update(delta);
}


bool GDCubismUserModel::check_cubism_effect_dirty() const {
    return this->cubism_effect_dirty;
}


void GDCubismUserModel::cubism_effect_dirty_reset() {
    this->cubism_effect_dirty = false;
}


void GDCubismUserModel::effect_init() {
    for (int i = 0; i < this->_list_cubism_effect.size(); i++) {
        GDCubismEffect* effect = Object::cast_to<GDCubismEffect>(this->_list_cubism_effect[i]);
        effect->_cubism_init(this->internal_model);
    }
}


void GDCubismUserModel::effect_term() {
    for (int i = 0; i < this->_list_cubism_effect.size(); i++) {
        GDCubismEffect* effect = Object::cast_to<GDCubismEffect>(this->_list_cubism_effect[i]);
        effect->_cubism_term(this->internal_model);
    }
}


void GDCubismUserModel::effect_batch(const double delta, const EFFECT_CALL efx_call) {
    for (int i = 0; i < this->_list_cubism_effect.size(); i++) {
        GDCubismEffect* effect = Object::cast_to<GDCubismEffect>(this->_list_cubism_effect[i]);
        switch(efx_call) {
            case EFFECT_CALL_PROLOGUE:  effect->_cubism_prologue(this->internal_model, delta);    break;
            case EFFECT_CALL_PROCESS:   effect->_cubism_process(this->internal_model, delta);     break;
            case EFFECT_CALL_EPILOGUE:  effect->_cubism_epilogue(this->internal_model, delta);    break;
        }
    }
}


bool GDCubismUserModel::_set(const StringName &p_name, const Variant &p_value) {
    if (this->parameters.has(p_name)) {
        this->parameter_values[p_name] = p_value;
        return true;
    }

    if (this->parts.has(p_name)) {
        this->part_opacity_values[p_name] = p_value;
        return true;
    }

    return false;
}


bool GDCubismUserModel::_get(const StringName &p_name, Variant &r_ret) const {
    if (this->parameters.has(p_name)) {
        r_ret = this->parameter_values[p_name];
        return true;
    }

    if (this->parts.has(p_name)) {
        r_ret = this->part_opacity_values[p_name];
        return true;
    }

    return false;
}


bool GDCubismUserModel::_property_can_revert(const StringName &p_name) const {
    if (this->parameters.has(p_name)) return true;
    if (this->parts.has(p_name)) return true;

    return false;
}


bool GDCubismUserModel::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    if (this->parameters.has(p_name)) {
        Dictionary param = this->parameters[p_name];
        r_property = (float)param["default"];
        return true;
    }

    if (this->parts.has(p_name)) {
        Dictionary param = this->parts[p_name];
        r_property = (float)param["default"];
        return true;
    }

    return false;
}

void GDCubismUserModel::_validate_property(PropertyInfo &p_property) const {
	String name = p_property.name;
	if (name == "parameters") {
		p_property.usage = PROPERTY_USAGE_NO_EDITOR;
	}
    if (name == "parts") {
        p_property.usage = PROPERTY_USAGE_NO_EDITOR;
    }
}

void GDCubismUserModel::_get_property_list(List<godot::PropertyInfo> *p_list) {
    // Property - Parameter
    {
        p_list->push_back(PropertyInfo(Variant::STRING, PROP_PARAMETER_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

        Array ary_parameter = this->parameters.values();
        for(int64_t index = 0; index < ary_parameter.size(); index++) {
            Dictionary param = ary_parameter[index];
            String p_name = param["name"];
            
            Array ary_value;
            ary_value.append((float)param["min"]);
            ary_value.append((float)param["max"]);

            PropertyInfo pinfo(
                Variant::FLOAT,
                p_name,
                PROPERTY_HINT_RANGE,
                String("{0},{1}").format(ary_value),
                PROPERTY_USAGE_DEFAULT
            );

            p_list->push_back(pinfo);
        }
    }

    // Property - PartOpacity
    {
        p_list->push_back(PropertyInfo(Variant::STRING, PROP_PART_OPACITY_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

        Array ary_part_opacity = this->parts.values();
        for(int64_t index = 0; index < ary_part_opacity.size(); index++) {
            Dictionary part = ary_part_opacity[index];
            String p_name = part["name"];

            Array ary_value;
            ary_value.append(0.0);
            ary_value.append(1.0);

            PropertyInfo pinfo(
                Variant::FLOAT,
                p_name,
                PROPERTY_HINT_RANGE,
                String("{0},{1}").format(ary_value),
                PROPERTY_USAGE_DEFAULT
            );

            p_list->push_back(pinfo);
        }
    }
}

void GDCubismUserModel::_ready() {
    this->cubism_effect_dirty = true;

    // reattach GDCubismModel
    this->internal_model = CSM_NEW InternalCubismUserModel(this);
    this->internal_model->model_bind();
    
    Node *meshes = this->get_node_or_null(NodePath(MESHES_NODE));
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
    Node *masks = this->get_node_or_null(NodePath(MASKS_NODE));
    if (masks != nullptr) {
        for (int i = 0; i < masks->get_child_count(); i++) {
            SubViewport *mask = Object::cast_to<SubViewport>(masks->get_child(i));
            if (mask == nullptr) continue;

            this->ary_masks.append(mask);
        }
    }
}


void GDCubismUserModel::_process(double delta) {
    if(this->is_initialized() == false) return;

    this->_update(delta);
}


void GDCubismUserModel::_on_append_child_act(GDCubismEffect* node) {
    this->_list_cubism_effect.append(node);
    this->cubism_effect_dirty = true;
}


void GDCubismUserModel::_on_remove_child_act(GDCubismEffect* node) {
    this->_list_cubism_effect.erase(node);
    this->cubism_effect_dirty = true;
}


// ------------------------------------------------------------------ method(s)
