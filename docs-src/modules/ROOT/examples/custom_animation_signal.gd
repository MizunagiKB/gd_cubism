extends Node2D


var param_valid: bool = false
var l_eye: GDCubismParameter
var r_eye: GDCubismParameter


func _ready():
    pass


func _process(delta):
    pass


func _on_gd_cubism_effect_custom_cubism_init(model: GDCubismUserModel):
    for _o in model.get_parameters():
        var o: GDCubismParameter = _o
        if o.id == "ParamEyeLOpen":
            l_eye = o
        if o.id == "ParamEyeROpen":
            r_eye = o
    param_valid = true


func _on_gd_cubism_effect_custom_cubism_term(model):
    param_valid = false


func _on_gd_cubism_effect_custom_cubism_prologue(model, delta):
    pass


func _on_gd_cubism_effect_custom_cubism_process(model: GDCubismUserModel, delta: float):
    if param_valid == true:
        l_eye.value = 0.0
        r_eye.value = 0.0


func _on_gd_cubism_effect_custom_cubism_epilogue(model, delta):
    pass
