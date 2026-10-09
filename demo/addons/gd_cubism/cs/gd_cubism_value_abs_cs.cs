// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050


// 修正: 原实现为 `: GodotObject` —— 每次 new GDCubismXxxCS(o) 都会额外创建一个原生空 Object
//       (派生类构造链走 GodotObject()，只为承载包装对象本身；实测 Free/GC 都不会释放它，
//       而 GetParameters() 每次调用都会新建 130 个包装对象 → 原生对象无上限增长)。
//       改成普通类只持有原生引用；与 GDCubismUserModelCS 的既有做法一致。
//       配套: GetParameters()/GetPartOpacities() 的返回类型 Array<T> → List<T>
//       (Godot.Collections.Array<T> 要求 T 是 Variant 兼容类型，GD0301)。
public partial class GDCubismValueAbsCS
{
    private static readonly StringName NativeName = "GDCubismValueAbs";

    protected GodotObject InternalObject = null;

    public GDCubismValueAbsCS()
    {
        // 修正: 原实现强转 (SubViewport)，而 GDCubismValueAbs 是 Resource → 必然 InvalidCastException
        this.InternalObject = Godot.ClassDB.Instantiate(NativeName).As<GodotObject>();
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
