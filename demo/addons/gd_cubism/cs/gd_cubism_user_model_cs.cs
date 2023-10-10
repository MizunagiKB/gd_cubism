// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
namespace Godot;
using Godot.NativeInterop;
using System;
using System.Diagnostics;

#pragma warning disable CA1050


public partial class GDCubismUserModelCS : GodotObject
{
    private enum GDCubismShaderEnum : long
    {
        GD_CUBISM_SHADER_NORM_ADD,
        GD_CUBISM_SHADER_NORM_MIX,
        GD_CUBISM_SHADER_NORM_MUL,
        GD_CUBISM_SHADER_MASK,
        GD_CUBISM_SHADER_MASK_ADD,
        GD_CUBISM_SHADER_MASK_ADD_INV,
        GD_CUBISM_SHADER_MASK_MIX,
        GD_CUBISM_SHADER_MASK_MIX_INV,
        GD_CUBISM_SHADER_MASK_MUL,
        GD_CUBISM_SHADER_MASK_MUL_INV,
        GD_CUBISM_SHADER_MAX
    }

    public enum Moc3FileFormatVersionEnum : long
    {
        CSM_MOC_VERSION_UNKNOWN = 0,
        CSM_MOC_VERSION_30 = 1,
        CSM_MOC_VERSION_33 = 2,
        CSM_MOC_VERSION_40 = 3,
        CSM_MOC_VERSION_42 = 4,
        CSM_MOC_VERSION_50 = 5
    }

    public enum PriorityEnum : long
    {
        PriorityNone = 0,
        PriorityIdle = 1,
        PriorityNormal = 2,
        PriorityForce = 3
    }

    public enum ParameterModeEnum : long
    {
        FullParameter = 0,
        NoneParameter = 1
    }

    public enum MotionProcessCallbackEnum : long
    {
        Physics = 0,
        Idle = 1,
        Manual = 2
    }

    public static readonly StringName MotionEventName = "motion_event";
    public static readonly StringName MotionFinishedName = "motion_finished";

    GDScript _glue;
    SubViewport _GDCubismUserModel;

    public GDCubismUserModelCS()
    {
        this._glue = ResourceLoader.Load("res://addons/gd_cubism/cs/gd_cubism_user_model_cs.gd") as GDScript;
        this._GDCubismUserModel = (SubViewport)this._glue.Call("create_object");
    }

    public SubViewport Get()
    {
        return this._GDCubismUserModel;
    }

    // Property

    public String Assets
    {
        set { this._glue.Call("set_assets", this._GDCubismUserModel, value); }
        get { return (String)this._glue.Call("get_assets", this._GDCubismUserModel); }
    }

    public bool AutoScale
    {
        set { this._glue.Call("set_auto_scale", this._GDCubismUserModel, value); }
        get { return (bool)this._glue.Call("get_auto_scale", this._GDCubismUserModel); }
    }

    public ParameterModeEnum ParameterMode
    {
        set { this._glue.Call("set_parameter_mode", this._GDCubismUserModel, (int)value); }
        get
        {
            int value = (int)this._glue.Call("get_parameter_mode", this._GDCubismUserModel);
            return (ParameterModeEnum)Enum.ToObject(typeof(ParameterModeEnum), value);
        }
    }

    public MotionProcessCallbackEnum PlaybackProcessMode
    {
        set { this._glue.Call("set_playback_process_mode", this._GDCubismUserModel, (int)value); }
        get
        {
            int value = (int)this._glue.Call("get_playback_process_mode", this._GDCubismUserModel);
            return (MotionProcessCallbackEnum)Enum.ToObject(typeof(MotionProcessCallbackEnum), value);
        }
    }

