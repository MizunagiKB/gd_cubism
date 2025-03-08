#include <gd_cubism_expression_controller.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void GDCubismExpressionController::_ready() {
	this->_queue = Array();
	this->_queue.resize(2);

	Dictionary x = Dictionary();
	x["fade"] = 0.0;
	x["add"] = Dictionary();
	x["mul"] = Dictionary();
	x["rpl"] = Dictionary();

	this->_queue[0] = x;
	this->_queue[1] = x;
}

void GDCubismExpressionController::set_mode(bool exclusive) {

}

void GDCubismExpressionController::queue_expression(float fade) {
	Dictionary _add;
	Dictionary _mul;
	Dictionary _rpl;

	Array expressions = this->_expression_library.keys();
	for (int i = 0; i < expressions.size(); i++) {
		String e = expressions[i];
		GDCubismExpression *ex = Object::cast_to<GDCubismExpression>(_expression_library[e]);
		bool is_activated = _activated_expressions.get(e, false);

		if (!is_activated) continue;

		Array effect = ex->get_parameters();
		for (int i = 0; i < effect.size(); i++) {
			Dictionary e = effect[i];

			String id = e["Id"];
			String blend = e["Blend"];
			float v = (float)e["Value"];
			if (blend == "Add") {
				_add[e["Id"]] = (float)_add.get_or_add(id, 0.0) + v;
			} else if (blend == "Multiply") {
				_mul[id] = (float)_mul.get_or_add(id, 0.0) + v;
			} else if (blend == "Overwrite") {
				_rpl[id] = v;
			}
		}
	}

	Dictionary x;
	x["fade"] = fade;
	x["add"] = _add;
	x["mul"] = _mul;
	x["rpl"] = _rpl;

	_queue[0] = _queue[1];
	_queue[1] = x;
	this->fade_time = 0;
}

void GDCubismExpressionController::deactivate_expression(String expression_name, float fade) {
	GDCubismExpression *expression = Object::cast_to<GDCubismExpression>(this->_expression_library.get(expression_name, nullptr));
	if (expression == nullptr) return;
	if (!this->is_activated(expression_name)) return;

	this->_activated_expressions[expression_name] = false;

	this->queue_expression(fade);
}

void GDCubismExpressionController::activate_expression(String expression_name, float fade) {
	UtilityFunctions::print("activating expression ", expression_name);
	GDCubismExpression *expression = Object::cast_to<GDCubismExpression>(this->_expression_library.get(expression_name, nullptr));
	if (expression == nullptr) return;

	if (this->is_activated(expression_name)) return;

	this->_activated_expressions[expression_name] = true;

	this->queue_expression(fade);
}

void GDCubismExpressionController::update(CubismModel *model, float delta) {
	// evaluate
	Dictionary now = this->_queue[0];
	Dictionary next = this->_queue[1];
	this->fade_time += delta;
		
	float progress = ((float)next.get("fade", 0.0) <= 0.0)
		? 1.0
		: Math::clamp(
			Math::inverse_lerp(0.0, next.get("fade", 0.0), this->fade_time), 
			0.0, 1.0
		);

	for (int i = 0; i < model->GetParameterCount(); i++) {
		auto p = model->GetParameterId(i);
		String p_name = p->GetString().GetRawString();
		float v = model->GetParameterValue(i);

		float add = Math::lerp(
			((Dictionary)now["add"]).get(p_name, 0.0),
			((Dictionary)next["add"]).get(p_name, 0.0),
			progress
		);
		float mul = Math::lerp(
			((Dictionary)now["mul"]).get(p_name, 1.0),
			((Dictionary)next["mul"]).get(p_name, 1.0),
			progress
		);
		float rpl = Math::lerp(
			((Dictionary)now["rpl"]).get(p_name, v),
			((Dictionary)next["rpl"]).get(p_name, v),
			progress
		);
		
		model->SetParameterValue(
			p,
			(rpl != v) 
				? rpl
				: (v + add) * mul
		);
	}
}