// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;


#pragma warning disable CA1050


// 修正: 原实现为 `: Node` —— 每次 new GDCubismEffectXxxCS(o) 都会额外创建一个游离的原生 Node
//       (派生类构造链走 Node()，只为承载包装对象本身，不入树也不释放 → 退出时报 ObjectDB 泄漏)。
//       包装类只负责把调用转发给 InternalObject，本身不需要是 Godot 对象。
public partial class GDCubismEffectCS
{
    private static readonly StringName NativeName = "GDCubismEffect";

    protected Node InternalObject = null;

    public GDCubismEffectCS()
    {
        // 修正: 原实现强转 (SubViewport)，而 GDCubismEffect 是 Node → 必然 InvalidCastException
        this.InternalObject = Godot.ClassDB.Instantiate(NativeName).As<Node>();
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
