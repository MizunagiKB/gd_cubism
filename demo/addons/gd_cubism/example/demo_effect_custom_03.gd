# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Node2D



const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

var cubism_model: GDCubismUserModel
var cubism_efx: GDCubismEffectCustom

@export var param_mouth_name: String = "ParamMouthA"
@export var min_db: float = 60.0
@export var min_voice_freq: int = 0
@export var max_voice_freq: int = 3200
@export var history_size: int = 6

@export_category("Audio")
@export var audio_bus_name: String = "Voice"
@export var audio_efx_index: int = 0

var spectrum: AudioEffectSpectrumAnalyzerInstance
var param_mouth: GDCubismParameter
var ary_volume_history: Array
var history_position: int = 0
var lipsync_ready: bool = false


func array_rebuild():
    if history_size != ary_volume_history.size():
        ary_volume_history.resize(history_size)
        ary_volume_history.fill(0.0)


func array_avg() -> float:
    var sum_v: float = 0.0
    for v in ary_volume_history:
        sum_v += v
    return sum_v / ary_volume_history.size()


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
        self.cubism_model.assets = DEFAULT_ASSET


func _process(delta: float) -> void:
    self.recalc_model_position(self.cubism_model)


func _on_cubism_init(model: GDCubismUserModel):
    param_mouth = null
    ary_volume_history.clear()
    history_position = 0

    var ary_param = model.get_parameters()

    for param in ary_param:
        if param.id == param_mouth_name:
            param_mouth = param

    var bus_index = AudioServer.get_bus_index(audio_bus_name)
    spectrum = AudioServer.get_bus_effect_instance(bus_index, audio_efx_index)

    if spectrum == null:
        lipsync_ready = false
    else:
        lipsync_ready = true


func _on_cubism_term(model: GDCubismUserModel):
    lipsync_ready = false
    param_mouth = null
    ary_volume_history.clear()
    history_position = 0


func _on_cubism_process(model: GDCubismUserModel, delta: float):
    if lipsync_ready == false:
        return

    array_rebuild()

    var m: float = spectrum.get_magnitude_for_frequency_range(
        min_voice_freq,
        max_voice_freq
    ).length()
    var v = clamp((min_db + linear_to_db(m)) / min_db, 0.0, 1.0)

    ary_volume_history[history_position] = v
    history_position += 1

    if history_position >= ary_volume_history.size():
        history_position = 0

    var avg_v: float = array_avg()

    if param_mouth != null:
        param_mouth.value = max(param_mouth.value - 0.2, 0.0)
        if avg_v > 0.1:
            param_mouth.value = 1.0
