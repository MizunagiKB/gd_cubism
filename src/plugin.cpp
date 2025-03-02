#include <plugin.hpp>

#include <importers/gd_cubism_motion_importer.hpp>
#include <importers/gd_cubism_model_importer.hpp>

using namespace godot;

static Ref<GDCubismMotionImporter> motionImporter;
static Ref<GDCubismModelImporter> modelImporter;

void GDCubismPlugin::_enter_tree() {
    motionImporter.instantiate();
    modelImporter.instantiate();

    this->add_import_plugin(motionImporter);
    this->add_import_plugin(modelImporter);
}

void GDCubismPlugin::_exit_tree() {
    this->remove_import_plugin(modelImporter);
    this->remove_import_plugin(motionImporter);
    
    modelImporter.unref();
    motionImporter.unref();
}
