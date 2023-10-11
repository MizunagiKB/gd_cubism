// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class custom_efx_02 : Node
{
    [Export]
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


    private float array_avg()
    {
        float sum_v = 0.0f;
        foreach (float v in this.ary_volume_history)
        {
            sum_v += v;
        }
        return sum_v / this.ary_volume_history.Count;
    }


    private void _on_cubism_init(SubViewport _model)
    {
        GDCubismUserModelCS model = new(_model);

        this.param_mouth = null;
        this.ary_volume_history.Clear();
        this.history_position = 0;

        Godot.Collections.Array<GDCubismParameterCS> ary_param = model.GetParameters();

        foreach (GDCubismParameterCS param in ary_param)
        {
            if (param.Id == this.param_mouth_name)
            {
                this.param_mouth = param;
            }
        }

        var bus_index = AudioServer.GetBusIndex(this.audio_bus_name);
        this.spectrum = (AudioEffectSpectrumAnalyzerInstance)AudioServer.GetBusEffectInstance(bus_index, this.audio_efx_index);

        if (this.spectrum == null)
        {
            this.lipsync_ready = false;
        }
        else
        {
            this.lipsync_ready = true;
        }
    }


    private void _on_cubism_term(SubViewport _model)
    {
        this.lipsync_ready = false;
        this.param_mouth = null;
        this.ary_volume_history.Clear();
        this.history_position = 0;
    }

    private void _on_cubism_process(SubViewport _model, float delta)
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
