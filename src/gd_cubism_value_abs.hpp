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
class GDCubismValueAbs : public godot::Resource {
    GDCLASS(GDCubismValueAbs, godot::Resource)
    friend GDCubismUserModel;

public:
    enum ValueType {
        UNKONWN = 0,
        PARAMETER = 1,
        PART_OPACITY = 2
    };

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_id"), &GDCubismValueAbs::get_id);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "id"), String(), "get_id");

        ClassDB::bind_method(D_METHOD("set_value"), &GDCubismValueAbs::set_value);
        ClassDB::bind_method(D_METHOD("get_value"), &GDCubismValueAbs::get_value);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "value"), "set_value", "get_value");
    }

protected:
    String id;
    float value;
    float *raw_value;
    bool changed;
    const ValueType _value_type = UNKONWN;

private:
    virtual void setup(Csm::CubismModel *model, Csm::csmInt32 index) {}

public:
    GDCubismValueAbs() {}
    GDCubismValueAbs(const ValueType value_type)
        : _value_type(value_type) {}

    String get_id() const { return this->id; }
    void set_value(const float value) { this->value = value; this->changed = true; }
    float get_value() const { return this->value; }

    void set_raw_value() {
        if(this->changed == true) {
            *this->raw_value = this->value;
            this->changed = false;
        }        
    }

    void get_raw_value() {
        this->value = *this->raw_value;
    }
};


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_VALUE_ABS_H
