// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism_effect.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
void GDCubismEffect::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_active", "value"), &GDCubismEffect::set_active);
	ClassDB::bind_method(D_METHOD("get_active"), &GDCubismEffect::get_active);
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "get_active");
}


void GDCubismEffect::_cubism_init(InternalCubismUserModel* model) {}


void GDCubismEffect::_cubism_term(InternalCubismUserModel* model) {}


void GDCubismEffect::_cubism_process(InternalCubismUserModel* model, const float delta) {}


void GDCubismEffect::_enter_tree() {
    GDCubismUserModel* node = Object::cast_to<GDCubismUserModel>(this->get_parent());
    if(node != nullptr) {
        node->_on_append_child_act(this);
    }
}


void GDCubismEffect::_exit_tree() {
    GDCubismUserModel* node = Object::cast_to<GDCubismUserModel>(this->get_parent());
    if(node != nullptr) {
        node->_on_remove_child_act(this);
    }
}


// ------------------------------------------------------------------ method(s)
