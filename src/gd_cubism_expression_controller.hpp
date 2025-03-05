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
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class GDCubismExpressionController : public Node {
	GDCLASS(GDCubismExpressionController, Node);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("is_activated"), &GDCubismExpressionController::is_activated);

		ClassDB::bind_method(D_METHOD("get_expression_library"), &GDCubismExpressionController::get_expression_library);
        ClassDB::bind_method(D_METHOD("set_expression_library", "library"), &GDCubismExpressionController::set_expression_library);
        ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "expression_library"), "set_expression_library", "get_expression_library");

		ClassDB::bind_method(D_METHOD("set_expression", "expression", "fade"), &GDCubismExpressionController::set_expression);
		ClassDB::bind_method(D_METHOD("activate_expression", "expression", "fade"), &GDCubismExpressionController::activate_expression);
        ClassDB::bind_method(D_METHOD("deactivate_expression", "expression", "fade"), &GDCubismExpressionController::deactivate_expression);

		ClassDB::bind_method(D_METHOD("apply", "values", "parameters"), &GDCubismExpressionController::apply);
	}

private:
	Dictionary _expression_library;
	Dictionary _weights;
	Dictionary _fade;
	Array _order;

	void queue_expression(const String expression, const float fade, const bool reverse);

public:
	bool _get(const StringName &p_name, Variant &r_ret) const;
    void _get_property_list(List<godot::PropertyInfo> *p_list);

	Dictionary get_expression_library() const {
		return this->_expression_library;
	}
	void set_expression_library(const Dictionary expressions) {
		this->_expression_library = expressions;
	}
	bool is_activated(const String expression) {
		return (double)this->_weights.get(expression, 0.0) > (double)0.0;
	}
	void reset(const float fade = 0);
    void set_expression(const String expression, const float fade);
	void activate_expression(const String expression, const float fade);
	void deactivate_expression(const String expression, const float fade);

	void apply(Dictionary values, Dictionary parameters);
	void _process(const double delta) override;
};

#endif // GD_CUBISM_EXPRESSION_CONTROLLER
