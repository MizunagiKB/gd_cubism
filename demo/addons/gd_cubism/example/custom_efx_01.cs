// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class custom_efx_01 : Node
{
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


    private void _on_cubism_process(SubViewport _model, float delta)
    {
        GDCubismUserModelCS model = new(_model);
        Godot.Collections.Dictionary dict_mesh = model.GetMeshes();

        if (dict_mesh.ContainsKey(art_mesh_name) == false) return;

        ArrayMesh ary_mesh = (ArrayMesh)dict_mesh[art_mesh_name];
        Godot.Collections.Array ary_surface = ary_mesh.SurfaceGetArrays(0);
        Vector2[] mesh_vertex = (Vector2[])ary_surface[(int)ArrayMesh.ArrayType.Vertex];

        if (this.first_time == true)
        {
            Vector2 fr0 = mesh_vertex[this.vector_index_0];
            Vector2 to0 = mesh_vertex[this.vector_index_1];
            this.base_vct = (to0 - fr0);
            this.base_scale = GetNode<Sprite2D>("Sprite2D").Scale;
            this.first_time = false;
        }

        Vector2 fr1 = mesh_vertex[this.vector_index_0];
        Vector2 to1 = mesh_vertex[this.vector_index_1];
        Vector2 calc_vct = (to1 - fr1);
        float scale = calc_vct.Length() / base_vct.Length();

        GetNode<Sprite2D>("Sprite2D").Position = fr1;
        GetNode<Sprite2D>("Sprite2D").Rotation = this.base_vct.Normalized().Cross(calc_vct.Normalized());
        GetNode<Sprite2D>("Sprite2D").Scale = this.base_scale * scale;
    }
}
