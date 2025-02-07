// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#ifdef GD_CUBISM_USE_RENDERER_2D

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Rendering/CubismRenderer.hpp>

#include <private/internal_cubism_renderer_2d.hpp>
#include <private/internal_cubism_renderer_resource.hpp>
#include <private/internal_cubism_user_model.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Core;
using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
PackedInt32Array make_PackedArrayInt32(const csmUint16 *ptr, const int32_t &size);
PackedVector2Array make_PackedArrayVector2(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size);
PackedVector2Array make_PackedArrayVector3(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size, const Csm::csmFloat32 &ppunit, const Vector2 &vct_adjust);
const Vector4 make_vector4(const Live2D::Cubism::Core::csmVector4 &src_vec4);

// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismRenderer2D::InternalCubismRenderer2D()
{
}

InternalCubismRenderer2D::~InternalCubismRenderer2D()
{
}

void InternalCubismRenderer2D::update_material(const Csm::CubismModel *model, const Csm::csmInt32 index, const Ref<ShaderMaterial> mat) const
{
    const CubismTextureColor color_base = this->GetModelColorWithOpacity(model->GetDrawableOpacity(index));

    mat->set_shader_parameter("color_base", Vector4(color_base.R, color_base.G, color_base.B, color_base.A));
    mat->set_shader_parameter("color_screen", make_vector4(model->GetDrawableScreenColor(index)));
    mat->set_shader_parameter("color_multiply", make_vector4(model->GetDrawableMultiplyColor(index)));
}

void InternalCubismRenderer2D::make_ArrayMesh_prepare(
    const Csm::CubismModel *model,
    InternalCubismRendererResource &res)
{
    const Vector2 vct_size = this->get_size(model);
    const Vector2 vct_origin = this->get_origin(model);
    const float ppunit = this->get_ppunit(model);

    res.vct_canvas_size = res._owner_viewport->get_size();

    if (res._owner_viewport->mask_viewport_size.x > 0 && res._owner_viewport->mask_viewport_size.y > 0)
    {
        res.vct_mask_size = res._owner_viewport->mask_viewport_size;
    }
    else
    {
        res.vct_mask_size = res._owner_viewport->get_size();
    }

    res.CALCULATED_ORIGIN_C = (Vector2(res.vct_canvas_size) * vct_origin) / vct_size;
    res.CALCULATED_ORIGIN_M = (Vector2(res.vct_mask_size) * vct_origin) / vct_size;
    res.RATIO = float(res.vct_mask_size.x) / float(res.vct_canvas_size.x);

    if (res._owner_viewport->auto_scale == true)
    {
        float fdstC = godot::MIN(res.vct_canvas_size.x, res.vct_canvas_size.y);
        float fdstM = godot::MIN(res.vct_mask_size.x, res.vct_mask_size.y);
        float fsrc = godot::MAX(vct_size.x, vct_size.y);

        res.CALCULATED_PPUNIT_C = (fdstC * ppunit) / fsrc;
        res.CALCULATED_PPUNIT_M = (fdstM * ppunit) / fsrc;
    }
    else
    {
        res.CALCULATED_PPUNIT_C = ppunit;
        res.CALCULATED_PPUNIT_M = ppunit * res.RATIO;
    }
}

