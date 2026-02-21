#ifndef GD_CUBISM_EFFECT_POSE
#define GD_CUBISM_EFFECT_POSE

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Effect/CubismPose.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <gd_cubism_effect.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class GDCubismEffectPose : public GDCubismEffect {
	GDCLASS(GDCubismEffectPose, GDCubismEffect);

protected:
    static void _bind_methods() {
		
	}

private:
	CubismPose* _pose = nullptr;
	
public:
    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;
        
		String path = model->get_model_settings()->GetPoseFileName();
		String _model_dir = model->get_scene_file_path().get_base_dir();
        if (!path.is_empty()) {
            PackedByteArray buffer = FileAccess::get_file_as_bytes(_model_dir.path_join(path));
            if(buffer.size() > 0) {
				this->_pose = CubismPose::Create(buffer.ptr(), buffer.size());
            }
        }

        this->_initialized = true;
    }

	virtual void _cubism_process(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
		if(this->_active == false) return;
        if(this->_pose == nullptr) return;

		this->_pose->UpdateParameters(model->get_internal_model(), (float_t)delta);
    }

	virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_pose != nullptr) {
			CubismPose::Delete(this->_pose);
			this->_pose = nullptr;
        }

        this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_POSE
