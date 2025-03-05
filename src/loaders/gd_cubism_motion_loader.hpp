#ifndef GD_CUBISM_MOTION_LOADER
#define GD_CUBISM_MOTION_LOADER

// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/animation.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <gd_cubism.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

class GDCubismMotionLoader : public ResourceFormatLoader {
    GDCLASS(GDCubismMotionLoader, ResourceFormatLoader);

protected:
    static void _bind_methods() {}

public:
    PackedStringArray _get_recognized_extensions() const override {
        PackedStringArray ext;
        ext.append(MOTION_FILE_EXTENSION);
        return ext;
    }

    bool _recognize_path(const String &p_path, const StringName &p_type) const override {
        return p_path.ends_with(MOTION_FILE_EXTENSION);
    }

    bool _handles_type(const StringName &p_type) const override {
        return ClassDB::is_parent_class(p_type, "Animation");
    }

    String _get_resource_type(const String &p_path) const override {
        if (p_path.ends_with(MOTION_FILE_EXTENSION)) {
            return "Animation";
        }
            
        return "";
    }

    bool _exists(const String &p_path) const override {
        return FileAccess::file_exists(p_path);
    }
    
    Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
};

#endif // GD_CUBISM_MOTION_LOADER
