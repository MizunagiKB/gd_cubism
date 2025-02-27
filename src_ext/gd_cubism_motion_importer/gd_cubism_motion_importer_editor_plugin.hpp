#ifndef GD_CUBISM_PLUGIN
#define GD_CUBISM_PLUGIN

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/editor_plugin.hpp>
#include <godot_cpp/classes/animation.hpp>

#include <gd_cubism_motion_importer.hpp>

using namespace godot;

class GDCubismMotionImporterEditorPlugin : public EditorPlugin {
    GDCLASS(GDCubismMotionImporterEditorPlugin, EditorPlugin);

protected:
    static void _bind_methods() {};

    GDCubismMotionImporter *motionImporter;

public:
    void _enter_tree() override;
    void _exit_tree() override;
};

#endif // GD_CUBISM_PLUGIN