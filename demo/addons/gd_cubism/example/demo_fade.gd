# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2025 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json"

var color_h: float = 0.0


func recalc_model_position(model: GDCubismUserModel):
    if model.assets == "":
        return

    var canvas_info: Dictionary = model.get_canvas_info()

    if canvas_info.is_empty() != true:
        var vct_viewport_size = Vector2(get_viewport_rect().size)
        var scale: float = vct_viewport_size.y / max(canvas_info.size_in_pixels.x, canvas_info.size_in_pixels.y)
        model.position = vct_viewport_size / 2.0
        model.scale = Vector2(scale, scale)


func _ready() -> void:
    if $GDCubismUserModel.assets == "":
        $GDCubismUserModel.assets = DEFAULT_ASSET

    if $Sprite2D/SubViewport/GDCubismUserModel.assets == "":
        $Sprite2D/SubViewport/GDCubismUserModel.assets = DEFAULT_ASSET


func _process(delta: float) -> void:
    color_h += (delta * 0.25);
    
    if color_h > 360.0:
        color_h -= 360.0

    var color: Color = Color.from_hsv(color_h, 1.0, 1.0, $HSlider.value / 100.0)

    $GDCubismUserModel.modulate = color
    $Sprite2D.modulate = color
