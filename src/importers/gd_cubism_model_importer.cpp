#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/canvas_group.hpp>
#include <godot_cpp/classes/animation_player.hpp>

#include <importers/gd_cubism_model_importer.hpp>
#include <importers/gd_cubism_motion_importer.hpp>
#include <private/internal_cubism_renderer_2d.hpp>
#include <gd_cubism_value_parameter.hpp>
#include <gd_cubism_value_part_opacity.hpp>
#include <gd_cubism_expression_controller.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework;
using namespace godot;


MeshInstance2D* GDCubismModelImporter::request_mesh_instance() {
    ArrayMesh* mesh = memnew(ArrayMesh);
    mesh->set_local_to_scene(true);
    MeshInstance2D* node = memnew(MeshInstance2D);
    node->set_mesh(mesh);
    return node;
}

ShaderMaterial* GDCubismModelImporter::request_shader_material(const Csm::CubismModel *model, const Csm::csmInt32 index, Array shaders) {
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

    Ref<Shader> shader = Object::cast_to<Shader>(shaders[e]);
    // TODO change to using canvas item uniforms once Godot 4.4 is stable
    shader->set_local_to_scene(true);
    
    mat->set_shader(shader);
    mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
    mat->set_local_to_scene(true);

    return mat;
}

ShaderMaterial* GDCubismModelImporter::request_mask_material(Array shaders) {
    ShaderMaterial* mat = memnew(ShaderMaterial);

    Ref<Shader> shader = Object::cast_to<Shader>(shaders[GD_CUBISM_SHADER_MASK]);
    mat->set_shader(shader);
    
    return mat;
}

void GDCubismModelImporter::build_model(InternalCubismRenderer2D* renderer, GDCubismUserModel* target_node, Array textures, Array shaders)
{
	const CubismModel *model = renderer->GetModel();
    const Csm::csmInt32 *renderOrder = model->GetDrawableRenderOrders();
    const Csm::csmInt32 *maskCount = model->GetDrawableMaskCounts();

    const Vector2 vct_size = InternalCubismUserModel::get_size(model);
    const Vector2 vct_origin = InternalCubismUserModel::get_origin(model);

    Node2D *meshes = memnew(Node2D);
    meshes->set_name("Meshes");
    meshes->set_position(-vct_origin);
    target_node->add_child(meshes);
    meshes->set_owner(target_node);

    Node *masks = memnew(Node);
    masks->set_name("Masks");
    target_node->add_child(masks);
    masks->set_owner(target_node);

    for (Csm::csmInt32 index = 0; index < model->GetDrawableCount(); index++)
    {
        if (model->GetDrawableVertexCount(index) == 0)
            continue;
        if (model->GetDrawableVertexIndexCount(index) == 0)
            continue;

        CubismIdHandle handle = model->GetDrawableId(index);
        String node_name(handle->GetString().GetRawString());

        MeshInstance2D* node = GDCubismModelImporter::request_mesh_instance();
        ShaderMaterial* mat = GDCubismModelImporter::request_shader_material(model, index, shaders);
        node->set_material(mat);
		renderer->update_mesh(model, index, false, node);
        renderer->update_material(model, index, mat);
        node->set_name(node_name);
        node->set_texture(textures[model->GetDrawableTextureIndex(index)]);
        mat->set_shader_parameter("tex_main", textures[model->GetDrawableTextureIndex(index)]);
        node->set_z_index(renderOrder[index]);
        node->set_meta("index", index);
		
        const bool visible = model->GetDrawableDynamicFlagIsVisible(index) && model->GetDrawableOpacity(index) > 0.0f;
        node->set_visible(visible);

        meshes->add_child(node);
        node->set_owner(target_node);

        // build mask
        if (model->GetDrawableMaskCounts()[index] == 0) continue;
        
        SubViewport* viewport = memnew(SubViewport);
        viewport->set_disable_3d(SUBVIEWPORT_DISABLE_3D_FLAG);
        viewport->set_clear_mode(SubViewport::ClearMode::CLEAR_MODE_ALWAYS);
        // set_update_mode must be specified
        viewport->set_update_mode(SubViewport::UpdateMode::UPDATE_WHEN_VISIBLE);
        viewport->set_disable_input(true);
        // Memory leak when set_use_own_world_3d is true
        // https://github.com/godotengine/godot/issues/81476
        viewport->set_use_own_world_3d(SUBVIEWPORT_USE_OWN_WORLD_3D_FLAG);
        // Memory leak when set_transparent_background is true(* every time & window minimize)
        // https://github.com/godotengine/godot/issues/89651
        viewport->set_transparent_background(true);

        // canvas transform only available after the viewport canvas has been initialized
        // on load the mask will not be the right size or offset, but will be corrected immediately on first update
        AABB bounds = node->get_mesh()->get_aabb();
        Vector2i viewport_size = Vector2i(bounds.size.x, bounds.size.y);
        Vector2 viewport_offset = Vector2(bounds.position.x, bounds.position.y);
        viewport->set_size(viewport_size);
        mat->set_shader_parameter("canvas_size", vct_size);
        mat->set_shader_parameter("mesh_offset", viewport_offset);

        viewport->set_name(node_name);
        masks->add_child(viewport);
        viewport->set_owner(target_node);

        node->set_meta("viewport", NodePath("../../Masks/" + node_name));

        for (Csm::csmInt32 m_index = 0; m_index < model->GetDrawableMaskCounts()[index]; m_index++)
        {
            Csm::csmInt32 j = model->GetDrawableMasks()[index][m_index];
            
            if (model->GetDrawableVertexCount(j) == 0)
                continue;
            if (model->GetDrawableVertexIndexCount(j) == 0)
                continue;
    
            CubismIdHandle handle = model->GetDrawableId(j);
            String mask_name(handle->GetString().GetRawString());

            MeshInstance2D *node = GDCubismModelImporter::request_mesh_instance();
            ShaderMaterial *mat = GDCubismModelImporter::request_mask_material(shaders);
            renderer->update_mesh(model, j, true, node);

            node->set_name(mask_name);
            node->set_material(mat);
            mat->set_shader_parameter("channel", Vector4(0.0, 0.0, 0.0, 1.0));
            mat->set_shader_parameter("tex_main", textures[model->GetDrawableTextureIndex(j)]);
            node->set_z_index(renderOrder[j]);
            node->set_meta("index", j);
            node->set_meta("mask_index", m_index);
            node->set_visible(true);

            viewport->add_child(node);
            node->set_owner(target_node);
        }
    }
}

