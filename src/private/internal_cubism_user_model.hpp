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

public:
    InternalCubismUserModel(GDCubismUserModel *owner_viewport);
    virtual ~InternalCubismUserModel();

    static Vector2 get_size(const Csm::CubismModel *model);
    static Vector2 get_origin(const Csm::CubismModel *model);
    static float get_ppunit(const Csm::CubismModel *model);

public:
    GDCubismUserModel *_owner_viewport = nullptr;
    
private:
    String _model_pathname;
    ICubismModelSetting *model_settings;
    
public:
    bool model_bind();
    ICubismModelSetting* get_model_settings() {
        if (!this->IsInitialized()) return nullptr;
        return this->model_settings;
    };
    String get_model_path() {
        return this->_model_pathname;
    }
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_USER_MODEL
