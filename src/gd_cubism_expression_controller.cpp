#include <gd_cubism_expression_controller.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <private/internal_cubism_user_model.hpp>

GDCubismExpressionController::GDCubismExpressionController() :
	model(nullptr)
	,active_expression(nullptr) {

}

GDCubismExpressionController::~GDCubismExpressionController() {
	
}

void GDCubismExpressionController::_enter_tree() {
	//this->internal_manager = memnew(CubismExpressionMotionManager);
}

void GDCubismExpressionController::_exit_tree() {
	//memdelete(this->internal_manager);
}

void GDCubismExpressionController::_process(double delta) {
	this->internal_manager->UpdateMotion(
		this->model->internal_model->GetModel(),
		delta
	);
}

void GDCubismExpressionController::set_expression(Ref<GDCubismExpression> expression) {
	//this->internal_manager->StartMotion(expression->internal_expression, false, 0);
	this->active_expression = expression;
}
