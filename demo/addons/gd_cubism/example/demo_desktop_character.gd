# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


var order_window_position: bool = false
var mouse_button_pressed: bool = false
var window_position: Vector2i
var mouse_position: Vector2

var enable_transparent: bool = false
var ary_character_expression: Array
var ary_character_motion: Array


func _ready():

    $Sprite2D.position = Vector2(get_viewport_rect().size / 2)

    ary_character_expression = $Sprite2D/GDCubismUserModel.get_expressions()

    var dict_motion = $Sprite2D/GDCubismUserModel.get_motions()
    for group in dict_motion.keys():
        for no in range(dict_motion[group]):
            ary_character_motion.append({"group": group, "no": no})
    


func _process(delta):

    if order_window_position == true:
        DisplayServer.window_set_position(Vector2i(window_position))
        order_window_position = false


func _input(event):

    if event is InputEventKey:
        if event.is_pressed() == true:

            if event.keycode == KEY_ESCAPE:
                get_tree().quit()

            if event.keycode == KEY_SPACE:
                var motion = ary_character_motion[randi_range(0, ary_character_motion.size() - 1)]
                $Sprite2D/GDCubismUserModel.start_motion_loop(
                    motion.group,
                    motion.no,
                    GDCubismUserModel.PRIORITY_FORCE,
                    true,
                    true
                )
            if event.keycode == KEY_TAB:
                if enable_transparent == true:
                    enable_transparent = false
                else:
                    enable_transparent = true
                get_tree().root.transparent_bg = enable_transparent
                DisplayServer.window_set_flag(DisplayServer.WINDOW_FLAG_TRANSPARENT, enable_transparent, 0)
                DisplayServer.window_set_flag(DisplayServer.WINDOW_FLAG_ALWAYS_ON_TOP, true, 0)

            if event.keycode >= KEY_0 and event.keycode <= KEY_9:
                var index: int = event.keycode - KEY_0
                if index < ary_character_expression.size():
                    $Sprite2D/GDCubismUserModel.start_expression(
                        ary_character_expression[index]
                    )

    if event is InputEventMouseButton:
        if event.pressed == true:
            window_position = DisplayServer.window_get_position()
            mouse_position = get_viewport().get_mouse_position()
        else:
            get_viewport().warp_mouse(mouse_position)
        mouse_button_pressed = event.pressed
                
    if event is InputEventMouseMotion:
        if mouse_button_pressed == true:
            order_window_position = true
            window_position += Vector2i(event.relative)


func _on_button_pressed():
    $AudioStreamPlayer2D.play()
