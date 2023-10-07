// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_TARGET_POINT
#define GD_CUBISM_EFFECT_TARGET_POINT


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <gd_cubism_effect.hpp>
#include <private/internal_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
class GDCubismEffectTargetPoint : public GDCubismEffect {
    GDCLASS(GDCubismEffectTargetPoint, GDCubismEffect);

private:
    enum E_PARAM {
        ANGLE_X,
        ANGLE_Y,
        ANGLE_Z,
        BODY_ANGLE_X,
        EYE_BALL_X,
        EYE_BALL_Y
    };

protected:
    static void _bind_methods() {
    	ClassDB::bind_method(D_METHOD("set_angle_x", "id"), &GDCubismEffectTargetPoint::set_angle_x);
    	ClassDB::bind_method(D_METHOD("get_angle_x"), &GDCubismEffectTargetPoint::get_angle_x);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "angle_x"), "set_angle_x", "get_angle_x");

    	ClassDB::bind_method(D_METHOD("set_angle_y", "id"), &GDCubismEffectTargetPoint::set_angle_y);
    	ClassDB::bind_method(D_METHOD("get_angle_y"), &GDCubismEffectTargetPoint::get_angle_y);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "angle_y"), "set_angle_y", "get_angle_y");

    	ClassDB::bind_method(D_METHOD("set_angle_z", "id"), &GDCubismEffectTargetPoint::set_angle_z);
    	ClassDB::bind_method(D_METHOD("get_angle_z"), &GDCubismEffectTargetPoint::get_angle_z);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "angle_z"), "set_angle_z", "get_angle_z");

    	ClassDB::bind_method(D_METHOD("set_body_angle_x", "id"), &GDCubismEffectTargetPoint::set_body_angle_x);
    	ClassDB::bind_method(D_METHOD("get_body_angle_x"), &GDCubismEffectTargetPoint::get_body_angle_x);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "body_angle_x"), "set_body_angle_x", "get_body_angle_x");

    	ClassDB::bind_method(D_METHOD("set_eye_ball_x", "id"), &GDCubismEffectTargetPoint::set_eye_ball_x);
    	ClassDB::bind_method(D_METHOD("get_eye_ball_x"), &GDCubismEffectTargetPoint::get_eye_ball_x);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "eye_ball_x"), "set_eye_ball_x", "get_eye_ball_x");

    	ClassDB::bind_method(D_METHOD("set_eye_ball_y", "id"), &GDCubismEffectTargetPoint::set_eye_ball_y);
    	ClassDB::bind_method(D_METHOD("get_eye_ball_y"), &GDCubismEffectTargetPoint::get_eye_ball_y);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "eye_ball_y"), "set_eye_ball_y", "get_eye_ball_y");

    	ClassDB::bind_method(D_METHOD("set_position", "position"), &GDCubismEffectTargetPoint::set_position);
    }

private:
    void set_angle_x(const String id) { this->angle_x = id; this->initialized = false; }
    String get_angle_x() const { return this->angle_x; }
    void set_angle_y(const String id) { this->angle_y = id; this->initialized = false; }
    String get_angle_y() const { return this->angle_y; }
    void set_angle_z(const String id) { this->angle_z = id; this->initialized = false; }
    String get_angle_z() const { return this->angle_z; }

    void set_body_angle_x(const String id) { this->body_angle_x = id; this->initialized = false; }
    String get_body_angle_x() const { return this->body_angle_x; }

    void set_eye_ball_x(const String id) { this->eye_ball_x = id; this->initialized = false; }
    String get_eye_ball_x() const { return this->eye_ball_x; }
    void set_eye_ball_y(const String id) { this->eye_ball_y = id; this->initialized = false; }
    String get_eye_ball_y() const { return this->eye_ball_y; }

private:
    Csm::CubismTargetPoint* _target_point = nullptr;
    String angle_x = "ParamAngleX";
    String angle_y = "ParamAngleY";
    String angle_z = "ParamAngleZ";
    String body_angle_x = "ParamBodyAngleX";
    String eye_ball_x = "ParamEyeBallX";
    String eye_ball_y = "ParamEyeBallY";

    Csm::csmMap<E_PARAM, Csm::csmInt32> _map_param_idx;
    bool initialized = false;

private:
    Csm::csmInt32 find_idx(Csm::CubismModel* _model, const Csm::csmString name) const {
        for(Csm::csmInt32 index = 0; index < _model->GetParameterCount(); index++) {
            if(_model->GetParameterId(index)->GetString() == name) return index;
        }
        return -1;
    }

public:
    void set_position(Vector2 position) {
        if(this->_target_point == nullptr) return;

        this->_target_point->Set(
            CLAMP<float, float, float>(position.x, -1.0, 1.0),
            CLAMP<float, float, float>(position.y * -1.0, -1.0, 1.0)
        );
    }

    virtual void _cubism_init(InternalCubismUserModel* model) override {
        this->_target_point = memnew(Csm::CubismTargetPoint);
        this->_target_point->Set(0.0, 0.0);
        this->_map_param_idx.Clear();
        this->initialized = false;
    }

    virtual void _cubism_term(InternalCubismUserModel* model) override {

        this->_map_param_idx.Clear();

        if(this->_target_point != nullptr) {
            memdelete(this->_target_point);
            this->_target_point = nullptr;
        }
    }

    virtual void _cubism_process(InternalCubismUserModel* model, const float delta) override {
        if(this->_target_point == nullptr) return;
 
        Csm::CubismModel* _model = model->GetModel();

        if(this->initialized == false) {
            this->_map_param_idx[ANGLE_X] = this->find_idx(_model, Csm::csmString(this->angle_x.ascii().ptr()));
            this->_map_param_idx[ANGLE_Y] = this->find_idx(_model, Csm::csmString(this->angle_y.ascii().ptr()));
            this->_map_param_idx[ANGLE_Z] = this->find_idx(_model, Csm::csmString(this->angle_z.ascii().ptr()));
            this->_map_param_idx[BODY_ANGLE_X] = this->find_idx(_model, Csm::csmString(this->body_angle_x.ascii().ptr()));
            this->_map_param_idx[EYE_BALL_X] = this->find_idx(_model, Csm::csmString(this->eye_ball_x.ascii().ptr()));
            this->_map_param_idx[EYE_BALL_Y] = this->find_idx(_model, Csm::csmString(this->eye_ball_y.ascii().ptr()));
            this->initialized = true;
        }

        this->_target_point->Update(delta);
        Csm::csmFloat32 _dragX = this->_target_point->GetX();
        Csm::csmFloat32 _dragY = this->_target_point->GetY();

        //ドラッグによる変化
        //ドラッグによる顔の向きの調整
        _model->AddParameterValue(this->_map_param_idx[ANGLE_X], _dragX * 30); // -30から30の値を加える
        _model->AddParameterValue(this->_map_param_idx[ANGLE_Y], _dragY * 30);
        _model->AddParameterValue(this->_map_param_idx[ANGLE_Z], _dragX * _dragY * -30);

        //ドラッグによる体の向きの調整
        _model->AddParameterValue(this->_map_param_idx[BODY_ANGLE_X], _dragX * 10); // -10から10の値を加える

        //ドラッグによる目の向きの調整
        _model->AddParameterValue(this->_map_param_idx[EYE_BALL_X], _dragX); // -1から1の値を加える
        _model->AddParameterValue(this->_map_param_idx[EYE_BALL_Y], _dragY);
    }
};


#endif // GD_CUBISM_EFFECT_TARGET_POINT
