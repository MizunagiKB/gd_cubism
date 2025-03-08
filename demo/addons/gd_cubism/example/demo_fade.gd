# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2025 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json"


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

    $CheckBox.button_pressed = $GDCubismUserModel.pose_update


func _process(delta: float) -> void:
    pass


func _on_h_slider_value_changed(value: float) -> void:
    $Sprite2D.modulate = Color(1.0, 1.0, 1.0, value / 100.0)

    for o in $GDCubismUserModel.get_part_opacities():
        o.value = value / 100.0


func _on_check_box_toggled(toggled_on: bool) -> void:
    $GDCubismUserModel.pose_update = toggled_on
