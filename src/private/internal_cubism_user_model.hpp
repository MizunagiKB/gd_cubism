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
#include <ICubismModelSetting.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class GDCubismUserModel;
class GDCubismEffect;
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

    static Vector2 get_size(const Csm::CubismModel *model);
    static Vector2 get_origin(const Csm::CubismModel *model);
    static float get_ppunit(const Csm::CubismModel *model);

public:
    GDCubismUserModel *_owner_viewport = nullptr;
    
private:
    ICubismModelSetting *model_settings;
    Array _list_eye_blink;
    Array _list_lipsync;
    Array ary_hit_areas;

public:
    bool model_bind();
    ICubismModelSetting* get_model_settings() {
        if (!this->IsInitialized()) return nullptr;
        return this->model_settings;
    };
    void pro_update(const float delta);
    void efx_update(const float delta);
    void epi_update(const float delta);
    void update_node();
    void clear();

private:
    void effect_init();
    void effect_term();
    void effect_batch(const float delta, const EFFECT_CALL efx_call);
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_USER_MODEL
