// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/object.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>

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
    , speed_scale(1.0)
    , auto_scale(true)
    , parameter_mode(ParameterMode::FULL_PARAMETER)
    , playback_process_mode(MotionProcessCallback::IDLE)
    , anim_loop(DEFAULT_PROP_ANIM_LOOP)
    , anim_loop_fade_in(DEFAULT_PROP_ANIM_LOOP_FADE_IN)
    , cubism_effect_dirty(false) {

    this->ary_shader.resize(GD_CUBISM_SHADER_MAX);
}


GDCubismUserModel::~GDCubismUserModel() {
    this->ary_shader.clear();
    this->clear();
}


void GDCubismUserModel::_bind_methods() {

    // csm
    ClassDB::bind_method(D_METHOD("csm_get_version"), &GDCubismUserModel::csm_get_version);
    ClassDB::bind_method(D_METHOD("csm_get_latest_moc_version"), &GDCubismUserModel::csm_get_latest_moc_version);
    ClassDB::bind_method(D_METHOD("csm_get_moc_version"), &GDCubismUserModel::csm_get_moc_version);

    // ModelData
	ClassDB::bind_method(D_METHOD("set_assets", "assets"), &GDCubismUserModel::set_assets);
	ClassDB::bind_method(D_METHOD("get_assets"), &GDCubismUserModel::get_assets);
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "assets", PROPERTY_HINT_FILE, "*.model3.json"), "set_assets", "get_assets");

	ClassDB::bind_method(D_METHOD("get_canvas_info"), &GDCubismUserModel::get_canvas_info);

	ClassDB::bind_method(D_METHOD("clear"), &GDCubismUserModel::clear);

	ClassDB::bind_method(D_METHOD("set_parameter_mode", "value"), &GDCubismUserModel::set_parameter_mode);
	ClassDB::bind_method(D_METHOD("get_parameter_mode"), &GDCubismUserModel::get_parameter_mode);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "parameter_mode", PROPERTY_HINT_ENUM, "FullParameter,NoneParameter"), "set_parameter_mode", "get_parameter_mode");

	ClassDB::bind_method(D_METHOD("set_process_callback", "value"), &GDCubismUserModel::set_process_callback);
	ClassDB::bind_method(D_METHOD("get_process_callback"), &GDCubismUserModel::get_process_callback);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "playback_process_mode", PROPERTY_HINT_ENUM, "Physics,Idle,Manual"), "set_process_callback", "get_process_callback");

	ClassDB::bind_method(D_METHOD("set_speed_scale", "value"), &GDCubismUserModel::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &GDCubismUserModel::get_speed_scale);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed_scale", PROPERTY_HINT_RANGE, "0.0,256.0,0.1"), "set_speed_scale", "get_speed_scale");

	ClassDB::bind_method(D_METHOD("set_auto_scale", "value"), &GDCubismUserModel::set_auto_scale);
	ClassDB::bind_method(D_METHOD("get_auto_scale"), &GDCubismUserModel::get_auto_scale);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_scale"), "set_auto_scale", "get_auto_scale");

	ClassDB::bind_method(D_METHOD("set_shader_mix"), &GDCubismUserModel::set_shader_mix);
	ClassDB::bind_method(D_METHOD("get_shader_mix"), &GDCubismUserModel::get_shader_mix);
	ClassDB::bind_method(D_METHOD("set_shader_mask"), &GDCubismUserModel::set_shader_mask);
	ClassDB::bind_method(D_METHOD("get_shader_mask"), &GDCubismUserModel::get_shader_mask);
	ClassDB::bind_method(D_METHOD("set_shader_mask_mix"), &GDCubismUserModel::set_shader_mask_mix);
	ClassDB::bind_method(D_METHOD("get_shader_mask_mix"), &GDCubismUserModel::get_shader_mask_mix);
	ClassDB::bind_method(D_METHOD("set_shader_mask_mix_inv"), &GDCubismUserModel::set_shader_mask_mix_inv);
	ClassDB::bind_method(D_METHOD("get_shader_mask_mix_inv"), &GDCubismUserModel::get_shader_mask_mix_inv);
    ADD_GROUP("Shader", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mix", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mix", "get_shader_mix");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask", "get_shader_mask");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mix", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mix", "get_shader_mask_mix");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "shader_mask_mix_inv", PROPERTY_HINT_RESOURCE_TYPE, "Shader"), "set_shader_mask_mix_inv", "get_shader_mask_mix_inv");

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
	ClassDB::bind_method(D_METHOD("get_parameter"), &GDCubismUserModel::get_parameter);

    // PartOpacity
	ClassDB::bind_method(D_METHOD("get_part_opacity"), &GDCubismUserModel::get_part_opacity);

	ClassDB::bind_method(D_METHOD("advance", "delta"), &GDCubismUserModel::advance);

	//ADD_SIGNAL(MethodInfo("motion_finished", PropertyInfo(Variant::STRING, "group"), PropertyInfo(Variant::INT, "no")));
	ADD_SIGNAL(MethodInfo("motion_event", PropertyInfo(Variant::STRING, "value")));

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
    if(this->is_initialized() == false) return moc3FileFormatVersion::CSM_MOC_VERSION_UNKNOWN;

    return this->internal_model->_moc3_file_format_version;
}


