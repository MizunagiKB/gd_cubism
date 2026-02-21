// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef GD_CUBISM
#define GD_CUBISM


// ----------------------------------------------------------------- include(s)
// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
// -------------------------------------------------------------------- enum(s)
enum GDCubismShader {
    GD_CUBISM_SHADER_NORM_ADD,
    GD_CUBISM_SHADER_NORM_MIX,
    GD_CUBISM_SHADER_NORM_MUL,
    GD_CUBISM_SHADER_MASK,
    GD_CUBISM_SHADER_MAX
};


// ------------------------------------------------------------------- const(s)
const static int MAX_PRINTLOG_LENGTH = 256;

const static char* PROP_PARAMETER_GROUP = "Parameters";
const static char* PROP_PART_OPACITY_GROUP = "PartOpacity";

const static char* SIGNAL_EFFECT_HIT_AREA_ENTERED = "hit_area_entered";
const static char* SIGNAL_EFFECT_HIT_AREA_EXITED = "hit_area_exited";

const static char* MOTION_FILE_EXTENSION = "motion3.json";
const static char* EXPRESSION_FILE_EXTENSION = "exp3.json";
const static char* MODEL_FILE_EXTENSION = "model3.json";

#ifdef CUBISM_MOTION_CUSTOMDATA
const static char* SIGNAL_MOTION_FINISHED = "motion_finished";
#endif //CUBISM_MOTION_CUSTOMDATA

// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)

#endif // GD_CUBISM
