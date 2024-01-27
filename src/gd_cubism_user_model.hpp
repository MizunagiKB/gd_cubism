// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
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
struct anim_expression {
public:
    String expression_id;

public:
    anim_expression() {}
    anim_expression(const Csm::csmChar* c_expression_id)
        : expression_id(String(c_expression_id)) {}

    String to_string() const {
        return String(this->expression_id);
    }
};


class anim_motion {
public:
    String group;
    int32_t no;

public:
    anim_motion() {}
    anim_motion(const Csm::csmChar* c_group, const int32_t c_no)
        : group(String(c_group))
        , no(c_no) {}

    String to_string() const {
        Array ary;
        ary.append(this->group);
        ary.append(this->no);

        return String("{0}_{1}").format(ary);
    }
};


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
    bool enable_load_expressions;
    bool enable_load_motions;

    float speed_scale;
    bool auto_scale;
    float adjust_scale;
    Vector2 adjust_pos;

    ParameterMode parameter_mode;
    MotionProcessCallback playback_process_mode;

    Array ary_shader;
    Array ary_parameter;
    Array ary_part_opacity;

    Csm::csmBool anim_loop;
    Csm::csmBool anim_loop_fade_in;
    Csm::csmMap<String,anim_expression> dict_anim_expression;
    String curr_anim_expression_key;
    Csm::csmMap<String,anim_motion> dict_anim_motion;
    String curr_anim_motion_key;

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

    void set_load_expressions(const bool enable);
    bool get_load_expressions() const;
    void set_load_motions(const bool enable);
    bool get_load_motions() const;

    Dictionary get_canvas_info() const;

    bool is_initialized() const;
    void clear();

    void set_parameter_mode(const ParameterMode value);
    GDCubismUserModel::ParameterMode get_parameter_mode() const;

    void set_process_callback(const MotionProcessCallback value);
    GDCubismUserModel::MotionProcessCallback get_process_callback() const;

    void set_speed_scale(const float speed);
    float get_speed_scale() const;

    void set_auto_scale(const bool value);
    bool get_auto_scale() const;

    void set_adjust_scale(const float scale) { this->adjust_scale = scale; }
    float get_adjust_scale() const { return this->adjust_scale; }

    void set_adjust_position(const Vector2 pos) { this->adjust_pos = pos; }
    Vector2 get_adjust_position() const { return this->adjust_pos; }

    Dictionary get_motions() const;
    Ref<GDCubismMotionQueueEntryHandle> start_motion(const String str_group, const int32_t no, const Priority priority);
    Ref<GDCubismMotionQueueEntryHandle> start_motion_loop(const String str_group, const int32_t no, const Priority priority, const bool loop, const bool loop_fade_in);
    Array get_cubism_motion_queue_entries() const;
    void stop_motion();

    Array get_expressions() const;
    void start_expression(const String expression_id);
    void stop_expression();

    Array get_hit_areas() const;

    Array get_parameters() const;
    Array get_part_opacities() const;

    Dictionary get_meshes() const;

    void set_shader(const GDCubismShader e, Ref<Shader> shader) { this->ary_shader[e] = shader; }
    Ref<Shader> get_shader(const GDCubismShader e) const { return this->ary_shader[e]; }

    // NormAdd
    void set_shader_add(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_NORM_ADD, shader); }
    Ref<Shader> get_shader_add() const { return this->get_shader(GD_CUBISM_SHADER_NORM_ADD); }
    // NormMix
    void set_shader_mix(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_NORM_MIX, shader); }
    Ref<Shader> get_shader_mix() const { return this->get_shader(GD_CUBISM_SHADER_NORM_MIX); }
    // NormMul
    void set_shader_mul(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_NORM_MUL, shader); }
    Ref<Shader> get_shader_mul() const { return this->get_shader(GD_CUBISM_SHADER_NORM_MUL); }
    // Mask
    void set_shader_mask(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK, shader); }
    Ref<Shader> get_shader_mask() const { return this->get_shader(GD_CUBISM_SHADER_MASK); }
    // MaskAdd
    void set_shader_mask_add(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_ADD, shader); }
    Ref<Shader> get_shader_mask_add() const { return this->get_shader(GD_CUBISM_SHADER_MASK_ADD); }
    void set_shader_mask_add_inv(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_ADD_INV, shader); }
    Ref<Shader> get_shader_mask_add_inv() const { return this->get_shader(GD_CUBISM_SHADER_MASK_ADD_INV); }    
    // MaskMix
    void set_shader_mask_mix(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MIX, shader); }
    Ref<Shader> get_shader_mask_mix() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MIX); }
    void set_shader_mask_mix_inv(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MIX_INV, shader); }
    Ref<Shader> get_shader_mask_mix_inv() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MIX_INV); }    
    // MaskMul
    void set_shader_mask_mul(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MUL, shader); }
    Ref<Shader> get_shader_mask_mul() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MUL); }
    void set_shader_mask_mul_inv(Ref<Shader> shader) { this->set_shader(GD_CUBISM_SHADER_MASK_MUL_INV, shader); }
    Ref<Shader> get_shader_mask_mul_inv() const { return this->get_shader(GD_CUBISM_SHADER_MASK_MUL_INV); }    

    // for Signal
    static void on_motion_finished(Csm::ACubismMotion* motion);

    void _update(const float delta);

    void advance(const float delta);

    bool check_cubism_effect_dirty() const;
    void cubism_effect_dirty_reset();

    // Properties
    void setup_property();

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
