#ifndef GD_CUBISM_EXPRESSION_CONTROLLER
#define GD_CUBISM_EXPRESSION_CONTROLLER

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotion.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node.hpp>
#include <gd_cubism_expression.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

class GDCubismExpressionController : public Node {
	GDCLASS(GDCubismExpressionController, Node);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("is_activated"), &GDCubismExpressionController::is_activated);

		ClassDB::bind_method(D_METHOD("get_expression_library"), &GDCubismExpressionController::get_expression_library);
        ClassDB::bind_method(D_METHOD("set_expression_library", "library"), &GDCubismExpressionController::set_expression_library);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "expression_library"), "set_expression_library", "get_expression_library");

		ClassDB::bind_method(D_METHOD("activate_expression", "expression", "fade"), &GDCubismExpressionController::activate_expression);
        ClassDB::bind_method(D_METHOD("deactivate_expression", "expression", "fade"), &GDCubismExpressionController::deactivate_expression);
	}

private:
	Dictionary _expression_library;
	Dictionary _activated_expressions;
	bool _exclusive_mode;
	
	double fade_time;
	Array _queue;

	void queue_expression(float fade);

public:
    void _ready() override;

	Dictionary get_expression_library() const {
		return this->_expression_library;
	}
	void set_expression_library(const Dictionary expressions) {
		this->_expression_library = expressions;
	}
	void set_mode(bool exclusive);
	bool is_activated(const String expression) {
		return this->_activated_expressions.get(expression, false);
	}
	void activate_expression(const String expression, float fade);
	void deactivate_expression(const String expression, float fade);
	void update(CubismModel *model, float delta);
};

#endif // GD_CUBISM_EXPRESSION_CONTROLLER
