// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;


#pragma warning disable IDE1006


public partial class viewer : Control
{
    const bool ENABLE_MOTION_FINISHED = true;

    GDCubismUserModelCS cubism_model;
    Godot.Collections.Dictionary last_motion;

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

    public void Setup(String pathname)
    {
        this.cubism_model.Assets = pathname;

        this.recalc_model_position(this.cubism_model);

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
        this.AddChild(this.cubism_model.GetInternalObject());

        if (ENABLE_MOTION_FINISHED == true)
        {
            this.cubism_model.MotionFinished += this._on_motion_finished;
        }

        GetNode<OptionButton>("UI/OptModel").Clear();
        GetNode<OptionButton>("UI/OptModel").AddItem("");
        Model3Search("res://addons/gd_cubism/example/res/live2d");
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
