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

    /// <value>
    ///     Property <c>Monitoring</c>
    ///     <br />
    ///     <c>true</c> 时只在进入/离开状态翻转时发信号；<c>false</c> 时每帧都发进入信号。
    ///     <br />
    ///     设置该属性会清空已记录的进入状态。
    /// </value>
    public bool Monitoring
    {
        set { this.InternalObject.Call("set_monitoring", (bool)value); }
        // 修正: 原实现调用的是 set_monitoring(无参), 每次读取都会报 Invalid call 且恒返回 false
        get { return (bool)this.InternalObject.Call("get_monitoring"); }
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
