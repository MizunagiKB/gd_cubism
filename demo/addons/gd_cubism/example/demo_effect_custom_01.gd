# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json"

var cubism_model: GDCubismUserModel
var cubism_efx: GDCubismEffectCustom

enum E_PARAM_MODE {
    PROCESS,
    SIGNAL
}

var param_valid: bool = false
var param_mode: E_PARAM_MODE = E_PARAM_MODE.PROCESS
var l_eye: GDCubismParameter
var r_eye: GDCubismParameter


func recalc_model_position(model: GDCubismUserModel):
    if model.assets == "":
        return

    var canvas_info: Dictionary = model.get_canvas_info()

    if canvas_info.is_empty() != true:
        var vct_viewport_size = Vector2(get_viewport_rect().size)
        var scale: float = vct_viewport_size.y / max(canvas_info.size_in_pixels.x, canvas_info.size_in_pixels.y)
        model.position = vct_viewport_size / 2.0
        model.scale = Vector2(scale, scale)


func _ready():
    self.cubism_efx = $GDCubismUserModel/GDCubismEffectCustom
    self.cubism_efx.connect("cubism_prologue", self._on_cubism_prologue)
    self.cubism_efx.connect("cubism_epilogue", self._on_cubism_epilogue)
    self.cubism_efx.connect("cubism_init", self._on_cubism_init)
    self.cubism_efx.connect("cubism_term", self._on_cubism_term)
    self.cubism_efx.connect("cubism_process", self._on_cubism_process)

    self.cubism_model = $GDCubismUserModel
    if self.cubism_model.assets == "":
        self.cubism_model.assets = DEFAULT_ASSET;


func _process(delta):
    recalc_model_position($GDCubismUserModel)

    if param_mode == E_PARAM_MODE.PROCESS:
        var model: GDCubismUserModel = $GDCubismUserModel

        for _o in self.cubism_model.get_parameters():
            var o: GDCubismParameter = _o
            if o.id == "ParamEyeLOpen":
                o.value = 0.0
            if o.id == "ParamEyeROpen":
                o.value = 0.0


func _on_cubism_init(model: GDCubismUserModel):
    if param_mode == E_PARAM_MODE.SIGNAL:
        for _o in model.get_parameters():
            var o: GDCubismParameter = _o
            if o.id == "ParamEyeLOpen":
                l_eye = o
            if o.id == "ParamEyeROpen":
                r_eye = o
        param_valid = true


func _on_cubism_term(model: GDCubismUserModel):
    if param_mode == E_PARAM_MODE.SIGNAL:
        param_valid = false


func _on_cubism_prologue(model: GDCubismUserModel, delta: float):
    pass # Replace with function body.


func _on_cubism_process(model: GDCubismUserModel, delta: float):
    if param_mode == E_PARAM_MODE.SIGNAL:
        if param_valid == true:
            l_eye.value = 0.0
            r_eye.value = 0.0


func _on_cubism_epilogue(model: GDCubismUserModel, delta: float):
    pass # Replace with function body.
