#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/placeholder_texture2d.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/canvas_group.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

#include <loaders/gd_cubism_model_loader.hpp>
#include <private/internal_cubism_renderer_2d.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;


MeshInstance2D* request_mesh_instance() {
    Ref<ArrayMesh> mesh;
	mesh.instantiate();
    mesh->set_local_to_scene(true);
    MeshInstance2D* node = memnew(MeshInstance2D);
    node->set_mesh(mesh);
    return node;
}

Ref<ShaderMaterial> request_shader_material(const Csm::CubismModel *model, const Csm::csmInt32 index, Array shaders) {
    GDCubismShader e = GD_CUBISM_SHADER_NORM_MIX;
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

    return shaders[e];
}

void build_model(CubismModel* model, GDCubismUserModel* target_node, Array textures, Array shaders) {
	const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    const Vector2 vct_size = target_node->get_size();
    const float ppunit = target_node->get_pp_unit();
    
    Ref<PlaceholderTexture2D> placeholder;
    placeholder.instantiate();

    Array base_materials;
    base_materials.resize(shaders.size());
    for (auto i = 0; i < shaders.size(); i++) {
        Ref<Shader> shader = Object::cast_to<Shader>(shaders[i]);
        shader->set_local_to_scene(true);

        Ref<ShaderMaterial> mat;
        mat.instantiate();
            
        mat->set_shader(shader);
        mat->set_shader_parameter("tex_mask", placeholder);
        mat->set_local_to_scene(true);
        base_materials[i] = mat;
    }

    Node2D *meshes = memnew(Node2D);
    meshes->set_name(MESHES_NODE);
    target_node->add_child(meshes);
    meshes->set_owner(target_node);

    Node *masks = memnew(Node);
    masks->set_name("Masks");
    target_node->add_child(masks);
    masks->set_owner(target_node);

    Array mesh_instances;
    mesh_instances.resize(model->GetDrawableCount());

    Array ordered_meshes;
    ordered_meshes.resize(model->GetDrawableCount());

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;

        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());

        MeshInstance2D* node = request_mesh_instance();
        // share drawable mesh between nodes and masks so we only have to update once
        if (mesh_instances[index]) {
            node->set_mesh(mesh_instances[index]);
        } else {
            mesh_instances[index] = node->get_mesh();
        }
        Ref<ArrayMesh> ary_mesh = node->get_mesh();
        Ref<ShaderMaterial> mat = request_shader_material(model, index, base_materials);
        node->set_material(mat);
        
		InternalCubismRenderer2D::update_mesh(model, index, ary_mesh, ppunit);
        InternalCubismRenderer2D::update_material(model, index, node);
        node->set_name(node_name);
        node->set_meta("index", index);
        node->set_texture(textures[model->GetDrawableTextureIndex(index)]);
		
        const bool visible = model->GetDrawableDynamicFlagIsVisible(index) && model->GetDrawableOpacity(index) > 0.0f;
        node->set_visible(visible);

        meshes->add_child(node);
        node->set_owner(target_node);

        ordered_meshes[renderOrder[index]] = node;

        // build mask
        if (model->GetDrawableMaskCounts()[index] == 0) continue;

        node->set_instance_shader_parameter("has_mask", true);
        node->set_instance_shader_parameter("invert", model->GetDrawableInvertedMask(index));
        
        mat = mat->duplicate();
        node->set_material(mat);
        
        SubViewport* viewport = memnew(SubViewport);
        viewport->set_disable_3d(true);
        viewport->set_clear_mode(SubViewport::ClearMode::CLEAR_MODE_ALWAYS);
        // set_update_mode must be specified
        viewport->set_update_mode(SubViewport::UpdateMode::UPDATE_WHEN_VISIBLE);
        viewport->set_disable_input(true);
        // Memory leak when set_use_own_world_3d is true
        // https://github.com/godotengine/godot/issues/81476
        viewport->set_use_own_world_3d(false);
        // Memory leak when set_transparent_background is true(* every time & window minimize)
        // https://github.com/godotengine/godot/issues/89651
        viewport->set_transparent_background(true);

        // canvas transform only available after the viewport canvas has been initialized
        // on load the mask will not be the right size or offset, but will be corrected immediately on first update
        AABB bounds = node->get_mesh()->get_aabb();
        Vector2i viewport_size = Vector2i(bounds.size.x, bounds.size.y);
        Vector2 viewport_offset = Vector2(bounds.position.x, bounds.position.y);
        viewport->set_size(Vector2i(2,2));
        mat->set_shader_parameter("canvas_size", vct_size);
        mat->set_shader_parameter("mesh_offset", viewport_offset);

        masks->add_child(viewport);
        viewport->set_owner(target_node);
        
        String hash_name;

        for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
        {
            Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];
            
            if (model->GetDrawableVertexCount(j) == 0)
                continue;
            if (model->GetDrawableVertexIndexCount(j) == 0)
                continue;
    
            CubismIdHandle handle = model->GetDrawableId(j);
            String mask_name(handle->GetString().GetRawString());

            hash_name += mask_name;
        
            MeshInstance2D *node = request_mesh_instance();
            if (mesh_instances[j]) {
                node->set_mesh(mesh_instances[j]);
            } else {
                mesh_instances[j] = node->get_mesh();
            }
            Ref<ShaderMaterial> mat = base_materials[GD_CUBISM_SHADER_MASK];
            
            node->set_name(mask_name);
            node->set_material(mat);
            
            node->set_z_index(renderOrder[j]);
            node->set_meta("index", j);
            node->set_meta("mask_index", m_index);
            node->set_texture(textures[model->GetDrawableTextureIndex(j)]);
            node->set_visible(true);

            viewport->add_child(node);
            node->set_owner(target_node);
        }

        // check if mask exists
        String vp_hash = String::num_int64(hash_name.hash());
        node->set_meta("viewport", NodePath("../../Masks/" + vp_hash));
        NodePath mesh_path = NodePath("../../Meshes/" + node_name);
        Array vp_meshes;

        if (masks->has_node(vp_hash)) {
            memdelete(viewport);
            viewport = Object::cast_to<SubViewport>(masks->get_node_or_null(NodePath(vp_hash)));
            vp_meshes = viewport->get_meta("meshes");
        } else {
            viewport->set_name(vp_hash);
        }
        vp_meshes.append(mesh_path);
        viewport->set_meta("meshes", vp_meshes);
    }

    // reorder meshes if dirty to properly maintain z-index
    for (int i = 0, n = 0; i < ordered_meshes.size(); i++) {
        Node *node = Object::cast_to<Node>(ordered_meshes[i]);
        if (node == nullptr) continue;

        // use secondary index because some drawables do not exist, making our z-index not match with scene index
        node->get_parent()->move_child(node, n);
        n++;
    }
}

