#ifndef GD_CUBISM_MODEL_LOADER
#define GD_CUBISM_MODEL_LOADER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <private/internal_cubism_user_model.hpp>
#include <private/internal_cubism_renderer_2d.hpp>
#include <gd_cubism_user_model.hpp>

#include <gd_cubism.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismModelLoader : public ResourceFormatLoader {
    GDCLASS(GDCubismModelLoader, ResourceFormatLoader);

protected:
    static void _bind_methods() {
		ClassDB::bind_static_method("GDCubismModelLoader", D_METHOD("load_model", "model_file", "include_expressions", "include_motions", "shaders"), &GDCubismModelLoader::load_model);
	}

public:
    bool _recognize_path(const String &p_path, const StringName &p_type) const override {
        return p_path.ends_with(MODEL_FILE_EXTENSION) && ClassDB::is_parent_class(p_type, "PackedScene");
    }

	static GDCubismUserModel* load_model(const String &p_path, const bool include_expressions, const bool include_motions, Array shaders);
    Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

#endif // GD_CUBISM_MODEL_LOADER
