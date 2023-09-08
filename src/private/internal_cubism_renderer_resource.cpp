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
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
InternalCubismRendererResource::InternalCubismRendererResource(GDCubismUserModel *owner_viewport, Node *parent_node)
    : _owner_viewport(owner_viewport)
    , _parent_node(parent_node)
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


void InternalCubismRendererResource::pro_proc(const Csm::csmInt32 viewport_count, const Csm::csmInt32 mesh_instance_count) {
    this->dispose_node();
}


void InternalCubismRendererResource::epi_proc() {}


void _recurisive_dispose_node(const Node* parent_node) {

    TypedArray<Node> ary_node = parent_node->get_children();

    for(Csm::csmInt32 i = 0; i < ary_node.size(); i++) {

        if(Object::cast_to<GDCubismEffect>(ary_node[i]) != nullptr) continue;

        MeshInstance2D *m_node = Object::cast_to<MeshInstance2D>(ary_node[i]);
        if(m_node != nullptr) m_node->set_material(nullptr);

        Node* node = Object::cast_to<Node>(ary_node[i]);
        if(node != nullptr) {
            _recurisive_dispose_node(node);
            if(node->get_parent() != nullptr) node->get_parent()->remove_child(node);
            node->queue_free();
        }
    }
}

void InternalCubismRendererResource::dispose_node() {
    _recurisive_dispose_node(this->_parent_node);
}


void InternalCubismRendererResource::clear() {

    this->dispose_node();
    this->ary_texture.clear();
}


// ------------------------------------------------------------------ method(s)
