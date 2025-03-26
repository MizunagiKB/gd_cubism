// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/window.hpp>

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
    , enable_load_expressions(true)
    , enable_load_motions(true)
    , speed_scale(1.0)
    , parameter_mode(ParameterMode::FULL_PARAMETER)
    , physics_evaluate(true)
    , pose_update(true)
    , playback_process_mode(MotionProcessCallback::IDLE)
    , anim_loop(DEFAULT_PROP_ANIM_LOOP)
    , anim_loop_fade_in(DEFAULT_PROP_ANIM_LOOP_FADE_IN)
    , cubism_effect_dirty(false)
    , mask_viewport_size(0) {

    this->ary_shader.resize(GD_CUBISM_SHADER_MAX);
}


GDCubismUserModel::~GDCubismUserModel() {}


void GDCubismUserModel::_bind_methods() {

    // csm
    ClassDB::bind_method(D_METHOD("csm_get_version"), &GDCubismUserModel::csm_get_version);
    ClassDB::bind_method(D_METHOD("csm_get_latest_moc_version"), &GDCubismUserModel::csm_get_latest_moc_version);
    ClassDB::bind_method(D_METHOD("csm_get_moc_version"), &GDCubismUserModel::csm_get_moc_version);

    // ModelData
    ClassDB::bind_method(D_METHOD("set_assets", "assets"), &GDCubismUserModel::set_assets);
    ClassDB::bind_method(D_METHOD("get_assets"), &GDCubismUserModel::get_assets);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "assets", PROPERTY_HINT_FILE, "*.model3.json"), "set_assets", "get_assets");

    // Enable Load Expressions
    ClassDB::bind_method(D_METHOD("set_load_expressions", "enable"), &GDCubismUserModel::set_load_expressions);
    ClassDB::bind_method(D_METHOD("get_load_expressions"), &GDCubismUserModel::get_load_expressions);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "load_expressions"), "set_load_expressions", "get_load_expressions");

    // Enable Load Motions
    ClassDB::bind_method(D_METHOD("set_load_motions", "enable"), &GDCubismUserModel::set_load_motions);
    ClassDB::bind_method(D_METHOD("get_load_motions"), &GDCubismUserModel::get_load_motions);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "load_motions"), "set_load_motions", "get_load_motions");

    ClassDB::bind_method(D_METHOD("get_canvas_info"), &GDCubismUserModel::get_canvas_info);

    ClassDB::bind_method(D_METHOD("set_parameter_mode", "value"), &GDCubismUserModel::set_parameter_mode);
    ClassDB::bind_method(D_METHOD("get_parameter_mode"), &GDCubismUserModel::get_parameter_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "parameter_mode", PROPERTY_HINT_ENUM, "FullParameter,NoneParameter"), "set_parameter_mode", "get_parameter_mode");

    ClassDB::bind_method(D_METHOD("set_physics_evaluate", "enable"), &GDCubismUserModel::set_physics_evaluate);
    ClassDB::bind_method(D_METHOD("get_physics_evaluate"), &GDCubismUserModel::get_physics_evaluate);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "physics_evaluate"), "set_physics_evaluate", "get_physics_evaluate");

    ClassDB::bind_method(D_METHOD("set_pose_update", "enable"), &GDCubismUserModel::set_pose_update);
    ClassDB::bind_method(D_METHOD("get_pose_update"), &GDCubismUserModel::get_pose_update);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pose_update"), "set_pose_update", "get_pose_update");

    ClassDB::bind_method(D_METHOD("set_process_callback", "value"), &GDCubismUserModel::set_process_callback);
    ClassDB::bind_method(D_METHOD("get_process_callback"), &GDCubismUserModel::get_process_callback);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "playback_process_mode", PROPERTY_HINT_ENUM, "Physics,Idle,Manual"), "set_process_callback", "get_process_callback");

    ClassDB::bind_method(D_METHOD("set_speed_scale", "value"), &GDCubismUserModel::set_speed_scale);
    ClassDB::bind_method(D_METHOD("get_speed_scale"), &GDCubismUserModel::get_speed_scale);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed_scale", PROPERTY_HINT_RANGE, "0.0,256.0,0.1"), "set_speed_scale", "get_speed_scale");

    ClassDB::bind_method(D_METHOD("set_mask_viewport_size", "value"), &GDCubismUserModel::set_mask_viewport_size);
    ClassDB::bind_method(D_METHOD("get_mask_viewport_size"), &GDCubismUserModel::get_mask_viewport_size);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "mask_viewport_size", PROPERTY_HINT_RANGE, "0,4096"), "set_mask_viewport_size", "get_mask_viewport_size");

    ClassDB::bind_method(D_METHOD("set_shader_add"), &GDCubismUserModel::set_shader_add);
    ClassDB::bind_method(D_METHOD("get_shader_add"), &GDCubismUserModel::get_shader_add);
    ClassDB::bind_method(D_METHOD("set_shader_mix"), &GDCubismUserModel::set_shader_mix);
    ClassDB::bind_method(D_METHOD("get_shader_mix"), &GDCubismUserModel::get_shader_mix);
    ClassDB::bind_method(D_METHOD("set_shader_mul"), &GDCubismUserModel::set_shader_mul);
    ClassDB::bind_method(D_METHOD("get_shader_mul"), &GDCubismUserModel::get_shader_mul);

    ClassDB::bind_method(D_METHOD("set_shader_mask"), &GDCubismUserModel::set_shader_mask);
    ClassDB::bind_method(D_METHOD("get_shader_mask"), &GDCubismUserModel::get_shader_mask);
    // MaskAdd
    ClassDB::bind_method(D_METHOD("set_shader_mask_add"), &GDCubismUserModel::set_shader_mask_add);
    ClassDB::bind_method(D_METHOD("get_shader_mask_add"), &GDCubismUserModel::get_shader_mask_add);
    ClassDB::bind_method(D_METHOD("set_shader_mask_add_inv"), &GDCubismUserModel::set_shader_mask_add_inv);
    ClassDB::bind_method(D_METHOD("get_shader_mask_add_inv"), &GDCubismUserModel::get_shader_mask_add_inv);
    // MaskMix
    ClassDB::bind_method(D_METHOD("set_shader_mask_mix"), &GDCubismUserModel::set_shader_mask_mix);
    ClassDB::bind_method(D_METHOD("get_shader_mask_mix"), &GDCubismUserModel::get_shader_mask_mix);
    ClassDB::bind_method(D_METHOD("set_shader_mask_mix_inv"), &GDCubismUserModel::set_shader_mask_mix_inv);
    ClassDB::bind_method(D_METHOD("get_shader_mask_mix_inv"), &GDCubismUserModel::get_shader_mask_mix_inv);
    // MaskMul
    ClassDB::bind_method(D_METHOD("set_shader_mask_mul"), &GDCubismUserModel::set_shader_mask_mul);
    ClassDB::bind_method(D_METHOD("get_shader_mask_mul"), &GDCubismUserModel::get_shader_mask_mul);
    ClassDB::bind_method(D_METHOD("set_shader_mask_mul_inv"), &GDCubismUserModel::set_shader_mask_mul_inv);
    ClassDB::bind_method(D_METHOD("get_shader_mask_mul_inv"), &GDCubismUserModel::get_shader_mask_mul_inv);

    ADD_GROUP("Shader", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_add", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_add", "get_shader_add");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mix", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mix", "get_shader_mix");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mul", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mul", "get_shader_mul");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask", "get_shader_mask");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_add", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_add", "get_shader_mask_add");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_add_inv", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_add_inv", "get_shader_mask_add_inv");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mix", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mix", "get_shader_mask_mix");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mix_inv", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mix_inv", "get_shader_mask_mix_inv");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mul", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mul", "get_shader_mask_mul");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mul_inv", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mul_inv", "get_shader_mask_mul_inv");

    // CubismMotion
    ClassDB::bind_method(D_METHOD("get_motions"), &GDCubismUserModel::get_motions);
    ClassDB::bind_method(D_METHOD("start_motion", "group", "no", "priority"), &GDCubismUserModel::start_motion);
    ClassDB::bind_method(D_METHOD("start_motion_loop", "group", "no", "priority", "loop", "loop_fade_in"), &GDCubismUserModel::start_motion_loop);
    ClassDB::bind_method(D_METHOD("get_cubism_motion_queue_entries"), &GDCubismUserModel::get_cubism_motion_queue_entries);
    ClassDB::bind_method(D_METHOD("stop_motion"), &GDCubismUserModel::stop_motion);

    // CubismExpression
    ClassDB::bind_method(D_METHOD("get_expressions"), &GDCubismUserModel::get_expressions);
    ClassDB::bind_method(D_METHOD("start_expression", "expression_id"), &GDCubismUserModel::start_expression);
    ClassDB::bind_method(D_METHOD("stop_expression"), &GDCubismUserModel::stop_expression);

    // HitArea
    ClassDB::bind_method(D_METHOD("get_hit_areas"), &GDCubismUserModel::get_hit_areas);

    // Parameter
    ClassDB::bind_method(D_METHOD("get_parameters"), &GDCubismUserModel::get_parameters);

    // PartOpacity
    ClassDB::bind_method(D_METHOD("get_part_opacities"), &GDCubismUserModel::get_part_opacities);

    // Meshs
    ClassDB::bind_method(D_METHOD("get_meshes"), &GDCubismUserModel::get_meshes);

    ClassDB::bind_method(D_METHOD("advance", "delta"), &GDCubismUserModel::advance);

    ADD_SIGNAL(MethodInfo("motion_event", PropertyInfo(Variant::STRING, "value")));
    #ifdef CUBISM_MOTION_CUSTOMDATA
    ADD_SIGNAL(MethodInfo(SIGNAL_MOTION_FINISHED));
    #endif // #ifdef CUBISM_MOTION_CUSTOMDATA

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

    // ParameterMode
    BIND_ENUM_CONSTANT(FULL_PARAMETER);
    BIND_ENUM_CONSTANT(NONE_PARAMETER);

    // MotionProcessCallback
    BIND_ENUM_CONSTANT(PHYSICS);
    BIND_ENUM_CONSTANT(IDLE);
    BIND_ENUM_CONSTANT(MANUAL);
}


