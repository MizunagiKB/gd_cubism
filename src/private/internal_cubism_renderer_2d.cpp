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
PackedInt32Array make_PackedArrayInt32(const csmUint16 *ptr, const int32_t size);
PackedVector2Array make_PackedArrayVector2(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t size);
PackedVector2Array make_PackedArrayVector3(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t size, const Csm::csmFloat32 ppunit, Vector2 vct_adjust);
const Vector4 make_vector4(const Live2D::Cubism::Core::csmVector4 &src_vec4);

// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismRenderer2D::InternalCubismRenderer2D()
{
}

InternalCubismRenderer2D::~InternalCubismRenderer2D()
{
}

Ref<ShaderMaterial> InternalCubismRenderer2D::make_ShaderMaterial(const Csm::CubismModel *model, const Csm::csmInt32 index, const InternalCubismRendererResource &res) const
{
    Ref<ShaderMaterial> mat;
    mat.instantiate();

    GDCubismShader e = GD_CUBISM_SHADER_NORM_MIX;

    if (model->GetDrawableMaskCounts()[index] == 0)
    {
        switch (model->GetDrawableBlendMode(index))
        {
        case CubismBlendMode_Additive:
            e = GD_CUBISM_SHADER_NORM_ADD;
            break;
        case CubismBlendMode_Normal:
            e = GD_CUBISM_SHADER_NORM_MIX;
            break;
        case CubismBlendMode_Multiplicative:
            e = GD_CUBISM_SHADER_NORM_MUL;
            break;
        default:
            e = GD_CUBISM_SHADER_NORM_MIX;
            break;
        }
    }
    else
    {
        if (model->GetDrawableInvertedMask(index) == false)
        {
            switch (model->GetDrawableBlendMode(index))
            {
            case CubismBlendMode_Additive:
                e = GD_CUBISM_SHADER_MASK_ADD;
                break;
            case CubismBlendMode_Normal:
                e = GD_CUBISM_SHADER_MASK_MIX;
                break;
            case CubismBlendMode_Multiplicative:
                e = GD_CUBISM_SHADER_MASK_MUL;
                break;
            default:
                e = GD_CUBISM_SHADER_MASK_MIX;
                break;
            }
        }
        else
        {
            switch (model->GetDrawableBlendMode(index))
            {
            case CubismBlendMode_Additive:
                e = GD_CUBISM_SHADER_MASK_ADD_INV;
                break;
            case CubismBlendMode_Normal:
                e = GD_CUBISM_SHADER_MASK_MIX_INV;
                break;
            case CubismBlendMode_Multiplicative:
                e = GD_CUBISM_SHADER_MASK_MUL_INV;
                break;
            default:
                e = GD_CUBISM_SHADER_MASK_MIX_INV;
                break;
            }
        }
    }

    Ref<Shader> shader = res._owner_viewport->get_shader(e);
    if (shader.is_null())
        shader = res.get_shader(e);

    mat->set_shader(shader);

    Live2D::Cubism::Core::csmVector2 tmpSizeInPixels;
    Live2D::Cubism::Core::csmVector2 tmpOriginInPixels;
    Csm::csmFloat32 tmpPixelsPerUnit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &tmpSizeInPixels, &tmpOriginInPixels, &tmpPixelsPerUnit);

    mat->set_shader_parameter("vct_origin", Vector2(tmpOriginInPixels.X, tmpOriginInPixels.Y));

    CubismTextureColor color_base = this->GetModelColorWithOpacity(model->GetDrawableOpacity(index));

    mat->set_shader_parameter("color_base", Vector4(color_base.R, color_base.G, color_base.B, color_base.A));

    mat->set_shader_parameter("color_screen", make_vector4(model->GetDrawableScreenColor(index)));
    mat->set_shader_parameter("color_multiply", make_vector4(model->GetDrawableMultiplyColor(index)));
    mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
    mat->set_shader_parameter("tex_main", res.ary_texture[model->GetDrawableTextureIndex(index)]);

    return mat;
}

Ref<ArrayMesh> InternalCubismRenderer2D::make_ArrayMesh(
    const Csm::CubismModel *model, const Vector2 vct_canvas_size,
    const Csm::csmInt32 index,
    const bool auto_scale,
    const float adjust_scale,
    const Vector2 &adjust_pos,
    const bool maskmode) const
{

    const Vector2 vct_size = this->get_size(model);
    const Vector2 vct_origin = this->get_origin(model);
    const float ppunit = this->get_ppunit(model);
    float calc_ppunit = ppunit;

    if (auto_scale == true)
    {
        const float fdst = godot::MIN<float, float>(vct_canvas_size.x, vct_canvas_size.y);
        const float fsrc = godot::MAX<float, float>(vct_size.x, vct_size.y);
        calc_ppunit = (fdst * ppunit) / fsrc;
    }

    Array ary;
    ary.resize(Mesh::ARRAY_MAX);

    ary[Mesh::ARRAY_VERTEX] = make_PackedArrayVector3(
        model->GetDrawableVertexPositions(index),
        model->GetDrawableVertexCount(index),
        calc_ppunit * adjust_scale,
        Vector2(
            vct_canvas_size.x * vct_origin.x / vct_size.x,
            vct_canvas_size.y * vct_origin.y / vct_size.y) +
            adjust_pos);

    ary[Mesh::ARRAY_TEX_UV] = make_PackedArrayVector2(
        model->GetDrawableVertexUvs(index),
        model->GetDrawableVertexCount(index));

    ary[Mesh::ARRAY_INDEX] = make_PackedArrayInt32(
        model->GetDrawableVertexIndices(index),
        model->GetDrawableVertexIndexCount(index));

    Ref<ArrayMesh> ary_mesh;
    ary_mesh.instantiate();
    ary_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, ary);

    return ary_mesh;
}

