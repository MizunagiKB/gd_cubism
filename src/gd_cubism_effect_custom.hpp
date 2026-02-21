// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_CUSTOM
#define GD_CUBISM_EFFECT_CUSTOM


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
class GDCubismEffectCustom : public GDCubismEffect {
    GDCLASS(GDCubismEffectCustom, GDCubismEffect);

protected:
    static void _bind_methods() {
        ADD_SIGNAL(MethodInfo("cubism_init", PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel")));
        ADD_SIGNAL(MethodInfo("cubism_term", PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel")));

        ADD_SIGNAL(MethodInfo("cubism_prologue", PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel"), PropertyInfo(Variant::FLOAT, "delta")));
        ADD_SIGNAL(MethodInfo("cubism_process", PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel"), PropertyInfo(Variant::FLOAT, "delta")));
        ADD_SIGNAL(MethodInfo("cubism_epilogue", PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel"), PropertyInfo(Variant::FLOAT, "delta")));
    }

public:
    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;

        this->emit_signal("cubism_init", model);

        this->_initialized = true;
    }

    virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

        this->emit_signal("cubism_term", model);

        this->_initialized = false;
    }

    virtual void _cubism_prologue(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
        this->emit_signal("cubism_prologue", model, delta);
    }

    virtual void _cubism_process(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
        this->emit_signal("cubism_process", model, delta);
    }

    virtual void _cubism_epilogue(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
        this->emit_signal("cubism_epilogue", model, delta);
    }
};


#endif // GD_CUBISM_EFFECT_CUSTOM
