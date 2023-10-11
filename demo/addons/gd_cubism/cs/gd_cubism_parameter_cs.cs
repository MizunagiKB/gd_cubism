// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;


#pragma warning disable CA1050


public partial class GDCubismParameterCS : GDCubismValueAbsCS
{
    // private static readonly StringName NativeName = "GDCubismParameter";

    public GDCubismParameterCS(GodotObject o) : base(o) { }

    // ------------------------------------------------------------ Property(s)

    public float DefaultValue
    {
        get { return (float)this.InternalObject.Call("get_default_value"); }
    }

    public float MaximumValue
    {
        get { return (float)this.InternalObject.Call("get_maximum_value"); }
    }

    public float MinimumValue
    {
        get { return (float)this.InternalObject.Call("get_minimum_value"); }
    }

    // -------------------------------------------------------------- Method(s)
    // -------------------------------------------------------------- Signal(s)
}
