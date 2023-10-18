# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json"
const CONVEX_MESH_SRC: Array[String] = [
    "ArtMesh121",
    "ArtMesh122",
    "ArtMesh135",
    "ArtMesh146",
    "ArtMesh147",
    "ArtMesh231",
    "ArtMesh278"
]
var order_window_position: bool = false
var mouse_button_pressed: bool = false
var window_position: Vector2i
var mouse_position: Vector2

var enable_transparent: bool = false
var ary_character_expression: Array
var ary_character_motion: Array


# ---------------------------------------------------------------------- ported
# I have ported the code for check_cross and convex_hull to GDScript.
# * https://tjkendev.github.io/procon-library/
# * https://tjkendev.github.io/procon-library/cpp/geometry/graham_scan.html

func check_cross(ary_check: Array, vtx: Vector2) -> bool:
    var va: Vector2 = ary_check[ary_check.size() - 2]
    var vb: Vector2 = ary_check[ary_check.size() - 1]

    return (((vb.x - va.x) * (vtx.y - va.y)) - ((vb.y - va.y) * (vtx.x - va.x))) > 0


func convex_hull(ary_vertex: Array) -> Array:
    ary_vertex.sort()

    var ary_result: Array
    var n: int = ary_vertex.size()
    
    for vtx in ary_vertex:
        while ary_result.size() > 1 and check_cross(ary_result, vtx):
            ary_result.pop_back()
        ary_result.push_back(vtx)

    var t: int = ary_result.size()

    var i: int = n - 2
    while i >= 0:
        var vtx: Vector2 = ary_vertex[i]
        while ary_result.size() > t and check_cross(ary_result, vtx):
            ary_result.pop_back()
        ary_result.push_back(vtx)

        i -= 1

    ary_result.pop_back()

    return ary_result

# ---------------------------------------------------------------------- ported


func _ready():

    if $Sprite2D/GDCubismUserModel.assets == "":
        $Sprite2D/GDCubismUserModel.assets = DEFAULT_ASSET

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

    var dict_mesh = $Sprite2D/GDCubismUserModel.get_meshes()
    var ary: PackedVector2Array
    for name in CONVEX_MESH_SRC:
        ary += dict_mesh[name].surface_get_arrays(0)[Mesh.ARRAY_VERTEX]
    var ary_polygon = convex_hull(ary)

    if ary_polygon.size() > 2:
        var region: PackedVector2Array
        var adjust: Vector2 = get_viewport_rect().size
        adjust -= Vector2($Sprite2D/GDCubismUserModel.size.x, $Sprite2D/GDCubismUserModel.size.y)
        adjust *= 0.5
        for v in ary_polygon:
            region.append(v + adjust)

        $Polygon2D.polygon = PackedVector2Array(region)


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
                    DisplayServer.window_set_mouse_passthrough([])
                else:
                    enable_transparent = true
                    DisplayServer.window_set_mouse_passthrough($Polygon2D.polygon)    
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

