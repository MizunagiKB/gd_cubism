extends Node2D


enum E_PARAM_MODE {
    PROCESS,
    SIGNAL
}

var param_valid: bool = false
var param_mode: E_PARAM_MODE = E_PARAM_MODE.PROCESS
var l_eye: GDCubismParameter
var r_eye: GDCubismParameter


func _ready():
    pass


func _process(delta):
    if param_mode == E_PARAM_MODE.PROCESS:
        var model: GDCubismUserModel = $Sprite2D/GDCubismUserModel

        for _o in model.get_parameters():
            var o: GDCubismParameter = _o
            if o.id == "ParamEyeLOpen":
                o.value = 0.0
            if o.id == "ParamEyeROpen":
                o.value = 0.0


func _on_gd_cubism_effect_custom_cubism_init(model: GDCubismUserModel):
    if param_mode == E_PARAM_MODE.SIGNAL:
        for _o in model.get_parameters():
            var o: GDCubismParameter = _o
            if o.id == "ParamEyeLOpen":
                l_eye = o
            if o.id == "ParamEyeROpen":
                r_eye = o
        param_valid = true


func _on_gd_cubism_effect_custom_cubism_term(model):
    if param_mode == E_PARAM_MODE.SIGNAL:
        param_valid = false


func _on_gd_cubism_effect_custom_cubism_prologue(model, delta):
    pass # Replace with function body.


func _on_gd_cubism_effect_custom_cubism_process(model: GDCubismUserModel, delta: float):
    if param_mode == E_PARAM_MODE.SIGNAL:
        if param_valid == true:
            l_eye.value = 0.0
            r_eye.value = 0.0


func _on_gd_cubism_effect_custom_cubism_epilogue(model, delta):
    pass # Replace with function body.
