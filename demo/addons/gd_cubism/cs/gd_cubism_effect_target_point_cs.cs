// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050


public partial class GDCubismEffectTargetPointCS : GDCubismEffectCS
{
    private static readonly StringName NativeName = "GDCubismEffectTargetPoint";

    public GDCubismEffectTargetPointCS(Node o) : base(o) { }

    // ------------------------------------------------------------ Property(s)

    public String HeadAngleX
    {
        set { this.InternalObject.Call("set_head_angle_x", (String)value); }
        get { return (String)this.InternalObject.Call("get_head_angle_x"); }
    }

    public String HeadAngleY
    {
        set { this.InternalObject.Call("set_head_angle_y", (String)value); }
        get { return (String)this.InternalObject.Call("get_head_angle_y"); }
    }

    public String HeadAngleZ
    {
        set { this.InternalObject.Call("set_head_angle_z", (String)value); }
        get { return (String)this.InternalObject.Call("get_head_angle_z"); }
    }

    public float HeadRange
    {
        set { this.InternalObject.Call("set_head_range", (float)value); }
        get { return (float)this.InternalObject.Call("get_head_range"); }
    }

    public String BodyAngleX
    {
        set { this.InternalObject.Call("set_body_angle_x", (String)value); }
        get { return (String)this.InternalObject.Call("get_body_angle_x"); }
    }

    public float BodyRange
    {
        set { this.InternalObject.Call("set_body_range", (float)value); }
        get { return (float)this.InternalObject.Call("get_body_range"); }
    }

    /// <value>
    ///     Property <c>EyesAngleX</c>
    ///     <br />
    ///     对应原生属性 <c>eyes_ball_x</c>（眼球 X 参数名，默认 <c>ParamEyeBallX</c>）。
    ///     <br />
    ///     注: 原实现调用的是不存在的 <c>set/get_eyes_angle_x</c>，读写全部失效，此处已修正。
    /// </value>
    public String EyesAngleX
    {
        set { this.InternalObject.Call("set_eyes_ball_x", (String)value); }
        get { return (String)this.InternalObject.Call("get_eyes_ball_x"); }
    }

    /// <value>
    ///     Property <c>EyesAngleY</c>
    ///     <br />
    ///     对应原生属性 <c>eyes_ball_y</c>（眼球 Y 参数名，默认 <c>ParamEyeBallY</c>）。
    ///     <br />
    ///     注: 原实现调用的是不存在的 <c>set/get_eyes_angle_y</c>，读写全部失效，此处已修正。
    /// </value>
    public String EyesAngleY
    {
        set { this.InternalObject.Call("set_eyes_ball_y", (String)value); }
        get { return (String)this.InternalObject.Call("get_eyes_ball_y"); }
    }

    public float EyesRange
    {
        set { this.InternalObject.Call("set_eyes_range", (float)value); }
        get { return (float)this.InternalObject.Call("get_eyes_range"); }
    }

    // -------------------------------------------------------------- Method(s)

    public void SetTarget(Vector2 v)
    {
        this.InternalObject.Call("set_target", v);
    }

    public Vector2 GetTarget()
    {
        return (Vector2)this.InternalObject.Call("get_target");
    }

    // -------------------------------------------------------------- Signal(s)
}