void GDCubismUserModel::set_assets(const String assets) {
    this->assets = assets;

    Ref<FileAccess> f = FileAccess::open(assets, FileAccess::READ);
    if(f.is_null() == true) { this->clear(); return; }

    this->clear();
    this->internal_model = CSM_NEW InternalCubismUserModel(this, this);

    if(this->internal_model->model_load(assets) == false) { this->clear(); return; }
    if(this->internal_model->IsInitialized() == false) { this->clear(); return; }

    {
        Csm::CubismModel *model = this->internal_model->GetModel();

        this->ary_parameter.clear();

        for(Csm::csmInt32 index = 0; index < model->GetParameterCount(); index++) {
            Ref<GDCubismParameter> param;
            param.instantiate();
            param->setup(model, index);
            this->ary_parameter.append(param);
        }
    }

    {
        Csm::CubismModel *model = this->internal_model->GetModel();

        this->ary_part_opacity.clear();

        for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
            Ref<GDCubismPartOpacity> param;
            param.instantiate();
            param->setup(model, index);
            this->ary_part_opacity.append(param);
        }
    }

    this->notify_property_list_changed();
}


String GDCubismUserModel::get_assets() const {
    return this->assets;
}


Dictionary GDCubismUserModel::get_canvas_info() const {
    if(this->is_initialized() == false) return Dictionary();

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


void GDCubismUserModel::clear() {
    if(this->internal_model != nullptr) {
        this->internal_model->clear();
        CSM_DELETE(this->internal_model);
        this->internal_model = nullptr;
    }
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


void GDCubismUserModel::set_auto_scale(const bool value) {
    this->auto_scale = value;
}


bool GDCubismUserModel::get_auto_scale() const {
    return this->auto_scale;
}


Dictionary GDCubismUserModel::get_motions() const {
    if(this->is_initialized() == false) return Dictionary();

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Dictionary dict_motion;

    for(Csm::csmInt32 i = 0; i < setting->GetMotionGroupCount(); i++) {
        const Csm::csmChar* group = setting->GetMotionGroupName(i);
        dict_motion[String(group)] = setting->GetMotionCount(group);
    }

    return dict_motion;
}


Ref<GDCubismMotionQueueEntryHandle> GDCubismUserModel::start_motion(const String str_group, const int32_t no, const Priority priority) {
    return this->start_motion_loop(str_group, no, priority, false, true);
}


Ref<GDCubismMotionQueueEntryHandle> GDCubismUserModel::start_motion_loop(const String str_group, const int32_t no, const Priority priority, const bool loop, const bool loop_fade_in) {
    Csm::csmChar group[MAX_MOTION_NAME_LENGTH];
    Ref<GDCubismMotionQueueEntryHandle> queue_handle;
    queue_handle.instantiate();

    if(this->is_initialized() == false) return queue_handle;
    if(str_group.length() > MAX_MOTION_NAME_LENGTH) return queue_handle;

    ::memset(group, 0, sizeof(group));
    PackedByteArray buffer = str_group.to_ascii_buffer();
    ::memcpy(group, buffer.ptr(), buffer.size());

    queue_handle->_handle = this->internal_model->motion_start(
        group,
        no,
        priority,
        loop,
        loop_fade_in
    );


    return queue_handle;
}


Array GDCubismUserModel::get_cubism_motion_queue_entries() const {
    if(this->is_initialized() == false) return Array();

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
    if(this->is_initialized() == false) return Array();

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Array ary_expression;

    for(Csm::csmInt32 i = 0; i < setting->GetExpressionCount(); i++) {
        const Csm::csmChar* name = setting->GetExpressionName(i);
        ary_expression.append(String(name));
    }

    return ary_expression;
}


void GDCubismUserModel::start_expression(const String str_expression_id) {
    Csm::csmChar expression_id[MAX_EXPRESSION_NAME_LENGTH];

    if(this->is_initialized() == false) return;

    ::memset(expression_id, 0, sizeof(expression_id));
    PackedByteArray buffer = str_expression_id.to_ascii_buffer();
    ::memcpy(expression_id, buffer.ptr(), buffer.size());

    this->internal_model->expression_set(expression_id);
}


void GDCubismUserModel::stop_expression() {
    if(this->is_initialized() == false) return;

    this->internal_model->expression_stop();
}


Array GDCubismUserModel::get_hit_areas() const {
    if(this->is_initialized() == false) return Array();

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    Array ary_hitarea;

    for(Csm::csmInt32 i = 0; i < setting->GetHitAreasCount(); i++) {
        const Csm::csmChar* name = setting->GetHitAreaName(i);
        ary_hitarea.append(String(name));
    }

    return ary_hitarea;
}


Array GDCubismUserModel::get_parameter() const {
    if(this->is_initialized() == false) return Array();

    return this->ary_parameter;
}


Array GDCubismUserModel::get_part_opacity() const {
    if(this->is_initialized() == false) return Array();

    return this->ary_part_opacity;
}


void GDCubismUserModel::on_motion_finished(Csm::ACubismMotion* motion) {
    #if 0
    Csm::CubismMotion* m = static_cast<Csm::CubismMotion*>(motion);
    InternalCubismUserModel* model = static_cast<InternalCubismUserModel*>(m->_userData);

    if(model != nullptr && model->_owner_viewport != nullptr) {
        model->_owner_viewport->emit_signal("motion_finished", String(), 0);
    }
    #endif
}


void GDCubismUserModel::_update(const float delta) {
    if(this->parameter_mode == NONE_PARAMETER) {
        for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++ ) {
            Ref<GDCubismParameter> param = this->ary_parameter[index];
            if(param.is_null() != true) param->set_raw_value();
        }
    }

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++ ) {
        Ref<GDCubismPartOpacity> param = this->ary_part_opacity[index];
        if(param.is_null() != true) param->set_raw_value();
    }

    this->internal_model->update(delta * this->speed_scale);
    this->internal_model->update_node();

    for(Csm::csmInt32 index = 0; index < this->ary_part_opacity.size(); index++ ) {
        Ref<GDCubismPartOpacity> param = this->ary_part_opacity[index];
        if(param.is_null() != true) param->get_raw_value();
    }

    for(Csm::csmInt32 index = 0; index < this->ary_parameter.size(); index++ ) {
        Ref<GDCubismParameter> param = this->ary_parameter[index];
        if(param.is_null() != true) param->get_raw_value();
    }
}


