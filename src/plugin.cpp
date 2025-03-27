// SPDX-License-Identifier: MIT
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/editor_selection.hpp>
#include <godot_cpp/classes/editor_settings.hpp>
#include <godot_cpp/classes/geometry2d.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

#include <gd_cubism_user_model.hpp>
#include <plugin.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
const char *SNAPMODE_LABEL = "Grid Snap";
const bool SNAPMODE_INIT_VALUE = false;

const char *SNAPSIZE_SUFFIX = "px";
const double SNAPSIZE_MIN = 0;
const double SNAPSIZE_MAX = 256;
const double SNAPSIZE_STEP = 1;
const double SNAPSIZE_INIT_VALUE = 8;


// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
Rect2 GDCubismPlugin::get_cubism_model_rect(GDCubismUserModel *model) const {
    if (model == nullptr) return Rect2();
    if (model->get_assets().length() == 0) return Rect2();
    if (model->get_canvas_info().size() == 0) return Rect2();

    Dictionary dict = model->get_canvas_info();
    Vector2 size = dict["size_in_pixels"];
    Vector2 orig = dict["origin_in_pixels"];

    return Rect2(
        model->get_global_position() + ((orig - size) * model->get_scale()),
        size * model->get_scale()
    );
}

PackedVector2Array GDCubismPlugin::get_cubism_model_vertex(GDCubismUserModel *model) const {

    PackedVector2Array ary_vtx;

    ary_vtx.resize(4);

    if (model == nullptr) return ary_vtx;
    if (model->get_assets().length() == 0) return ary_vtx;
    if (model->get_canvas_info().size() == 0) return ary_vtx;

    Dictionary dict = model->get_canvas_info();
    Vector2 size = dict["size_in_pixels"];
    Vector2 orig = dict["origin_in_pixels"];

    ary_vtx[0] = ((orig - size) + Vector2(     0,      0));
    ary_vtx[1] = ((orig - size) + Vector2(size.x,      0));
    ary_vtx[2] = ((orig - size) + Vector2(size.x, size.y));
    ary_vtx[3] = ((orig - size) + Vector2(     0, size.y));

    return ary_vtx;
}


bool GDCubismPlugin::update_selected_info() {
    if (this->selected_model == nullptr) return false;
    if (this->selected_model->get_assets().length() == 0) return false;
    if (this->selected_model->get_canvas_info().size() == 0) return false;

    this->selected_rect = this->get_cubism_model_rect(this->selected_model);

    return true;
}


void GDCubismPlugin::_enter_tree() {

    this->drag = false;

    this->p_snapmode_button = memnew(Button);
    this->p_snapmode_button->set_text(SNAPMODE_LABEL);
    this->p_snapmode_button->set_toggle_mode(true);
    this->p_snapmode_button->set_focus_mode(Control::FOCUS_NONE);
    this->p_snapmode_button->set_pressed(SNAPMODE_INIT_VALUE);
    this->add_control_to_container(CONTAINER_CANVAS_EDITOR_MENU, this->p_snapmode_button);

    this->p_snapsize_spinbox = memnew(SpinBox);
    this->p_snapsize_spinbox->set_suffix(SNAPSIZE_SUFFIX);
    this->p_snapsize_spinbox->set_min(SNAPSIZE_MIN);
    this->p_snapsize_spinbox->set_max(SNAPSIZE_MAX);
    this->p_snapsize_spinbox->set_step(SNAPSIZE_STEP);
    this->p_snapsize_spinbox->set_value(SNAPSIZE_INIT_VALUE);
    this->p_snapsize_spinbox->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_RIGHT);
    this->add_control_to_container(CONTAINER_CANVAS_EDITOR_MENU, this->p_snapsize_spinbox);
}


void GDCubismPlugin::_exit_tree() {

    if (this->p_snapsize_spinbox != nullptr) {
        this->remove_control_from_container(CONTAINER_CANVAS_EDITOR_MENU, this->p_snapsize_spinbox);
        memdelete(this->p_snapsize_spinbox);
        this->p_snapsize_spinbox = nullptr;
    }

    if (this->p_snapmode_button != nullptr) {
        this->remove_control_from_container(CONTAINER_CANVAS_EDITOR_MENU, this->p_snapmode_button);
        memdelete(this->p_snapmode_button);
        this->p_snapmode_button = nullptr;
    }
}


