// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM_EFFECT_HIT_AREA
#define GD_CUBISM_EFFECT_HIT_AREA


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <CubismFramework.hpp>
#include <CubismDefaultParameterId.hpp>
#include <Id/CubismIdManager.hpp>

#include <private/internal_cubism_user_model.hpp>
#include <gd_cubism_effect.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::DefaultParameterId;
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
class GDCubismEffectHitArea : public GDCubismEffect {
    GDCLASS(GDCubismEffectHitArea, GDCubismEffect);

protected:
    static void _bind_methods() {
    	ClassDB::bind_method(D_METHOD("set_target", "target"), &GDCubismEffectHitArea::set_target);
    	ClassDB::bind_method(D_METHOD("get_target"), &GDCubismEffectHitArea::get_target);

    	ClassDB::bind_method(D_METHOD("get_detail", "model", "id"), &GDCubismEffectHitArea::get_detail);

    	ClassDB::bind_method(D_METHOD("set_monitoring", "value"), &GDCubismEffectHitArea::set_monitoring);
    	ClassDB::bind_method(D_METHOD("get_monitoring"), &GDCubismEffectHitArea::get_monitoring);
        ADD_PROPERTY(PropertyInfo(Variant::BOOL, "monitoring"), "set_monitoring", "get_monitoring");

        ADD_SIGNAL(MethodInfo(
            SIGNAL_EFFECT_HIT_AREA_ENTERED,
            PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel"),
            PropertyInfo(Variant::STRING, "id")
        ));
        ADD_SIGNAL(MethodInfo(
            SIGNAL_EFFECT_HIT_AREA_EXITED,
            PropertyInfo(Variant::OBJECT, "model", PROPERTY_HINT_RESOURCE_TYPE, "GDCubismUserModel"),
            PropertyInfo(Variant::STRING, "id")
        ));
    }

private:
    Vector2 _target;
    bool _target_update = false;
    bool _monitoring = true;
    Dictionary _dict_monitoring;

    Rect2 build_rect2(const Ref<ArrayMesh>& ref_ary_mesh) const {
        Array ary = ref_ary_mesh->surface_get_arrays(0);
        PackedVector2Array ary_vtx = ary[Mesh::ARRAY_VERTEX];
        const int64_t vtx_count = ary_vtx.size();

        Rect2 check_rect(ary_vtx[0].x, ary_vtx[0].y, 0, 0);

        for(int64_t i = 1; i < vtx_count; i++) {
            check_rect.expand_to(ary_vtx[i]);
        }

        return check_rect;
    }

    Array check_collide_triangle(const Ref<ArrayMesh>& ref_ary_mesh) const {
        if(ref_ary_mesh->surface_get_primitive_type(0) != Mesh::PRIMITIVE_TRIANGLES) return Array();

        Array ary = ref_ary_mesh->surface_get_arrays(0);
        PackedVector2Array ary_vtx = ary[Mesh::ARRAY_VERTEX];
        PackedInt32Array ary_idx = ary[Mesh::ARRAY_INDEX];
        const int64_t vtx_count = ary_vtx.size();

        Array ary_result;

        for(int64_t i = 0; i < ary_idx.size(); i += 3) {
            const int32_t ia = ary_idx[i + 0];
            const int32_t ib = ary_idx[i + 1];
            const int32_t ic = ary_idx[i + 2];

            const Vector2 ab = ary_vtx[ib] - ary_vtx[ia];
            const Vector2 bc = ary_vtx[ic] - ary_vtx[ib];
            const Vector2 ca = ary_vtx[ia] - ary_vtx[ic];
            const Vector2 ax = this->_target - ary_vtx[ia];
            const Vector2 bx = this->_target - ary_vtx[ib];
            const Vector2 cx = this->_target - ary_vtx[ic];

            if((ab.cross(ax) * bc.cross(bx)) > 0.0) {
                if((bc.cross(bx) * ca.cross(cx)) > 0.0) {
                    ary_result.append(ary_vtx[ia]);
                    ary_result.append(ary_vtx[ib]);
                    ary_result.append(ary_vtx[ic]);
                    break;
                }
            }
        }

        return ary_result;
    }

public:
    void set_target(const Vector2& target) { this->_target = target; this->_target_update = true; }
    Vector2 get_target() const { return this->_target; }

