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

    public String EyesAngleX
    {
        set { this.InternalObject.Call("set_eyes_angle_x", (String)value); }
        get { return (String)this.InternalObject.Call("get_eyes_angle_x"); }
    }

    public String EyesAngleY
    {
        set { this.InternalObject.Call("set_eyes_angle_y", (String)value); }
        get { return (String)this.InternalObject.Call("get_eyes_angle_y"); }
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
