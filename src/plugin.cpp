#include <plugin.hpp>

#include <importers/gd_cubism_model_importer.hpp>

using namespace godot;

static Ref<GDCubismModelImporter> modelImporter;

void GDCubismPlugin::_enter_tree() {
    modelImporter.instantiate();

    this->add_import_plugin(modelImporter);
}

void GDCubismPlugin::_exit_tree() {
    this->remove_import_plugin(modelImporter);
    
    modelImporter.unref();
}
