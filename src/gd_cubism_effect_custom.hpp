// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_CUSTOM
#define GD_CUBISM_EFFECT_CUSTOM


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <gd_cubism_effect.hpp>
#include <private/internal_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
class GDCubismEffectCustom : public GDCubismEffect {
    GDCLASS(GDCubismEffectCustom, GDCubismEffect);

protected:
    static void _bind_methods() {}

public:
    virtual void _cubism_init(InternalCubismUserModel* model) override {
        call("_cubism_init", model->_owner_viewport);
    }

    virtual void _cubism_term(InternalCubismUserModel* model) override {
        call("_cubism_term", model->_owner_viewport);
    }

    virtual void _cubism_process(InternalCubismUserModel* model, const float delta) override {
        if(this->_active == false) return;
        call("_cubism_process", model->_owner_viewport, delta);
    }
};


#endif // GD_CUBISM_EFFECT_CUSTOM
