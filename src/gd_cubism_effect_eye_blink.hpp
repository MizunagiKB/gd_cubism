// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_EYE_BLINK
#define GD_CUBISM_EFFECT_EYE_BLINK


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <gd_cubism_effect.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
class GDCubismEffectEyeBlink : public GDCubismEffect {
    GDCLASS(GDCubismEffectEyeBlink, GDCubismEffect);

protected:
    static void _bind_methods() {}

private:
    Csm::CubismEyeBlink* _eye_blink = nullptr;

public:
    virtual void _cubism_init(InternalCubismUserModel* model) override {
        if(this->_initialized == true) return;

        if(this->_eye_blink == nullptr) {
            Csm::ICubismModelSetting* _model_setting = model->_model_setting;
            if(_model_setting->GetEyeBlinkParameterCount() > 0) {
                this->_eye_blink = Csm::CubismEyeBlink::Create(_model_setting);
            }
        }

        this->_initialized = true;
    }

    virtual void _cubism_term(InternalCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_eye_blink != nullptr) {
            Csm::CubismEyeBlink::Delete(this->_eye_blink);
            this->_eye_blink = nullptr;
        }

        this->_initialized = false;
    }

    virtual void _cubism_process(InternalCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
        if(this->_eye_blink == nullptr) return;

        this->_eye_blink->UpdateParameters(model->GetModel(), delta);
    }
};


#endif // GD_CUBISM_EFFECT_EYE_BLINK
