// SPDX-License-Identifier: MIT
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <gdextension_interface.h>

#include <gd_cubism_motion_importer.hpp>
#include <gd_cubism_motion_importer_editor_plugin.hpp>
#include <register_types_motion_importer.hpp>


// --------------------------------------------------------------- namespace(s)
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)

void initialize_gd_cubism_motion_importer_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        ClassDB::register_class<GDCubismMotionImporter>();
        ClassDB::register_class<GDCubismMotionImporterEditorPlugin>();
        EditorPlugins::add_by_type<GDCubismMotionImporterEditorPlugin>();
    }
}

void uninitialize_gd_cubism_motion_importer_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        // EditorPlugins::remove_by_type<GDCubismMotionImporterEditorPlugin>();
    }
}

extern "C" {

    // GDCubismMotionImporter init.
    GDExtensionBool GDE_EXPORT gd_cubism_motion_importer_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_gd_cubism_motion_importer_module);
        init_obj.register_terminator(uninitialize_gd_cubism_motion_importer_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
