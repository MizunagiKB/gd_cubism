// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_VALUE_OPACITY_H
#define GD_CUBISM_VALUE_OPACITY_H
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
class GDCubismPartOpacity : public GDCubismValueAbs {
    GDCLASS(GDCubismPartOpacity, GDCubismValueAbs)
    friend GDCubismUserModel;

protected:
    static void _bind_methods() {}

private:
    void setup(Csm::CubismModel *model, Csm::csmInt32 index) override {
        Core::csmModel *csm_model = model->GetModel();

        this->id = String(Core::csmGetPartIds(csm_model)[index]);
        this->value = Core::csmGetPartOpacities(csm_model)[index];
        this->raw_value = &(Core::csmGetPartOpacities(csm_model)[index]);
        this->changed = false;
    }

public:
    GDCubismPartOpacity()
        : GDCubismValueAbs(GDCubismValueAbs::ValueType::PART_OPACITY) {}
};


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_VALUE_OPACITY_H
