#ifndef GD_CUBISM_EFFECT_EXPRESSION
#define GD_CUBISM_EFFECT_EXPRESSION

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotion.hpp>

#include <godot_cpp/core/class_db.hpp>
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
class GDCubismEffectExpression : public GDCubismEffect {
	GDCLASS(GDCubismEffectExpression, GDCubismEffect);

protected:
    static void _bind_methods() {
		
	}

private:
	CubismExpressionMotionManager* _expressionManager;
	
public:
	void set_expression(Csm::CubismExpressionMotion* motion) {
		if(motion != nullptr) {
			this->_expressionManager->StartMotionPriority(
				motion,
				false,
				GDCubismUserModel::Priority::PRIORITY_FORCE
			);
		} else {
			this->_expressionManager->StopAllMotions();
		}
	}

    virtual void _cubism_init(InternalCubismUserModel* model) override {
        if(this->_initialized == true) return;

		_expressionManager = CSM_NEW CubismExpressionMotionManager();

        this->_initialized = true;
    }

	virtual void _cubism_process(InternalCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;
    
		this->_expressionManager->UpdateMotion(model->GetModel(), delta);
    }

	virtual void _cubism_term(InternalCubismUserModel* model) override {
        if(this->_initialized == false) return;

        if(this->_expressionManager != nullptr) {
			CSM_DELETE(this->_expressionManager);
			this->_expressionManager = nullptr;
        }

        this->_initialized = false;
    }
};

#endif // GD_CUBISM_EFFECT_EXPRESSION