void InternalCubismRenderer2D::update_mesh(
    const Csm::CubismModel *model,
    const Csm::csmInt32 index,
    const bool maskmode,
    const InternalCubismRendererResource &res,
    const Ref<ArrayMesh> ary_mesh
) const
{
    const Vector2 adjust_pos = res.adjust_pos;

    ary_mesh->clear_surfaces();

    Array ary;

    ary.resize(Mesh::ARRAY_MAX);

    if (maskmode == true)
    {
        if (res._owner_viewport->auto_scale == true)
        {
            ary[Mesh::ARRAY_VERTEX] = make_PackedArrayVector3(
                model->GetDrawableVertexPositions(index),
                model->GetDrawableVertexCount(index),
                res.CALCULATED_PPUNIT_M,
                res.CALCULATED_ORIGIN_M + adjust_pos * res.RATIO);
        }
        else
        {
            ary[Mesh::ARRAY_VERTEX] = make_PackedArrayVector3(
                model->GetDrawableVertexPositions(index),
                model->GetDrawableVertexCount(index),
                res.CALCULATED_PPUNIT_M * res.adjust_scale,
                res.CALCULATED_ORIGIN_M + adjust_pos * res.RATIO);
        }
    }
    else
    {
        ary[Mesh::ARRAY_VERTEX] = make_PackedArrayVector3(
            model->GetDrawableVertexPositions(index),
            model->GetDrawableVertexCount(index),
            res.CALCULATED_PPUNIT_C * res.adjust_scale,
            res.CALCULATED_ORIGIN_C + res.adjust_pos);
    }

    ary[Mesh::ARRAY_TEX_UV] = make_PackedArrayVector2(
        model->GetDrawableVertexUvs(index),
        model->GetDrawableVertexCount(index));

    ary[Mesh::ARRAY_INDEX] = make_PackedArrayInt32(
        model->GetDrawableVertexIndices(index),
        model->GetDrawableVertexIndexCount(index));

    ary_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, ary);
}

Vector2 InternalCubismRenderer2D::get_size(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_size.X, vct_size.Y);
}

Vector2 InternalCubismRenderer2D::get_origin(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_origin.X, vct_origin.Y);
}

float InternalCubismRenderer2D::get_ppunit(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return ppunit;
}

void InternalCubismRenderer2D::update(InternalCubismRendererResource &res)
{
    const CubismModel *model = this->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    this->make_ArrayMesh_prepare(
        model,
        res);

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;
        
        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());
        MeshInstance2D *node = Object::cast_to<MeshInstance2D>(res.dict_mesh[node_name]);
        if (node == nullptr) {
            continue;
        }
        const bool visible = model->GetDrawableDynamicFlagIsVisible(index);
        node->set_visible(visible);
        if (!visible) {
            continue;
        }
        Ref<ShaderMaterial> mat = node->get_material();

        this->update_mesh(model, index, false, res, node->get_mesh());
        this->update_material(model, index, mat);
        node->set_z_index(renderOrder[index]);

        if (model->GetDrawableMaskCounts()[index] > 0) {
            SubViewport *viewport = Object::cast_to<SubViewport>(node->get_meta("viewport"));
            viewport->set_size(res.vct_mask_size);
            mat->set_shader_parameter("auto_scale", res._owner_viewport->auto_scale);
            mat->set_shader_parameter("canvas_size", Vector2(res.vct_canvas_size));
            mat->set_shader_parameter("mask_size", Vector2(res.vct_mask_size));
            mat->set_shader_parameter("ratio", res.RATIO);
            mat->set_shader_parameter("adjust_scale", res.adjust_scale);
            mat->set_shader_parameter("adjust_pos", res.adjust_pos);

            const Array masks = res.dict_mask[node_name];
            
            for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
            {
                Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];

                if (model->GetDrawableVertexCount(j) == 0)
                    continue;
                if (model->GetDrawableVertexIndexCount(j) == 0)
                    continue;
        
                MeshInstance2D *node = Object::cast_to<MeshInstance2D>(masks[m_index]);
                this->update_mesh(model, j, true, res, node->get_mesh());
                node->set_z_index(renderOrder[index]);
            }
        }
    }
}

