// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <CubismFramework.hpp>
#include <Rendering/CubismRenderer.hpp>

#ifdef GD_CUBISM_USE_RENDERER_2D
    #include <private/internal_cubism_renderer_2d.hpp>
#else
    #include <private/internal_cubism_renderer_3d.hpp>
#endif // GD_CUBISM_USE_RENDERER_2D

#include <private/internal_cubism_user_model.hpp>


// ------------------------------------------------------------------ define(s)
// --------------------------------------------------------------- namespace(s)
using namespace Live2D::Cubism::Framework::Rendering;


// -------------------------------------------------------------------- enum(s)
// ------------------------------------------------------------------- const(s)
// ------------------------------------------------------------------ static(s)
// ----------------------------------------------------------- class:forward(s)
// ------------------------------------------------------------------- class(s)
CubismRenderer* CubismRenderer::Create(Csm::csmUint32 width, Csm::csmUint32 height) {
    #ifdef GD_CUBISM_USE_RENDERER_2D
    return CSM_NEW InternalCubismRenderer2D(width, height);
    #else
    return CSM_NEW InternalCubismRenderer3D(width, height);
    #endif // GD_CUBISM_USE_RENDERER_2D
}

void CubismRenderer::StaticRelease() {
}


// ------------------------------------------------------------------ method(s)
