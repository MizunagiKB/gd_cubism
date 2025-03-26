// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT
#define GD_CUBISM_EFFECT


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>

#include <gd_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;


// ------------------------------------------------------------------- class(s)
class GDCubismEffect : public Node {
    GDCLASS(GDCubismEffect, Node);

protected:
    bool _active = true;
    bool _initialized = false;

protected:
    static void _bind_methods();

public:
    void set_active(const bool active) { this->_active = active; }
    bool get_active() const { return this->_active; }

    virtual void _cubism_init(InternalCubismUserModel* model);
    virtual void _cubism_term(InternalCubismUserModel* model);
    virtual void _cubism_prologue(InternalCubismUserModel* model, const double delta);
    virtual void _cubism_process(InternalCubismUserModel* model, const double delta);
    virtual void _cubism_epilogue(InternalCubismUserModel* model, const double delta);

    void _enter_tree() override;
    void _exit_tree() override;
};


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_EFFECT