    public Shader ShaderAdd
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_ADD, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_ADD); }
    }

    public Shader ShaderMix
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_MIX, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_MIX); }
    }

    public Shader ShaderMul
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_MUL, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_NORM_MUL); }
    }

    public Shader ShaderMask
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK); }
    }

    public Shader ShaderMaskAdd
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_ADD, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_ADD); }
    }

    public Shader ShaderMaskAddInv
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_ADD_INV, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_ADD_INV); }
    }

    public Shader ShaderMaskMix
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MIX, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MIX); }
    }

    public Shader ShaderMaskMixInv
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MIX_INV, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MIX_INV); }
    }

    public Shader ShaderMaskMul
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MUL, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MUL); }
    }

    public Shader ShaderMaskMulInv
    {
        set { this._glue.Call("set_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MUL_INV, (Shader)value); }
        get { return (Shader)this._glue.Call("get_shader", (int)GDCubismShaderEnum.GD_CUBISM_SHADER_MASK_MUL_INV); }
    }

    public float SpeedScale
    {
        set { this._glue.Call("set_speed_scale", (float)value); }
        get { return (float)this._glue.Call("get_speed_scale"); }
    }

    public void Advance()
    {
        this._glue.Call("advance", this._GDCubismUserModel);
    }

    public void Clear()
    {
        this._glue.Call("clear", this._GDCubismUserModel);
    }

    public Moc3FileFormatVersionEnum CsmGetLatestMocVersion()
    {
        int value = (int)this._glue.Call("csm_get_latest_moc_version", this._GDCubismUserModel);
        return (Moc3FileFormatVersionEnum)Enum.ToObject(typeof(Moc3FileFormatVersionEnum), value);
    }

    public Moc3FileFormatVersionEnum CsmGetMocVersion()
    {
        int value = (int)this._glue.Call("csm_get_moc_version", this._GDCubismUserModel);
        return (Moc3FileFormatVersionEnum)Enum.ToObject(typeof(Moc3FileFormatVersionEnum), value);
    }

    public Godot.Collections.Dictionary CsmGetVersion()
    {
        return (Godot.Collections.Dictionary)this._glue.Call("csm_get_version", this._GDCubismUserModel);
    }

    public Godot.Collections.Dictionary GetCanvasInfo()
    {
        return (Godot.Collections.Dictionary)this._glue.Call("get_canvas_info", this._GDCubismUserModel);
    }

    public Godot.Collections.Array GetCubismMotionQueueEntries()
    {
        return (Godot.Collections.Array)this._glue.Call("get_cubism_motion_queue_entries", this._GDCubismUserModel);
    }

    public Godot.Collections.Array<String> GetExpressions()
    {
        return (Godot.Collections.Array<String>)this._glue.Call("get_expressions", this._GDCubismUserModel);
    }

    public Godot.Collections.Array GetHitAreas()
    {
        return (Godot.Collections.Array)this._glue.Call("get_hit_areas", this._GDCubismUserModel);
    }

    public Godot.Collections.Dictionary GetMeshes()
    {
        return (Godot.Collections.Dictionary)this._glue.Call("get_meshes", this._GDCubismUserModel);
    }

    public Godot.Collections.Dictionary<String, int> GetMotions()
    {
        return (Godot.Collections.Dictionary<String, int>)this._glue.Call("get_motions", this._GDCubismUserModel);
    }

    public Godot.Collections.Array GetParameters()
    {
        return (Godot.Collections.Array)this._glue.Call("get_parameters", this._GDCubismUserModel);
    }

    public Godot.Collections.Array GetPartOpacities()
    {
        return (Godot.Collections.Array)this._glue.Call("get_part_opacities", this._GDCubismUserModel);
    }

    public void StartExpression(String expression_id)
    {
        this._glue.Call("start_expression", this._GDCubismUserModel, expression_id);
    }

    public void StartMotion(String group, int no, PriorityEnum priority)
    {
        this._glue.Call("start_motion", this._GDCubismUserModel, group, no, (int)priority);
    }

    public void StartMotionLoop(String group, int no, PriorityEnum priority, bool loop, bool loop_in_fade)
    {
        this._glue.Call("start_motion_loop", this._GDCubismUserModel, group, no, (int)priority, loop, loop_in_fade);
    }

    public void StopExpression()
    {
        this._glue.Call("stop_expression", this._GDCubismUserModel);
    }

    public void StopMotion()
    {
        this._glue.Call("stop_motion", this._GDCubismUserModel);
    }

    public Vector2I Size
    {
        set
        {
            Vector2I vct = value;
            this._GDCubismUserModel.Size = new Vector2I(vct.X, vct.Y);
        }
        get { return this._GDCubismUserModel.Size; }
    }

    public ViewportTexture GetTexture()
    {
        return (ViewportTexture)this._glue.Call("get_texture", this._GDCubismUserModel);
    }

    // Signal

    public delegate void MotionEventEventHandler(string value);

    private static void MotionEventTrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((MotionEventEventHandler)delegateObj)(VariantUtils.ConvertTo<string>(args[0]));
        ret = default;
    }

    public unsafe event MotionEventEventHandler MotionEvent
    {
        add => this._GDCubismUserModel.Connect(MotionEventName, Callable.CreateWithUnsafeTrampoline(value, &MotionEventTrampoline));
        remove => this._GDCubismUserModel.Disconnect(MotionEventName, Callable.CreateWithUnsafeTrampoline(value, &MotionEventTrampoline));
    }

    public event Action MotionFinished
    {
        add => this._GDCubismUserModel.Connect(MotionFinishedName, Callable.From(value));
        remove => this._GDCubismUserModel.Disconnect(MotionFinishedName, Callable.From(value));
    }
}
