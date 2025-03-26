// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>

#ifdef GD_CUBISM_USE_RENDERER_2D
    #include <private/internal_cubism_renderer_2d.hpp>
#else
    #include <private/internal_cubism_renderer_3d.hpp>
#endif // GD_CUBISM_USE_RENDERER_2D
#include <private/internal_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismUserModel::InternalCubismUserModel(GDCubismUserModel *owner_viewport)
    : CubismUserModel()
    , _moc3_file_format_version(GDCubismUserModel::moc3FileFormatVersion::CSM_MOC_VERSION_UNKNOWN)
    , _renderer_resource(owner_viewport)
    , _owner_viewport(owner_viewport)
    , _model_pathname("")
    , _model_setting(nullptr) {

    _debugMode = false;
}


InternalCubismUserModel::~InternalCubismUserModel() {
    this->clear();
}


bool InternalCubismUserModel::model_load(
    const String &model_pathname
) {

    this->_model_pathname = model_pathname;
    this->_updating = true;
    this->_initialized = false;
    this->_model_setting = nullptr;

    PackedByteArray buffer = FileAccess::get_file_as_bytes(this->_model_pathname);
    if(buffer.size() == 0) return false;

    this->clear();

    this->_model_setting = CSM_NEW CubismModelSettingJson(buffer.ptr(), buffer.size());

    // setup Live2D model
    if (strcmp(this->_model_setting->GetModelFileName(), "") == 0) {
        return false;
    } else {
        String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetModelFileName());
        String moc3_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

        buffer = FileAccess::get_file_as_bytes(moc3_pathname);
        this->LoadModel(buffer.ptr(), buffer.size());

        const Live2D::Cubism::Core::csmVersion version = Live2D::Cubism::Core::csmGetMocVersion(buffer.ptr(), buffer.size());
        this->_moc3_file_format_version = static_cast<GDCubismUserModel::moc3FileFormatVersion>(version);
    }

    // Expression
    if(this->_owner_viewport->enable_load_expressions == true) {
        this->expression_load();
    }

    // Physics
    this->physics_load();
    // Pose
    this->pose_load();
    //UserData
    this->userdata_load();

    // EyeBlink(Parameters)
    {
        Csm::csmInt32 param_count = this->_model_setting->GetEyeBlinkParameterCount();
        for(Csm::csmInt32 i = 0; i < param_count; ++i)
        {
            this->_list_eye_blink.PushBack(this->_model_setting->GetEyeBlinkParameterId(i));
        }
    }

    // LipSync(Parameters)
    {
        Csm::csmInt32 param_count = this->_model_setting->GetLipSyncParameterCount();
        for(Csm::csmInt32 i = 0; i < param_count; ++i)
        {
            this->_list_lipsync.PushBack(this->_model_setting->GetLipSyncParameterId(i));
        }
    }

    if(this->_model_setting == nullptr || this->_modelMatrix == nullptr) {
        this->clear();
        return false;
    }

    this->_model->SaveParameters();

    // Motion
    if(this->_owner_viewport->enable_load_motions == true) {
        this->motion_load();
    }

    this->CreateRenderer();

    // Resource(Texture)
    this->model_load_resource();

    this->stop();

    this->_updating = false;
    this->_initialized = true;

    // ------------------------------------------------------------------------
    // The process to make the mesh available immediately after initialization.
    // The process is almost the same as the InternalCubismUserModel::update_node() function.
    {
        #ifdef GD_CUBISM_USE_RENDERER_2D
        InternalCubismRenderer2D* renderer = this->GetRenderer<InternalCubismRenderer2D>();
        #else
        #endif // GD_CUBISM_USE_RENDERER_2D

        renderer->IsPremultipliedAlpha(false);
        renderer->DrawModel();
        renderer->build_model(this->_renderer_resource, this->_owner_viewport);
    }
    // ------------------------------------------------------------------------

    return true;
}


