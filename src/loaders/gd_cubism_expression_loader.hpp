#ifndef GD_CUBISM_EXPRESSION_LOADER
#define GD_CUBISM_EXPRESSION_LOADER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <gd_cubism.hpp>
#include <gd_cubism_expression.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismExpressionLoader : public ResourceFormatLoader {
    GDCLASS(GDCubismExpressionLoader, ResourceFormatLoader);

protected:
    static void _bind_methods() {}

public:    
    PackedStringArray _get_recognized_extensions() const override {
        PackedStringArray ext;
        ext.append(EXPRESSION_FILE_EXTENSION);
        return ext;
    }

    bool _recognize_path(const String &p_path, const StringName &p_type) const override {
        return p_path.ends_with(EXPRESSION_FILE_EXTENSION) && ClassDB::is_parent_class(p_type, "GDCubismExpression");
    }

    bool _handles_type(const StringName &p_type) const override {
        return p_type == StringName("GDCubismExpression");
    }

    String _get_resource_type(const String &p_path) const override {
        if (p_path.ends_with(EXPRESSION_FILE_EXTENSION)) {
            return "GDCubismExpression";
        }
        return "";
    }
    
    Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override {
        String buffer = FileAccess::get_file_as_string(p_original_path);
        ERR_FAIL_COND_V(buffer.is_empty(), Error::FAILED);

        Ref<GDCubismExpression> resource;
        resource.instantiate();
        resource->set_path(p_original_path);

        String name = p_original_path.get_file();
        String clean_name = name.to_lower().left(-10).replace(" ", "_");

        resource->set_name(clean_name);

        Dictionary data = JSON::parse_string(buffer);
        Array parameters = data["Parameters"];

        resource->set_parameters(parameters);

        return resource;
    }
};

#endif // GD_CUBISM_EXPRESSION_LOADER
