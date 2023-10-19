# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
extends Control

const MIX_RENDER_SIZE := 32
const MAX_RENDER_SIZE := 2048
const RENDER_SIZE_STEP := 256
const ENABLE_MOTION_FINISHED := true

var cubism_model: GDCubismUserModel
var ary_param: Array
var last_motion = null


func setup(pathname: String):
    cubism_model.assets = pathname
    
    var canvas_info = cubism_model.get_canvas_info()

    var idx: int = 0
    var dict_motion = cubism_model.get_motions()
    $UI/ItemListMotion.clear()
    for k in dict_motion:
        for v in range(dict_motion[k]):
            $UI/ItemListMotion.add_item("{0}_{1}".format([k, v]))
            $UI/ItemListMotion.set_item_metadata(idx, {"group": k, "no": v})
            idx += 1

    $UI/ItemListExpression.clear()
    for item in cubism_model.get_expressions():
        $UI/ItemListExpression.add_item(item)

    cubism_model.playback_process_mode = GDCubismUserModel.IDLE
    $Sprite2D.texture = cubism_model.get_texture()
    var mat = CanvasItemMaterial.new()
    mat.blend_mode = CanvasItemMaterial.BLEND_MODE_PREMULT_ALPHA
    mat.light_mode = CanvasItemMaterial.LIGHT_MODE_UNSHADED
    $Sprite2D.material = mat


func model3_search(dirname: String):

    var dir: DirAccess = DirAccess.open(dirname)
    if dir == null:
        return

    dir.list_dir_begin()
    var name = dir.get_next()
    while name != "":
        if dir.current_is_dir():
            model3_search(dirname.path_join(name))
        else:
            if name.ends_with(".model3.json"):
                print(dirname.path_join(name))
                $UI/OptModel.add_item(dirname.path_join(name))
        name = dir.get_next()


func _ready():
    cubism_model = GDCubismUserModel.new()
    if ENABLE_MOTION_FINISHED == true:
        cubism_model.motion_finished.connect(_on_motion_finished)
    $Sprite2D.add_child(cubism_model)

    $UI/OptModel.clear()
    $UI/OptModel.add_item("")
    model3_search("res://addons/gd_cubism/example/res/live2d")


func _process(delta):
    var vct_resolution = Vector2(get_window().size)        
    var texture_height = floor(vct_resolution.y / RENDER_SIZE_STEP) * RENDER_SIZE_STEP
    texture_height = clampi(texture_height, MIX_RENDER_SIZE, MAX_RENDER_SIZE)

    cubism_model.size = Vector2.ONE * texture_height

    var vct_viewport_size = Vector2(get_viewport_rect().size)
    $Sprite2D.position = vct_viewport_size / 2
    $Sprite2D.scale.x = vct_viewport_size.y / cubism_model.size.y
    $Sprite2D.scale.y = $Sprite2D.scale.x


func _on_motion_finished():
    cubism_model.start_motion(
        last_motion.group,
        last_motion.no,
        GDCubismUserModel.PRIORITY_FORCE
    )


func _on_opt_model_item_selected(index):
    setup($UI/OptModel.get_item_text(index))


func _on_item_list_motion_item_selected(index):
    var motion = $UI/ItemListMotion.get_item_metadata(index)
    var m = cubism_model.start_motion(motion.group, motion.no, GDCubismUserModel.PRIORITY_FORCE)
    last_motion = motion


func _on_item_list_expression_item_selected(index):
    var expression_id = $UI/ItemListExpression.get_item_text(index)
    cubism_model.start_expression(expression_id)