void GDCubismUserModel::_notification(int p_what) {
    if (p_what == NOTIFICATION_PREDELETE) {
        this->clear();
        this->ary_shader.clear();
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


GDCubismUserModel::moc3FileFormatVersion GDCubismUserModel::csm_get_latest_moc_version() {
    return static_cast<moc3FileFormatVersion>(Live2D::Cubism::Core::csmGetLatestMocVersion());
}


GDCubismUserModel::moc3FileFormatVersion GDCubismUserModel::csm_get_moc_version() {
    ERR_FAIL_COND_V(this->is_initialized() == false, moc3FileFormatVersion::CSM_MOC_VERSION_UNKNOWN);
    return this->internal_model->_moc3_file_format_version;
}


void GDCubismUserModel::set_assets(const String assets) {
    if (!assets.ends_with(".model3.json")) {
        WARN_PRINT("GDCubismUserModel must point to a Live2D model3.json file");
    }
    if (!FileAccess::file_exists(assets)) {
        WARN_PRINT("Live2D file does not exist, will be unable to initialize model.");
    }
    this->assets = assets;
    this->load_model(assets);
}

String GDCubismUserModel::get_assets() const {
    return this->assets;
}

void GDCubismUserModel::set_load_expressions(const bool enable) { this->enable_load_expressions = enable; }
bool GDCubismUserModel::get_load_expressions() const { return this->enable_load_expressions; }
void GDCubismUserModel::set_load_motions(const bool enable) { this->enable_load_motions = enable; }
bool GDCubismUserModel::get_load_motions() const { return this->enable_load_motions; }


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

void GDCubismUserModel::set_parameter_mode(const ParameterMode value) {
    this->parameter_mode = value;
}


GDCubismUserModel::ParameterMode GDCubismUserModel::get_parameter_mode() const {
    return this->parameter_mode;
}


void GDCubismUserModel::set_process_callback(const MotionProcessCallback value) {
    this->playback_process_mode = value;
}


GDCubismUserModel::MotionProcessCallback GDCubismUserModel::get_process_callback() const {
    return this->playback_process_mode;
}


void GDCubismUserModel::set_speed_scale(const float speed) {
    this->speed_scale = CLAMP<float, float, float>(speed, 0.0, 256.0);
}


float GDCubismUserModel::get_speed_scale() const {
    return this->speed_scale;
}


Dictionary GDCubismUserModel::get_motions() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Dictionary());
    if(this->enable_load_motions == false) return Dictionary();

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Dictionary dict_motion;

    for(Csm::csmInt32 i = 0; i < setting->GetMotionGroupCount(); i++) {
        const Csm::csmChar* group = setting->GetMotionGroupName(i);
        String gd_group; gd_group.parse_utf8(group);
        dict_motion[gd_group] = setting->GetMotionCount(group);
    }

    return dict_motion;
}


