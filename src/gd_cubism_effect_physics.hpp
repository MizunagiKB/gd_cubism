#ifndef GD_CUBISM_EFFECT_PHYSICS
#define GD_CUBISM_EFFECT_PHYSICS

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Physics/CubismPhysics.hpp>

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
class GDCubismEffectPhysics : public GDCubismEffect {
	GDCLASS(GDCubismEffectPhysics, GDCubismEffect);

protected:
    static void _bind_methods() {
		
	}

private:
	CubismPhysics* _physics = nullptr;
	
public:
    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;
        
		String path = ((Dictionary)model->get_meta("filerefs", Dictionary())).get("Physics", "");
		String _model_dir = model->get_scene_file_path().get_base_dir();
        if (!path.is_empty()) {
            PackedByteArray buffer = FileAccess::get_file_as_bytes(_model_dir.path_join(path));
			if (buffer.size() > 0) {
				_physics = CubismPhysics::Create(buffer.ptr(), buffer.size());
			}
		}

        this->_initialized = true;
    }

	virtual void _cubism_process(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
		if(this->_active == false) return;
		if(this->_physics == nullptr) return;

		this->_physics->Evaluate(model->get_internal_model(), (float_t)delta);
    }

	virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_physics != nullptr) {
			CubismPhysics::Delete(this->_physics);
			this->_physics = nullptr;
        }

        this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_PHYSICS