void InternalCubismUserModel::model_load_resource()
{
    ResourceLoader *res_loader = ResourceLoader::get_singleton();

    this->_renderer_resource.ary_texture.clear();

    for (csmInt32 index = 0; index < this->_model_setting->GetTextureCount(); index++)
    {
        if (strcmp(this->_model_setting->GetTextureFileName(index), "") == 0) continue;

        String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetTextureFileName(index));
        String texture_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

        Ref<Texture2D> tex;
        // allow dynamically loading image textures for models provided from disk or user data
        if (!res_loader->exists(texture_pathname)) {
            Ref<Image> img = Image::load_from_file(texture_pathname);
            tex = ImageTexture::create_from_image(img);
            tex->take_over_path(texture_pathname);
        } else {
            tex = res_loader->load(texture_pathname);
        }

        this->_renderer_resource.ary_texture.append(tex);
    }
}


void InternalCubismUserModel::pro_update(const double delta) {
    if(this->IsInitialized() == false) return;
    if(this->_model_setting == nullptr) return;
    if(this->_model == nullptr) return;

    this->effect_batch(delta, EFFECT_CALL_PROLOGUE);

    if(this->_owner_viewport->parameter_mode == GDCubismUserModel::ParameterMode::FULL_PARAMETER) {
        this->_model->LoadParameters();
        this->_motionManager->UpdateMotion(this->_model, delta);
        this->_model->SaveParameters();
    }

    if(this->_expressionManager != nullptr) {
        this->_expressionManager->UpdateMotion(this->_model, delta);
    }

    this->_model->GetModelOpacity();
}


void InternalCubismUserModel::efx_update(const double delta) {
    if(this->IsInitialized() == false) return;
    if(this->_model_setting == nullptr) return;
    if(this->_model == nullptr) return;

    if(this->_owner_viewport->check_cubism_effect_dirty() == true) {
        this->effect_term();
        this->effect_init();
        this->_owner_viewport->cubism_effect_dirty_reset();
    }

    this->effect_batch(delta, EFFECT_CALL_PROCESS);
}


void InternalCubismUserModel::epi_update(const double delta) {
    if(this->IsInitialized() == false) return;
    if(this->_model_setting == nullptr) return;
    if(this->_model == nullptr) return;

    if(this->_owner_viewport->physics_evaluate == true) {
        if(this->_physics != nullptr) { this->_physics->Evaluate(this->_model, delta); }
    }

    if(this->_owner_viewport->pose_update == true) {
        if(this->_pose != nullptr) { this->_pose->UpdateParameters(this->_model, delta); }
    }

    this->_model->Update();
    this->effect_batch(delta, EFFECT_CALL_EPILOGUE);
}


void InternalCubismUserModel::update_node() {
    if(this->IsInitialized() == false) return;

    #ifdef GD_CUBISM_USE_RENDERER_2D
    InternalCubismRenderer2D* renderer = this->GetRenderer<InternalCubismRenderer2D>();
    #else
    #endif // GD_CUBISM_USE_RENDERER_2D

    renderer->IsPremultipliedAlpha(false);
    renderer->DrawModel();
    renderer->update(this->_renderer_resource, this->_owner_viewport->mask_viewport_size);
}


void InternalCubismUserModel::clear() {

    this->DeleteRenderer();

    this->_renderer_resource.clear();

    {
        this->expression_stop();
        for(csmMap<csmString,CubismExpressionMotion*>::const_iterator i = this->_map_expression.Begin(); i != this->_map_expression.End(); i++) {
            ACubismMotion::Delete(i->Second);
        }
        this->_map_expression.Clear();
    }

    {
        this->motion_stop();
        for(csmMap<csmString,CubismMotion*>::const_iterator i = this->_map_motion.Begin(); i != this->_map_motion.End(); i++) {
            ACubismMotion::Delete(i->Second);
        }
        this->_map_motion.Clear();
    }

    this->effect_term();

    this->_list_eye_blink.Clear();
    this->_list_lipsync.Clear();

    if(this->_model_setting != nullptr) {
        this->_initialized = false;

        CSM_DELETE(this->_model_setting);
        this->_model_setting = nullptr;
    }
}


