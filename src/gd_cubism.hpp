#ifndef GD_CUBISM
#define GD_CUBISM


// ----------------------------------------------------------------- include(s)
// ------------------------------------------------------------------ define(s)
#define GD_CUBISM_USE_RENDERER_2D
//#define GD_CUBISM_USE_RENDERER_3D

#ifdef GD_CUBISM_USE_RENDERER_2D
const bool SUBVIEWPORT_DISABLE_3D_FLAG = true;
#else
const bool SUBVIEWPORT_DISABLE_3D_FLAG = false;
#endif // GD_CUBISM_USE_RENDERER_2D

// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
enum GDCubismShader {
    GD_CUBISM_SHADER_NORM_ADD,
    GD_CUBISM_SHADER_NORM_MIX,
    GD_CUBISM_SHADER_NORM_MUL,
    GD_CUBISM_SHADER_MASK,
    GD_CUBISM_SHADER_MASK_ADD,
    GD_CUBISM_SHADER_MASK_ADD_INV,
    GD_CUBISM_SHADER_MASK_MIX,
    GD_CUBISM_SHADER_MASK_MIX_INV,
    GD_CUBISM_SHADER_MASK_MUL,
    GD_CUBISM_SHADER_MASK_MUL_INV,
    GD_CUBISM_SHADER_MAX
};


// ------------------------------------------------------------------- const(s)
const static int MAX_MOTION_NAME_LENGTH = 256;
const static int MAX_EXPRESSION_NAME_LENGTH = 256;
const static int MAX_PRINTLOG_LENGTH = 256;

const static bool DEFAULT_PROP_ANIM_LOOP = false;
const static bool DEFAULT_PROP_ANIM_LOOP_FADE_IN = true;

const static char* PROP_ANIM_GROUP = "Animation";
const static char* PROP_ANIM_EXPRESSION = "anim_expression";
const static char* PROP_ANIM_MOTION = "anim_motion";
const static char* PROP_ANIM_LOOP = "anim_loop";
const static char* PROP_ANIM_LOOP_FADE_IN = "anim_loop_fade_in";
const static char* PROP_PARAMETER_GROUP = "Parameter";
const static char* PROP_PART_OPACITY_GROUP = "PartOpacity";


// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)


#endif // GD_CUBISM
