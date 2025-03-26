// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using Godot.NativeInterop;
using System;

#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_effect_custom_02 : Node2D
{
    private const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;
    private GDCubismEffectCustomCS cubism_efx;

    private bool first_time = true;
    private float counter = 0.0f;
    private Vector2 base_vct;
    private Vector2 base_scale;

    [Export]
    private String art_mesh_name = "ArtMesh278";
    [Export]
    private int vector_index_0 = 0;
    [Export]
    private int vector_index_1 = 1;


    private void recalc_model_position(GDCubismUserModelCS model)
    {
        if(model.Assets == "") return;

        Godot.Collections.Dictionary canvas_info = model.GetCanvasInfo();

        if(canvas_info.Count > 0)
        {
            Vector2 vct_viewport_size = new(GetViewportRect().Size.X, GetViewportRect().Size.Y);
            Vector2 size_in_pixels = (Vector2)canvas_info["size_in_pixels"];
            float scale = vct_viewport_size.Y / Math.Max(size_in_pixels.X, size_in_pixels.Y);
            model.GetInternalObject().Position = new(vct_viewport_size.X * 0.5f, vct_viewport_size.Y * 0.5f);
            model.GetInternalObject().Scale = new(scale, scale);
       }
    }

    public override void _Ready()
    {
        this.cubism_efx = new(GetNode<Node>("GDCubismUserModel/GDCubismEffectCustom"));
        // this.cubism_efx.CubismPrologue += this._on_cubism_prologue;
        // this.cubism_efx.CubismEpilogue += this._on_cubism_epilogue;
        // this.cubism_efx.CubismInit += this._on_cubism_init;
        // this.cubism_efx.CubismTerm += this._on_cubism_term;
        this.cubism_efx.CubismProcess += this._on_cubism_process;

        this.cubism_model = new(GetNode<Node2D>("GDCubismUserModel"));
        if (this.cubism_model.Assets == "") this.cubism_model.Assets = DEFAULT_ASSET;

        Godot.Collections.Dictionary<String, int> dict_motion = this.cubism_model.GetMotions();

        foreach (var (k, item_count) in dict_motion)
        {
            this.cubism_model.StartMotionLoop(k, 0, GDCubismUserModelCS.PriorityEnum.PriorityForce, true, true);
            break;            
        }        
    }

    public override void _Process(double delta)
    {
        recalc_model_position(this.cubism_model);
    }

    private void _on_cubism_process(Node2D model, double delta)
    {
        const int ARRAY_VERTEX = (int)ArrayMesh.ArrayType.Vertex;

        var ref_sprite = GetNode<Sprite2D>("GDCubismUserModel/GDCubismEffectCustom/Sprite2D");

        Godot.Collections.Dictionary dict_mesh = this.cubism_model.GetMeshes();

        if (dict_mesh.ContainsKey(this.art_mesh_name) == false) return;

        MeshInstance2D ary_mesh = (dict_mesh[this.art_mesh_name]).AsGodotObject() as MeshInstance2D;
        Godot.Collections.Array ary_surface = ary_mesh.Mesh.SurfaceGetArrays(0);
        Godot.Collections.Array<Vector2> mesh_vertex = (Godot.Collections.Array<Vector2>)ary_surface[ARRAY_VERTEX];
        
        if (this.first_time == true)
        {
            var _fr = mesh_vertex[this.vector_index_0];
            var _to = mesh_vertex[this.vector_index_1];

            this.base_vct = (_to - _fr);
            this.base_scale = ref_sprite.Scale;
            this.first_time = false;
        }

        var fr = mesh_vertex[this.vector_index_0];
        var to = mesh_vertex[this.vector_index_1];

        var calc_vct = (to - fr);
        var scale = calc_vct.Length() / base_vct.Length();

        ref_sprite.Position = this.cubism_model.GetInternalObject().ToGlobal(fr);
        ref_sprite.Rotation = base_vct.Normalized().Cross(calc_vct.Normalized());
        ref_sprite.Scale = base_scale * scale;
    }
}
