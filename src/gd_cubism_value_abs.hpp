// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_VALUE_ABS_H
#define GD_CUBISM_VALUE_ABS_H
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/classes/resource.hpp>

#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotionManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>


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
class GDCubismValueAbs : public godot::Node {
    GDCLASS(GDCubismValueAbs, godot::Node)
    friend GDCubismUserModel;

public:
    enum ValueType {
        UNKONWN = 0,
        PARAMETER = 1,
        PART_OPACITY = 2
    };

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("set_value"), &GDCubismValueAbs::set_value);
        ClassDB::bind_method(D_METHOD("get_value"), &GDCubismValueAbs::get_value);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "value"), "set_value", "get_value");

        ClassDB::bind_method(D_METHOD("get_minimum_value"), &GDCubismValueAbs::get_minimum_value);
        ClassDB::bind_method(D_METHOD("set_minimum_value"), &GDCubismValueAbs::set_minimum_value);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "minimum_value"), "set_minimum_value", "get_minimum_value");
        ClassDB::bind_method(D_METHOD("get_maximum_value"), &GDCubismValueAbs::get_maximum_value);
        ClassDB::bind_method(D_METHOD("set_maximum_value"), &GDCubismValueAbs::set_maximum_value);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "maximum_value"), "set_maximum_value", "get_maximum_value");
        ClassDB::bind_method(D_METHOD("get_default_value"), &GDCubismValueAbs::get_default_value);
        ClassDB::bind_method(D_METHOD("set_default_value"), &GDCubismValueAbs::set_default_value);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "default_value"), "set_default_value", "get_default_value");

    }

protected:
    float value = 0.0;
    float minimum_value = 0.0;
    float maximum_value = 1.0;
    float default_value = 0.0;
    const ValueType _value_type = UNKONWN;

private:
    virtual void setup(Csm::CubismModel *model) {}
    
public:
    GDCubismValueAbs() {}
    GDCubismValueAbs(const ValueType value_type)
        : _value_type(value_type) {}
    
    virtual void set_value(const float value) {
        this->value = value;
    }
    virtual void reset() {
        this->set_value(this->default_value);
    }
    
    float get_value() const { return this->value; }

    float get_minimum_value() const { return this->minimum_value; }
    float get_maximum_value() const { return this->maximum_value; }
    float get_default_value() const { return this->default_value; }
    void set_minimum_value(float v) { this->minimum_value = v; }
    void set_maximum_value(float v) { this->maximum_value = v; }
    void set_default_value(float v) { this->default_value = v; }

    String _to_string() const {
        return String(this->get_name()) + ":" + String::num_real(this->get_value());
    }

    virtual void apply(Csm::CubismModel *model) {}
};


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_VALUE_ABS_H
