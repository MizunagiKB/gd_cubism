extends Node2D


const DEFAULT_ASSET: String = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json"

var pressed: bool = false
var local_pos: Vector2 = Vector2.ZERO
var adjust_pos: Vector2 = Vector2.ZERO
var dict_detail: Dictionary
var dict_pickup: Dictionary
var id_mesh: String = ""


func _ready():

    if $Sprite2D/GDCubismUserModel.assets == "":
        $Sprite2D/GDCubismUserModel.assets = DEFAULT_ASSET

    $Control/ItemList.clear()
    for id in $Sprite2D/GDCubismUserModel.get_meshes():
        $Control/ItemList.add_item(id)


func _process(delta):

    if $Sprite2D.centered == true:
        adjust_pos = ($Sprite2D/GDCubismUserModel.size * 0.5)

    if pressed == true:
        $Sprite2D/GDCubismUserModel/GDCubismEffectHitArea.set_target(local_pos)

    $Control/lbl_mouse_x.text = "%6.3f" % local_pos.x
    $Control/lbl_mouse_y.text = "%6.3f" % local_pos.y

    if id_mesh != "":
        dict_pickup = $Sprite2D/GDCubismUserModel/GDCubismEffectHitArea.get_detail(
            $Sprite2D/GDCubismUserModel,
            id_mesh
        )

    $Sprite2D/Canvas.queue_redraw()


func _input(event):

    if event as InputEventMouseButton:
        pressed = event.is_pressed()

    if event as InputEventMouseMotion:
        local_pos = $Sprite2D.to_local(event.position) + adjust_pos


func mark_hit_area(dict_hit_area: Dictionary, color_box: Color, color_tri: Color):

    if dict_hit_area.is_empty() == false:
        var r: Rect2 = Rect2(dict_hit_area.rect.position - adjust_pos, dict_hit_area.rect.size)
        $Sprite2D/Canvas.draw_rect(r, color_box, false, 5)

        if dict_hit_area.has("vertices") == true:
            var v = dict_hit_area.vertices
            $Sprite2D/Canvas.draw_line(v[0] - adjust_pos, v[1] - adjust_pos, color_tri, 3)
            $Sprite2D/Canvas.draw_line(v[1] - adjust_pos, v[2] - adjust_pos, color_tri, 3)
            $Sprite2D/Canvas.draw_line(v[2] - adjust_pos, v[0] - adjust_pos, color_tri, 3)


func _on_gd_cubism_effect_hit_area_hit_area_entered(model, id):
    dict_detail = $Sprite2D/GDCubismUserModel/GDCubismEffectHitArea.get_detail(model, id)
    $Control/lbl_id.text = id


func _on_gd_cubism_effect_hit_area_hit_area_exited(model, id):
    dict_detail = {}
    $Control/lbl_id.text = ""


func _on_canvas_draw():
    mark_hit_area(dict_detail, Color.GREEN, Color.RED)
    mark_hit_area(dict_pickup, Color.BLUE, Color.RED)


func _on_item_list_item_selected(index):
    id_mesh = $Control/ItemList.get_item_text(index)