void GDCubismUserModel::advance(const float delta) {
    if(this->is_initialized() == false) return;
    if(this->playback_process_mode != MANUAL) return;

    this->_update(delta);
}


bool GDCubismUserModel::check_cubism_effect_dirty() const {
    return this->cubism_effect_dirty;
}


void GDCubismUserModel::cubism_effect_dirty_reset() {
    this->cubism_effect_dirty = false;
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
        const String name(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) {
            model->SetParameterValue(index, p_value);
            return true;
        }
    }

    for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
        const String name(model->GetPartId(index)->GetString().GetRawString());

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
        const String name(model->GetParameterId(index)->GetString().GetRawString());

        if(p_name == name) {
            r_ret = model->GetParameterValue(index);
            return true;
        }
    }

    for(Csm::csmInt32 index = 0; index < model->GetPartCount(); index++) {
        const String name(model->GetPartId(index)->GetString().GetRawString());

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
        const String name(model->GetParameterId(index)->GetString().GetRawString());

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
        const String name(model->GetParameterId(index)->GetString().GetRawString());

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


    //


    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_GROUP, PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

    Csm::ICubismModelSetting* setting = this->internal_model->_model_setting;

    PackedStringArray ary_enum;

    this->dict_anim_expression.Clear();
    ary_enum.clear();
    for(Csm::csmInt32 i = 0; i < setting->GetExpressionCount(); i++) {
        const Csm::csmChar* expression_id = setting->GetExpressionName(i);
        anim_expression anim_e(expression_id);

        ary_enum.append(anim_e.to_string());
        this->dict_anim_expression[anim_e.to_string()] = anim_e;
    }

    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_EXPRESSION, PROPERTY_HINT_ENUM, String(",").join(ary_enum)));


    this->dict_anim_motion.Clear();
    ary_enum.clear();
    for(Csm::csmInt32 i = 0; i < setting->GetMotionGroupCount(); i++) {
        const Csm::csmChar* group = setting->GetMotionGroupName(i);
        for(Csm::csmInt32 no = 0; no < setting->GetMotionCount(group); no++) {
            anim_motion anim_m(group, no);

            ary_enum.append(anim_m.to_string());
            this->dict_anim_motion[anim_m.to_string()] = anim_m;
        }
    }

    p_list->push_back(PropertyInfo(Variant::STRING, PROP_ANIM_MOTION, PROPERTY_HINT_ENUM, String(",").join(ary_enum)));

    p_list->push_back(PropertyInfo(Variant::BOOL, PROP_ANIM_LOOP));
    p_list->push_back(PropertyInfo(Variant::BOOL, PROP_ANIM_LOOP_FADE_IN));

    //


    p_list->push_back(PropertyInfo(Variant::STRING, "Parameter", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

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

    p_list->push_back(PropertyInfo(Variant::STRING, "PartOpacity", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

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


void GDCubismUserModel::_ready() {

    // Setup SubViewport
    this->set_clear_mode(SubViewport::CLEAR_MODE_ALWAYS);

    this->set_disable_3d(SUBVIEWPORT_DISABLE_3D_FLAG);
    this->set_clear_mode(SubViewport::ClearMode::CLEAR_MODE_ALWAYS);
    this->set_update_mode(SubViewport::UpdateMode::UPDATE_ALWAYS);
    this->set_disable_input(true);
    this->set_use_own_world_3d(true);
    this->set_transparent_background(true);
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
    Csm::CubismModel *model = this->internal_model->GetModel();

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
