// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using System;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_effect_custom_01 : Node2D
{
    private const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;
    private GDCubismEffectCustomCS cubism_efx;

    private enum E_PARAM_MODE {
        PROCESS,
        SIGNAL
    }

    private bool param_valid = false;
    private E_PARAM_MODE param_mode = E_PARAM_MODE.PROCESS;
    private GDCubismParameterCS l_eye;
    private GDCubismParameterCS r_eye;


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

    // Called when the node enters the scene tree for the first time.
    public override void _Ready()
    {
        this.cubism_efx = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectCustom"));
        // this.cubism_efx.CubismPrologue += this._on_cubism_prologue;
        // this.cubism_efx.CubismEpilogue += this._on_cubism_epilogue;
        // this.cubism_efx.CubismInit += this._on_cubism_init;
        // this.cubism_efx.CubismTerm += this._on_cubism_term;
        // this.cubism_efx.CubismProcess += this._on_cubism_process;

        this.cubism_model = new(GetNode<Node2D>("GDCubismUserModel"));
        if (this.cubism_model.Assets == "") this.cubism_model.Assets = DEFAULT_ASSET;
    }

    // Called every frame. 'delta' is the elapsed time since the previous frame.
    public override void _Process(double delta)
    {
        recalc_model_position(this.cubism_model);

        if (this.param_mode == E_PARAM_MODE.PROCESS) {

            Godot.Collections.Array<GDCubismParameterCS> ary_param = this.cubism_model.GetParameters();

            foreach (GDCubismParameterCS o in this.cubism_model.GetParameters()) 
            {
                if (o.Id == "ParamEyeLOpen") {
                    o.Value = 0.0f;
                }

                if (o.Id == "ParamEyeROpen") {
                    o.Value = 0.0f;
                }
            }
        }
    }
}
