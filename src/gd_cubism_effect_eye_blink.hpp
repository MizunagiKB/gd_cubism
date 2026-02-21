// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_EYE_BLINK
#define GD_CUBISM_EFFECT_EYE_BLINK


// ----------------------------------------------------------------- include(s)
#include <Effect/CubismEyeBlink.hpp>

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
    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;
        if(this->_eye_blink != nullptr) return;

        ICubismModelSetting *model_settings = model->get_model_settings();
        if (model_settings->GetEyeBlinkParameterCount() > 0) {
            this->_eye_blink = Csm::CubismEyeBlink::Create(model_settings);
        }

        this->_initialized = true;
    }

    virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_eye_blink != nullptr) {
            Csm::CubismEyeBlink::Delete(this->_eye_blink);
            this->_eye_blink = nullptr;
        }

        this->_initialized = false;
    }

    virtual void _cubism_process(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
        if(this->_eye_blink == nullptr) return;

        this->_eye_blink->UpdateParameters(model->get_internal_model(), delta);
    }
};


#endif // GD_CUBISM_EFFECT_EYE_BLINK