static Array walk_files(String dir, String extension) {
	Array files;

	// pick files
	{
		Array dir_files = DirAccess::get_files_at(dir);
		for (int i = 0; i < dir_files.size(); i++) {
			String f = dir_files[i];
			if (f.ends_with(extension)) {
				files.append(dir.path_join(f));
			}
		}
	}

	// walk subdirectories
	{
		Array sub_dirs = DirAccess::get_directories_at(dir);
		for (int i = 0; i < sub_dirs.size(); i++) {
			String f = sub_dirs[i];
			files.append_array(walk_files(dir.path_join(f), extension));
		}
	}

	return files;
}


GDCubismUserModel* GDCubismModelImporter::load_model(const String &assets, bool include_expressions, bool include_motions, Array shaders) {
	GDCubismUserModel *model = memnew(GDCubismUserModel);

    Ref<FileAccess> f = FileAccess::open(assets, FileAccess::READ);
    ERR_FAIL_COND_V_MSG(f.is_null(), nullptr, "Could not open model path.  Make sure to point to the model3.json");

    Dictionary model_settings = JSON::parse_string(FileAccess::get_file_as_string(assets));
	ERR_FAIL_COND_V_MSG(model_settings.is_empty(), nullptr, "Could not open model path.  Make sure to point to the model3.json");

	// setup Live2D model
	Dictionary file_refs = model_settings.get("FileReferences", Dictionary());
	String model_file = file_refs.get("Moc", "");
	ERR_FAIL_COND_V_MSG(model_file.is_empty(), nullptr, "Could not open model path.  Moc3 path undefined");

	String model_path = assets.get_base_dir().path_join(model_file);

	PackedByteArray buffer = FileAccess::get_file_as_bytes(model_path);
	
	InternalCubismUserModel *internal_model = CSM_NEW InternalCubismUserModel(model);
	internal_model->LoadModel(buffer.ptr(), buffer.size());

	if (internal_model->GetModel() == nullptr || internal_model->GetModelMatrix() == nullptr) {
		CSM_DELETE(internal_model);
		internal_model = nullptr;
	
		return nullptr;
	}

	// Load Parameters
    {
        model->ary_parameter.resize(internal_model->GetModel()->GetParameterCount());
        for(Csm::csmInt32 index = 0; index < internal_model->GetModel()->GetParameterCount(); index++) {
            Ref<GDCubismParameter> param;
            param.instantiate();
            param->setup(internal_model->GetModel(), index);
            param->set_local_to_scene(true);
            model->ary_parameter[index] = param;
        }
    }

	// Load Parts
    {
        model->ary_part_opacity.resize(internal_model->GetModel()->GetPartCount());
        for(Csm::csmInt32 index = 0; index < internal_model->GetModel()->GetPartCount(); index++) {
            Ref<GDCubismPartOpacity> param;
            param.instantiate();
            param->setup(internal_model->GetModel(), index);
            param->set_local_to_scene(true);
            model->ary_part_opacity[index] = param;
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
			
			String texture_pathname = assets.get_base_dir().path_join(tex_filename);

			Ref<Texture2D> tex;
			// allow dynamically loading image textures for models provided from disk or user data
			if (!res_loader->exists(texture_pathname)) {
				Ref<Image> img = Image::load_from_file(texture_pathname);
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
		UtilityFunctions::print("preparing model");
		internal_model->CreateRenderer();
		InternalCubismRenderer2D* renderer = internal_model->GetRenderer<InternalCubismRenderer2D>();
		
		renderer->IsPremultipliedAlpha(false);
		renderer->DrawModel();
		GDCubismModelImporter::build_model(renderer, model, textures, shaders);
	}

	// Load Animations
    {
        AnimationPlayer *anim_player = memnew(AnimationPlayer);
        AnimationLibrary *animations = memnew(AnimationLibrary);
        anim_player->add_animation_library("", animations);
        model->add_child(anim_player);
        anim_player->set_callback_mode_process(AnimationPlayer::ANIMATION_CALLBACK_MODE_PROCESS_MANUAL);
        anim_player->set_owner(model);
        anim_player->set_name("MotionController");
        anim_player->set_root_node(NodePath(".."));

        if (include_motions) {
            ResourceLoader *res_loader = ResourceLoader::get_singleton();
            Array motion_files = walk_files(assets.get_base_dir(), "motion3.json");
            for (int i = 0; i < motion_files.size(); i++) {
                String motion = motion_files[i];
                UtilityFunctions::print(motion);
                Ref<Animation> anim = res_loader->load(motion);
                if (anim.is_null()) {
                    anim = GDCubismMotionImporter::parse_motion(motion);
                }
                if (anim.is_valid()) {
                    animations->add_animation(motion.get_file(), anim);
                }
            }
        }
    }

	model->set_name(assets.get_file());
	model->set_scene_file_path(assets);

    CSM_DELETE(internal_model);

    return model;
}

static void get_expressions(GDCubismUserModel *model) {
    TypedArray<GDCubismExpression> ary_expression;

	GDCubismExpressionController *controller = memnew(GDCubismExpressionController);

	controller->set_expression_library(ary_expression);

	model->add_child(controller);
}

Error GDCubismModelImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V(!p_source_file.ends_with(".model3.json"), Error::FAILED);

	ResourceLoader* res_loader = ResourceLoader::get_singleton();
	Array shaders;
    shaders.resize(GD_CUBISM_SHADER_MAX);

    shaders[GD_CUBISM_SHADER_NORM_ADD] = p_options["shader_add"];
    shaders[GD_CUBISM_SHADER_NORM_MIX] = p_options["shader_mix"];
    shaders[GD_CUBISM_SHADER_NORM_MUL] = p_options["shader_mul"];

    shaders[GD_CUBISM_SHADER_MASK] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask.gdshader");

    shaders[GD_CUBISM_SHADER_MASK_ADD] = p_options["shader_mask_add"];
    shaders[GD_CUBISM_SHADER_MASK_ADD_INV] = p_options["shader_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MIX] = p_options["shader_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MIX_INV] = p_options["shader_inv_mask_mix"];
    shaders[GD_CUBISM_SHADER_MASK_MUL] = p_options["shader_mask_mul"];
    shaders[GD_CUBISM_SHADER_MASK_MUL_INV] = p_options["shader_inv_mask_mul"];

    GDCubismUserModel *m = GDCubismModelImporter::load_model(
        p_source_file,
        p_options["include_expressions"],
        p_options["include_motions"], 
        shaders
    );
	ERR_FAIL_COND_V(m == nullptr, Error::FAILED);

	PackedScene *p = memnew(PackedScene);
	ERR_FAIL_COND_V(p->pack(m) != OK, Error::FAILED);

    String filename = p_save_path + String(".") + this->_get_save_extension();
    return ResourceSaver::get_singleton()->save(p, filename);
}

TypedArray<Dictionary> GDCubismModelImporter::_get_import_options(const String &p_path, int32_t p_preset_index) const { 
    TypedArray<Dictionary> options;

    ResourceLoader *res_loader = ResourceLoader::get_singleton();

    Dictionary include_expressions;
    include_expressions["name"] = "include_expressions";
    include_expressions["default_value"] = true;
    options.append(include_expressions);

    Dictionary include_motions;
    include_motions["name"] = "include_motions";
    include_motions["default_value"] = true;
    options.append(include_motions);

    {
        Dictionary shader;
        shader["name"] = "shader_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mix.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_add.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_norm_mul.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_mask_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_mix";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mix_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_add";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_add_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    {
        Dictionary shader;
        shader["name"] = "shader_inv_mask_mul";
        shader["default_value"] = res_loader->load("res://addons/gd_cubism/res/shader/2d_cubism_mask_mul_inv.gdshader");
        shader["property_hint"] = PropertyHint::PROPERTY_HINT_RESOURCE_TYPE;
        options.append(shader);
    }
    
    return options; 
}