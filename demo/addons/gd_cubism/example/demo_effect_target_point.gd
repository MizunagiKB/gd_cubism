# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json"

var pressed: bool = false


func _ready():
    if $GDCubismUserModel.assets == "":
        $GDCubismUserModel.assets = DEFAULT_ASSET


func _process(delta):
    pass


func _input(event):
    if event as InputEventMouseButton:
        pressed = event.is_pressed()

    if event as InputEventMouseMotion:
        if pressed == true:
            var calc_pos: Vector2 = $GDCubismUserModel.to_local(event.position) * Vector2(1, -1)
            $GDCubismUserModel/GDCubismEffectTargetPoint.set_target(calc_pos.normalized())
        else:
            $GDCubismUserModel/GDCubismEffectTargetPoint.set_target(Vector2.ZERO)
