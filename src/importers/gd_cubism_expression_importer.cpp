#include <importers/gd_cubism_expression_importer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)

Ref<GDCubismExpression> GDCubismExpressionImporter::parse_expression(const String &p_source_file) {
    Ref<GDCubismExpression> resource = memnew(GDCubismExpression);
    resource->set_path(p_source_file);

    String name = p_source_file.get_file();
    String clean_name = name.to_lower().left(-10).replace(" ", "_");

    resource->set_name(clean_name);

    Dictionary data = JSON::parse_string(FileAccess::get_file_as_string(p_source_file));
    resource->set_parameters(data["Parameters"]);

    return resource;
}

Error GDCubismExpressionImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V(!p_source_file.ends_with(EXPRESSION_FILE_EXTENSION), Error::FAILED);

	auto resource = GDCubismExpressionImporter::parse_expression(p_source_file);
    
    String filename = p_save_path + String(".") + this->_get_save_extension();
    return ResourceSaver::get_singleton()->save(resource, filename);
}

// ------------------------------------------------------------------ method(s)