void InternalCubismUserModel::stop() {
    this->expression_stop();
    this->motion_stop();
}


void InternalCubismUserModel::expression_set(const char* expression_id) {
    csmString id = expression_id;

    ACubismMotion* motion = this->_map_expression[csmString(expression_id)];

    if(motion != nullptr) {
        this->_expressionManager->StartMotionPriority(
            motion,
            false,
            GDCubismUserModel::Priority::PRIORITY_FORCE
        );
    }
}


void InternalCubismUserModel::expression_stop() {
    if(this->_expressionManager == nullptr) return;
    this->_expressionManager->StopAllMotions();
}


CubismMotionQueueEntryHandle InternalCubismUserModel::motion_start(const char* group, const int32_t no, const int32_t priority, const bool loop, const bool loop_fade_in, void* custom_data) {

    if (priority == GDCubismUserModel::Priority::PRIORITY_FORCE) {
        this->_motionManager->SetReservePriority(priority);
    } else if (!this->_motionManager->ReserveMotion(priority)) {
        return InvalidMotionQueueEntryHandleValue;
    }

    csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, no);

    CubismMotion* motion = this->_map_motion[name];

    if(motion == nullptr ) return InvalidMotionQueueEntryHandleValue;

    motion->IsLoop(loop);
    motion->IsLoopFadeIn(loop_fade_in);
    motion->SetFinishedMotionHandler(GDCubismUserModel::on_motion_finished);
    #ifdef CUBISM_MOTION_CUSTOMDATA
    motion->SetFinishedMotionCustomData(custom_data);
    #endif // CUBISM_MOTION_CUSTOMDATA

    return this->_motionManager->StartMotionPriority(motion, false, priority);
}


void InternalCubismUserModel::motion_stop() {
    if(this->_motionManager == nullptr) return;
    this->_motionManager->StopAllMotions();
}


void InternalCubismUserModel::MotionEventFired(const csmString& eventValue) {
    if(this->_owner_viewport != nullptr) {
        String value; value.parse_utf8(eventValue.GetRawString());
        this->_owner_viewport->emit_signal("motion_event", value);
    }
}


void InternalCubismUserModel::expression_load() {
    if(this->_model_setting == nullptr) return;
    if(this->_model_setting->GetExpressionCount() == 0) return;

    for (csmInt32 i = 0; i < this->_model_setting->GetExpressionCount(); i++)
    {
        csmString name = this->_model_setting->GetExpressionName(i);

        String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetExpressionFileName(i));
        String expression_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

        PackedByteArray buffer = FileAccess::get_file_as_bytes(expression_pathname);
        CubismExpressionMotion* motion = static_cast<CubismExpressionMotion*>(this->LoadExpression(
            buffer.ptr(),
            buffer.size(),
            this->_model_setting->GetExpressionName(i)
        ));

        if(this->_map_expression[name] != nullptr) {
            ACubismMotion::Delete(this->_map_expression[name]);
            this->_map_expression[name] = nullptr;
        }

        this->_map_expression[name] = motion;
    }
}


void InternalCubismUserModel::physics_load() {
    if(strcmp(this->_model_setting->GetPhysicsFileName(), "") == 0) return;

    String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetPhysicsFileName());
    String physics_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

    PackedByteArray buffer = FileAccess::get_file_as_bytes(physics_pathname);
    if(buffer.size() > 0) {
        this->LoadPhysics(buffer.ptr(), buffer.size());
    }
}


