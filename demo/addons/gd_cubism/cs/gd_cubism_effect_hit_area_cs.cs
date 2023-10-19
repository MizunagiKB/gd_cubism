// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050


public partial class GDCubismEffectHitAreaCS : GDCubismEffectCS
{
    private static readonly StringName NativeName = "GDCubismEffectHitArea";

    public GDCubismEffectHitAreaCS(Node o) : base(o) { }

    // ------------------------------------------------------------ Property(s)

    public bool Monitoring
    {
        set { this.InternalObject.Call("set_monitoring", (bool)value); }
        get { return (bool)this.InternalObject.Call("set_monitoring"); }
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

    public Godot.Collections.Dictionary GetDetail(GDCubismUserModelCS model, String id)
    {
        return (Godot.Collections.Dictionary)this.InternalObject.Call("get_detail", model.GetInternalObject(), id);
    }

    // -------------------------------------------------------------- Signal(s)
}
