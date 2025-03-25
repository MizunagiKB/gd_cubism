// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef INTERNAL_CUBISM_RENDERER_RESOURCE
#define INTERNAL_CUBISM_RENDERER_RESOURCE


// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/texture2d.hpp>

#include <gd_cubism_effect.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class GDCubismUserModel;


// ------------------------------------------------------------------- class(s)
class InternalCubismRendererResource {
public:
    InternalCubismRendererResource(GDCubismUserModel *owner_viewport);
    ~InternalCubismRendererResource();

    void clear();

    SubViewport* request_viewport();
    MeshInstance2D* request_mesh_instance();
    ShaderMaterial* request_shader_material(const Csm::CubismModel *model, const Csm::csmInt32 index);
    ShaderMaterial* request_mask_material();

    // Shader
    Ref<Shader> get_shader(const GDCubismShader e) const { return this->ary_shader[e]; }

public:
    GDCubismUserModel *_owner_viewport;

    TypedArray<Node> managed_nodes;
    Array ary_texture;
    Array ary_shader;
    Dictionary dict_mesh;
    Dictionary dict_mask;
    Dictionary dict_mask_meshes;

    // Render parameters
    Vector2i vct_canvas_size;
    float CALCULATED_PPUNIT_C;
    Vector2 CALCULATED_ORIGIN_C;
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_RENDERER_RESOURCE
