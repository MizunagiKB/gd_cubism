// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef INTERNAL_CUBISM_RENDERER_2D
#define INTERNAL_CUBISM_RENDERER_2D

// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/texture2d.hpp>

#include <Rendering/CubismRenderer.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class InternalCubismRenderer2D
{
public:
    static void update(const Csm::CubismModel *model, const Array meshes, Array masks, int32_t viewport_size = 0);
    
    static void update_material(const Csm::CubismModel *model, const Csm::csmInt32 index, const Ref<ShaderMaterial> mat);

    static void update_mesh(
        const Csm::CubismModel *model,
        const Csm::csmInt32 index,
        const Ref<ArrayMesh> mesh
    );
};

// ------------------------------------------------------------------ method(s)

#endif // INTERNAL_CUBISM_RENDERER_2D
