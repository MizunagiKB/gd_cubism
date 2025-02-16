// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
using System;
using Godot;
using Godot.NativeInterop;


#pragma warning disable CA1050


public partial class GDCubismUserModelCS : GodotObject
{
    public enum Moc3FileFormatVersionEnum : long
    {
        /// <summary>Unknown or failed to load.</summary>
        CSM_MOC_VERSION_UNKNOWN = 0,
        /// <summary>moc3 file version 3.0.00 - 3.2.07</summary>
        CSM_MOC_VERSION_30 = 1,
        /// <summary>moc3 file version 3.3.00 - 3.3.03</summary>
        CSM_MOC_VERSION_33 = 2,
        /// <summary>moc3 file version 4.0.00 - 4.1.05</summary>
        CSM_MOC_VERSION_40 = 3,
        /// <summary>moc3 file version 4.2.00 - 4.2.02</summary>
        CSM_MOC_VERSION_42 = 4,
        /// <summary>moc3 file version 5.0.00 -</summary>
        CSM_MOC_VERSION_50 = 5
    }

    public enum PriorityEnum : long
    {
        PriorityNone = 0,
        PriorityIdle = 1,
        PriorityNormal = 2,
        PriorityForce = 3
    }

    public enum ParameterModeEnum : long
    {
        FullParameter = 0,
        NoneParameter = 1
    }

    public enum MotionProcessCallbackEnum : long
    {
        /// <summary>
        ///     The animation update process is performed within the <c>_physics_process</c> function.
        ///     This is used when you want to interact with physical simulations.
        /// </summary>
        Physics = 0,
        /// <summary>
        ///     The animation update process is performed within the _process function.
        /// </summary>
        Idle = 1,
        /// <summary>
        ///     No animation update process is performed. Use the <c>advance</c> function to process the animation.
        /// </summary>
        Manual = 2
    }

    private static readonly StringName NativeName = "GDCubismUserModel";
    public static readonly StringName MotionEventName = "motion_event";
    public static readonly StringName MotionFinishedName = "motion_finished";

    protected SubViewport InternalObject = null;

    public GDCubismUserModelCS()
    {
        this.InternalObject = (SubViewport)Godot.ClassDB.Instantiate(NativeName);
    }

    public GDCubismUserModelCS(SubViewport o)
    {
        this.InternalObject = o;
    }

    public SubViewport GetInternalObject()
    {
        return this.InternalObject;
    }

    public void Attach(SubViewport o)
    {
        this.InternalObject = o;
    }

    public SubViewport Detach()
    {
        SubViewport o = this.InternalObject;
        this.InternalObject = null;
        return o;
    }

    // ------------------------------------------------------------ Property(s)

    /// <value>
    ///     Property <c>AdjustPosition</c>
    ///     <br />
    ///     Changes the rendering position of the Live2D model.
    ///     GDCubism performs the rendering of the Live2D model within its own SubViewport.
    ///     <br />
    ///     By adjusting this parameter, you can close-up a part of the Live2D model or use it when the auto_scale rendered result is not drawn well.
    /// </value>
    public Vector2 AdjustPosition
    {
        set { this.InternalObject.Call("set_adjust_position", value); }
        get { return (Vector2)this.InternalObject.Call("get_adjust_position"); }
    }

    /// <value>
    ///     Property <c>AdjustScale</c>
    ///     <br />
    ///     Changes the rendering size of the Live2D model.
    ///     GDCubism performs the rendering of the Live2D model within its own SubViewport.
    ///     <br />
    ///     By adjusting this parameter, you can freely change the rendering size of the Live2D model.
    /// </value>
    public float AdjustScale
    {
        set { this.InternalObject.Call("set_adjust_scale", value); }
        get { return (float)this.InternalObject.Call("get_adjust_scale"); }
    }

    /// <value>
    ///     Property <c>Assets</c>
    ///     <br />
    ///     You can load a Live2D model by specifying a file with the *.model3.json extension.
    ///     The file is loaded immediately upon specification.
    ///     <br />
    ///     The <c>clear</c> function is called internally, so if you want to switch the Live2D model, you can do so simply by specifying a new file.
    /// </value>
    public String Assets
    {
        set { this.InternalObject.Call("set_assets", value); }
        get { return (String)this.InternalObject.Call("get_assets"); }
    }

    /// <value>
    ///     Property <c>AutoScale</c>
    ///     <br />
    ///     <c>GDCubismUserModel</c> attempts to render the Live2D model to fit within the specified <c>SubViewport</c> size.
    ///     This may result in outcomes that the creator of the Live2D model did not intend.
    ///     <br />
    ///     By unchecking this, the display will be performed without scaling.
    /// </value>
    public bool AutoScale
    {
        set { this.InternalObject.Call("set_auto_scale", value); }
        get { return (bool)this.InternalObject.Call("get_auto_scale"); }
    }

