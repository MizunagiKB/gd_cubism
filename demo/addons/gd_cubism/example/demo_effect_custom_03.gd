# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json"

var pressed: bool = false


func _ready():
    
    if $Sprite2D/GDCubismUserModel.assets == "":
        $Sprite2D/GDCubismUserModel.assets = DEFAULT_ASSET


func _process(delta):
    pass


func _input(event):

    if event as InputEventMouseButton:
        pressed = event.is_pressed()

    if event as InputEventMouseMotion:
        if pressed == true:
            # Convert to local position.
            var local_pos = $Sprite2D.to_local(event.position)
            # Adjust position.
            var render_size: Vector2 = Vector2(
                float($Sprite2D/GDCubismUserModel.size.x) * $Sprite2D.scale.x,
                float($Sprite2D/GDCubismUserModel.size.y) * $Sprite2D.scale.y * -1.0
            )
            local_pos /= (render_size * 0.5)
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_target(local_pos)
        else:
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_target(Vector2.ZERO)