Ref<GDCubismMotionQueueEntryHandle> GDCubismUserModel::start_motion(const String str_group, const int32_t no, const Priority priority) {
    return this->start_motion_loop(str_group, no, priority, false, true);
}


Ref<GDCubismMotionQueueEntryHandle> GDCubismUserModel::start_motion_loop(const String str_group, const int32_t no, const Priority priority, const bool loop, const bool loop_fade_in) {
    Ref<GDCubismMotionQueueEntryHandle> queue_handle;
    queue_handle.instantiate();

    if(this->is_initialized() == false) return queue_handle;

    queue_handle->_handle = this->internal_model->motion_start(
        str_group.utf8().ptr(),
        no,
        priority,
        loop,
        loop_fade_in,
        this
    );


    return queue_handle;
}


Array GDCubismUserModel::get_cubism_motion_queue_entries() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Array());

    Array ary_motion_entry;

    Csm::csmVector<Csm::CubismMotionQueueEntry*>* entry_vector_ptr = this->internal_model->_motionManager->GetCubismMotionQueueEntries();

    for(Csm::csmVector<Csm::CubismMotionQueueEntry*>::iterator i = entry_vector_ptr->Begin(); i != entry_vector_ptr->End(); i++) {
        Ref<GDCubismMotionEntry> e;
        e.instantiate();
        e->_entry = *i;
        ary_motion_entry.append(e);
    }

    return ary_motion_entry;
}


