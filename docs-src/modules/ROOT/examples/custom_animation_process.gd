extends Node2D


var param_valid: bool = false
var l_eye: GDCubismParameter
var r_eye: GDCubismParameter


func _ready():
    pass


func _process(delta):
    var model: GDCubismUserModel = $Sprite2D/GDCubismUserModel

    for _o in model.get_parameters():
        var o: GDCubismParameter = _o
        if o.id == "ParamEyeLOpen":
            o.value = 0.0
        if o.id == "ParamEyeROpen":
            o.value = 0.0

