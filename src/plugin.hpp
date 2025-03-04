// SPDX-License-Identifier: MIT
#ifndef GD_CUBISM_PLUGIN
#define GD_CUBISM_PLUGIN
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/animation.hpp>

using namespace godot;

class GDCubismPlugin : public EditorPlugin {
    GDCLASS(GDCubismPlugin, EditorPlugin);

protected:
    static void _bind_methods() {};

public:
    void _enter_tree() override;
    void _exit_tree() override;
};

#endif // GD_CUBISM_PLUGIN