// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using System;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_effect_hit_area : Node2D
{
    const String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json";

    bool pressed = false;
    Vector2 local_pos = new(0.0f, 0.0f);
    Vector2 adjust_pos = new(0.0f, 0.0f);
    Godot.Collections.Dictionary dict_detail = new();
    Godot.Collections.Dictionary dict_pickup = new();
    String id_mesh = "";


    public override void _Ready()
    {
        GDCubismUserModelCS model = new(GetNode<SubViewport>("Sprite2D/GDCubismUserModel"));
        if (model.Assets == "") model.Assets = DEFAULT_ASSET;

        GetNode<ItemList>("Control/ItemList").Clear();
        Godot.Collections.Dictionary dict_meshes = model.GetMeshes();
        foreach (var id in dict_meshes.Keys)
        {
            GetNode<ItemList>("Control/ItemList").AddItem((String)id);
        }
    }

    public override void _Process(double delta)
    {
        GDCubismUserModelCS model = new(GetNode<SubViewport>("Sprite2D/GDCubismUserModel"));
        GDCubismEffectHitAreaCS hit_area = new(GetNode<Node>("Sprite2D/GDCubismUserModel/GDCubismEffectHitArea"));

        if (GetNode<Sprite2D>("Sprite2D").Centered == true)
        {
            this.adjust_pos = new Vector2(model.Size.X * 0.5f, model.Size.Y * 0.5f);
        }

        if (this.pressed == true)
        {
            hit_area.SetTarget(this.local_pos);
        }

        GetNode<Label>("Control/lbl_mouse_x").Text = String.Format("{0:######.###}", this.local_pos.X);
        GetNode<Label>("Control/lbl_mouse_y").Text = String.Format("{0:######.###}", this.local_pos.Y);

        if (this.id_mesh != "")
        {
            this.dict_pickup = hit_area.GetDetail(model, this.id_mesh);
        }

        GetNode<CanvasItem>("Sprite2D/Canvas").QueueRedraw();
    }

    public override void _Input(InputEvent @event)
    {

        if (@event is InputEventMouseButton mouseButton)
        {
            this.pressed = mouseButton.IsPressed();
        }

        if (@event is InputEventMouseMotion mouseMotion)
        {
            this.local_pos = GetNode<Sprite2D>("Sprite2D").ToLocal(mouseMotion.Position) + this.adjust_pos;
        }
    }

    private void mark_hit_area(Godot.Collections.Dictionary dict_hit_area, Color color_box, Color color_tri)
    {
        if (dict_hit_area.Count > 0)
        {
            Rect2 rect = (Rect2)dict_hit_area["rect"];

            Rect2 r = new(rect.Position - this.adjust_pos, rect.Size);
            GetNode<CanvasItem>("Sprite2D/Canvas").DrawRect(r, color_box, false, 5);

            if (dict_hit_area.ContainsKey("vertices") == true)
            {
                Godot.Collections.Array v = (Godot.Collections.Array)dict_hit_area["vertices"];

                GetNode<CanvasItem>("Sprite2D/Canvas").DrawLine((Vector2)(v[0]) - this.adjust_pos, (Vector2)(v[1]) - adjust_pos, color_tri, 3);
                GetNode<CanvasItem>("Sprite2D/Canvas").DrawLine((Vector2)(v[1]) - this.adjust_pos, (Vector2)(v[2]) - adjust_pos, color_tri, 3);
                GetNode<CanvasItem>("Sprite2D/Canvas").DrawLine((Vector2)(v[2]) - this.adjust_pos, (Vector2)(v[0]) - adjust_pos, color_tri, 3);
            }
        }
    }

    private void _on_gd_cubism_effect_hit_area_hit_area_entered(SubViewport _model, String id)
    {
        GDCubismUserModelCS model = new(_model);
        GDCubismEffectHitAreaCS hit_area = new(GetNode<Node>("Sprite2D/GDCubismUserModel/GDCubismEffectHitArea"));

        this.dict_detail = hit_area.GetDetail(model, id);
        GetNode<Label>("Control/lbl_id").Text = id;
    }

    private void _on_gd_cubism_effect_hit_area_hit_area_exited(SubViewport _model, String id)
    {
        this.dict_detail = new();
        GetNode<Label>("Control/lbl_id").Text = "";
    }

    private void _on_canvas_draw()
    {
        this.mark_hit_area(this.dict_detail, new Color(0.0f, 1.0f, 0.0f), new Color(1.0f, 0.0f, 0.0f));
        this.mark_hit_area(this.dict_pickup, new Color(0.0f, 0.0f, 1.0f), new Color(1.0f, 0.0f, 0.0f));
    }

    private void _on_item_list_item_selected(long index)
    {
        this.id_mesh = GetNode<ItemList>("Control/ItemList").GetItemText((int)index);
    }
}

