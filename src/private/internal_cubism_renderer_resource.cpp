// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/resource_loader.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Rendering/CubismRenderer.hpp>

#include <private/internal_cubism_renderer_resource.hpp>
#include <gd_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)

// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismRendererResource::InternalCubismRendererResource(GDCubismUserModel *owner_viewport)
    : _owner_viewport(owner_viewport)
{
    ResourceLoader* res_loader = memnew(ResourceLoader);

    this->ary_shader.resize(GD_CUBISM_SHADER_MAX);

    this->ary_shader[GD_CUBISM_SHADER_NORM_ADD] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_add.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_NORM_MIX] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_NORM_MUL] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mul.gdshader");

    this->ary_shader[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    this->ary_shader[GD_CUBISM_SHADER_MASK_ADD] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_ADD_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add_inv.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MIX] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MIX_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MUL] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MUL_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul_inv.gdshader");

    memdelete(res_loader);
}


InternalCubismRendererResource::~InternalCubismRendererResource() {
    this->clear();
    this->ary_shader.clear();
    this->_owner_viewport = nullptr;
}


void InternalCubismRendererResource::clear() {
    for (int i = 0; i < this->managed_nodes.size(); i++) {
        Node *c = Object::cast_to<Node>(this->managed_nodes[i]);
        c->get_parent()->remove_child(c);
        c->queue_free();
    }

    this->managed_nodes.clear();

    this->ary_texture.clear();
    this->dict_mesh.clear();
    this->dict_mask.clear();
    this->dict_mask_meshes.clear();
}

MeshInstance2D* InternalCubismRendererResource::request_mesh_instance() {
    ArrayMesh* mesh = memnew(ArrayMesh);
    MeshInstance2D* node = memnew(MeshInstance2D);
    node->set_mesh(mesh);
    return node;
}

ShaderMaterial* InternalCubismRendererResource::request_shader_material(const Csm::CubismModel *model, const Csm::csmInt32 index) {
    ShaderMaterial* mat = memnew(ShaderMaterial);
    
    GDCubismShader e = GD_CUBISM_SHADER_NORM_MIX;
    if (model->GetDrawableMaskCounts()[index] == 0)
    {
        switch (model->GetDrawableBlendMode(index))
        {
        case CubismRenderer::CubismBlendMode_Additive:
            e = GD_CUBISM_SHADER_NORM_ADD;
            break;
        case CubismRenderer::CubismBlendMode_Normal:
            e = GD_CUBISM_SHADER_NORM_MIX;
            break;
        case CubismRenderer::CubismBlendMode_Multiplicative:
            e = GD_CUBISM_SHADER_NORM_MUL;
            break;
        default:
            e = GD_CUBISM_SHADER_NORM_MIX;
            break;
        }
    }
    else if (model->GetDrawableInvertedMask(index) == false)
    {
        switch (model->GetDrawableBlendMode(index))
        {
        case CubismRenderer::CubismBlendMode_Additive:
            e = GD_CUBISM_SHADER_MASK_ADD;
            break;
        case CubismRenderer::CubismBlendMode_Normal:
            e = GD_CUBISM_SHADER_MASK_MIX;
            break;
        case CubismRenderer::CubismBlendMode_Multiplicative:
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
        case CubismRenderer::CubismBlendMode_Additive:
            e = GD_CUBISM_SHADER_MASK_ADD_INV;
            break;
        case CubismRenderer::CubismBlendMode_Normal:
            e = GD_CUBISM_SHADER_MASK_MIX_INV;
            break;
        case CubismRenderer::CubismBlendMode_Multiplicative:
            e = GD_CUBISM_SHADER_MASK_MUL_INV;
            break;
        default:
            e = GD_CUBISM_SHADER_MASK_MIX_INV;
            break;
        }
    }

    Ref<Shader> shader = this->_owner_viewport->get_shader(e);
    if (shader.is_null())
        shader = this->get_shader(e);

    mat->set_shader(shader);
    mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
    mat->set_shader_parameter("tex_main", this->ary_texture[model->GetDrawableTextureIndex(index)]);

    return mat;
}

ShaderMaterial* InternalCubismRendererResource::request_mask_material() {
    ShaderMaterial* mat = memnew(ShaderMaterial);

    Ref<Shader> shader = this->_owner_viewport->get_shader(GD_CUBISM_SHADER_MASK);
    if (shader.is_null())
        shader = this->get_shader(GD_CUBISM_SHADER_MASK);

    mat->set_shader(shader);
    
    return mat;
}