    /// <value>
    ///     Property <c>LoadExpressions</c>
    ///     <br />
    ///     If set to <c>false</c>, it will not load _Expressions_ when loading the Live2D Model.
    /// </value>
    public bool LoadExpressions
    {
        set { this.InternalObject.Call("set_load_expressions", value); }
        get { return (bool)this.InternalObject.Call("get_load_expressions"); }
    }

    /// <value>
    ///     Property <c>LoadMotions</c>
    ///     <br />
    ///     If set to <c>false</c>, it will not load _Motions_ when loading the Live2D Model.
    /// </value>
    public bool LoadMotions
    {
        set { this.InternalObject.Call("set_load_motions", value); }
        get { return (bool)this.InternalObject.Call("get_load_motions"); }
    }

    /// <value>
    ///     Property <c>ParameterMode</c>
    ///     <br />
    ///     Specifies the control method for the currently held Live2D model.
    /// </value>
    public ParameterModeEnum ParameterMode
    {
        set { this.InternalObject.Call("set_parameter_mode", (int)value); }
        get
        {
            int value = (int)this.InternalObject.Call("get_parameter_mode");
            return (ParameterModeEnum)Enum.ToObject(typeof(ParameterModeEnum), value);
        }
    }

    /// <value>
    ///     Property <c>PlaybackProcessMode</c>
    ///     <br />
    ///     Specifies the playback method for the currently held Live2D model.
    /// </value>
    public MotionProcessCallbackEnum PlaybackProcessMode
    {
        set { this.InternalObject.Call("set_process_callback", (int)value); }
        get
        {
            int value = (int)this.InternalObject.Call("get_process_callback");
            return (MotionProcessCallbackEnum)Enum.ToObject(typeof(MotionProcessCallbackEnum), value);
        }
    }

