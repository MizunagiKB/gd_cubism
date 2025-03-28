#ifndef GD_CUBISM_EFFECT_MOTION
#define GD_CUBISM_EFFECT_MOTION

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotion.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node.hpp>
#include <private/internal_cubism_user_model.hpp>
#include <gd_cubism_effect.hpp>
#include <gd_cubism_expression.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class GDCubismEffectMotion : public GDCubismEffect {
	GDCLASS(GDCubismEffectMotion, GDCubismEffect);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_active_motion", "motion"), &GDCubismEffectMotion::set_active_motion);
        ClassDB::bind_method(D_METHOD("get_active_motion"), &GDCubismEffectMotion::get_active_motion);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "active_motion"), "set_active_motion", "get_active_motion");
	}

private:
	CubismMotionManager* _motion_manager;
	Csm::csmMap<Csm::csmString,Csm::CubismMotion*> _map_motion;
	String active_motion;
    
public:
	void set_active_motion(String motion_name) {
		Csm::csmString csm_motion_name(motion_name.utf8().ptr());
		Csm::CubismMotion *motion = this->_map_motion[csm_motion_name];
		ERR_FAIL_COND_MSG(motion == nullptr, "Unknown motion");

		this->active_motion = motion_name;
		this->_motion_manager->StartMotion(motion, false);
	}

	String get_active_motion() const {
		return this->active_motion;
	}

	void _validate_property(PropertyInfo &p_property) const {
		if (p_property.name != StringName("active_motion")) return;

		Array motions;
		for(csmMap<csmString,CubismMotion*>::const_iterator i = this->_map_motion.Begin(); i != this->_map_motion.End(); i++) {
			csmString csm_motion_name = i->First;
			String motion_name(csm_motion_name.GetRawString());
			motions.append(motion_name);
		}
		p_property.hint_string = String(",").join(motions);
	}


    virtual void _cubism_init(InternalCubismUserModel* model) override {
        if(this->_initialized == true) return;
        
		ICubismModelSetting *model_setting = model->get_model_settings();
		String model_path = model->get_model_path();
    	if(model_setting->GetMotionGroupCount() == 0) return;

		_motion_manager = CSM_NEW CubismMotionManager();
    	_motion_manager->SetEventCallback(model->CubismDefaultMotionEventCallback, model);

		for (csmInt32 ig = 0; ig < model_setting->GetMotionGroupCount(); ig++)
		{
			//PreloadMotionGroup(group);
			const csmChar* group = model_setting->GetMotionGroupName(ig);
			const csmInt32 motion_count = model_setting->GetMotionCount(group);

			if(motion_count == 0) continue;

			for (csmInt32 im = 0; im < motion_count; im++)
			{
				csmString name = Utils::CubismString::GetFormatedString("%s_%d", group, im);

				String gd_filename; gd_filename.parse_utf8(model_setting->GetMotionFileName(group, im));
				String motion_pathname = model_path.get_base_dir().path_join(gd_filename);

				PackedByteArray buffer = FileAccess::get_file_as_bytes(motion_pathname);
				CubismMotion* motion = static_cast<CubismMotion*>(model->LoadMotion(
					buffer.ptr(),
					buffer.size(),
					name.GetRawString()
				));

				csmFloat32 fade_time_sec = model_setting->GetMotionFadeInTimeValue(group, im);
				if (fade_time_sec >= 0.0f) {
					motion->SetFadeInTime(fade_time_sec);
				}

				fade_time_sec = model_setting->GetMotionFadeOutTimeValue(group, im);
				if (fade_time_sec >= 0.0f) {
					motion->SetFadeOutTime(fade_time_sec);
				}
				//static_cast<CubismMotion*>(motion)->SetEffectIds(this->_list_eye_blink, this->_list_lipsync);

				if (this->_map_motion[name] != nullptr) {
					ACubismMotion::Delete(this->_map_motion[name]);
					this->_map_motion[name] = nullptr;
				}

				this->_map_motion[name] = motion;
			}
		}
    
        this->_initialized = true;
    }

	virtual void _cubism_prologue(InternalCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
		if(this->_active == false) return;
        
		model->GetModel()->LoadParameters();
		this->_motion_manager->UpdateMotion(model->GetModel(), delta);
		model->GetModel()->SaveParameters();
    }

	virtual void _cubism_term(InternalCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_motion_manager != nullptr) {
			this->_motion_manager->StopAllMotions();
			CSM_DELETE(this->_motion_manager);
			this->_motion_manager = nullptr;

			for(csmMap<csmString,CubismMotion*>::const_iterator i = this->_map_motion.Begin(); i != this->_map_motion.End(); i++) {
				ACubismMotion::Delete(i->Second);
			}
			this->_map_motion.Clear();
        }

        this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_MOTION
