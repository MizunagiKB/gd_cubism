# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json"

var pressed: bool = false


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
    if $GDCubismUserModel.assets == "":
        $GDCubismUserModel.assets = DEFAULT_ASSET


func _process(delta):
    self.recalc_model_position($GDCubismUserModel)


func _input(event):
    if event as InputEventMouseButton:
        pressed = event.is_pressed()

    if event as InputEventMouseMotion:
        if pressed == true:
            var calc_pos: Vector2 = $GDCubismUserModel.to_local(event.position) * Vector2(1, -1)
            $GDCubismUserModel/GDCubismEffectTargetPoint.set_target(calc_pos.normalized())
        else:
            $GDCubismUserModel/GDCubismEffectTargetPoint.set_target(Vector2.ZERO)
