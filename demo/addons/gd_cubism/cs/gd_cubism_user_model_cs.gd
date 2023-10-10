# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Object
class_name internal_umcs


# Property
static func set_assets(o: GDCubismUserModel, value: String):
    o.assets = value

static func get_assets(o: GDCubismUserModel) -> String:
    return o.assets


static func set_auto_scale(o: GDCubismUserModel, value: bool):
    o.auto_scale = value

static func get_auto_scale(o: GDCubismUserModel) -> bool:
    return o.auto_scale


static func set_parameter_mode(o: GDCubismUserModel, value: GDCubismUserModel.ParameterMode):
    o.auto_scale = value

static func get_parameter_mode(o: GDCubismUserModel) -> GDCubismUserModel.ParameterMode:
    return o.parameter_mode


static func set_playback_process_mode(o: GDCubismUserModel, value: GDCubismUserModel.MotionProcessCallback):
    o.playback_process_mode = value;

static func get_playback_process_mode(o: GDCubismUserModel) -> GDCubismUserModel.MotionProcessCallback:
    return o.playback_process_mode;


static func set_shader(o: GDCubismUserModel, idx: int, value: Shader):
    match idx:
        0: o.shader_add = value
        1: o.shader_mix = value
        2: o.shader_mul = value
        3: o.shader_mask = value
        4: o.shader_mask_add = value
        5: o.shader_mask_add_inv = value
        6: o.shader_mask_mix = value
        7: o.shader_mask_mix_inv = value
        8: o.shader_mask_mul = value
        9: o.shader_mask_mul_inv = value

static func get_shader(o: GDCubismUserModel, idx: int) -> Shader:
    match idx:
        0: return o.shader_add
        1: return o.shader_mix
        2: return o.shader_mul
        3: return o.shader_mask
        4: return o.shader_mask_add
        5: return o.shader_mask_add_inv
        6: return o.shader_mask_mix
        7: return o.shader_mask_mix_inv
        8: return o.shader_mask_mul
        9: return o.shader_mask_mul_inv
    return Shader.new()


static func set_speed_scale(o: GDCubismUserModel, value: float):
    o.speed_scale = value;

static func get_speed_scale(o: GDCubismUserModel) -> float:
    return o.speed_scale;


# Method
static func advance(o: GDCubismUserModel, delta: float):
    o.advance(delta);


static func clear(o: GDCubismUserModel):
    o.clear();


static func csm_get_latest_moc_version(o: GDCubismUserModel) -> GDCubismUserModel.moc3FileFormatVersion:
    return o.csm_get_latest_moc_version();


static func csm_get_moc_version(o: GDCubismUserModel) -> GDCubismUserModel.moc3FileFormatVersion:
    return o.csm_get_moc_version();


static func csm_get_version(o: GDCubismUserModel) -> Dictionary:
    return o.csm_get_version()


static func get_canvas_info(o: GDCubismUserModel) -> Dictionary:
    return o.get_canvas_info()


static func get_cubism_motion_queue_entries(o: GDCubismUserModel) -> Array:
    return o.get_cubism_motion_queue_entries()


static func get_expressions(o: GDCubismUserModel) -> Array:
    return o.get_expressions()


static func get_hit_areas(o: GDCubismUserModel) -> Array:
    return o.get_hit_areas()


static func get_meshes(o: GDCubismUserModel) -> Dictionary:
    return o.get_meshes()


static func get_motions(o: GDCubismUserModel) -> Dictionary:
    return o.get_motions()
    
    
static func get_parameters(o: GDCubismUserModel) -> Array:
    return o.get_parameters()


static func get_part_opacities(o: GDCubismUserModel) -> Array:
    return o.get_part_opacities()


static func start_expression(o: GDCubismUserModel, expression_id: String):
    o.start_expression(expression_id)


static func start_motion(o: GDCubismUserModel, group: String, no: int, priority: GDCubismUserModel.Priority) -> GDCubismMotionQueueEntryHandle:
    return o.start_motion(group, no, priority)


static func start_motion_loop(o: GDCubismUserModel, group: String, no: int, priority: GDCubismUserModel.Priority, loop: bool, loop_fade_in: bool) -> GDCubismMotionQueueEntryHandle:
    return o.start_motion_loop(group, no, priority, loop, loop_fade_in)


static func stop_expression(o: GDCubismUserModel):
    o.stop_expression()


static func stop_motion(o: GDCubismUserModel):
    o.stop_motion()


#
static func get_texture(o: GDCubismUserModel) -> ViewportTexture:
    return o.get_texture()


#
static func create_object() -> GDCubismUserModel:
    var o = GDCubismUserModel.new()
    return o

