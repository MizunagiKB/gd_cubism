#include <plugin.hpp>

using namespace godot;

void GDCubismPlugin::_enter_tree() {
    this->motionImporter = memnew(GDCubismMotionImporter);
    this->modelImporter = memnew(GDCubismModelImporter);
    this->expressionImporter = memnew(GDCubismExpressionImporter);
    this->add_import_plugin(motionImporter);
    this->add_import_plugin(expressionImporter);
    this->add_import_plugin(modelImporter);
}

void GDCubismPlugin::_exit_tree() {
    this->remove_import_plugin(motionImporter);
    this->remove_import_plugin(expressionImporter);
    this->add_import_plugin(modelImporter);
    memdelete(this->motionImporter);
    memdelete(this->expressionImporter);
    memdelete(this->modelImporter);
}
