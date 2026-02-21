#ifndef GD_CUBISM_EFFECT_MOTION
#define GD_CUBISM_EFFECT_MOTION

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>

#include <Motion/ACubismMotion.hpp>
#include <Motion/CubismMotion.hpp>
#include <Motion/CubismMotionManager.hpp>

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
class GDCubismEffectMotion : public GDCubismEffect {
	GDCLASS(GDCubismEffectMotion, GDCubismEffect);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_active_motion", "motion"), &GDCubismEffectMotion::set_active_motion);
        ClassDB::bind_method(D_METHOD("get_active_motion"), &GDCubismEffectMotion::get_active_motion);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "active_motion"), "set_active_motion", "get_active_motion");
	}

private:
	CubismMotionManager* _motion_manager = nullptr;
	Csm::csmMap<String,Csm::CubismMotion*> _map_motion;
	Csm::csmVector<Csm::CubismIdHandle> _list_eye_blink;
	Csm::csmVector<Csm::CubismIdHandle> _list_lipsync;

	String active_motion;
    
	static void CubismDefaultMotionEventCallback(const CubismMotionQueueManager* caller, const csmString& eventValue, void* customData)
	{
		GDCubismUserModel* model = reinterpret_cast<GDCubismUserModel*>(customData);
		if (model != NULL)
		{
			//model->emit_signal();
		}
	}

	ACubismMotion *load_motion(String motion_filepath) {
		PackedByteArray buffer = FileAccess::get_file_as_bytes(motion_filepath);
		ACubismMotion* motion = CubismMotion::Create(buffer.ptr(), buffer.size(), NULL, NULL);

		if (!motion)
		{
			CubismLogError("Failed to create motion from buffer in LoadMotion().");
			return NULL;
		}

		return motion;
	}
public:
	void set_active_motion(String motion_name) {
		if (this->_motion_manager == nullptr) return;
		Csm::CubismMotion *motion = this->_map_motion[motion_name];
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
		for(csmMap<String,CubismMotion*>::const_iterator i = this->_map_motion.Begin(); i != this->_map_motion.End(); i++) {
			String motion_name = i->First;
			motions.append(motion_name);
		}
		p_property.hint_string = String(",").join(motions);
	}

    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;

		ICubismModelSetting *model_setting = model->get_model_settings();
		if(model_setting->GetMotionGroupCount() == 0){
			this->_initialized = true;
			return;
		}

		String model_path = model->get_scene_file_path();

    	this->_motion_manager = CSM_NEW CubismMotionManager();
    	this->_motion_manager->SetEventCallback(CubismDefaultMotionEventCallback, model);

		// EyeBlink(Parameters)
		{
			Csm::csmInt32 param_count = model_setting->GetEyeBlinkParameterCount();
			for(Csm::csmInt32 i = 0; i < param_count; ++i)
			{
				this->_list_eye_blink.PushBack(model_setting->GetEyeBlinkParameterId(i));
			}
		}

		// LipSync(Parameters)
    	{
			Csm::csmInt32 param_count = model_setting->GetLipSyncParameterCount();
			for(Csm::csmInt32 i = 0; i < param_count; ++i)
			{
				this->_list_lipsync.PushBack(model_setting->GetLipSyncParameterId(i));
			}
		}

		for (csmInt32 ig = 0; ig < model_setting->GetMotionGroupCount(); ig++)
		{
			//PreloadMotionGroup(group);
			const csmChar* group = model_setting->GetMotionGroupName(ig);
			const csmInt32 motion_count = model_setting->GetMotionCount(group);

			if(motion_count == 0) continue;

			for (csmInt32 im = 0; im < motion_count; im++)
			{
				String name = String(group) + String("_") + String::num_int64(im);

				String gd_filename; gd_filename.parse_utf8(model_setting->GetMotionFileName(group, im));
				String motion_pathname = model_path.get_base_dir().path_join(gd_filename);

				CubismMotion* motion = static_cast<CubismMotion*>(this->load_motion(motion_pathname));

				csmFloat32 fade_time_sec = model_setting->GetMotionFadeInTimeValue(group, im);
				if (fade_time_sec >= 0.0f) {
					motion->SetFadeInTime(fade_time_sec);
				}

				fade_time_sec = model_setting->GetMotionFadeOutTimeValue(group, im);
				if (fade_time_sec >= 0.0f) {
					motion->SetFadeOutTime(fade_time_sec);
				}
				
				motion->SetEffectIds(this->_list_eye_blink, this->_list_lipsync);

				if (this->_map_motion[gd_filename] != nullptr) {
					ACubismMotion::Delete(this->_map_motion[gd_filename]);
					this->_map_motion[gd_filename] = nullptr;
				}

				this->_map_motion[gd_filename] = motion;
			}
		}
    
        this->_initialized = true;
    }

	virtual void _cubism_prologue(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
		if(this->_active == false) return;
		if(this->_motion_manager == nullptr) return;
        
		model->get_internal_model()->LoadParameters();
		this->_motion_manager->UpdateMotion(model->get_internal_model(), delta);
		model->get_internal_model()->SaveParameters();
    }

	virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

		this->_list_eye_blink.Clear();
		this->_list_lipsync.Clear();

        if(this->_motion_manager != nullptr) {
			this->_motion_manager->StopAllMotions();
			CSM_DELETE(this->_motion_manager);
			this->_motion_manager = nullptr;

			for(csmMap<String,CubismMotion*>::const_iterator i = this->_map_motion.Begin(); i != this->_map_motion.End(); i++) {
				ACubismMotion::Delete(i->Second);
			}
			this->_map_motion.Clear();
        }

        this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_MOTION
