// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 MizunagiKB <mizukb@live.jp>
using Godot;
using System;


#pragma warning disable IDE1006


public partial class demo_effect_target_point : Node2D
{
    const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;
    private GDCubismEffectTargetPointCS cubism_efx;

    private bool pressed = false;


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

        this.cubism_efx = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectTargetPoint"));
    }

    public override void _Process(double delta)
    {
        this.recalc_model_position(this.cubism_model);
        base._Process(delta);
    }

    public override void _Input(InputEvent @event)
    {
        if (@event is InputEventMouseButton evt_mouse_button)
        {
            this.pressed = evt_mouse_button.IsPressed();
        }

        if (@event is InputEventMouseMotion evt_mouse_motion)
        {
            if (this.pressed == true)
            {
                Vector2 calc_pos = this.cubism_model.GetInternalObject().ToLocal(evt_mouse_motion.Position) * new Vector2(1.0f, -1.0f);
                this.cubism_efx.SetTarget(calc_pos.Normalized());
            } else {
                this.cubism_efx.SetTarget(Vector2.Zero);
            }
        }

        base._Input(@event);
    }
}
