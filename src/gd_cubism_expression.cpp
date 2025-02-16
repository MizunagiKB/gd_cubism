#include <gd_cubism_expression.hpp>
#include <godot_cpp/classes/file_access.hpp>

void GDCubismExpression::_setup_local_to_scene() {
	auto data = FileAccess::get_file_as_bytes(this->get_path());
	this->internal_expression = CubismExpressionMotion::Create(data.ptrw(), data.size());
}

GDCubismExpression::GDCubismExpression() {
	
}

GDCubismExpression::~GDCubismExpression() {
	if (this->internal_expression != nullptr) {
		//memdelete(this->internal_expression);
	}
}