void InternalCubismRenderer2D::build_model(InternalCubismRendererResource &res, Node* target_node)
{
    const CubismModel *model = this->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    this->make_ArrayMesh_prepare(
        model,
        res);

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;

        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());

        MeshInstance2D* node = res.request_mesh_instance();
        ShaderMaterial* mat = res.request_shader_material(model, index);
        node->set_material(mat);        
        this->update_mesh(model, index, false, res, node->get_mesh());
        node->set_name(node_name);

        // build mask
        if (model->GetDrawableMaskCounts()[index] > 0)
        {
            TypedArray<MeshInstance2D> masks;
            
            SubViewport* viewport = memnew(SubViewport);
            {
                viewport->set_size(res.vct_mask_size);

                viewport->set_disable_3d(SUBVIEWPORT_DISABLE_3D_FLAG);
                viewport->set_clear_mode(SubViewport::ClearMode::CLEAR_MODE_ALWAYS);
                // set_update_mode must be specified
                viewport->set_update_mode(SubViewport::UpdateMode::UPDATE_ALWAYS);
                viewport->set_disable_input(true);
                // Memory leak when set_use_own_world_3d is true
                // https://github.com/godotengine/godot/issues/81476
                viewport->set_use_own_world_3d(SUBVIEWPORT_USE_OWN_WORLD_3D_FLAG);
                // Memory leak when set_transparent_background is true(* every time & window minimize)
                // https://github.com/godotengine/godot/issues/89651
                viewport->set_transparent_background(true);

                masks.resize(model->GetDrawableMaskCounts()[index]);

                for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
                {
                    Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];
                    
                    if (model->GetDrawableVertexCount(j) == 0)
                        continue;
                    if (model->GetDrawableVertexIndexCount(j) == 0)
                        continue;
            
                    CubismIdHandle handle = model->GetDrawableId(j);
                    String mask_name(handle->GetString().GetRawString());

                    MeshInstance2D *node = res.request_mesh_instance();
                    ShaderMaterial *mat = res.request_mask_material();
                    this->update_mesh(model, j, true, res, node->get_mesh());

                    node->set_name(mask_name);
                    node->set_material(mat);
                    mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
                    mat->set_shader_parameter("tex_main", res.ary_texture[model->GetDrawableTextureIndex(index)]);

                    node->set_z_index(model->GetDrawableRenderOrders()[index]);
                    node->set_visible(true);

                    masks[m_index] = node;

                    viewport->add_child(node);
                    res.managed_nodes.append(node);
                }
            }

            target_node->add_child(viewport);
            res.managed_nodes.append(viewport);

            mat->set_shader_parameter("tex_mask", viewport->get_texture());
            mat->set_shader_parameter("auto_scale", res._owner_viewport->auto_scale);
            mat->set_shader_parameter("canvas_size", Vector2(res.vct_canvas_size));
            mat->set_shader_parameter("mask_size", Vector2(res.vct_mask_size));
            mat->set_shader_parameter("ratio", res.RATIO);
            mat->set_shader_parameter("adjust_scale", res.adjust_scale);
            mat->set_shader_parameter("adjust_pos", res.adjust_pos);

            viewport->set_name(node_name + "__mask");
            res.dict_mask[node_name] = masks;

            node->set_meta("viewport", viewport);
        }

        res.dict_mesh[node_name] = node;
        target_node->add_child(node);
        res.managed_nodes.append(node);
    }
}

void InternalCubismRenderer2D::Initialize(Csm::CubismModel *model, Csm::csmInt32 maskBufferCount)
{
    CubismRenderer::Initialize(model, maskBufferCount);
}

void InternalCubismRenderer2D::DoDrawModel() {}
void InternalCubismRenderer2D::SaveProfile() {}
void InternalCubismRenderer2D::RestoreProfile() {}

// ------------------------------------------------------------------ method(s)
PackedInt32Array make_PackedArrayInt32(const csmUint16 *ptr, const int32_t &size)
{
    PackedInt32Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, ptr[i]);
    }
    return ary;
}

PackedVector2Array make_PackedArrayVector2(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size)
{
    PackedVector2Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, Vector2(ptr[i].X, ptr[i].Y));
    }
    return ary;
}

PackedVector2Array make_PackedArrayVector3(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size, const Csm::csmFloat32 &ppunit, const Vector2 &vct_adjust)
{
    PackedVector2Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, (Vector2(ptr[i].X, ptr[i].Y * -1.0) * ppunit) + vct_adjust);
    }
    return ary;
}

const Vector4 make_vector4(const Live2D::Cubism::Core::csmVector4 &src_vec4)
{
    return Vector4(src_vec4.X, src_vec4.Y, src_vec4.Z, src_vec4.W);
}

#endif // GD_CUBISM_USE_RENDERER_2D
