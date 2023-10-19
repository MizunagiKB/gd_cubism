// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050


public partial class GDCubismValueAbsCS : GodotObject
{
    private static readonly StringName NativeName = "GDCubismValueAbs";

    protected GodotObject InternalObject = null;

    public GDCubismValueAbsCS()
    {
        this.InternalObject = (SubViewport)Godot.ClassDB.Instantiate(NativeName);
    }

    public GDCubismValueAbsCS(GodotObject o)
    {
        this.InternalObject = o;
    }

    public GodotObject GetInternalObject()
    {
        return this.InternalObject;
    }

    public void Attach(GodotObject o)
    {
        this.InternalObject = o;
    }

    public GodotObject Detach()
    {
        GodotObject o = this.InternalObject;
        this.InternalObject = null;
        return o;
    }

    // ------------------------------------------------------------ Property(s)

    public String Id
    {
        get { return (String)this.InternalObject.Call("get_id"); }
    }

    public float Value
    {
        set { this.InternalObject.Call("set_value", (float)value); }
        get { return (float)this.InternalObject.Call("get_value"); }
    }

    // -------------------------------------------------------------- Method(s)
    // -------------------------------------------------------------- Signal(s)
}
