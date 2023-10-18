// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using Godot;
using Godot.Collections;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_transparent : Node2D
{
    private const System.String DEFAULT_ASSET = "res://addons/gd_cubism/example/res/live2d/mao_pro_jp/runtime/mao_pro_t02.model3.json";
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
    private Array<System.String> ary_character_expression;
    private Array ary_character_motion = new();

    private GDCubismUserModelCS model;
    private Polygon2D polygon_2d;


    // ----------------------------------------------------------------- ported
    // I have ported the code for check_cross and convex_hull to GDScript.
    // * https://tjkendev.github.io/procon-library/
    // * https://tjkendev.github.io/procon-library/cpp/geometry/graham_scan.html

    private static bool check_cross(Array<Vector2> ary_check, Vector2 vtx)
    {
        Vector2 va = ary_check[^2];
        Vector2 vb = ary_check[^1];

        return (((vb.X - va.X) * (vtx.Y - va.Y)) - ((vb.Y - va.Y) * (vtx.X - va.X))) > 0;
    }
    private static Array<Vector2> convex_hull(Array<Vector2> ary_vertex)
    {
        //ary_vertex
        ary_vertex.Sort();
        Array<Vector2> ary_result = new();
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

    public override void _Ready()
    {
        this.model = new(GetNode<SubViewport>("Sprite2D/GDCubismUserModel"));
        if (this.model.Assets == "") model.Assets = DEFAULT_ASSET;

        this.polygon_2d = GetNode<Polygon2D>("Polygon2D");

        GetNode<Sprite2D>("Sprite2D").Position = new(GetViewportRect().Size.X / 2, GetViewportRect().Size.Y / 2);

        this.ary_character_expression = model.GetExpressions();

        Dictionary<System.String, int> dict_motion = model.GetMotions();

        foreach (var (k, item_count) in dict_motion)
        {
            for (int no = 0; no < item_count; no++)
            {
                this.ary_character_motion.Add(
                    new Dictionary { { "group", k }, { "no", no } }
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

        Dictionary dict_mesh = model.GetMeshes();
        Array<Vector2> ary = new();
        foreach (System.String name in CONVEX_MESH_SRC)
        {
            var ary_vtx = (Array<Vector2>)((ArrayMesh)dict_mesh[name]).SurfaceGetArrays(0)[0];
            ary += ary_vtx;
        }
        var ary_polygon = convex_hull(ary);

        if (ary_polygon.Count > 2)
        {
            Vector2[] region = System.Array.Empty<Vector2>();
            Vector2 adjust = GetViewportRect().Size;
            adjust -= new Vector2(model.Size.X, model.Size.Y);
            adjust *= new Vector2(0.5f, 0.5f);

            foreach (Vector2 v in ary_polygon)
            {
                System.Array.Resize(ref region, region.Length + 1);
                region[^1] = v + adjust;
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

                    Dictionary motion = (Dictionary)ary_character_motion[random.RandiRange(0, ary_character_motion.Count - 1)];
                    this.model.StartMotionLoop(
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
                        model.StartExpression(this.ary_character_expression[(int)index]);
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

