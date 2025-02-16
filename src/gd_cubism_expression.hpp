#ifndef GD_CUBISM_EXPRESSION
#define GD_CUBISM_EXPRESSION

// ----------------------------------------------------------------- include(s)
#include <CubismFramework.hpp>
#include <Motion/CubismExpressionMotion.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/resource.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;

class GDCubismExpression : public Resource {
	GDCLASS(GDCubismExpression, Resource);

public:
	GDCubismExpression();
    ~GDCubismExpression();

	CubismExpressionMotion *internal_expression;

	void _setup_local_to_scene() override;

protected:
    static void _bind_methods() {

	}
};

#endif // GD_CUBISM_EXPRESSION
