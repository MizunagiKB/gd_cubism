#include <plugin.hpp>

using namespace godot;

void GDCubismPlugin::_enter_tree() {
    this->motionImporter = memnew(GDCubismMotionImporter);    
    this->add_import_plugin(motionImporter);
}

void GDCubismPlugin::_exit_tree() {
    this->remove_import_plugin(motionImporter);
    memdelete(this->motionImporter);
}