// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using System;


#pragma warning disable IDE1006


public partial class demo_effect_hit_area : Node2D
{
    const String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;

    private bool pressed = false;
    private Vector2 local_pos = new(0.0f, 0.0f);
    private Vector2 adjust_pos = new(0.0f, 0.0f);
    private Godot.Collections.Dictionary dict_detail = new();
    private Godot.Collections.Dictionary dict_pickup = new();
    private String id_mesh = "";


    private void recalc_model_position(GDCubismUserModelCS model)
    {
        if(model.Assets == "") return;

        Godot.Collections.Dictionary canvas_info = model.GetCanvasInfo();

        if(canvas_info.Count > 0)
        {
            Vector2 vct_viewport_size = new(GetViewportRect().Size.X, GetViewportRect().Size.Y);
            Vector2 size_in_pixels = (Vector2)canvas_info["size_in_pixels"];
            float scale = vct_viewport_size.Y / Math.Max(size_in_pixels.X, size_in_pixels.Y);
            model.GetInternalObject().Position = new(vct_viewport_size.X * 0.5f, vct_viewport_size.Y * 0.5f);
            model.GetInternalObject().Scale = new(scale, scale);
       }
    }

    public override void _Ready()
    {
        this.cubism_model = new(GetNode<Node2D>("GDCubismUserModel"));
        if (this.cubism_model.Assets == "") this.cubism_model.Assets = DEFAULT_ASSET;

        GetNode<ItemList>("Control/ItemList").Clear();
        Godot.Collections.Dictionary dict_meshes = this.cubism_model.GetMeshes();
        foreach (var id in dict_meshes.Keys)
        {
            GetNode<ItemList>("Control/ItemList").AddItem(id.AsString());
        }
    }

    public override void _Process(double delta)
    {
        this.recalc_model_position(this.cubism_model);

        GDCubismEffectHitAreaCS hit_area = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectHitArea"));

        if (this.pressed == true)
        {
            hit_area.SetTarget(this.local_pos);
        }

        GetNode<Label>("Control/lbl_mouse_x").Text = String.Format("{0:######.000}", this.local_pos.X);
        GetNode<Label>("Control/lbl_mouse_y").Text = String.Format("{0:######.000}", this.local_pos.Y);

        if (this.id_mesh != "")
        {
            this.dict_pickup = hit_area.GetDetail(this.cubism_model, this.id_mesh);
        }

        GetNode<CanvasItem>("Canvas").QueueRedraw();
        
        base._Process(delta);
    }

    public override void _Input(InputEvent @event)
    {
        if (@event is InputEventMouseButton mouseButton)
        {
            this.pressed = mouseButton.IsPressed();
        }

        if (@event is InputEventMouseMotion mouseMotion)
        {
            this.local_pos = this.cubism_model.GetInternalObject().ToLocal(mouseMotion.Position) + this.adjust_pos;
        }
        
        base._Input(@event);
    }

    private void mark_hit_area(Godot.Collections.Dictionary dict_hit_area, Color color_box, Color color_tri)
    {
        if (dict_hit_area.Count > 0)
        {
            Rect2 rect = (Rect2)dict_hit_area["rect"];

            Rect2 r = new(
                (rect.Position - this.adjust_pos) * this.cubism_model.GetInternalObject().Scale,
                rect.Size * this.cubism_model.GetInternalObject().Scale
            );
            GetNode<CanvasItem>("Canvas").DrawRect(r, color_box, false, 5);

            if (dict_hit_area.ContainsKey("vertices") == true)
            {
                Godot.Collections.Array<Vector2> vtx = (Godot.Collections.Array<Vector2>)dict_hit_area["vertices"];

                Godot.Collections.Array<Vector2> v = new();
                v.Add(vtx[0] * this.cubism_model.GetInternalObject().Scale);
                v.Add(vtx[1] * this.cubism_model.GetInternalObject().Scale);
                v.Add(vtx[2] * this.cubism_model.GetInternalObject().Scale);

                GetNode<CanvasItem>("Canvas").DrawLine(v[0] - this.adjust_pos, v[1] - adjust_pos, color_tri, 3);
                GetNode<CanvasItem>("Canvas").DrawLine(v[1] - this.adjust_pos, v[2] - adjust_pos, color_tri, 3);
                GetNode<CanvasItem>("Canvas").DrawLine(v[2] - this.adjust_pos, v[0] - adjust_pos, color_tri, 3);                
            }
        }
    }

    private void _on_gd_cubism_effect_hit_area_hit_area_entered(Node2D _model, String id)
    {
        GDCubismEffectHitAreaCS hit_area = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectHitArea"));

        this.dict_detail = hit_area.GetDetail(this.cubism_model, id);
        GetNode<Label>("Control/lbl_id").Text = id;
    }

    private void _on_gd_cubism_effect_hit_area_hit_area_exited(Node2D _model, String id)
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
