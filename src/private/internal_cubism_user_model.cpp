// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#include <CubismModelSettingJson.hpp>

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <private/internal_cubism_renderer_2d.hpp>
#include <private/internal_cubism_user_model.hpp>
#include <gd_cubism_user_model.hpp>
#include <gd_cubism_effect.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismUserModel::InternalCubismUserModel(GDCubismUserModel *owner_viewport)
    : CubismUserModel()
    , _owner_viewport(owner_viewport)
    , _model_pathname(owner_viewport->get_scene_file_path()) {

    _debugMode = false;
}


InternalCubismUserModel::~InternalCubismUserModel() {
}


bool InternalCubismUserModel::model_bind() {
    String _model_dir = this->get_model_path().get_base_dir();
    this->_updating = true;
    this->_initialized = false;
    
    PackedByteArray buffer = FileAccess::get_file_as_bytes(_model_pathname);
    if(buffer.size() == 0) return false;

    ICubismModelSetting *model_settings = CSM_NEW CubismModelSettingJson(buffer.ptr(), buffer.size());

    // setup Live2D model
    {
        String moc3_pathname = _model_dir.path_join(model_settings->GetModelFileName());
        PackedByteArray buffer = FileAccess::get_file_as_bytes(moc3_pathname);
        this->LoadModel(buffer.ptr(), buffer.size());

        if(this->_modelMatrix == nullptr) {
            return false;
        }

        this->_model->SaveParameters();
    }
    // Pose
    {
        String path = model_settings->GetPoseFileName();
        if (!path.is_empty()) {
            PackedByteArray buffer = FileAccess::get_file_as_bytes(_model_dir.path_join(path));
            if(buffer.size() > 0) {
                this->LoadPose(buffer.ptr(), buffer.size());
            }
        }
    }
    //UserData
    {
        String path = model_settings->GetUserDataFile();
        if (!path.is_empty()) {
            PackedByteArray buffer = FileAccess::get_file_as_bytes(_model_dir.path_join(path));
            if(buffer.size() > 0) {
                this->LoadUserData(buffer.ptr(), buffer.size());
            }
        }
    }
    this->model_settings = model_settings;
    
    this->_updating = false;
    this->_initialized = true;

    return true;
}


Vector2 InternalCubismUserModel::get_size(const Csm::CubismModel *model)
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_size.X, vct_size.Y);
}

Vector2 InternalCubismUserModel::get_origin(const Csm::CubismModel *model)
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_origin.X, vct_origin.Y);
}

float InternalCubismUserModel::get_ppunit(const Csm::CubismModel *model)
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return ppunit;
}


// ------------------------------------------------------------------ method(s)