    Dictionary get_detail(const GDCubismUserModel* model, const String& id) {
        if(this->_initialized == false) return Dictionary();
        if(model == nullptr) return Dictionary();
        if(model->is_initialized() == false) return Dictionary();

        const Dictionary dict_mesh = model->get_meshes();
        if(dict_mesh.is_empty() == true) return Dictionary();

        MeshInstance2D* p_mesh_inst = cast_to<MeshInstance2D>(dict_mesh[id]);
        Ref<ArrayMesh> ref_ary_mesh =  static_cast<ArrayMesh*>(p_mesh_inst->get_mesh().ptr());
        if(ref_ary_mesh.is_valid() != true) return Dictionary();
        if(ref_ary_mesh->surface_get_array_index_len(0) < 3) return Dictionary();

        Dictionary dict_result;

        Rect2 rect = this->build_rect2(ref_ary_mesh);
        dict_result["rect"] = rect;

        if(rect.has_point(this->_target) == true) {
            Array vertices = this->check_collide_triangle(ref_ary_mesh);
            if(vertices.size() == 3) {
                dict_result["vertices"] = vertices;
            }
        }

        return dict_result;
    }

    void set_monitoring(const bool& value) {
        this->_dict_monitoring.clear();
        this->_monitoring = value;
    }

    bool get_monitoring() const { return this->_monitoring; }

    virtual void _cubism_init(InternalCubismUserModel* model) override {
        if(this->_initialized == true) return;

        this->_dict_monitoring.clear();
        this->_target_update = false;

        this->_initialized = true;
    }

    virtual void _cubism_term(InternalCubismUserModel* model) override {
        if(this->_initialized == false) return;

        this->_dict_monitoring.clear();

        this->_initialized = false;
    }

    virtual void _cubism_process(InternalCubismUserModel* model, const double delta) override {
        if(this->_initialized == false) return;
        if(this->_active == false) return;

        Csm::ICubismModelSetting* setting = model->_model_setting;
        Array ary = model->_owner_viewport->get_hit_areas();
        Dictionary dict_mesh = model->_owner_viewport->get_meshes();

        for(int64_t i = 0; i < ary.size(); i++) {
            const String id = static_cast<Dictionary>(ary[i]).get("id", String());
            if(dict_mesh.has(id) != true) continue;

            MeshInstance2D* p_mesh_inst = cast_to<MeshInstance2D>(dict_mesh[id]);
            Ref<ArrayMesh> ref_ary_mesh =  static_cast<ArrayMesh*>(p_mesh_inst->get_mesh().ptr());
            if(ref_ary_mesh.is_valid() != true) continue;
            if(ref_ary_mesh->surface_get_array_index_len(0) < 3) continue;

            Rect2 check_rect = this->build_rect2(ref_ary_mesh);

            if(this->_target_update == true && check_rect.has_point(this->_target) == true) {
                if(this->_monitoring == true) {
                    if(static_cast<bool>(this->_dict_monitoring.get(id, false)) == false) {
                        this->emit_signal(SIGNAL_EFFECT_HIT_AREA_ENTERED, model->_owner_viewport, id);
                    }
                } else {
                    this->emit_signal(SIGNAL_EFFECT_HIT_AREA_ENTERED, model->_owner_viewport, id);
                }
                this->_dict_monitoring[id] = true;
            } else {
                if(static_cast<bool>(this->_dict_monitoring.get(id, false)) == true) {
                    this->emit_signal(SIGNAL_EFFECT_HIT_AREA_EXITED, model->_owner_viewport, id);
                }
                this->_dict_monitoring[id] = false;
            }
        }

        this->_target_update = false;
    }
};


#endif // GD_CUBISM_EFFECT_HIT_AREA
