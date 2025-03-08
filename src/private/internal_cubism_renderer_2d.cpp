// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#ifdef GD_CUBISM_USE_RENDERER_2D

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/rendering_server.hpp>

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
PackedInt32Array make_Indices(const csmUint16 *ptr, const int32_t &size);
PackedVector2Array make_UVs(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size);
PackedVector2Array make_Vertices(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size, const Csm::csmFloat32 &ppunit);
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

    res.vct_canvas_size = vct_size;
    res.CALCULATED_ORIGIN_C = vct_origin;
    res.CALCULATED_PPUNIT_C = ppunit;
}

void InternalCubismRenderer2D::update_mesh(
    const Csm::CubismModel *model,
    const Csm::csmInt32 index,
    const bool maskmode,
    const InternalCubismRendererResource &res,
    const MeshInstance2D *node
) const
{
    Ref<ArrayMesh> ary_mesh = node->get_mesh();
    ary_mesh->clear_surfaces();

    Array ary;

    ary.resize(Mesh::ARRAY_MAX);

    ary[Mesh::ARRAY_VERTEX] = make_Vertices(
        model->GetDrawableVertexPositions(index),
        model->GetDrawableVertexCount(index),
        res.CALCULATED_PPUNIT_C);

    ary[Mesh::ARRAY_TEX_UV] = make_UVs(
        model->GetDrawableVertexUvs(index),
        model->GetDrawableVertexCount(index));

    ary[Mesh::ARRAY_INDEX] = make_Indices(
        model->GetDrawableVertexIndices(index),
        model->GetDrawableVertexIndexCount(index));

    ary_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, ary);

    AABB bounds = ary_mesh->get_aabb();
    RenderingServer::get_singleton()->canvas_item_set_custom_rect(
        node->get_canvas_item(), true,
        Rect2(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y)
    );
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

void InternalCubismRenderer2D::update(InternalCubismRendererResource &res, int32_t mask_viewport_size)
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
        const bool visible = model->GetDrawableDynamicFlagIsVisible(index) && model->GetDrawableOpacity(index) > 0.0f;
        node->set_visible(visible);
        Ref<ShaderMaterial> mat = node->get_material();
        
        if (visible) {
            this->update_mesh(model, index, false, res, node);
            this->update_material(model, index, mat);
            node->set_z_index(renderOrder[index]);
        }
        
        // adjust real bounds to prevent the mesh being culled
        AABB bounds = node->get_mesh()->get_aabb();
        Rect2 canvas_bounds = Rect2(bounds.position.x, bounds.position.y, bounds.size.x, bounds.size.y);
        RenderingServer::get_singleton()->canvas_item_set_custom_rect(
            node->get_canvas_item(), true,
            canvas_bounds
        );

        if (!node->has_meta("viewport")) continue;

        SubViewport *viewport = Object::cast_to<SubViewport>(node->get_meta("viewport"));
        
        // detect if the canvas item is going to be culled
        // only cull viewports when not looking at the model in the editor
        Rect2 bounds_in_viewport = node->get_global_transform_with_canvas().xform(canvas_bounds);
        const bool is_culled = 
            !Engine::get_singleton()->is_editor_hint() &&
            !(
                node->get_viewport_rect().intersects(bounds_in_viewport) 
                || node->get_viewport_rect().encloses(bounds_in_viewport)
            );

        if (!visible || is_culled){
            viewport->set_size(Vector2i(2,2));
            continue;
        }

        // optimize mask viewport size by scaling it relative to the viewport transform
        // maximum resolution should be equal to raw size from mesh dimensions, or the upper bound defined on the model
        Vector2 mask_size = Math::min(bounds_in_viewport.size, canvas_bounds.size);
        Vector2 vp_scale = Vector2(mask_size) / Vector2(canvas_bounds.size);
        double scalar = 1.0;
        if (mask_viewport_size > 0) {
            if (mask_size.x > mask_viewport_size || mask_size.y > mask_viewport_size) {
                scalar = mask_viewport_size / Math::max(mask_size.x, mask_size.y);
                Vector2 ratio = Vector2(
                    Math::min(1.0f, mask_size.x / mask_size.y),
                    Math::min(1.0f, mask_size.y / mask_size.x)
                );
                mask_size = Vector2(mask_viewport_size, mask_viewport_size) * ratio;
            }
        }

        Vector2 viewport_offset = Vector2(bounds.position.x, bounds.position.y);
        Transform2D transform = Transform2D(0, -viewport_offset);
        transform.scale(Size2(scalar, scalar) * vp_scale);
        viewport->set_size(mask_size);
        viewport->set_canvas_transform(transform);

        mat->set_shader_parameter("tex_mask", viewport->get_texture());
        mat->set_shader_parameter("mask_scale", scalar * vp_scale.x);
        mat->set_shader_parameter("mesh_offset", viewport_offset);

        const Array masks = res.dict_mask[node_name];
        
        for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
        {
            Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];

            if (model->GetDrawableVertexCount(j) == 0)
                continue;
            if (model->GetDrawableVertexIndexCount(j) == 0)
                continue;
    
            MeshInstance2D *node = Object::cast_to<MeshInstance2D>(masks[m_index]);
            node->set_visible(visible);

            if (!visible) {
                continue;
            }

            this->update_mesh(model, j, true, res, node);
            node->set_z_index(renderOrder[index]);
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
        this->update_mesh(model, index, false, res, node);
        node->set_name(node_name);

        // build mask
        if (model->GetDrawableMaskCounts()[index] > 0)
        {
            TypedArray<MeshInstance2D> masks;
            masks.resize(model->GetDrawableMaskCounts()[index]);
            
            SubViewport* viewport = memnew(SubViewport);
            {
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
                    this->update_mesh(model, j, true, res, node);

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

            viewport->set_name(node_name + "__mask");
            res.dict_mask[node_name] = masks;

            node->set_meta("viewport", viewport);

            // canvas transform only available after the viewport canvas has been initialized
            // on load the mask will not be the right size or offset, but will be corrected immediately on first update
            Vector2i viewport_size = Vector2i(1,1);
            Vector2 viewport_offset = Vector2(0,0);
            viewport->set_size(viewport_size);
            
            mat->set_shader_parameter("tex_mask", viewport->get_texture());
            mat->set_shader_parameter("canvas_size", Vector2(res.vct_canvas_size));
            mat->set_shader_parameter("mesh_offset", viewport_offset);
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
PackedInt32Array make_Indices(const csmUint16 *ptr, const int32_t &size)
{
    PackedInt32Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, ptr[i]);
    }
    return ary;
}

PackedVector2Array make_UVs(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size)
{
    PackedVector2Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, Vector2(ptr[i].X, 1.0 - ptr[i].Y));
    }
    return ary;
}

PackedVector2Array make_Vertices(const Live2D::Cubism::Core::csmVector2 *ptr, const int32_t &size, const Csm::csmFloat32 &ppunit)
{
    PackedVector2Array ary;
    ary.resize(size);
    for (int i = 0; i < size; i++)
    {
        ary.set(i, Vector2(ptr[i].X, ptr[i].Y * -1.0) * ppunit);
    }
    return ary;
}

const Vector4 make_vector4(const Live2D::Cubism::Core::csmVector4 &src_vec4)
{
    return Vector4(src_vec4.X, src_vec4.Y, src_vec4.Z, src_vec4.W);
}

#endif // GD_CUBISM_USE_RENDERER_2D
