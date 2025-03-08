// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_VALUE_PARAM_H
#define GD_CUBISM_VALUE_PARAM_H
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/classes/resource.hpp>

#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotionManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>

#include <gd_cubism_value_abs.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism;
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;
class GDCubismUserModel;


// ------------------------------------------------------------------- class(s)
class GDCubismParameter : public GDCubismValueAbs {
    GDCLASS(GDCubismParameter, GDCubismValueAbs)
    friend GDCubismUserModel;

public:
    enum Type {
        NORMAL = 0,
        BLEND_SHAPE = 1
    };

private:
    GDCubismParameter::Type type;
    bool hold;

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_type"), &GDCubismParameter::get_type);
        ClassDB::bind_method(D_METHOD("set_type"), &GDCubismParameter::set_type);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "type", PROPERTY_HINT_ENUM, "Normal,BlendShape"), "set_type", "get_type");

        ClassDB::bind_method(D_METHOD("set_hold"), &GDCubismParameter::set_hold);
        ClassDB::bind_method(D_METHOD("get_hold"), &GDCubismParameter::get_hold);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "hold"), "set_hold", "get_hold");

        ClassDB::bind_method(D_METHOD("reset"), &GDCubismParameter::reset);

        BIND_ENUM_CONSTANT(NORMAL);
        BIND_ENUM_CONSTANT(BLEND_SHAPE);
    }

public:
    void setup(Csm::CubismModel *model) override {
        Core::csmModel *csm_model = model->GetModel();

        int32_t index = this->get_index();
        this->set_name(String(Core::csmGetParameterIds(csm_model)[index]));
        this->value = Core::csmGetParameterValues(csm_model)[index];

        this->type = static_cast<Type>(Core::csmGetParameterTypes(csm_model)[index]);
        this->minimum_value = Core::csmGetParameterMinimumValues(csm_model)[index];
        this->maximum_value = Core::csmGetParameterMaximumValues(csm_model)[index];
        this->default_value = Core::csmGetParameterDefaultValues(csm_model)[index];
        this->hold = false;
    }

    void apply(Csm::CubismModel *model) {
        if (this->get_index() < 0) return;
        model->SetParameterValue(this->get_index(), this->value);
    }

    GDCubismParameter()
        : GDCubismValueAbs(GDCubismValueAbs::ValueType::PARAMETER) {}

    void reset() { this->set_value(this->default_value); this->hold = false; }
    
    GDCubismParameter::Type get_type() const { return this->type; }

    void set_type(GDCubismParameter::Type type) { this->type = type; }

    void set_hold(const bool value) { this->hold = value; }
    bool get_hold() const { return this->hold; }
};

VARIANT_ENUM_CAST(GDCubismParameter::Type);


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_VALUE_PARAM_H
