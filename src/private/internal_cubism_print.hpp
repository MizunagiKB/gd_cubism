// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef INTERNAL_CUBISM_PRINT
#define INTERNAL_CUBISM_PRINT
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#ifdef WINDOWS_ENABLED
#include <stdio.h>
#include <stdarg.h>
#endif // WINDOWS_ENABLED
#include <string>

#include <godot_cpp/core/class_db.hpp>

#include <CubismFramework.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace godot;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
// ------------------------------------------------------------------ method(s)
void PrintLog(const Csm::csmChar* format, ...) {
    #ifdef DEBUG_ENABLED
    va_list args;
    Csm::csmChar buf[MAX_PRINTLOG_LENGTH];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    WARN_PRINT(buf);
    va_end(args);
    #endif // DEBUG_ENABLED
}


void PrintMessage(const Csm::csmChar* message) {
    #ifdef DEBUG_ENABLED
    PrintLog("%s", message);
    #endif // DEBUG_ENABLED
}


#endif // INTERNAL_CUBISM_PRINT