    public Shader ShaderAdd
    {
        set { this.InternalObject.Call("set_shader_add", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_add"); }
    }

    public Shader ShaderMix
    {
        set { this.InternalObject.Call("set_shader_mix", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mix"); }
    }

    public Shader ShaderMul
    {
        set { this.InternalObject.Call("set_shader_mul", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mul"); }
    }

    public Shader ShaderMask
    {
        set { this.InternalObject.Call("set_shader_mask", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask"); }
    }

    public Shader ShaderMaskAdd
    {
        set { this.InternalObject.Call("set_shader_mask_add", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_add"); }
    }

    public Shader ShaderMaskAddInv
    {
        set { this.InternalObject.Call("set_shader_mask_add_inv", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_add_inv"); }
    }

    public Shader ShaderMaskMix
    {
        set { this.InternalObject.Call("set_shader_mask_mix", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_mix"); }
    }

    public Shader ShaderMaskMixInv
    {
        set { this.InternalObject.Call("set_shader_mask_mix_inv", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_mix_inv"); }
    }

    public Shader ShaderMaskMul
    {
        set { this.InternalObject.Call("set_shader_mask_mul", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_mul"); }
    }

    public Shader ShaderMaskMulInv
    {
        set { this.InternalObject.Call("set_shader_mask_mul_inv", (Shader)value); }
        get { return (Shader)this.InternalObject.Call("get_shader_mask_mul_inv"); }
    }

    /// <value>
    ///     Property <c>SpeedScale</c>
    ///     <br />
    ///     Specifies the playback speed for the currently held Live2D model.
    /// </value>
    public float SpeedScale
    {
        set { this.InternalObject.Call("set_speed_scale", (float)value); }
        get { return (float)this.InternalObject.Call("get_speed_scale"); }
    }

    // -------------------------------------------------------------- Method(s)

    /// <summary>
    ///     Advances the animation by the specified <paramref name="delta">delta</paramref> time (in seconds).
    ///     Please specify a value of 0.0 or more for delta.
    /// </summary>
    /// <param name="delta">The time to progress the animation.</param>
    public void Advance(float delta)
    {
        this.InternalObject.Call("advance", delta);
    }

    /// <summary>
    ///     Disposes of the currently held Live2D model.
    /// </summary>
    public void Clear()
    {
        this.InternalObject.Call("clear");
    }

    /// <summary>
    ///     Returns the latest file version that GDCubismUserModel can load.
    /// </summary>
    /// <returns>Moc3FileFormatVersionEnum</returns>
    public Moc3FileFormatVersionEnum CsmGetLatestMocVersion()
    {
        long value = (long)this.InternalObject.Call("csm_get_latest_moc_version");
        return (Moc3FileFormatVersionEnum)Enum.ToObject(typeof(Moc3FileFormatVersionEnum), value);
    }

    /// <summary>
    ///     Returns the version of the loaded moc3 file.
    /// </summary>
    /// <returns>Moc3FileFormatVersionEnum</returns>
    public Moc3FileFormatVersionEnum CsmGetMocVersion()
    {
        long value = (long)this.InternalObject.Call("csm_get_moc_version");
        return (Moc3FileFormatVersionEnum)Enum.ToObject(typeof(Moc3FileFormatVersionEnum), value);
    }

    /// <summary>
    ///     Returns the version number of the Cubism Native SDK Core used by GDCubism in <c>Dictionary</c> format
    ///     <list type="bullet">
    ///         <item>
    ///             <term>version</term>
    ///             <description>int: The value returned by the csmVersion function is stored as is.</description>
    ///         </item>
    ///         <item>
    ///             <term>major</term>
    ///             <description>int: Only the major version is extracted from the version and stored.</description>
    ///         </item>
    ///         <item>
    ///             <term>minor</term>
    ///             <description>int: Only the minor version is extracted from the version and stored.</description>
    ///         </item>
    ///         <item>
    ///             <term>patch</term>
    ///             <description>int: Only the patch number is extracted from the version and stored.</description>
    ///         </item>
    ///     </list>
    /// </summary>
    /// <returns>Godot.Collections.Dictionary</returns>
    public Godot.Collections.Dictionary CsmGetVersion()
    {
        return (Godot.Collections.Dictionary)this.InternalObject.Call("csm_get_version");
    }

    /// <summary>
    ///     Returns the following information in <c>Dictionary</c> format.
    ///     <list type="bullet">
    ///         <item>
    ///             <term>size_in_pixels</term>
    ///             <description>Vector2: Returns the width and height of the canvas of the loaded Live2D model in pixels.</description>
    ///         </item>
    ///         <item>
    ///             <term>origin_in_pixels</term>
    ///             <description>Vector2: Returns the center position of the loaded Live2D model in pixels.</description>
    ///         </item>
    ///         <item>
    ///             <term>ixels_per_unit</term>
    ///             <description>float: Returns the pixelsPerUnit of the loaded Live2D model.</description>
    ///         </item>
    ///     </list>
    /// </summary>
    /// <returns>Godot.Collections.Dictionary</returns>
    public Godot.Collections.Dictionary GetCanvasInfo()
    {
        return (Godot.Collections.Dictionary)this.InternalObject.Call("get_canvas_info");
    }

    /// <summary>
    ///     Returns the information of the <c>Motion</c> currently being played.
    /// </summary>
    /// <returns>Godot.Collections.Array</returns>
    public Godot.Collections.Array GetCubismMotionQueueEntries()
    {
        return (Godot.Collections.Array)this.InternalObject.Call("get_cubism_motion_queue_entries");
    }

    /// <summary>
    ///     Returns a list of Expressions from the currently held Live2D model.
    ///     The information obtained can be used as an argument for the <c>start_expression</c> function.
    /// </summary>
    /// <returns>Godot.Collections.Array</returns>
    public Godot.Collections.Array<String> GetExpressions()
    {
        return (Godot.Collections.Array<String>)this.InternalObject.Call("get_expressions");
    }

    public Godot.Collections.Array GetHitAreas()
    {
        return (Godot.Collections.Array)this.InternalObject.Call("get_hit_areas");
    }

    /// <summary>
    ///     Retrieves the <c>ArrayMesh</c> information of the <c>MeshInstance</c> used in the current Motion state.
    ///     The keys held by this <c>Dictionary</c> are identical to the names of the <c>MeshInstance</c> generated by <c>GDCubismUserModel</c>.
    ///     <br />
    ///     The information that can be obtained is the <c>ArrayMesh</c> used internally.
    ///     <br />
    ///     CAUTION<br />
    ///     get_meshes is an experimental function added in v0.1. Please be aware that the specifications may change or be removed in the future.
    /// </summary>
    /// <returns>Godot.Collections.Dictionary</returns>
    public Godot.Collections.Dictionary GetMeshes()
    {
        return (Godot.Collections.Dictionary)this.InternalObject.Call("get_meshes");
    }

    /// <summary>
    ///     Returns a list of <c>Motions</c> from the currently held Live2D model.
    ///     The returned <c>Dictionary</c> is the <c>group</c> and the number of motions it contains.
    /// </summary>
    /// <returns>Godot.Collections.Dictionary</returns>
    public Godot.Collections.Dictionary<String, int> GetMotions()
    {
        return (Godot.Collections.Dictionary<String, int>)this.InternalObject.Call("get_motions");
    }

    /// <summary>
    ///     Retrieves the controller class for operating the currently held Live2D model.
    /// </summary>
    /// <returns>Godot.Collections.Array</returns>
    public Godot.Collections.Array<GDCubismParameterCS> GetParameters()
    {
        Godot.Collections.Array<GodotObject> ary = (Godot.Collections.Array<GodotObject>)this.InternalObject.Call("get_parameters");
        Godot.Collections.Array<GDCubismParameterCS> ary_result = new();
        foreach (GodotObject o in ary)
        {
            ary_result.Add(new GDCubismParameterCS(o));
        }

        return ary_result;
    }

    /// <summary>
    ///     Retrieves the controller class for operating the transparency of the parts of the currently held Live2D model.
    /// </summary>
    /// <returns>Godot.Collections.Array</returns>
    public Godot.Collections.Array<GDCubismPartOpacityCS> GetPartOpacities()
    {
        Godot.Collections.Array<GodotObject> ary = (Godot.Collections.Array<GodotObject>)this.InternalObject.Call("get_part_opacities");
        Godot.Collections.Array<GDCubismPartOpacityCS> ary_result = new();
        foreach (GodotObject o in ary)
        {
            ary_result.Add(new GDCubismPartOpacityCS(o));
        }

        return ary_result;
    }

    /// <summary>
    ///     Plays the specified <paramref name="expression_id">expression_id</paramref>.
    /// </summary>
    /// <param name="expression_id"></param>
    public void StartExpression(String expression_id)
    {
        this.InternalObject.Call("start_expression", expression_id);
    }

    /// <summary>
    ///     Plays the specified <paramref name="grp_name">group</paramref> and <paramref name="no">no</paramref>.
    /// </summary>
    /// <param name="grp_name"></param>
    /// <param name="no"></param>
    /// <param name="priority"></param>
    public void StartMotion(String grp_name, int no, PriorityEnum priority)
    {
        this.InternalObject.Call("start_motion", grp_name, no, (int)priority);
    }

    /// <summary>
    ///     Plays the specified <paramref name="grp_name">group</paramref> and <paramref name="no">no</paramref>.
    ///     If you want to play it repeatedly, specify it.
    ///     If the loop argument is <c>false</c>, it will behave the same as the <c>start_motion</c> function.
    /// </summary>
    /// <param name="grp_name"></param>
    /// <param name="no"></param>
    /// <param name="priority"></param>
    /// <param name="loop"></param>
    /// <param name="loop_in_fade"></param>
    public void StartMotionLoop(String grp_name, int no, PriorityEnum priority, bool loop, bool loop_in_fade)
    {
        this.InternalObject.Call("start_motion_loop", grp_name, no, (int)priority, loop, loop_in_fade);
    }

    /// <summary>
    ///     It stops the currently playing <c>Expression</c>.
    /// </summary>
    public void StopExpression()
    {
        this.InternalObject.Call("stop_expression");
    }

    /// <summary>
    ///     It stops the currently playing <c>Motion</c>.
    /// </summary>
    public void StopMotion()
    {
        this.InternalObject.Call("stop_motion");
    }

    public Vector2I Size
    {
        set { this.InternalObject.Size = (Vector2I)value; }
        get { return this.InternalObject.Size; }
    }

    public ViewportTexture GetTexture()
    {
        return (ViewportTexture)this.InternalObject.Call("get_texture");
    }

    // -------------------------------------------------------------- Signal(s)

    public delegate void MotionEventEventHandler(string value);

    private static void MotionEventTrampoline(object delegateObj, NativeVariantPtrArgs args, out godot_variant ret)
    {
        ((MotionEventEventHandler)delegateObj)(VariantUtils.ConvertTo<string>(args[0]));
        ret = default;
    }

    public unsafe event MotionEventEventHandler MotionEvent
    {
        add => this.InternalObject.Connect(MotionEventName, Callable.CreateWithUnsafeTrampoline(value, &MotionEventTrampoline));
        remove => this.InternalObject.Disconnect(MotionEventName, Callable.CreateWithUnsafeTrampoline(value, &MotionEventTrampoline));
    }

    public event Action MotionFinished
    {
        add => this.InternalObject.Connect(MotionFinishedName, Callable.From(value));
        remove => this.InternalObject.Disconnect(MotionFinishedName, Callable.From(value));
    }
}
