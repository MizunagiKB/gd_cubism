// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef INTERNAL_CUBISM_USER_MODEL
#define INTERNAL_CUBISM_USER_MODEL


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <Model/CubismUserModel.hpp>
#include <Motion/CubismMotion.hpp>
#include <Motion/CubismMotionQueueManager.hpp>
#include <Utils/CubismString.hpp>
#include <CubismFramework.hpp>
#include <CubismModelSettingJson.hpp>

#include <private/internal_cubism_renderer_resource.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class GDCubismEffectBreath;
class GDCubismEffectCustom;
class GDCubismEffectEyeBlink;
class GDCubismEffectHitArea;


// ------------------------------------------------------------------- class(s)
class InternalCubismUserModel : public Csm::CubismUserModel {
    friend GDCubismUserModel;
    friend GDCubismEffectBreath;
    friend GDCubismEffectCustom;
    friend GDCubismEffectEyeBlink;
    friend GDCubismEffectHitArea;

    enum EFFECT_CALL {
        EFFECT_CALL_PROLOGUE,
        EFFECT_CALL_PROCESS,
        EFFECT_CALL_EPILOGUE
    };

public:
    InternalCubismUserModel(GDCubismUserModel *owner_viewport);
    virtual ~InternalCubismUserModel();

public:
    GDCubismUserModel *_owner_viewport = nullptr;

private:
    InternalCubismRendererResource _renderer_resource;
    GDCubismUserModel::moc3FileFormatVersion _moc3_file_format_version;
    String _model_pathname;
    Csm::ICubismModelSetting* _model_setting;
    Csm::csmVector<Csm::CubismIdHandle> _list_eye_blink;
    Csm::csmVector<Csm::CubismIdHandle> _list_lipsync;
    Csm::csmMap<Csm::csmString,Csm::CubismExpressionMotion*> _map_expression;
    Csm::csmMap<Csm::csmString,Csm::CubismMotion*> _map_motion;

public:
    bool model_load(const String &model_pathname);
    void model_load_resource();
    void pro_update(const double delta);
    void efx_update(const double delta);
    void epi_update(const double delta);
    void update_node();
    void clear();

    void stop();

    void expression_set(const char* expression_id);
    void expression_stop();

    Csm::CubismMotionQueueEntryHandle motion_start(const char* group, const int32_t no, const int32_t priority, const bool loop, const bool loop_fade_in, void* custom_data);
    void motion_stop();

    virtual void MotionEventFired(const Csm::csmString& eventValue) override;

private:
    void expression_load();
    void physics_load();
    void pose_load();
    void userdata_load();
    void motion_load();

    void effect_init();
    void effect_term();
    void effect_batch(const double delta, const EFFECT_CALL efx_call);
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_USER_MODEL
