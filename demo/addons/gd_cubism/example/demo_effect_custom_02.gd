# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

var cubism_model: GDCubismUserModel
var cubism_efx: GDCubismEffectCustom

var first_time: bool = true
var counter: float = 0.0
var base_vct: Vector2
var base_scale: Vector2
@export var art_mesh_name: String = "ArtMesh278"
@export var vector_index_0: int = 0
@export var vector_index_1: int = 1


func recalc_model_position(model: GDCubismUserModel) -> void:
    if model.assets == "":
        return

    var canvas_info: Dictionary = model.get_canvas_info()

    if canvas_info.is_empty() != true:
        var vct_viewport_size = Vector2(get_viewport_rect().size)
        var scale: float = vct_viewport_size.y / max(canvas_info.size_in_pixels.x, canvas_info.size_in_pixels.y)
        model.position = vct_viewport_size / 2.0
        model.scale = Vector2(scale, scale)


func _ready() -> void:
    self.cubism_efx = $GDCubismUserModel/GDCubismEffectCustom
    # self.cubism_efx.connect("cubism_prologue", self._on_cubism_prologue)
    # self.cubism_efx.connect("cubism_epilogue", self._on_cubism_epilogue)
    # self.cubism_efx.connect("cubism_init", self._on_cubism_init)
    # self.cubism_efx.connect("cubism_term", self._on_cubism_term)
    self.cubism_efx.connect("cubism_process", self._on_cubism_process)

    self.cubism_model = $GDCubismUserModel
    if self.cubism_model.assets == "":
        self.cubism_model.assets = DEFAULT_ASSET;

    var dict_motion: Dictionary = self.cubism_model.get_motions()

    for k in dict_motion.keys():
        self.cubism_model.start_motion_loop(k, 0, GDCubismUserModel.PRIORITY_FORCE, true, true);
        break


func _process(delta: float) -> void:
    self.recalc_model_position(self.cubism_model)


func _on_cubism_process(model: GDCubismUserModel, delta: float):
    var ref_sprite: Sprite2D = $GDCubismUserModel/GDCubismEffectCustom/Sprite2D
    var dict_mesh: Dictionary = model.get_meshes()

    if dict_mesh.has(art_mesh_name) == false:
        return

    var ary_mesh: MeshInstance2D = dict_mesh[self.art_mesh_name]
    var ary_surface: Array = ary_mesh.mesh.surface_get_arrays(0)
    var mesh_vertex: PackedVector2Array = ary_surface[ArrayMesh.ARRAY_VERTEX]

    if self.first_time == true:
        var fr = ary_surface[ArrayMesh.ARRAY_VERTEX][self.vector_index_0]
        var to = ary_surface[ArrayMesh.ARRAY_VERTEX][self.vector_index_1]
        self.base_vct = (to - fr)
        self.base_scale = ref_sprite.scale
        self.first_time = false

    var fr = ary_surface[ArrayMesh.ARRAY_VERTEX][self.vector_index_0]
    var to = ary_surface[ArrayMesh.ARRAY_VERTEX][self.vector_index_1]
    var calc_vct = (to - fr)
    var scale: float = calc_vct.length() / base_vct.length()

    ref_sprite.position = model.to_global(fr)
    ref_sprite.rotation = base_vct.normalized().cross(calc_vct.normalized())
    ref_sprite.scale = base_scale * scale