void InternalCubismUserModel::pose_load() {
    if(strcmp(this->_model_setting->GetPoseFileName(), "") == 0) return;

    String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetPoseFileName());
    String pose_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

    PackedByteArray buffer = FileAccess::get_file_as_bytes(pose_pathname);
    if(buffer.size() > 0) {
        this->LoadPose(buffer.ptr(), buffer.size());
    }
}


void InternalCubismUserModel::userdata_load() {
    if(strcmp(this->_model_setting->GetUserDataFile(), "") == 0) return;

    String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetUserDataFile());
    String userdata_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

    PackedByteArray buffer = FileAccess::get_file_as_bytes(userdata_pathname);
    if(buffer.size() > 0) {
        this->LoadUserData(buffer.ptr(), buffer.size());
    }
}


void InternalCubismUserModel::motion_load() {
    if(this->_model_setting == nullptr) return;
    if(this->_model_setting->GetMotionGroupCount() == 0) return;

    for (csmInt32 ig = 0; ig < this->_model_setting->GetMotionGroupCount(); ig++)
    {
        //PreloadMotionGroup(group);
        const csmChar* group = this->_model_setting->GetMotionGroupName(ig);
        const csmInt32 motion_count = this->_model_setting->GetMotionCount(group);

        if(motion_count == 0) continue;

        for (csmInt32 im = 0; im < motion_count; im++)
        {
            csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, im);

            String gd_filename; gd_filename.parse_utf8(this->_model_setting->GetMotionFileName(group, im));
            String motion_pathname = this->_model_pathname.get_base_dir().path_join(gd_filename);

            PackedByteArray buffer = FileAccess::get_file_as_bytes(motion_pathname);
            CubismMotion* motion = static_cast<CubismMotion*>(this->LoadMotion(
                buffer.ptr(),
                buffer.size(),
                name.GetRawString()
            ));

            csmFloat32 fade_time_sec = this->_model_setting->GetMotionFadeInTimeValue(group, im);
            if (fade_time_sec >= 0.0f) {
                motion->SetFadeInTime(fade_time_sec);
            }

            fade_time_sec = this->_model_setting->GetMotionFadeOutTimeValue(group, im);
            if (fade_time_sec >= 0.0f) {
                motion->SetFadeOutTime(fade_time_sec);
            }
            static_cast<CubismMotion*>(motion)->SetEffectIds(this->_list_eye_blink, this->_list_lipsync);

            if(this->_map_motion[name] != nullptr) {
                ACubismMotion::Delete(this->_map_motion[name]);
                this->_map_motion[name] = nullptr;
            }

            this->_map_motion[name] = motion;
        }
    }
}


void InternalCubismUserModel::effect_init() {
    for(
        Csm::csmVector<GDCubismEffect*>::iterator i = this->_owner_viewport->_list_cubism_effect.Begin();
        i != this->_owner_viewport->_list_cubism_effect.End();
        i++
    ) {
        (*i)->_cubism_init(this);
    }
}


void InternalCubismUserModel::effect_term() {
    for(
        Csm::csmVector<GDCubismEffect*>::iterator i = this->_owner_viewport->_list_cubism_effect.Begin();
        i != this->_owner_viewport->_list_cubism_effect.End();
        i++
    ) {
        (*i)->_cubism_term(this);
    }
}


void InternalCubismUserModel::effect_batch(const double delta, const EFFECT_CALL efx_call) {
    for(
        Csm::csmVector<GDCubismEffect*>::iterator i = this->_owner_viewport->_list_cubism_effect.Begin();
        i != this->_owner_viewport->_list_cubism_effect.End();
        i++
    ) {
        switch(efx_call) {
            case EFFECT_CALL_PROLOGUE:  (*i)->_cubism_prologue(this, delta);    break;
            case EFFECT_CALL_PROCESS:   (*i)->_cubism_process(this, delta);     break;
            case EFFECT_CALL_EPILOGUE:  (*i)->_cubism_epilogue(this, delta);    break;
        }
    }
}


// ------------------------------------------------------------------ method(s)