GDCubismUserModel* GDCubismModelLoader::load_model(const String &assets, Array shaders, const bool generate_mipmaps) {
    Ref<FileAccess> f = FileAccess::open(assets, FileAccess::READ);
    ERR_FAIL_COND_V_MSG(f.is_null(), nullptr, "Could not open model path.  Make sure to point to the model3.json");

    Dictionary model_settings = JSON::parse_string(FileAccess::get_file_as_string(assets));
	ERR_FAIL_COND_V_MSG(model_settings.is_empty(), nullptr, "Could not open model path.  Make sure to point to the model3.json");

	// setup Live2D model
	Dictionary file_refs = model_settings.get("FileReferences", Dictionary());
	String model_file = file_refs.get("Moc", "");
	ERR_FAIL_COND_V_MSG(model_file.is_empty(), nullptr, "Could not open model path.  Moc3 path undefined");

	String model_path = assets.get_base_dir();

	GDCubismUserModel *model = memnew(GDCubismUserModel);
    model->set_meta("filerefs", file_refs);
    model->set_name(assets.get_file());
	model->set_scene_file_path(assets);

    model->load_model();

	if (model->get_internal_model() == nullptr) {
		memdelete(model);

		return nullptr;
	}

    CubismModel *internal_model = model->get_internal_model();

    // Read Canvas Info
    {
        Live2D::Cubism::Core::csmVector2 vct_size;
        Live2D::Cubism::Core::csmVector2 vct_origin;
        Csm::csmFloat32 ppunit;

        Live2D::Cubism::Core::csmReadCanvasInfo(internal_model->GetModel(), &vct_size, &vct_origin, &ppunit);

        model->size = Vector2(vct_size.X, vct_size.Y);
        model->origin = Vector2(vct_origin.X, vct_origin.Y);
        model->pp_unit = ppunit;
    }

    //UserData
    {
        String path = file_refs.get("UserData", "");
        if (!path.is_empty()) {
            String buffer = FileAccess::get_file_as_string(model_path.path_join(path));
            if(!buffer.is_empty()) {
                model->user_data = JSON::parse_string(buffer);
            }
        }
    }

	// Load Parameters
    {
        Dictionary parameters;
        Live2D::Cubism::Core::csmModel *csm_model  = internal_model->GetModel();
        for(Csm::csmInt32 index = 0; index < internal_model->GetParameterCount(); index++) {
            Dictionary param;
            StringName p_name = StringName(Live2D::Cubism::Core::csmGetParameterIds(csm_model)[index]);
            param["id"] = index;
            param["name"] = p_name;
            param["min"] = internal_model->GetParameterMinimumValue(index);
            param["max"] = internal_model->GetParameterMaximumValue(index);
            param["default"] = internal_model->GetParameterDefaultValue(index);

            parameters[p_name] = param;
        }

        model->set_parameters(parameters);

        Array values = parameters.values();
        for (int i = 0; i < values.size(); i++) {
            Dictionary param = values[i];
            String p_name = param["name"];
            model->set(p_name, (float)param["default"]);
        }
    }

	// Load Parts
    {
        Dictionary parts;
        Live2D::Cubism::Core::csmModel *csm_model  = internal_model->GetModel();
        for(Csm::csmInt32 index = 0; index < internal_model->GetPartCount(); index++) {
            String p_name = String(Live2D::Cubism::Core::csmGetPartIds(csm_model)[index]);
            Dictionary part;
            part["id"] = index;
            part["name"] = p_name;
            part["default"] = internal_model->GetPartOpacity(index);
            parts[p_name] = part;
        }

        model->set_parts(parts);

        Array values = parts.values();
        for (int i = 0; i < values.size(); i++) {
            Dictionary part = values[i];
            String p_name = part["name"];
            
            model->set(p_name, (float)part["default"]);
        }
    }

	// Resource(Texture)
	Array textures;
	{
		ResourceLoader *res_loader = ResourceLoader::get_singleton();

		Array texture_files = file_refs.get("Textures", Array());
		for (csmInt32 index = 0; index < texture_files.size(); index++)
		{
			String tex_filename = texture_files[index];
			if (tex_filename.is_empty()) continue;
			
			String texture_pathname = model_path.path_join(tex_filename);

			Ref<Texture2D> tex;
			// allow dynamically loading image textures for models provided from disk or user data
			if (!res_loader->exists(texture_pathname)) {
                ERR_FAIL_COND_V_MSG(!FileAccess::file_exists(texture_pathname), nullptr, "Model texture does not exists.  Make sure the paths in the model3.json match what's available in the file system");
                
				Ref<Image> img = Image::load_from_file(texture_pathname);
                if (generate_mipmaps) {
                    img->generate_mipmaps();
                }
				tex = ImageTexture::create_from_image(img);
				tex->take_over_path(texture_pathname);
			} else {
				tex = res_loader->load(texture_pathname);
			}

			textures.append(tex);
		}
	}

	// Prepare Meshes
	{
		build_model(internal_model, model, textures, shaders);
	}

    return model;
}

Variant GDCubismModelLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	ResourceLoader* res_loader = ResourceLoader::get_singleton();
	Array shaders;
    shaders.resize(GD_CUBISM_SHADER_MAX);

    shaders[GD_CUBISM_SHADER_NORM_ADD] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_add.gdshader");
    shaders[GD_CUBISM_SHADER_NORM_MIX] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
    shaders[GD_CUBISM_SHADER_NORM_MUL] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mul.gdshader");
    shaders[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    GDCubismUserModel *m = this->load_model(
        p_path,
        shaders
    );
	
	ERR_FAIL_COND_V(m == nullptr, Error::FAILED);

	Ref<PackedScene> p;
	p.instantiate();
	if (p->pack(m) != OK) {
		p.unref();
		memdelete(m);
		return Error::FAILED;
	}
	memdelete(m);
	return p;
}
