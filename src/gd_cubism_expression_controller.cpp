#include <gd_cubism_expression_controller.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/time.hpp>

void GDCubismExpressionController::queue_expression(String expression_name, const float fade, const bool reverse) {
	double start = Time::get_singleton()->get_unix_time_from_system();
	double end = start + fade;

	Dictionary fade_time;
	fade_time["start"] = start;
	fade_time["end"] = end;
	fade_time["reverse"] = reverse;
	this->_fade[expression_name] = fade_time;
}

void GDCubismExpressionController::reset(float fade) {
	for (int i = 0; i < this->_order.size(); i++) {
		this->queue_expression(this->_order[i], fade, true);
	}
}

void GDCubismExpressionController::set_expression(String expression_name, float fade) {
	GDCubismExpression *expression = Object::cast_to<GDCubismExpression>(this->_expression_library.get(expression_name, nullptr));
	if (expression == nullptr) return;
	if (!this->is_activated(expression_name)) return;

	// clears all other expressions
	this->_order.clear();
	this->_fade.clear();

	this->queue_expression(expression_name, fade, false);
	this->_order.append(expression_name);
}

void GDCubismExpressionController::deactivate_expression(const String expression_name, const float fade) {
	GDCubismExpression *expression = Object::cast_to<GDCubismExpression>(this->_expression_library.get(expression_name, nullptr));
	if (expression == nullptr) return;
	if (!this->is_activated(expression_name)) return;

	this->queue_expression(expression_name, fade, true);
}

void GDCubismExpressionController::activate_expression(const String expression_name, const float fade) {
	GDCubismExpression *expression = Object::cast_to<GDCubismExpression>(this->_expression_library.get(expression_name, nullptr));
	if (expression == nullptr) return;
	if (this->is_activated(expression_name)) return;

	this->queue_expression(expression_name, fade, false);
	this->_order.append(expression_name);
}

bool GDCubismExpressionController::_get(const StringName &p_name, Variant &r_ret) const {
	if (this->_expression_library.has(p_name)) {
		r_ret = this->_weights.get(p_name, 0);
		return true;
	}
	
	return false;
}

void GDCubismExpressionController::_get_property_list(List<godot::PropertyInfo> *p_list) {
    // Property - Parameter
    p_list->push_back(PropertyInfo(Variant::STRING, "Expressions", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_GROUP));

	Array ary_parameter = this->_expression_library.keys();
	for(int64_t index = 0; index < ary_parameter.size(); index++) {
		PropertyInfo pinfo(
			Variant::FLOAT,
			ary_parameter[index],
			PROPERTY_HINT_RANGE,
			String("0,1"),
			PROPERTY_USAGE_DEFAULT
		);

		p_list->push_back(pinfo);
	}
}

void GDCubismExpressionController::apply(Dictionary values, Dictionary parameters) {
// evaluate
	double now = Time::get_singleton()->get_unix_time_from_system();

	// apply expressions to parameters, with the parameter value being the set by the most recently activated expression
	Dictionary modified;

	Array order = this->_order.duplicate();
	
	for (int i = 0, n = 0; i < order.size(); i++) {
		String exp_name = order[i];
		GDCubismExpression *exp = Object::cast_to<GDCubismExpression>(this->_expression_library[exp_name]);

		double progress;
		Dictionary fade = this->_fade[exp_name];
		progress = Math::clamp(Math::inverse_lerp(fade["start"], fade["end"], now), 0.0, 1.0);
		if ((bool)fade["reverse"] == true) {
			progress = 1.0 - progress;
		}
		this->_weights[exp_name] = progress;

		Array ary_parameters = exp->get_parameters();

		for (int i = 0; i < ary_parameters.size(); i++) {
			Dictionary e = ary_parameters[i];
			String p_name = e["Id"];
			String blend = e["Blend"];
			float amount = e["Value"];
			Dictionary param = parameters[p_name];
			
			float v = values.get(p_name, (float)param["default"]);
			float to_v = v;

			if (blend == "Add") {
				to_v = v + amount;
			} else if (blend == "Multiply") {
				to_v = v * (1.0f + (amount - 1.0f));
			} else if (blend == "Overwrite") {
				to_v = amount;
			} else {
				continue;
			}

			float out = Math::lerp(
				v,
				to_v,
				(float)progress
			);

			// prevent emotion controller from having a permanent affect on the parameter
			modified[p_name] = out;
		}

		// dequeue after reversal complete
		if (fade["reverse"] && progress <= 0.0) {
			this->_order.pop_at(n);
		} else {
			n++;
		}
	}

	values.merge(modified, true);
}

void GDCubismExpressionController::_process(double delta) {
	Node *model = this->get_parent();
	Dictionary parameters = model->get("parameters");
	if (parameters.is_empty()) return;

	Dictionary values;
	Array keys = parameters.keys();
	for (int i = 0; i < keys.size(); i++) {
		String p_name = keys[i];
		values[p_name] = (float)model->get(p_name);
	}

	this->apply(values, parameters);

	for (int i = 0; i < keys.size(); i++) {
		String p_name = keys[i];
		model->set(p_name, (float)values[p_name]);
	}
}