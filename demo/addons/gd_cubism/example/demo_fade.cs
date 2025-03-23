// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable IDE1006


public partial class demo_fade : Node2D
{
    const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    GDCubismUserModelCS cubism_model;
    GDCubismUserModelCS cubism_model_sv;

    double color_h;


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
        if (this.cubism_model.Assets == "")
        {
            this.cubism_model.Assets = DEFAULT_ASSET;
        }

        this.cubism_model_sv = new(GetNode<Node2D>("Sprite2D/SubViewport/GDCubismUserModel"));
        if (this.cubism_model_sv.Assets == "")
        {
            this.cubism_model_sv.Assets = DEFAULT_ASSET;
        }

        this.color_h = GetNode<HSlider>("HSlider").Value;
    }

    public override void _Process(double delta)
    {
        this.color_h += (delta * 0.25);
        
        if (this.color_h > 360.0)
        {
            this.color_h -= 360.0;
        }

        Color color = Color.FromHsv((float)this.color_h, 1.0f, 1.0f, (float)GetNode<HSlider>("HSlider").Value / 100.0f);

        this.cubism_model.GetInternalObject().Modulate = color;
        GetNode<Sprite2D>("Sprite2D").Modulate = color;
    }
}
