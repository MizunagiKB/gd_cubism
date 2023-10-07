# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


var pressed: bool = false


func _ready():
    pass


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
            ) * 0.5
            local_pos /= render_size
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_target(local_pos)
        else:
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_target(Vector2.ZERO)
