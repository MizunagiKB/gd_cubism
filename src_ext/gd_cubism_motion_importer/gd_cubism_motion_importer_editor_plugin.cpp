#include <gd_cubism_motion_importer_editor_plugin.hpp>

using namespace godot;

void GDCubismMotionImporterEditorPlugin::_enter_tree() {
    this->motionImporter = memnew(GDCubismMotionImporter);    
    this->add_import_plugin(motionImporter);
}

void GDCubismMotionImporterEditorPlugin::_exit_tree() {
    this->remove_import_plugin(motionImporter);
    memdelete(this->motionImporter);
}