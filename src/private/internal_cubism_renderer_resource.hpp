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
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
class GDCubismUserModel;


// ------------------------------------------------------------------- class(s)
class InternalCubismRendererResource {
public:
    InternalCubismRendererResource(GDCubismUserModel *owner_viewport, Node *parent_node);
    ~InternalCubismRendererResource();

    void pro_proc(const Csm::csmInt32 viewport_count, const Csm::csmInt32 mesh_instance_count);
    void epi_proc();

    void dispose_node();
    void clear();

    // Shader
    Ref<Shader> get_shader(const GDCubismShader e) const { return this->ary_shader[e]; }

    SubViewport* request_viewport() {
        return memnew(SubViewport);
    };
    MeshInstance2D* request_mesh_instance() {
        return memnew(MeshInstance2D);
    }

public:
    GDCubismUserModel *_owner_viewport = nullptr;
    Node *_parent_node = nullptr;

    Array ary_texture;
    Array ary_shader;
};


// ------------------------------------------------------------------ method(s)


#endif // INTERNAL_CUBISM_RENDERER_RESOURCE
