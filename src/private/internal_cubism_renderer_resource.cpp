// ----------------------------------------------------------------- include(s)
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
void _recurisive_dispose_node(const Node* parent_node, const bool node_release);


// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismRendererResource::InternalCubismRendererResource(GDCubismUserModel *owner_viewport, Node *parent_node)
    : _owner_viewport(owner_viewport)
    , _parent_node(parent_node)
    , sub_viewport_counter(0)
    , mesh_instance_counter(0)
{
    ResourceLoader* res_loader = memnew(ResourceLoader);

    this->ary_shader.resize(GD_CUBISM_SHADER_MAX);

    this->ary_shader[GD_CUBISM_SHADER_NORM_ADD] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_NORM_MIX] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_NORM_MUL] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");

    this->ary_shader[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    this->ary_shader[GD_CUBISM_SHADER_MASK_ADD] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_ADD_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MIX] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MIX_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MUL] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
    this->ary_shader[GD_CUBISM_SHADER_MASK_MUL_INV] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");

    memdelete(res_loader);
}


InternalCubismRendererResource::~InternalCubismRendererResource() {
    this->clear();
    this->ary_shader.clear();
    this->_owner_viewport = nullptr;
    this->_parent_node = nullptr;
}


SubViewport* InternalCubismRendererResource::request_viewport() {
    const Csm::csmInt32 counter = this->sub_viewport_counter++;

    if(counter < this->ary_sub_viewport.size()) {
        return Object::cast_to<SubViewport>(this->ary_sub_viewport[counter]);
    } else {
        SubViewport* node = memnew(SubViewport);
        this->ary_sub_viewport.append(node);
        return node;
    }
};


MeshInstance2D* InternalCubismRendererResource::request_mesh_instance() {
    const Csm::csmInt32 counter = this->mesh_instance_counter++;

    if(counter < this->ary_mesh_instance.size()) {
        return Object::cast_to<MeshInstance2D>(this->ary_mesh_instance[counter]);
    } else {
        MeshInstance2D* node = memnew(MeshInstance2D);
        this->ary_mesh_instance.append(node);
        return node;
    }
}


void InternalCubismRendererResource::pro_proc(const Csm::csmInt32 viewport_count, const Csm::csmInt32 mesh_instance_count) {
    this->dispose_node(false);
    this->sub_viewport_counter = 0;
    this->mesh_instance_counter = 0;
}


void InternalCubismRendererResource::epi_proc() {}


void InternalCubismRendererResource::dispose_node(const bool node_release) {
    _recurisive_dispose_node(this->_parent_node, node_release);
}


void InternalCubismRendererResource::clear() {

    this->dispose_node(true);

    this->ary_texture.clear();
    this->ary_sub_viewport.clear();
    this->ary_mesh_instance.clear();
}


// ------------------------------------------------------------------ method(s)
void _recurisive_dispose_node(const Node* parent_node, const bool node_release) {

    TypedArray<Node> ary_node = parent_node->get_children();

    for(Csm::csmInt32 i = 0; i < ary_node.size(); i++) {

        if(Object::cast_to<GDCubismEffect>(ary_node[i]) != nullptr) continue;

        MeshInstance2D *m_node = Object::cast_to<MeshInstance2D>(ary_node[i]);
        if(m_node != nullptr) m_node->set_material(nullptr);

        Node* node = Object::cast_to<Node>(ary_node[i]);
        if(node != nullptr) {
            _recurisive_dispose_node(node, node_release);
            if(node->get_parent() != nullptr) node->get_parent()->remove_child(node);
            if(node_release == true) node->queue_free();
        }
    }
}

