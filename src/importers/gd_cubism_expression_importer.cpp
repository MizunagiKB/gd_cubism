#include <importers/gd_cubism_expression_importer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <gd_cubism_expression.hpp>

// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)

Error GDCubismExpressionImporter::_import(const String &p_source_file, const String &p_save_path, const Dictionary &p_options, const TypedArray<String> &p_platform_variants, const TypedArray<String> &p_gen_files) const {
    ERR_FAIL_COND_V(!p_source_file.ends_with(".exp3.json"), Error::FAILED);

	auto resource = memnew(GDCubismExpression);
    
    String filename = p_save_path + String(".") + this->_get_save_extension();
    return ResourceSaver::get_singleton()->save(resource, filename);
}

// ------------------------------------------------------------------ method(s)