void GDCubismUserModel::stop_motion() {
    if(this->is_initialized() == false) return;

    this->internal_model->motion_stop();
}


Array GDCubismUserModel::get_expressions() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Array());
    if(this->enable_load_expressions == false) return Array();

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Array ary_expression;

    for(Csm::csmInt32 i = 0; i < setting->GetExpressionCount(); i++) {
        const Csm::csmChar* name = setting->GetExpressionName(i);
        String gd_name; gd_name.parse_utf8(name);
        ary_expression.append(gd_name);
    }

    return ary_expression;
}


void GDCubismUserModel::start_expression(const String str_expression_id) {
    if(this->is_initialized() == false) return;

    this->internal_model->expression_set(str_expression_id.utf8().ptr());
}


void GDCubismUserModel::stop_expression() {
    if(this->is_initialized() == false) return;

    this->internal_model->expression_stop();
}


Array GDCubismUserModel::get_hit_areas() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Array());

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Array ary_hit_area;

    for(Csm::csmInt32 i = 0; i < setting->GetHitAreasCount(); i++) {
        Dictionary dict_hit_area;
        String id; id.parse_utf8(setting->GetHitAreaId(i)->GetString().GetRawString());
        String name; name.parse_utf8(setting->GetHitAreaName(i));

        dict_hit_area["id"] = id;
        dict_hit_area["name"] = name;
        ary_hit_area.append(dict_hit_area);
    }

    return ary_hit_area;
}


