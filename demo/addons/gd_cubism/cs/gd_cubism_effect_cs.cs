// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;


#pragma warning disable CA1050


public partial class GDCubismEffectCS : Node
{
    private static readonly StringName NativeName = "GDCubismEffect";

    protected Node InternalObject = null;

    public GDCubismEffectCS()
    {
        this.InternalObject = (SubViewport)Godot.ClassDB.Instantiate(NativeName);
    }

    public GDCubismEffectCS(Node o)
    {
        this.InternalObject = o;
    }

    public Node GetInternalObject()
    {
        return this.InternalObject;
    }

    public void Attach(Node o)
    {
        this.InternalObject = o;
    }

    public Node Detach()
    {
        Node o = this.InternalObject;
        this.InternalObject = null;
        return o;
    }

    // ------------------------------------------------------------ Property(s)

    public bool Active
    {
        set { this.InternalObject.Call("set_active", (bool)value); }
        get { return (bool)this.InternalObject.Call("get_active"); }
    }

    // -------------------------------------------------------------- Method(s)
    // -------------------------------------------------------------- Signal(s)
}
