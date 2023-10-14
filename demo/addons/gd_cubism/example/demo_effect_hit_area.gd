extends Node2D


var pressed: bool = false
var local_pos: Vector2 = Vector2.ZERO
var adjust_pos: Vector2 = Vector2.ZERO
var dict_detail: Dictionary


func _ready():
    pass


func _process(delta):

    if $Sprite2D.centered == true:
        adjust_pos = ($Sprite2D/GDCubismUserModel.size * 0.5)

    if pressed == true:
        $Sprite2D/GDCubismUserModel/GDCubismEffectHitArea.set_target(local_pos)

    $Control/lbl_mouse_x.text = "%6.3f" % local_pos.x
    $Control/lbl_mouse_y.text = "%6.3f" % local_pos.y

    $Sprite2D/Canvas.queue_redraw()


func _input(event):

    if event as InputEventMouseButton:
        pressed = event.is_pressed()

    if event as InputEventMouseMotion:
        local_pos = $Sprite2D.to_local(event.position) + adjust_pos


func _on_gd_cubism_effect_hit_area_hit_area_entered(model, id):
    dict_detail = $Sprite2D/GDCubismUserModel/GDCubismEffectHitArea.get_detail(model, id)
    $Control/lbl_id.text = id


func _on_gd_cubism_effect_hit_area_hit_area_exited(model, id):
    dict_detail = {}
    $Control/lbl_id.text = ""


func _on_canvas_draw():
    if dict_detail.is_empty() == false:
        var r: Rect2 = Rect2(dict_detail.rect.position - adjust_pos, dict_detail.rect.size)
        $Sprite2D/Canvas.draw_rect(r, Color.GREEN, false, 5)

        if dict_detail.has("vertices") == true:
            var v = dict_detail.vertices
            $Sprite2D/Canvas.draw_line(v[0] - adjust_pos, v[1] - adjust_pos, Color.RED, 3)
            $Sprite2D/Canvas.draw_line(v[1] - adjust_pos, v[2] - adjust_pos, Color.RED, 3)
            $Sprite2D/Canvas.draw_line(v[2] - adjust_pos, v[0] - adjust_pos, Color.RED, 3)