void GDCubismPlugin::_input(const Ref<InputEvent> &p_event) {

    InputEventMouseButton* p_evt_mouse_button = Object::cast_to<InputEventMouseButton>(p_event.ptr());

    if (p_evt_mouse_button != nullptr) {
        const SubViewport *editor_viewport = this->get_editor_interface()->get_editor_viewport_2d();
        const Rect2 viewport_rect(Point2(0.0, 0.0), editor_viewport->get_size());

        // Check in Viewport2D
        if (viewport_rect.has_point(p_evt_mouse_button->get_position()) == false) return;

        Vector2 mouse_pos = editor_viewport->get_mouse_position();

        if (p_evt_mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
            if (p_evt_mouse_button->is_pressed() == true) {
                TypedArray<Node> ary_node = get_tree()->get_edited_scene_root()->get_children();

                for(int64_t i = 0; i < ary_node.size(); i++) {
                    GDCubismUserModel *model = Object::cast_to<GDCubismUserModel>(ary_node[i]);
                    if (model == nullptr) continue;

                    Transform2D m_tform = model->get_global_transform();
                    PackedVector2Array ary_vtx = this->get_cubism_model_vertex(model);

                    for (int i = 0; i < ary_vtx.size(); i++) {
                        ary_vtx[i] = m_tform.xform(ary_vtx[i]);
                    }
            
                    if (Geometry2D::get_singleton()->is_point_in_polygon(mouse_pos, ary_vtx) == false) continue;

                    if (get_editor_interface()->get_selection()->get_selected_nodes().size() > 1) continue;

                    this->drag = true;
                    this->drag_position = mouse_pos;
                    this->base_position = model->get_global_position();

                    get_editor_interface()->edit_node(model);
                    break;
                }
            }
        }
    }
}


void GDCubismPlugin::_edit(Object *p_object) {
    GDCubismUserModel* model = Object::cast_to<GDCubismUserModel>(p_object);
    if (model != nullptr) {
        if (model != this->selected_model) {
            this->drag = false;
        }
    }
    this->selected_model = model;
}


bool GDCubismPlugin::_handles(Object *p_object) const {
    return Object::cast_to<GDCubismUserModel>(p_object) != nullptr;
}


bool GDCubismPlugin::_forward_canvas_gui_input(const Ref<InputEvent> &p_event) {

    InputEventMouseButton *p_evt_mouse_button = Object::cast_to<InputEventMouseButton>(p_event.ptr());

    if (p_evt_mouse_button != nullptr) {
        const SubViewport *editor_viewport = this->get_editor_interface()->get_editor_viewport_2d();
        const Rect2 viewport_rect(Point2(0.0, 0.0), editor_viewport->get_size());

        // Check in Viewport2D
        if (viewport_rect.has_point(p_evt_mouse_button->get_position()) == false) return false;

        Vector2 mouse_pos = editor_viewport->get_mouse_position();

        if (p_evt_mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
            if (p_evt_mouse_button->is_pressed() == true) {
                if (this->update_selected_info() == true) {
                    Transform2D m_tform = this->selected_model->get_global_transform();
                    PackedVector2Array ary_vtx = this->get_cubism_model_vertex(this->selected_model);

                    for (int i = 0; i < ary_vtx.size(); i++) {
                        ary_vtx[i] = m_tform.xform(ary_vtx[i]);
                    }
            
                    if (Geometry2D::get_singleton()->is_point_in_polygon(mouse_pos, ary_vtx) == true) {
                        this->drag = true;
                        this->drag_position = mouse_pos;
                        this->base_position = this->selected_model->get_global_position();

                        return true;
                    }
                }
            } else {
                this->drag = false;
            }
        }
    }

    InputEventMouseMotion *p_evt_mouse_motion = Object::cast_to<InputEventMouseMotion>(p_event.ptr());

    if (p_evt_mouse_motion != nullptr) {
        Vector2 mouse_pos = this->get_editor_interface()->get_editor_viewport_2d()->get_mouse_position();

        if (this->drag != true) return false;
        if (this->selected_model == nullptr) return false;

        Vector2 new_position = this->base_position + (mouse_pos - this->drag_position);

        if (this->p_snapmode_button->is_pressed() == true) {
            new_position.x = Math::snapped(new_position.x, this->p_snapsize_spinbox->get_value());
            new_position.y = Math::snapped(new_position.y, this->p_snapsize_spinbox->get_value());
        }

        this->selected_model->set_global_position(new_position);

        return true;
    }

    return false;
}


void GDCubismPlugin::_forward_canvas_draw_over_viewport(Control *p_viewport_control) {

    if (this->update_selected_info() == true) {
        Transform2D c_tform = this->get_editor_interface()->get_editor_viewport_2d()->get_global_canvas_transform();
        Transform2D m_tform = this->selected_model->get_global_transform();

        PackedVector2Array ary_vtx = this->get_cubism_model_vertex(this->selected_model);

        for (int i = 0; i < ary_vtx.size(); i++) {
            ary_vtx[i] = c_tform.xform(m_tform.xform(ary_vtx[i]));
        }

        Geometry2D::get_singleton()->is_point_in_polygon(Vector2(), ary_vtx);

        p_viewport_control->draw_line(ary_vtx[0], ary_vtx[1], this->selected_border_color);
        p_viewport_control->draw_line(ary_vtx[1], ary_vtx[2], this->selected_border_color);
        p_viewport_control->draw_line(ary_vtx[2], ary_vtx[3], this->selected_border_color);
        p_viewport_control->draw_line(ary_vtx[3], ary_vtx[0], this->selected_border_color);
    }
}
