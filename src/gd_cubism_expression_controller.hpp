#ifndef GD_CUBISM_EXPRESSION_CONTROLLER
#define GD_CUBISM_EXPRESSION_CONTROLLER

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotionManager.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <gd_cubism_user_model.hpp>
#include <gd_cubism_expression.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

class GDCubismExpressionController : public Node {
    GDCLASS(GDCubismExpressionController, Node);

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_model", "value"), &GDCubismExpressionController::set_model);
    	ClassDB::bind_method(D_METHOD("get_model"), &GDCubismExpressionController::get_model);
		ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "model", PROPERTY_HINT_NODE_TYPE, "GDCubismUserModel"), "set_model", "get_model");

		ClassDB::bind_method(D_METHOD("set_expression", "value"), &GDCubismExpressionController::set_expression);
    	ClassDB::bind_method(D_METHOD("get_expression"), &GDCubismExpressionController::get_expression);
		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "active_expression", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismExpression"), "set_expression", "get_expression");
		
		ClassDB::bind_method(D_METHOD("set_expression_library", "value"), &GDCubismExpressionController::set_expression_library);
    	ClassDB::bind_method(D_METHOD("get_expression_library"), &GDCubismExpressionController::get_expression_library);
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "expression_library"), "set_expression_library", "get_expression_library");
	}

private:
	CubismExpressionMotionManager *internal_manager;
	GDCubismUserModel *model = nullptr;
	Array expression_library;
	Ref<GDCubismExpression> active_expression;
	
public:
    GDCubismExpressionController();
    ~GDCubismExpressionController();

	void set_model(GDCubismUserModel *m) { this->model = m; }
	GDCubismUserModel* get_model() { return this->model; }

	void set_expression_library(Array expressions) { this->expression_library = expressions; };
	Array get_expression_library() { return this->expression_library; };

	void set_expression(Ref<GDCubismExpression> expression);
	Ref<GDCubismExpression> get_expression() { return this->active_expression; };

	void _process(double delta) override;
	void _enter_tree() override;
	void _exit_tree() override;
};

#endif // GD_CUBISM_EXPRESSION_CONTROLLER
