#include <gd_cubism_expression.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void GDCubismExpression::initialize() {
	if (this->internal_expression != nullptr) return;

	UtilityFunctions::print("loading expression from ", this->get_path());
	ERR_FAIL_COND_MSG(this->get_path().is_empty(), "invalid path for expression");
	PackedByteArray data = FileAccess::get_file_as_bytes(this->get_path());
	this->internal_expression = CubismExpressionMotion::Create(data.ptrw(), data.size());
	ERR_FAIL_COND_MSG(this->internal_expression == nullptr, "Unable to load expression");
}

GDCubismExpression::GDCubismExpression()
	:internal_expression(nullptr) {
	
}

GDCubismExpression::~GDCubismExpression() {
	if (this->internal_expression != nullptr) {
		//CubismExpressionMotion::Delete(this->internal_expression);
	}
}
