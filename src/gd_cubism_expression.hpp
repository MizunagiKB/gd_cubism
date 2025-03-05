#ifndef GD_CUBISM_EXPRESSION
#define GD_CUBISM_EXPRESSION

// ----------------------------------------------------------------- include(s)
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
	GDCubismExpression() {}
    ~GDCubismExpression() {}

	float fade_in_time;
	float fade_out_time;

	Array parameters;

protected:
    static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("get_fade_in_time"), &GDCubismExpression::get_fade_in_time);
        ClassDB::bind_method(D_METHOD("set_fade_in_time"), &GDCubismExpression::set_fade_in_time);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fade_in_time"), "set_fade_in_time", "get_fade_in_time");
		ClassDB::bind_method(D_METHOD("get_fade_out_time"), &GDCubismExpression::get_fade_out_time);
        ClassDB::bind_method(D_METHOD("set_fade_out_time"), &GDCubismExpression::set_fade_out_time);
        ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fade_out_time"), "set_fade_out_time", "get_fade_out_time");
		ClassDB::bind_method(D_METHOD("get_parameters"), &GDCubismExpression::get_parameters);
        ClassDB::bind_method(D_METHOD("set_parameters"), &GDCubismExpression::set_parameters);
        ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "parameters"), "set_parameters", "get_parameters");
	}

public:

	float get_fade_in_time() const {
		return this->fade_in_time;
	}
	void set_fade_in_time(const float fade) {
		this->fade_in_time = fade;
	}
	float get_fade_out_time() const {
		return this->fade_out_time;
	}
	void set_fade_out_time(const float fade) {
		this->fade_out_time = fade;
	}
	Array get_parameters() const {
		return this->parameters;
	}
	void set_parameters(const Array p) {
		this->parameters = p;
	}
};

#endif // GD_CUBISM_EXPRESSION
