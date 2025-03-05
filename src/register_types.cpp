// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <gdextension_interface.h>

#include <CubismFramework.hpp>

#include <loaders/gd_cubism_motion_loader.hpp>
#include <private/internal_cubism_allocator.hpp>
#include <gd_cubism_effect.hpp>
#include <gd_cubism_effect_breath.hpp>
#include <gd_cubism_effect_custom.hpp>
#include <gd_cubism_effect_eye_blink.hpp>
#include <gd_cubism_effect_hit_area.hpp>
#include <gd_cubism_effect_target_point.hpp>
#include <gd_cubism_motion_entry.hpp>
#include <gd_cubism_value_abs.hpp>
#include <gd_cubism_value_parameter.hpp>
#include <gd_cubism_value_part_opacity.hpp>
#include <gd_cubism_user_model.hpp>
#include <register_types.hpp>
#include <plugin.hpp>

// --------------------------------------------------------------- namespace(s)
using namespace godot;

static InternalCubismAllocator allocator;
static Csm::CubismFramework::Option option;

static Ref<GDCubismMotionLoader> motionLoader;

// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
void output(const char *message) {
    #ifdef DEBUG_ENABLED
    WARN_PRINT(message);
    #endif // DEBUG_ENABLED
}

void initialize_gd_cubism_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        ClassDB::register_class<GDCubismPlugin>();
        EditorPlugins::add_by_type<GDCubismPlugin>();
    }

    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    option.LogFunction = output;

    #ifdef DEBUG_ENABLED
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel::LogLevel_Verbose;
    #else
    option.LoggingLevel = Csm::CubismFramework::Option::LogLevel::LogLevel_Off;
    #endif // DEBUG_ENABLED

    Csm::CubismFramework::StartUp(&allocator, &option);
    Csm::CubismFramework::Initialize();

    GDREGISTER_VIRTUAL_CLASS(GDCubismEffect);
    GDREGISTER_CLASS(GDCubismEffectBreath);
    GDREGISTER_CLASS(GDCubismEffectCustom);
    GDREGISTER_CLASS(GDCubismEffectEyeBlink);
    GDREGISTER_CLASS(GDCubismEffectHitArea);
    GDREGISTER_CLASS(GDCubismEffectTargetPoint);

    GDREGISTER_VIRTUAL_CLASS(GDCubismValueAbs);
    GDREGISTER_CLASS(GDCubismParameter);
    GDREGISTER_CLASS(GDCubismPartOpacity);

    ClassDB::register_class<GDCubismMotionLoader>();
    ClassDB::register_class<GDCubismMotionQueueEntryHandle>();
    ClassDB::register_class<GDCubismMotionEntry>();
    ClassDB::register_class<GDCubismUserModel>();

    motionLoader.instantiate();

    // prioritize our format loaders so that the more generic json loader isn't preferred
    ResourceLoader::get_singleton()->add_resource_format_loader(motionLoader, true);
}

void uninitialize_gd_cubism_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        EditorPlugins::remove_by_type<GDCubismPlugin>();
    }
    
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

    ResourceLoader::get_singleton()->remove_resource_format_loader(motionLoader);
    motionLoader.unref();
    
    Csm::CubismFramework::Dispose();
}

extern "C" {

    // GDCubism init.
    GDExtensionBool GDE_EXPORT gd_cubism_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_gd_cubism_module);
        init_obj.register_terminator(uninitialize_gd_cubism_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
