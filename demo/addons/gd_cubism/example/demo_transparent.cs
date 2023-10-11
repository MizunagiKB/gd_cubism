// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using System.Linq;
using Godot;


#pragma warning disable CA1050
#pragma warning disable IDE1006


public partial class demo_transparent : Node2D
{
    private bool order_window_position = false;
    private bool mouse_button_pressed = false;
    private Vector2I window_position;
    private Vector2 mouse_position;

    private bool enable_transparent = false;
    private Godot.Collections.Array<String> ary_character_expression;
    private Godot.Collections.Array ary_character_motion = new Godot.Collections.Array();

    private GDCubismUserModelCS model;

    public override void _Ready()
    {
        this.model = new GDCubismUserModelCS(GetNode<SubViewport>("Sprite2D/GDCubismUserModel"));

        GetNode<Sprite2D>("Sprite2D").Position = new(GetViewportRect().Size.X / 2, GetViewportRect().Size.Y / 2);

        this.ary_character_expression = this.model.GetExpressions();

        Godot.Collections.Dictionary<String, int> dict_motion = this.model.GetMotions();

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
            DisplayServer.WindowSetPosition(new Vector2I((int)window_position.X, (int)window_position.Y));
            this.order_window_position = false;
        }
    }


    public override void _Input(InputEvent @event)
    {

        if (@event is InputEventKey eventKey)
        {

            if (eventKey.IsPressed() == true)
            {
                if (eventKey.Keycode == Key.Escape)
                {
                    GetTree().Quit();
                }

                if (eventKey.Keycode == Key.Space)
                {
                    var random = new RandomNumberGenerator();

                    Godot.Collections.Dictionary motion = (Godot.Collections.Dictionary)this.ary_character_motion[random.RandiRange(0, this.ary_character_motion.Count - 1)];
                    this.model.StartMotionLoop(
                        (String)motion["group"],
                        (int)motion["no"],
                        GDCubismUserModelCS.PriorityEnum.PriorityForce,
                        true,
                        true
                    );
                }
                if (eventKey.Keycode == Key.Tab)
                {
                    if (this.enable_transparent == true)
                    {
                        this.enable_transparent = false;
                    }
                    else
                    {
                        this.enable_transparent = true;
                    }
                    GetTree().Root.TransparentBg = this.enable_transparent;
                    DisplayServer.WindowSetFlag(DisplayServer.WindowFlags.Transparent, this.enable_transparent, 0);
                    DisplayServer.WindowSetFlag(DisplayServer.WindowFlags.AlwaysOnTop, true, 0);
                }

                if (eventKey.Keycode >= Key.Key0 && eventKey.Keycode <= Key.Key9)
                {
                    long index = eventKey.Keycode - Key.Key0;
                    if (index < this.ary_character_expression.Count)
                    {
                        this.model.StartExpression(
                            this.ary_character_expression[(int)index]
                        );
                    }
                }
            }
        }

        if (@event is InputEventMouseButton mouseButton)
        {
            if (mouseButton.Pressed == true)
            {
                this.window_position = DisplayServer.WindowGetPosition();
                this.mouse_position = GetViewport().GetMousePosition();
            }
            else
            {
                GetViewport().WarpMouse(this.mouse_position);
            }
            this.mouse_button_pressed = mouseButton.Pressed;
        }

        if (@event is InputEventMouseMotion mouseMotion)
        {
            if (this.mouse_button_pressed == true)
            {
                this.order_window_position = true;
                this.window_position += new Vector2I((int)mouseMotion.Relative.X, (int)mouseMotion.Relative.Y);
            }
        }
    }
}

