#ifndef GD_CUBISM_EFFECT_EXPRESSION
#define GD_CUBISM_EFFECT_EXPRESSION

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotion.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node.hpp>
#include <gd_cubism_effect.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class GDCubismEffectExpression : public GDCubismEffect {
	GDCLASS(GDCubismEffectExpression, GDCubismEffect);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_expression", "motion"), &GDCubismEffectExpression::set_expression);
        ClassDB::bind_method(D_METHOD("get_expression"), &GDCubismEffectExpression::get_expression);
        ADD_PROPERTY(PropertyInfo(Variant::STRING, "expression"), "set_expression", "get_expression");
	}

private:
	CubismExpressionMotionManager* _expressionManager = nullptr;
	csmMap<String, CubismExpressionMotion*> _expressions;
	String _active_expression;
	
public:
	String get_expression() const {
		return this->_active_expression;
	}
	void set_expression(String motion) {
		if (this->_expressionManager != nullptr) {
			if(this->_expressions[motion] != nullptr) {
				this->_expressionManager->StartMotionPriority(
					this->_expressions[motion],
					false,
					GDCubismUserModel::Priority::PRIORITY_FORCE
				);
			} else {
				this->_expressionManager->StopAllMotions();
			}
		}
		this->_active_expression = motion;
	}

	void _validate_property(PropertyInfo &p_property) const {
		if (p_property.name != StringName("active_motion")) return;

		Array motions;
		for(csmMap<String,CubismExpressionMotion*>::const_iterator i = this->_expressions.Begin(); i != this->_expressions.End(); i++) {
			String motion_name = i->First;
			motions.append(motion_name);
		}
		p_property.hint_string = String(",").join(motions);
	}

    virtual void _cubism_init(GDCubismUserModel* model) override {
        if(this->_initialized == true) return;

		_expressionManager = CSM_NEW CubismExpressionMotionManager();
		ICubismModelSetting *model_setting = model->get_model_settings();
		String model_path = model->get_scene_file_path().get_base_dir();

		for (int32_t i = 0; i < model_setting->GetExpressionCount(); i++) {
			String gd_filename; gd_filename.parse_utf8(model_setting->GetExpressionFileName(i));
			String motion_pathname = model_path.get_base_dir().path_join(gd_filename);

			PackedByteArray buffer = FileAccess::get_file_as_bytes(motion_pathname);

			CubismExpressionMotion *expression = CubismExpressionMotion::Create(buffer.ptr(), buffer.size());

			this->_expressions[gd_filename] = expression;
		}
		this->_active_expression = "";

        this->_initialized = true;
    }

	virtual void _cubism_process(GDCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
		if(this->_expressionManager == nullptr) return;
		
		this->_expressionManager->UpdateMotion(model->get_internal_model(), delta);
    }

	virtual void _cubism_term(GDCubismUserModel* model) override {
        if(this->_initialized == false) return;

		if(this->_expressionManager != nullptr) {
			this->_expressionManager->StopAllMotions();
		
			CSM_DELETE(this->_expressionManager);
			this->_expressionManager = nullptr;

			for(csmMap<String,CubismExpressionMotion*>::const_iterator i = this->_expressions.Begin(); i != this->_expressions.End(); i++) {
				CubismExpressionMotion::Delete(i->Second);
			}

        	this->_expressions.Clear();
		}
		this->_active_expression = "";
		this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_EXPRESSION
