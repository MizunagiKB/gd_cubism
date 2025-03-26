// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using Godot.NativeInterop;


#pragma warning disable CA1050


public partial class GDCubismEffectCustomCS : GDCubismEffectCS
{
    private static readonly StringName NativeName = "GDCubismEffectCustom";

    public GDCubismEffectCustomCS(Node o) : base(o) { }

    public static readonly StringName CubismEpilogueName = "cubism_epilogue";
    public static readonly StringName CubismInitName = "cubism_init";
    public static readonly StringName CubismProcessName = "cubism_process";
    public static readonly StringName CubismPrologueName = "cubism_prologue";
    public static readonly StringName CubismTermName = "cubism_term";

    // ------------------------------------------------------------ Property(s)
    // -------------------------------------------------------------- Method(s)
    // -------------------------------------------------------------- Signal(s)

    public delegate void CubismEpilogueEventHandler(Node2D value, double delta);

    private static void CubismEpilogueTrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((CubismEpilogueEventHandler)delegateObj)(
            VariantUtils.ConvertToGodotObject(args[0]) as Node2D,
            VariantUtils.ConvertToFloat32(args[1])
        );
        ret = default;
    }

    public unsafe event CubismEpilogueEventHandler CubismEpilogue
    {
        add => this.InternalObject.Connect(CubismEpilogueName, Callable.CreateWithUnsafeTrampoline(value, &CubismEpilogueTrampoline));
        remove => this.InternalObject.Disconnect(CubismEpilogueName, Callable.CreateWithUnsafeTrampoline(value, &CubismEpilogueTrampoline));
    }

    public delegate void CubismInitEventHandler(Node2D value);

    private static void CubismInitrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((CubismInitEventHandler)delegateObj)(
            VariantUtils.ConvertToGodotObject(args[0]) as Node2D
        );
        ret = default;
    }

    public unsafe event CubismInitEventHandler CubismInit
    {
        add => this.InternalObject.Connect(CubismInitName, Callable.CreateWithUnsafeTrampoline(value, &CubismInitrampoline));
        remove => this.InternalObject.Disconnect(CubismInitName, Callable.CreateWithUnsafeTrampoline(value, &CubismInitrampoline));
    }

    public delegate void CubsimPrologueEventHandler(Node2D value, double delta);

    private static void CubismPrologueTrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((CubsimPrologueEventHandler)delegateObj)(
            VariantUtils.ConvertToGodotObject(args[0]) as Node2D,
            VariantUtils.ConvertToFloat32(args[1])
        );
        ret = default;
    }

    public unsafe event CubsimPrologueEventHandler CubismPrologue
    {
        add => this.InternalObject.Connect(CubismPrologueName, Callable.CreateWithUnsafeTrampoline(value, &CubismPrologueTrampoline));
        remove => this.InternalObject.Disconnect(CubismPrologueName, Callable.CreateWithUnsafeTrampoline(value, &CubismPrologueTrampoline));
    }

    public delegate void CubismProcessEventHandler(Node2D value, double delta);

    private static void CubismProcessTrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((CubismProcessEventHandler)delegateObj)(
            VariantUtils.ConvertToGodotObject(args[0]) as Node2D,
            VariantUtils.ConvertToFloat32(args[1])
        );
        ret = default;
    }

    public unsafe event CubismProcessEventHandler CubismProcess
    {
        add => this.InternalObject.Connect(CubismProcessName, Callable.CreateWithUnsafeTrampoline(value, &CubismProcessTrampoline));
        remove => this.InternalObject.Disconnect(CubismProcessName, Callable.CreateWithUnsafeTrampoline(value, &CubismProcessTrampoline));
    }

    public delegate void CubismTermEventHandler(Node2D value);

    private static void CubismTermrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((CubismTermEventHandler)delegateObj)(
            VariantUtils.ConvertToGodotObject(args[0]) as Node2D
        );
        ret = default;
    }

    public unsafe event CubismTermEventHandler CubismTerm
    {
        add => this.InternalObject.Connect(CubismTermName, Callable.CreateWithUnsafeTrampoline(value, &CubismTermrampoline));
        remove => this.InternalObject.Disconnect(CubismTermName, Callable.CreateWithUnsafeTrampoline(value, &CubismTermrampoline));
    }
}