Csm::csmInt32 InternalCubismRenderer2D::calc_viewport_count() const
{
    const CubismModel *model = this->GetModel();
    Csm::csmInt32 count = 0;

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        if (model->GetDrawableMaskCounts()[index] > 0)
            count++;
    }

    return count;
}

Csm::csmInt32 InternalCubismRenderer2D::calc_mesh_instance_count() const
{
    const CubismModel *model = this->GetModel();
    Csm::csmInt32 count = 0;

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        count += model->GetDrawableMaskCounts()[index];
        count++;
    }

    return count;
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

void InternalCubismRenderer2D::update_mask(SubViewport *viewport, const Csm::csmInt32 index, InternalCubismRendererResource &res)
{
    CubismModel *model = this->GetModel();

    Ref<ShaderMaterial> mat;
    mat.instantiate();

    Ref<Shader> shader = res._owner_viewport->get_shader(GD_CUBISM_SHADER_MASK);
    if (shader.is_null())
        shader = res.get_shader(GD_CUBISM_SHADER_MASK);

    mat->set_shader(shader);
    mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
    mat->set_shader_parameter("tex_main", res.ary_texture[model->GetDrawableTextureIndex(index)]);

    for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
    {
        Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];
        MeshInstance2D *node = res.request_mesh_instance();

        node->set_mesh(
            this->make_ArrayMesh(model,
                                 viewport->get_size(),
                                 j,
                                 res._owner_viewport->auto_scale,
                                 res.adjust_scale,
                                 res.adjust_pos,
                                 true));
        node->set_material(mat);
        node->set_z_index(model->GetDrawableRenderOrders()[index]);
        node->set_visible(true);

        viewport->add_child(node);
    }
}

void InternalCubismRenderer2D::update(InternalCubismRendererResource &res)
{
    const CubismModel *model = this->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();
    Vector2i vct_canvas_size;

    if (res._owner_viewport->mask_viewport_size.x > 0 && res._owner_viewport->mask_viewport_size.y > 0)
    {
        vct_canvas_size = res._owner_viewport->mask_viewport_size;
    }
    else
    {
        vct_canvas_size = res._owner_viewport->get_size();
    }

    // 描画
    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {

        // Drawableが表示状態でなければ処理をパスする
        if (model->GetDrawableDynamicFlagIsVisible(index) == false)
            continue;
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;

        MeshInstance2D *node = res.request_mesh_instance();
        Ref<ShaderMaterial> mat = this->make_ShaderMaterial(model, index, res);

        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());

        if (model->GetDrawableMaskCounts()[index] > 0)
        {

            SubViewport *viewport = res.request_viewport();

            viewport->set_size(vct_canvas_size);

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

            this->update_mask(viewport, index, res);

            // res._parent_node->add_child(viewport);
            res._parent_node->call_deferred("add_child", viewport);

            mat->set_shader_parameter("tex_mask", viewport->get_texture());
        }

        Ref<ArrayMesh> m = this->make_ArrayMesh(
            model, res._owner_viewport->get_size(), index,
            res._owner_viewport->auto_scale,
            res.adjust_scale,
            res.adjust_pos,
            false);

        node->set_name(node_name);
        node->set_mesh(m);
        res.dict_mesh[node_name] = m;
        node->set_material(mat);
        node->set_z_index(renderOrder[index]);
        node->set_visible(true);

        // res._parent_node->add_child(node);
        res._parent_node->call_deferred("add_child", node);
    }
}

void InternalCubismRenderer2D::update(InternalCubismRendererResource &res, const bool update_node, const bool update_mesh)
{
    const CubismModel *model = this->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {

        if (model->GetDrawableDynamicFlagIsVisible(index) == false)
            continue;
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;

        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());

        res.dict_mesh[node_name] = this->make_ArrayMesh(
            model, res._owner_viewport->get_size(), index,
            res._owner_viewport->auto_scale,
            res.adjust_scale, res.adjust_pos, false);
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
PackedInt32Array make_PackedArrayInt32(const csmUint16 *ptr, const int32_t size)
{
    PackedInt32Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, ptr[i]);
    }
    return ary;
}

PackedVector2Array make_PackedArrayVector2(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t size)
{
    PackedVector2Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, Vector2(ptr[i].X, ptr[i].Y));
    }
    return ary;
}

PackedVector2Array make_PackedArrayVector3(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t size, const Csm::csmFloat32 ppunit, Vector2 vct_adjust)
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
