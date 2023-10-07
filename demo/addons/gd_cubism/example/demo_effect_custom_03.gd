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
            var local_pos = $Sprite2D.to_local(event.position)
            local_pos.x /= float($Sprite2D/GDCubismUserModel.size.x) * $Sprite2D.scale.x
            local_pos.y /= float($Sprite2D/GDCubismUserModel.size.y) * $Sprite2D.scale.y
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_position(local_pos)
        else:
            $Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint.set_position(Vector2.ZERO)
