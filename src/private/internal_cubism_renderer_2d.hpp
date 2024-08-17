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

#include <private/internal_cubism_renderer_resource.hpp>
#include <gd_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
class InternalCubismRenderer2D : public CubismRenderer {
    friend class CubismRenderer;

public:
    InternalCubismRenderer2D();
    virtual ~InternalCubismRenderer2D();

private:
    Ref<ShaderMaterial> make_ShaderMaterial(const Csm::CubismModel* model, const Csm::csmInt32 index, const InternalCubismRendererResource &res) const;
    Ref<ArrayMesh> make_ArrayMesh(const Csm::CubismModel* model, const Vector2 vct_canvas_size, const Csm::csmInt32 index, const InternalCubismRendererResource &res) const;

    void update_mask(SubViewport* viewport, const Csm::csmInt32 index, InternalCubismRendererResource &res);

public:
    Csm::csmInt32 calc_viewport_count() const;
    Csm::csmInt32 calc_mesh_instance_count() const;
    Vector2 get_size(const Csm::CubismModel* model) const;
    Vector2 get_origin(const Csm::CubismModel* model) const;
    float get_ppunit(const Csm::CubismModel* model) const;

    void update(InternalCubismRendererResource &res);
    void update(InternalCubismRendererResource &res, const bool update_node, const bool update_mesh);

    virtual void Initialize(Csm::CubismModel* model, Csm::csmInt32 maskBufferCount);
    void DoDrawModel();
    void SaveProfile();
    void RestoreProfile();
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_RENDERER_2D
