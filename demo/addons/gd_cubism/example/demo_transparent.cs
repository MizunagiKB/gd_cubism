// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable IDE1006


public partial class demo_transparent : Node2D
{
    private const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro.model3.json";

    private GDCubismUserModelCS cubism_model;

    private System.String[] CONVEX_MESH_SRC = {
        "ArtMesh121",
        "ArtMesh122",
        "ArtMesh135",
        "ArtMesh146",
        "ArtMesh147",
        "ArtMesh231",
        "ArtMesh278"
    };
    private bool order_window_position = false;
    private bool mouse_button_pressed = false;
    private Vector2I window_position;
    private Vector2 mouse_position;

    private bool enable_transparent = false;
    private Godot.Collections.Array<System.String> ary_character_expression = new();
    private Godot.Collections.Array ary_character_motion = new();

    private Polygon2D polygon_2d;


    // ----------------------------------------------------------------- ported
    // I have ported the code for check_cross and convex_hull to GDScript.
    // * https://tjkendev.github.io/procon-library/
    // * https://tjkendev.github.io/procon-library/cpp/geometry/graham_scan.html

    private static bool check_cross(Godot.Collections.Array<Vector2> ary_check, Vector2 vtx)
    {
        Vector2 va = ary_check[^2];
        Vector2 vb = ary_check[^1];

        return (((vb.X - va.X) * (vtx.Y - va.Y)) - ((vb.Y - va.Y) * (vtx.X - va.X))) > 0;
    }

    private static Godot.Collections.Array<Vector2> convex_hull(Godot.Collections.Array<Vector2> ary_vertex)
    {
        ary_vertex.Sort();

        Godot.Collections.Array<Vector2> ary_result = new();
        int n = ary_vertex.Count;

        foreach (Vector2 vtx in ary_vertex)
        {
            while (ary_result.Count > 1 && check_cross(ary_result, vtx))
            {
                ary_result.RemoveAt(ary_result.Count - 1);
            }
            ary_result.Insert(ary_result.Count, vtx);
        }

        int t = ary_result.Count;

        int i = n - 2;
        while (i >= 0)
        {
            Vector2 vtx = ary_vertex[i];
            while (ary_result.Count > t && check_cross(ary_result, vtx))
            {
                ary_result.RemoveAt(ary_result.Count - 1);
            }
            ary_result.Insert(ary_result.Count, vtx);

            i--;
        }

        ary_result.RemoveAt(ary_result.Count - 1);

        return ary_result;
    }

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
        this.cubism_model = new(GetNode<Node2D>("GDCubismUserModel"));
        if (this.cubism_model.Assets == "") this.cubism_model.Assets = DEFAULT_ASSET;

        this.recalc_model_position(this.cubism_model);
        this.polygon_2d = GetNode<Polygon2D>("Polygon2D");

        this.ary_character_expression = this.cubism_model.GetExpressions();

        Godot.Collections.Dictionary<System.String, int> dict_motion = this.cubism_model.GetMotions();

        foreach (var (k, item_count) in dict_motion)
        {
            for (int no = 0; no < item_count; no++)
            {
                this.ary_character_motion.Add(
                    new Godot.Collections.Dictionary { { "group", k }, { "no", no } }
                );
            }
        }
    }

    public override void _Process(double delta)
    {
        if (this.order_window_position == true)
        {
            DisplayServer.WindowSetPosition(new Vector2I(this.window_position.X, this.window_position.Y));
            this.order_window_position = false;
        }

        Godot.Collections.Dictionary dict_mesh = this.cubism_model.GetMeshes();
        Godot.Collections.Array<Vector2> ary = new();

        foreach (System.String name in CONVEX_MESH_SRC)
        {
            Godot.Collections.Array<Vector2> ary_vtx = (Godot.Collections.Array<Vector2>)((MeshInstance2D)dict_mesh[name]).Mesh.SurfaceGetArrays(0)[(int)Mesh.ArrayType.Vertex];
            ary += ary_vtx;
        }

        Godot.Collections.Array<Vector2> ary_polygon = convex_hull(ary);

        if (ary_polygon.Count > 2)
        {
            Vector2[] region = System.Array.Empty<Vector2>();

            foreach (Vector2 _v in ary_polygon)
            {
                Vector2 v = _v;
                v *= this.cubism_model.GetInternalObject().Scale;
                v += this.cubism_model.GetInternalObject().Position;
                System.Array.Resize(ref region, region.Length + 1);
                region[^1] = v;
            }

            this.polygon_2d.Polygon = region;
        }
    }

    public override void _Input(InputEvent @event)
    {
        if (@event is InputEventKey inputKey)
        {
            if (inputKey.IsPressed() == true)
            {
                if (inputKey.Keycode == Key.Escape)
                {
                    GetTree().Quit();
                }

                if (inputKey.Keycode == Key.Space)
                {
                    var random = new RandomNumberGenerator();

                    Godot.Collections.Dictionary motion = (Godot.Collections.Dictionary)ary_character_motion[random.RandiRange(0, ary_character_motion.Count - 1)];
                    this.cubism_model.StartMotionLoop(
                        (System.String)motion["group"],
                        (int)motion["no"],
                        GDCubismUserModelCS.PriorityEnum.PriorityForce,
                        true,
                        true
                    );
                }

                if (inputKey.Keycode == Key.Tab)
                {
                    if (this.enable_transparent == true)
                    {
                        this.enable_transparent = false;
                        DisplayServer.WindowSetMousePassthrough(System.Array.Empty<Vector2>());
                    }
                    else
                    {
                        this.enable_transparent = true;
                        DisplayServer.WindowSetMousePassthrough(this.polygon_2d.Polygon);
                    }
                    GetTree().Root.TransparentBg = this.enable_transparent;
                    DisplayServer.WindowSetFlag(DisplayServer.WindowFlags.Transparent, this.enable_transparent);
                    DisplayServer.WindowSetFlag(DisplayServer.WindowFlags.AlwaysOnTop, true);
                }

                if (inputKey.Keycode >= Key.Key0 && inputKey.Keycode <= Key.Key9)
                {
                    long index = inputKey.Keycode - Key.Key0;
                    if (index < this.ary_character_expression.Count)
                    {
                        this.cubism_model.StartExpression(this.ary_character_expression[(int)index]);
                    }
                }
            }
        }

        if (@event is InputEventMouseButton eventMouseButton)
        {
            if (eventMouseButton.Pressed == true)
            {
                this.window_position = DisplayServer.WindowGetPosition();
                this.mouse_position = GetViewport().GetMousePosition();
            }
            else
            {
                GetViewport().WarpMouse(this.mouse_position);
            }
            this.mouse_button_pressed = eventMouseButton.Pressed;
        }

        if (@event is InputEventMouseMotion inputMouseMotion)
        {
            if (this.mouse_button_pressed == true)
            {
                this.order_window_position = true;
                this.window_position += new Vector2I((int)inputMouseMotion.Relative.X, (int)inputMouseMotion.Relative.Y);
            }
        }
    }
}
