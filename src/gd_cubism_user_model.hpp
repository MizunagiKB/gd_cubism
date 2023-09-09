#ifndef GD_CUBISM_USER_MODEL_H
#define GD_CUBISM_USER_MODEL_H


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>

#include <CubismFramework.hpp>
#include <Math/CubismVector2.hpp>
#include <Motion/ACubismMotion.hpp>
#include <Motion/CubismMotionQueueEntry.hpp>

#include <gd_cubism_effect.hpp>
#include <gd_cubism_motion_entry.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class InternalCubismUserModel;
class GDCubismEffect;


// ------------------------------------------------------------------- class(s)
class GDCubismUserModel : public SubViewport {
    GDCLASS(GDCubismUserModel, SubViewport);

public:
    GDCubismUserModel();
    ~GDCubismUserModel();

public:
    enum moc3FileFormatVersion {
        CSM_MOC_VERSION_UNKNOWN = Live2D::Cubism::Core::csmMocVersion_Unknown,
        CSM_MOC_VERSION_30 = Live2D::Cubism::Core::csmMocVersion_30,
        CSM_MOC_VERSION_33 = Live2D::Cubism::Core::csmMocVersion_33,
        CSM_MOC_VERSION_40 = Live2D::Cubism::Core::csmMocVersion_40,
        CSM_MOC_VERSION_42 = Live2D::Cubism::Core::csmMocVersion_42,
        CSM_MOC_VERSION_50 = Live2D::Cubism::Core::csmMocVersion_50
    };

    enum Priority {
        PRIORITY_NONE = 0,
        PRIORITY_IDLE = 1,
        PRIORITY_NORMAL = 2,
        PRIORITY_FORCE = 3
    };

    enum ParameterMode {
        FULL_PARAMETER = 0,
        NONE_PARAMETER = 1
    };

    enum MotionProcessCallback {
        PHYSICS = 0,
        IDLE = 1,
        MANUAL = 2
    };

    String assets;
    InternalCubismUserModel *internal_model;
    float speed_scale;
    bool auto_scale;
    ParameterMode parameter_mode;
    MotionProcessCallback playback_process_mode;

    Array ary_shader;
    Array ary_parameter;
    Array ary_part_opacity;

    Csm::csmVector<GDCubismEffect*> _list_cubism_effect;
    bool cubism_effect_dirty;

protected:
    static void _bind_methods();

public:
    Dictionary csm_get_version();
    moc3FileFormatVersion csm_get_latest_moc_version();
    moc3FileFormatVersion csm_get_moc_version();

    void set_assets(const String assets);
    String get_assets() const;

    Dictionary get_canvas_info() const;

    bool is_initialized() const;
    void clear();

    void set_parameter_mode(const ParameterMode value);
    GDCubismUserModel::ParameterMode get_parameter_mode() const;

    void set_process_callback(const MotionProcessCallback value);
    GDCubismUserModel::MotionProcessCallback get_process_callback() const;

    void set_motion_group_idle(const String &speed);
    String get_motion_group_idle() const;

    void set_speed_scale(const float speed);
    float get_speed_scale() const;

    void set_auto_scale(const bool value);
    bool get_auto_scale() const;

    Dictionary get_motions() const;
    Ref<GDCubismMotionQueueEntryHandle> start_motion(const String str_group, Csm::csmInt32 no, Priority priority);
    Array get_cubism_motion_queue_entries() const;
    void stop_motion();

    Array get_expressions() const;
    void start_expression(const String expression_id);
    void stop_expression();

    Array get_hit_areas() const;

    Array get_parameter() const;
    Array get_part_opacity() const;

    void set_shader(const GDCubismShader e, Ref<Shader> shader) { this->ary_shader[e] = shader; }
    Ref<Shader> get_shader(const GDCubismShader e) const { return this->ary_shader[e]; }
    void set_shader_mix(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_NORM_MIX, shader); }
    Ref<Shader> get_shader_mix() const { return this->get_shader(GD_CUBISM_SHADER_NORM_MIX); }
    void set_shader_mask(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK, shader); }
    Ref<Shader> get_shader_mask() const { return this->get_shader(GD_CUBISM_SHADER_MASK); }
    void set_shader_mask_mix(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MIX, shader); }
    Ref<Shader> get_shader_mask_mix() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MIX); }
    void set_shader_mask_mix_inv(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MUL_INV, shader); }
    Ref<Shader> get_shader_mask_mix_inv() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MUL_INV); }    

    // for Signal
    static void on_motion_finished(Csm::ACubismMotion* motion);

    void _update(const float delta);

    void advance(const float delta);

    bool check_cubism_effect_dirty() const;
    void cubism_effect_dirty_reset();

    bool _set(const StringName &p_name, const Variant &p_value);
    bool _get(const StringName &p_name, Variant &r_ret) const;
    bool _property_can_revert(const StringName &p_name) const;
    bool _property_get_revert(const StringName &p_name, Variant &r_property) const;
    void _get_property_list(List<godot::PropertyInfo> *p_list);

    void _ready() override;
    void _enter_tree() override;
    void _exit_tree() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;

    void _on_append_child_act(GDCubismEffect* node);
    void _on_remove_child_act(GDCubismEffect* node);
};

VARIANT_ENUM_CAST(GDCubismUserModel::moc3FileFormatVersion);
VARIANT_ENUM_CAST(GDCubismUserModel::Priority);
VARIANT_ENUM_CAST(GDCubismUserModel::ParameterMode);
VARIANT_ENUM_CAST(GDCubismUserModel::MotionProcessCallback);


// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM_USER_MODEL_H
