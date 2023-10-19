// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_effect_custom_03 : Node2D
{
    private const String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json";

    private bool pressed = false;
    private GDCubismUserModelCS model = null;
    private GDCubismEffectTargetPointCS target = null;


    public override void _Ready()
    {
        this.model = new GDCubismUserModelCS(GetNode<SubViewport>("Sprite2D/GDCubismUserModel"));
        this.target = new GDCubismEffectTargetPointCS(GetNode<Node>("Sprite2D/GDCubismUserModel/GDCubismEffectTargetPoint"));

        if(this.model.Assets == "") this.model.Assets = DEFAULT_ASSET;
    }


    public override void _Process(double delta)
    {
    }


    public override void _Input(InputEvent @event)
    {
        if (@event is InputEventMouseButton mouseButton)
        {
            this.pressed = mouseButton.IsPressed();
        }

        if (@event is InputEventMouseMotion mouseMotion)
        {
            if (this.pressed == true)
            {
                // Convert to local position.
                var local_pos = GetNode<Sprite2D>("Sprite2D").ToLocal(mouseMotion.Position);
                // Adjust position.
                Vector2 render_size = new
                (
                    (float)this.model.Size.X * GetNode<Sprite2D>("Sprite2D").Scale.X,
                    (float)this.model.Size.Y * GetNode<Sprite2D>("Sprite2D").Scale.Y * -1.0f
                );
                local_pos /= (render_size * 0.5f);
                this.target.SetTarget(local_pos);
            }
        }
        else
        {
            this.target.SetTarget(new Vector2(0.0f, 0.0f));
        }
    }
}
