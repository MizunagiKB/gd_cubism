// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class viewer : Control
{
    const int MIX_RENDER_SIZE = 32;
    const int MAX_RENDER_SIZE = 2048;
    const int RENDER_SIZE_STEP = 256;
    const bool ENABLE_MOTION_FINISHED = true;

    GDCubismUserModelCS cubism_model;
    Godot.Collections.Dictionary last_motion;

    public void Setup(String pathname)
    {
        this.cubism_model.Assets = pathname;

        int idx = 0;
        Godot.Collections.Dictionary<String, int> dict_motion = this.cubism_model.GetMotions();

        GetNode<ItemList>("UI/ItemListMotion").Clear();

        foreach (var (k, item_count) in dict_motion)
        {
            for (int v = 0; v < item_count; v++)
            {
                GetNode<ItemList>("UI/ItemListMotion").AddItem($"{k}_{v}");
                GetNode<ItemList>("UI/ItemListMotion").SetItemMetadata(
                    idx,
                    new Godot.Collections.Dictionary { { "group", k }, { "no", v } }
                );
                idx++;
            }
        }

        GetNode<ItemList>("UI/ItemListExpression").Clear();
        Godot.Collections.Array<String> ary_item = this.cubism_model.GetExpressions();

        foreach (var item in ary_item)
        {
            GetNode<ItemList>("UI/ItemListExpression").AddItem($"{item}");
        }

        this.cubism_model.PlaybackProcessMode = GDCubismUserModelCS.MotionProcessCallbackEnum.Idle;
        ViewportTexture tex = this.cubism_model.GetTexture();
        GetNode<Sprite2D>("Sprite2D").Texture = tex;
        GetNode<Sprite2D>("Sprite2D").Material = new CanvasItemMaterial
        {
            BlendMode = CanvasItemMaterial.BlendModeEnum.PremultAlpha,
            LightMode = CanvasItemMaterial.LightModeEnum.Unshaded
        };
    }

    public void Model3Search(String dirname)
    {
        DirAccess dir = DirAccess.Open(dirname);
        if (dir == null) return;

        dir.ListDirBegin();
        var name = dir.GetNext();

        while (name != "")
        {
            if (dir.CurrentIsDir())
            {
                Model3Search(dirname.PathJoin(name));
            }
            else
            {
                if (name.EndsWith(".model3.json"))
                {
                    GD.Print(dirname.PathJoin(name));
                    GetNode<OptionButton>("UI/OptModel").AddItem(dirname.PathJoin(name));
                }
            }
            name = dir.GetNext();
        }
    }

    public override void _Ready()
    {
        this.cubism_model = new GDCubismUserModelCS();

        if (ENABLE_MOTION_FINISHED == true)
        {
            this.cubism_model.MotionFinished += this._on_motion_finished;
        }
        this.AddChild(this.cubism_model.GetInternalObject());

        GetNode<OptionButton>("UI/OptModel").Clear();
        GetNode<OptionButton>("UI/OptModel").AddItem("");
        Model3Search("res://addons/gd_cubism/example/res/live2d");
    }

    public override void _Process(double delta)
    {
        Vector2 vct_resolution = new(GetWindow().Size.X, GetWindow().Size.Y);
        float texture_height = Mathf.Floor(vct_resolution.Y / RENDER_SIZE_STEP) * RENDER_SIZE_STEP;
        texture_height = Mathf.Clamp(texture_height, MIX_RENDER_SIZE, MAX_RENDER_SIZE);

        this.cubism_model.Size = new((int)texture_height, (int)texture_height);

        Vector2 vct_viewport_size = new(GetViewportRect().Size.X, GetViewportRect().Size.Y);
        GetNode<Sprite2D>("Sprite2D").Position = vct_viewport_size / 2.0f;

        Vector2I vct_size = this.cubism_model.Size;
        GetNode<Sprite2D>("Sprite2D").Scale = new Vector2(
            vct_viewport_size.Y / vct_size.Y,
            vct_viewport_size.Y / vct_size.Y
        );
    }

    private void _on_motion_finished()
    {
        this.cubism_model.StartMotion(
            (String)this.last_motion["group"],
            (int)this.last_motion["no"],
            GDCubismUserModelCS.PriorityEnum.PriorityForce
        );
    }

    public void _on_opt_model_item_selected(long index)
    {
        Setup(GetNode<OptionButton>("UI/OptModel").GetItemText((int)index));
    }

    private void _on_item_list_motion_item_selected(long index)
    {
        var _motion = GetNode<ItemList>("UI/ItemListMotion").GetItemMetadata((int)index);
        Godot.Collections.Dictionary motion = (Godot.Collections.Dictionary)_motion;
        this.cubism_model.StartMotion(
            (String)motion["group"],
            (int)motion["no"],
            GDCubismUserModelCS.PriorityEnum.PriorityForce
        );
        this.last_motion = motion;
    }

    public void _on_item_list_expression_item_selected(long index)
    {
        var _expression_id = GetNode<ItemList>("UI/ItemListExpression").GetItemText((int)index);
        this.cubism_model.StartExpression((String)_expression_id);
    }
}