Array GDCubismUserModel::get_parameters() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Array());

    return this->ary_parameter;
}


Array GDCubismUserModel::get_part_opacities() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Array());

    return this->ary_part_opacity;
}


Dictionary GDCubismUserModel::get_meshes() const {
    ERR_FAIL_COND_V(this->is_initialized() == false, Dictionary());

    return this->internal_model->_renderer_resource.dict_mesh;
}


void GDCubismUserModel::on_motion_finished(Csm::ACubismMotion* motion) {
    #ifdef CUBISM_MOTION_CUSTOMDATA
    GDCubismUserModel* m = static_cast<GDCubismUserModel*>(motion->GetFinishedMotionCustomData());
    if(m != nullptr) {
        m->emit_signal(SIGNAL_MOTION_FINISHED);
    }
    #endif // CUBISM_MOTION_CUSTOMDATA
}


void GDCubismUserModel::_update(const double delta) {

    this->internal_model->pro_update(delta * this->speed_scale);

    this->internal_model->efx_update(delta * this->speed_scale);

    for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++ ) {
        Ref<GDCubismParameter> param = this->ary_parameter[index];
        if(param.is_null() != true) {
            if(param->hold == true) {
                param->set_raw_value();
                param->changed = true;
            } else {
                param->set_raw_value();
                param->get_raw_value();
            }
        }
    }

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++ ) {
        Ref<GDCubismPartOpacity> param = this->ary_part_opacity[index];
        if(param.is_null() != true) {
            param->set_raw_value();
            param->get_raw_value();
        }
    }

    this->internal_model->epi_update(delta * this->speed_scale);

    // https://github.com/godotengine/godot/issues/90030
    // https://github.com/godotengine/godot/issues/90017
    #ifdef COUNTERMEASURES_90017_90030
        if(get_window()->is_visible() == true) {
            if(get_window()->get_mode() != Window::Mode::MODE_MINIMIZED) {
                this->internal_model->update_node();
            }
        }
    #else
    this->internal_model->update_node();
    #endif // COUNTERMEASURES_90017_90030
}


void GDCubismUserModel::advance(const double delta) {
    ERR_FAIL_COND(this->is_initialized() == false);
    if(this->playback_process_mode != MANUAL) return;

    this->_update(delta);
}


bool GDCubismUserModel::check_cubism_effect_dirty() const {
    return this->cubism_effect_dirty;
}


void GDCubismUserModel::cubism_effect_dirty_reset() {
    this->cubism_effect_dirty = false;
}


void GDCubismUserModel::setup_property() {
    this->dict_anim_expression.Clear();
    this->dict_anim_motion.Clear();

    if(this->is_initialized() == false) return;
    Csm::CubismModel *model = this->internal_model->GetModel();
    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    // Property - Expression
    if(this->enable_load_expressions == true) {
        for(Csm::csmInt32 i = 0; i < setting->GetExpressionCount(); i++) {
            const Csm::csmChar* expression_id = setting->GetExpressionName(i);
            anim_expression anim_e(expression_id);

            this->dict_anim_expression[anim_e.to_string()] = anim_e;
        }
    }

    // Property - Motion
    if(this->enable_load_motions == true) {
        for(Csm::csmInt32 i = 0; i < setting->GetMotionGroupCount(); i++) {
            const Csm::csmChar* group = setting->GetMotionGroupName(i);
            for(Csm::csmInt32 no = 0; no < setting->GetMotionCount(group); no++) {
                anim_motion anim_m(group, no);

                this->dict_anim_motion[anim_m.to_string()] = anim_m;
            }
        }
    }
}


