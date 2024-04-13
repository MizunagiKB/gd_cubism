// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_MOTION_ENTRY_H
#define GD_CUBISM_MOTION_ENTRY_H
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/classes/resource.hpp>

#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotionManager.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;
class GDCubismUserModel;


// ------------------------------------------------------------------- class(s)
class GDCubismMotionQueueEntryHandle : public Resource {
    GDCLASS(GDCubismMotionQueueEntryHandle, Resource)
    friend GDCubismUserModel;

public:
    enum HandleError {
        OK = godot::Error::OK,
        FAILED = godot::Error::FAILED
    };

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("get_error"), &GDCubismMotionQueueEntryHandle::get_error);
        ADD_PROPERTY(PropertyInfo(Variant::INT, "error"), "", "get_error");

        BIND_ENUM_CONSTANT(OK);
        BIND_ENUM_CONSTANT(FAILED);
    }

public:
    HandleError get_error() const {
        if(this->_handle == Csm::InvalidMotionQueueEntryHandleValue) return FAILED;

        return OK;
    }


private:
    Csm::CubismMotionQueueEntryHandle _handle = Csm::InvalidMotionQueueEntryHandleValue;
};

VARIANT_ENUM_CAST(GDCubismMotionQueueEntryHandle::HandleError);


class GDCubismMotionEntry : public godot::Resource {
    GDCLASS(GDCubismMotionEntry, godot::Resource)
    friend GDCubismUserModel;

protected:
    static void _bind_methods() {}

private:
    Csm::CubismMotionQueueEntry *_entry;
};


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_MOTION_ENTRY_H
