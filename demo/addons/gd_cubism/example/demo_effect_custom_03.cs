// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;

#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_effect_custom_03 : Node2D
{
    private const String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;
    private GDCubismEffectCustomCS cubism_efx;

    private String param_mouth_name = "ParamMouthA";
    [Export]
    private float min_db = 60.0f;
    [Export]
    private int min_voice_freq = 0;
    [Export]
    private int max_voice_freq = 3200;
    [Export]
    private int history_size = 6;

    [ExportCategory("Audio")]
    [Export]
    private String audio_bus_name = "Voice";
    [Export]
    private int audio_efx_index = 0;

    private AudioEffectSpectrumAnalyzerInstance spectrum;
    private GDCubismParameterCS param_mouth;
    private Godot.Collections.Array<float> ary_volume_history;
    private int history_position = 0;
    private bool lipsync_ready = false;

    private void array_rebuild()
    {
        if (this.history_size != this.ary_volume_history.Count)
        {
            this.ary_volume_history.Resize(this.history_size);
            this.ary_volume_history.Fill(0.0f);
        }
    }

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
        this.cubism_efx = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectCustom"));
        this.cubism_efx.CubismPrologue += this._on_cubism_prologue;
        this.cubism_efx.CubismEpilogue += this._on_cubism_epilogue;
        this.cubism_efx.CubismInit += this._on_cubism_init;
        this.cubism_efx.CubismTerm += this._on_cubism_term;
        this.cubism_efx.CubismProcess += this._on_cubism_process;

        this.cubism_model = new(GetNode<Node2D>("GDCubismUserModel"));
        if (this.cubism_model.Assets == "") this.cubism_model.Assets = DEFAULT_ASSET;
    }

    public override void _Process(double delta)
    {
        this.recalc_model_position(this.cubism_model);        
    }

    private float array_avg()
    {
        float sum_v = 0.0f;
        foreach (float v in this.ary_volume_history)
        {
            sum_v += v;
        }
        return sum_v / this.ary_volume_history.Count;
    }

    private void _on_cubism_prologue(Node2D _model, float v) {}
    private void _on_cubism_epilogue(Node2D _model, float v) {}
    private void _on_cubism_init(Node2D _model) {}

    private void _on_cubism_term(Node2D _model)
    {
        this.lipsync_ready = false;
        this.param_mouth = null;
        this.ary_volume_history.Clear();
        this.history_position = 0;
    }

    private void _on_cubism_process(Node2D _model, double delta)
    {
        if (this.lipsync_ready == false) return;

        this.array_rebuild();

        float m = this.spectrum.GetMagnitudeForFrequencyRange(
            this.min_voice_freq,
            this.max_voice_freq
        ).Length();

        var v = Math.Clamp((min_db + Mathf.LinearToDb(m)) / this.min_db, 0.0f, 1.0f);

        this.ary_volume_history[this.history_position] = v;
        this.history_position += 1;

        if (this.history_position >= this.ary_volume_history.Count)
        {
            this.history_position = 0;
        }

        float avg_v = this.array_avg();

        if (this.param_mouth != null)
        {
            this.param_mouth.Value = Math.Max(this.param_mouth.Value - 0.2f, 0.0f);
            if (avg_v > 0.1f)
            {
                this.param_mouth.Value = 1.0f;
            }
        }
    }
}