bool GDCubismUserModel::_set(const StringName &p_name, const Variant &p_value) {
    if(this->is_initialized() == false) return false;
    Csm::CubismModel *model = this->internal_model->GetModel();

    if(p_name == String(PROP_ANIM_EXPRESSION)) {
        this->curr_anim_expression_key = p_value;
        if(this->dict_anim_expression.IsExist(this->curr_anim_expression_key) == true) {
            anim_expression anim_e = this->dict_anim_expression[this->curr_anim_expression_key];
            this->start_expression(anim_e.expression_id);
        }

        return true;
    }

    if(p_name == String(PROP_ANIM_MOTION)) {
        this->curr_anim_motion_key = p_value;
        if(this->dict_anim_motion.IsExist(this->curr_anim_motion_key) == true) {
            anim_motion anim_m = this->dict_anim_motion[this->curr_anim_motion_key];
            this->start_motion_loop(
                anim_m.group,
                anim_m.no,
                Priority::PRIORITY_FORCE,
                this->anim_loop,
                this->anim_loop_fade_in
            );
        }

        return true;
    }

    if(p_name == String(PROP_ANIM_LOOP)) { this->anim_loop = p_value; return true; }
    if(p_name == String(PROP_ANIM_LOOP_FADE_IN)) { this->anim_loop_fade_in = p_value; return true; }

    for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
        String name; name.parse_utf8(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) {
            model->SetParameterValue(index, p_value);
            return true;
        }
    }

    for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
        String name; name.parse_utf8(model->GetPartId(index)->GetString().GetRawString());

        if(p_name == name) {
            model->SetPartOpacity(index, p_value);
            return true;
        }
    }
    
    return false;
}


bool GDCubismUserModel::_get(const StringName &p_name, Variant &r_ret) const {
    if(this->is_initialized() == false) return false;
    Csm::CubismModel *model = this->internal_model->GetModel();

    if(p_name == String(PROP_ANIM_EXPRESSION)) {
        r_ret = this->curr_anim_expression_key;
        return true;
    }

    if(p_name == String(PROP_ANIM_MOTION)) {
        r_ret = this->curr_anim_motion_key;
        return true;
    }

    if(p_name == String(PROP_ANIM_LOOP)) { r_ret = this->anim_loop; return true; }
    if(p_name == String(PROP_ANIM_LOOP_FADE_IN)) { r_ret = this->anim_loop_fade_in; return true; }

    for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
        String name; name.parse_utf8(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) {
            r_ret = model->GetParameterValue(index);
            return true;
        }
    }

    for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
        String name; name.parse_utf8(model->GetPartId(index)->GetString().GetRawString());

        if(p_name == name) {
            r_ret = model->GetPartOpacity(index);
            return true;
        }
    }

    return false;
}


bool GDCubismUserModel::_property_can_revert(const StringName &p_name) const {
    if(this->is_initialized() == false) return false;
    Csm::CubismModel *model = this->internal_model->GetModel();

    if(p_name == String(PROP_ANIM_LOOP)) return true;
    if(p_name == String(PROP_ANIM_LOOP_FADE_IN)) return true;

    for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
        String name; name.parse_utf8(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) return true;
    }

    return false;
}


bool GDCubismUserModel::_property_get_revert(const StringName &p_name, Variant &r_property) const {
    if(this->is_initialized() == false) return false;
    Csm::CubismModel *model = this->internal_model->GetModel();

    if(p_name == String(PROP_ANIM_LOOP)) { r_property = DEFAULT_PROP_ANIM_LOOP; return true; }
    if(p_name == String(PROP_ANIM_LOOP_FADE_IN)) { r_property = DEFAULT_PROP_ANIM_LOOP_FADE_IN; return true; }

    for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
        String name; name.parse_utf8(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) {
            r_property = model->GetParameterDefaultValue(index);
            return true;
        }
    }

    return false;
}


