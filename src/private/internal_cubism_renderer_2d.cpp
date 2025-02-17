// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#ifdef GD_CUBISM_USE_RENDERER_2D

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Rendering/CubismRenderer.hpp>

#include <private/internal_cubism_renderer_2d.hpp>
#include <private/internal_cubism_user_model.hpp>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

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

void InternalCubismRenderer2D::update_mesh(
    const Csm::CubismModel *model,
    const Csm::csmInt32 index,
    const bool maskmode,
    const MeshInstance2D *node
) const
{
    const Vector2 vct_origin = InternalCubismUserModel::get_origin(model);
    const float pp_unit = InternalCubismUserModel::get_ppunit(model);
    
    Ref<ArrayMesh> ary_mesh = node->get_mesh();
    
    ary_mesh->clear_surfaces();

    Array ary;

    ary.resize(Mesh::ARRAY_MAX);

    ary[Mesh::ARRAY_VERTEX] = make_PackedArrayVector3(
        model->GetDrawableVertexPositions(index),
        model->GetDrawableVertexCount(index),
        pp_unit,
        vct_origin);

    ary[Mesh::ARRAY_TEX_UV] = make_PackedArrayVector2(
        model->GetDrawableVertexUvs(index),
        model->GetDrawableVertexCount(index));

    ary[Mesh::ARRAY_INDEX] = make_PackedArrayInt32(
        model->GetDrawableVertexIndices(index),
        model->GetDrawableVertexIndexCount(index));

    ary_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, ary);

    AABB bounds = ary_mesh->get_aabb();
    RenderingServer::get_singleton()->canvas_item_set_custom_rect(
        node->get_canvas_item(), true,
        Rect2(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y)
    );
}

void InternalCubismRenderer2D::update(Array meshes)
{
    const CubismModel *model = this->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    const float ppunit = InternalCubismUserModel::get_ppunit(model);

    for (int i = 0; i < meshes.size(); i++)
    {
        MeshInstance2D *node = Object::cast_to<MeshInstance2D>(meshes[i]);
        if (node == nullptr) continue;
        
        int32_t index = node->get_meta("index", -1);
        if (index < 0) continue;

        const bool visible = model->GetDrawableDynamicFlagIsVisible(index) && model->GetDrawableOpacity(index) > 0.0f;
        node->set_visible(visible);
        Ref<ShaderMaterial> mat = node->get_material();
        
        this->update_mesh(model, index, false, node);
        this->update_material(model, index, mat);
        node->set_z_index(renderOrder[index]);
        
        AABB bounds = node->get_mesh()->get_aabb();
        Vector2i mask_size = Vector2i(bounds.size.x, bounds.size.y);
        RenderingServer::get_singleton()->canvas_item_set_custom_rect(
            node->get_canvas_item(), true,
            Rect2(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y)
        );

        if (!node->has_meta("viewport")) continue;

        NodePath path_to_viewport = node->get_meta("viewport");
        SubViewport *viewport = Object::cast_to<SubViewport>(node->get_node_or_null(path_to_viewport));
        if (viewport == nullptr) continue;

        if (!visible) {
            viewport->set_size(Vector2i(1,1));
        } else {
            AABB bounds = node->get_mesh()->get_aabb();
            Vector2i viewport_size = Vector2i(bounds.size.x, bounds.size.y);
            Vector2 viewport_offset = Vector2(bounds.position.x, bounds.position.y);
            viewport->set_size(viewport_size);
            viewport->set_canvas_transform(
                Transform2D(0, -viewport_offset)
            );

            mat->set_shader_parameter("tex_mask", viewport->get_texture());
            mat->set_shader_parameter("mesh_offset", viewport_offset);
        }
        
        const Array masks = viewport->get_children();
        
        for (int j = 0; j < masks.size(); j++)
        {
            MeshInstance2D *node = Object::cast_to<MeshInstance2D>(masks[j]);
            int32_t m_idx = node->get_meta("index", -1);
            if (m_idx < 0) continue;
            
            // masks are always drawn
            node->set_visible(true);

            this->update_mesh(model, m_idx, true, node);
            node->set_z_index(renderOrder[m_idx]);
        }
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
