# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends GDCubismEffectCustom


var first_time: bool = true
var counter: float = 0.0
var base_vct: Vector2
var base_scale: Vector2
@export var art_mesh_name: String = "ArtMesh278"
@export var vector_index_0: int = 0
@export var vector_index_1: int = 1


func _on_cubism_process(model: GDCubismUserModel, delta: float):
    var dict_mesh: Dictionary = model.get_meshes()

    if dict_mesh.has(art_mesh_name) == false:
        return

    var ary_mesh: ArrayMesh = dict_mesh[art_mesh_name]
    var ary_surface: Array = ary_mesh.surface_get_arrays(0)
    var mesh_vertex: PackedVector2Array = ary_surface[ArrayMesh.ARRAY_VERTEX]

    if first_time == true:
        var fr = ary_surface[ArrayMesh.ARRAY_VERTEX][vector_index_0]
        var to = ary_surface[ArrayMesh.ARRAY_VERTEX][vector_index_1]
        base_vct = (to - fr)
        base_scale = $Sprite2D.scale
        first_time = false

    var fr = ary_surface[ArrayMesh.ARRAY_VERTEX][vector_index_0]
    var to = ary_surface[ArrayMesh.ARRAY_VERTEX][vector_index_1]
    var calc_vct = (to - fr)
    var scale: float = calc_vct.length() / base_vct.length()

    $Sprite2D.position = fr
    $Sprite2D.rotation = base_vct.normalized().cross(calc_vct.normalized())
    $Sprite2D.scale = base_scale * scale