void GDCubismUserModel::_get_property_list(List<godot::PropertyInfo> *p_list) {
    if(this->is_initialized() == false) return;
    Csm::CubismModel *model = this->internal_model->GetModel();
    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;
    PackedStringArray ary_enum;

    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    p_list->push_back(PropertyInfo(Variant::BOOL, PROP_ANIM_LOOP));
    p_list->push_back(PropertyInfo(Variant::BOOL, PROP_ANIM_LOOP_FADE_IN));

    // Property - Expression
    ary_enum.clear();
    if(this->enable_load_expressions == true) {
        for(Csm::csmInt32 i = 0; i < setting->GetExpressionCount(); i++) {
            const Csm::csmChar* expression_id = setting->GetExpressionName(i);
            anim_expression anim_e(expression_id);

            ary_enum.append(anim_e.to_string());
        }
    }

    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_EXPRESSION, PROPERTY_HINT_ENUM, String(",").join(ary_enum)));

    // Property - Motion
    ary_enum.clear();
    if(this->enable_load_motions == true) {
        for(Csm::csmInt32 i = 0; i < setting->GetMotionGroupCount(); i++) {
            const Csm::csmChar* group = setting->GetMotionGroupName(i);
            for(Csm::csmInt32 no = 0; no < setting->GetMotionCount(group); no++) {
                anim_motion anim_m(group, no);

                ary_enum.append(anim_m.to_string());
            }
        }
    }

    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_MOTION, PROPERTY_HINT_ENUM, String(",").join(ary_enum)));

    // Property - Parameter
    p_list->push_back(PropertyInfo(Variant::STRING, PROP_PARAMETER_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++) {
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

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++) {
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

void GDCubismUserModel::clear() {
    if(this->internal_model == nullptr) {
        return;
    }

    this->internal_model->clear();
    CSM_DELETE(this->internal_model);
    this->internal_model = nullptr;
}

void GDCubismUserModel::load_model(const String assets) {
    this->clear();

    if (assets.is_empty()) {
        return;
    }

    Ref<FileAccess> f = FileAccess::open(assets, FileAccess::READ);
    ERR_FAIL_COND_MSG(f.is_null(), "Could not open model path.  Make sure to point to the model3.json");

    this->internal_model = CSM_NEW InternalCubismUserModel(this);

    if(
        this->internal_model->model_load(assets) == false ||
        this->internal_model->IsInitialized() == false
    ) { 
        this->clear();
        return; 
    }

    Csm::CubismModel *model = this->internal_model->GetModel();

    {
        this->ary_parameter.clear();

        for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
            Ref<GDCubismParameter> param;
            param.instantiate();
            param->setup(model, index);
            this->ary_parameter.append(param);
        }
    }

    {
        this->ary_part_opacity.clear();

        for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
            Ref<GDCubismPartOpacity> param;
            param.instantiate();
            param->setup(model, index);
            this->ary_part_opacity.append(param);
        }
    }

    this->cubism_effect_dirty = true;
  
    this->setup_property();
    this->notify_property_list_changed();
}

void GDCubismUserModel::_ready() {
    if (!this->assets.is_empty()) {
        this->load_model(this->assets);
    }
}


void GDCubismUserModel::_enter_tree() {
    if(this->is_initialized() == false) return;
}


void GDCubismUserModel::_exit_tree() {
    if(this->is_initialized() == false) return;
}


void GDCubismUserModel::_process(double delta) {
    if(this->is_initialized() == false) return;
    if(this->playback_process_mode != IDLE) return;

    this->_update(delta);
}


void GDCubismUserModel::_physics_process(double delta) {
    if(this->is_initialized() == false) return;
    if(this->playback_process_mode != PHYSICS) return;

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
