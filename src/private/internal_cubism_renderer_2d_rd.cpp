// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>
#ifdef GD_CUBISM_USE_RENDERER_2D

#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/rd_texture_format.hpp>
#include <godot_cpp/classes/rd_texture_view.hpp>
#include <godot_cpp/classes/rd_attachment_format.hpp>
#include <godot_cpp/classes/rd_vertex_attribute.hpp>
#include <godot_cpp/classes/rd_pipeline_color_blend_state.hpp>
#include <godot_cpp/classes/rd_pipeline_color_blend_state_attachment.hpp>
#include <godot_cpp/classes/rd_pipeline_depth_stencil_state.hpp>
#include <godot_cpp/classes/rd_pipeline_multisample_state.hpp>
#include <godot_cpp/classes/rd_pipeline_rasterization_state.hpp>
#include <godot_cpp/classes/rd_pipeline_specialization_constant.hpp>
#include <godot_cpp/classes/rd_uniform.hpp>
#include <godot_cpp/classes/uniform_set_cache_rd.hpp>
#include <godot_cpp/classes/rd_sampler_state.hpp>
#include <godot_cpp/classes/rd_shader_source.hpp>
#include <godot_cpp/classes/rd_shader_spirv.hpp>
#include <godot_cpp/classes/texture2drd.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite2d.hpp>

#include <CubismFramework.hpp>
#include <Model/CubismModel.hpp>
#include <Rendering/CubismRenderer.hpp>

#include <private/internal_cubism_renderer_2d_rd.hpp>
#include <private/internal_cubism_renderer_rd_shaders.hpp>
#include <private/internal_cubism_renderer_resource.hpp>
#include <private/internal_cubism_user_model.hpp>

using namespace Live2D::Cubism::Core;
using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;

InternalCubismRenderer2D_RD::InternalCubismRenderer2D_RD() : 
_renderingDevice(NULL), _modelInitialized(false), _clippingManager(NULL), _clippingContextBufferForDraw(NULL), _clippingContextBufferForMask(NULL)
{
}

InternalCubismRenderer2D_RD::~InternalCubismRenderer2D_RD()
{
    CsmDelete(_outputTexture);
    CsmDelete(_clippingManager);

    {
        for(int i = 0; i < _uniform_sets.GetSize(); i++) {
            _renderingDevice->free_rid(_uniform_sets[i]);
        }
    }

    _renderingDevice->free_rid(_uniformBuffer);
    _renderingDevice->free_rid(_sampler);
    _outputTexture->DestroyOffscreenSurface();

    {
        for(int i = 0; i < _offscreenSurfaces.GetSize(); i++) {
            _offscreenSurfaces[i].DestroyOffscreenSurface();
        }
    }

    {
        for(int i = 0; i < _textures.GetSize(); i++) {
            _renderingDevice->free_rid(_textures[i]);
        }
    }

    {
        for(int i = 0; i < _drawableIndexData.GetSize(); i++) {
            if(GetModel()->GetDrawableVertexCount(i) == 0 || GetModel()->GetDrawableVertexIndexCount(i) == 0) {continue;}
            _renderingDevice->free_rid(_drawableIndexData[i].indexArray);
            _renderingDevice->free_rid(_drawableIndexData[i].indexBuffer);
            _renderingDevice->free_rid(_drawableIndexData[i].vertexArray);
            _renderingDevice->free_rid(_drawableIndexData[i].vertexPositionBuffer);
            _renderingDevice->free_rid(_drawableIndexData[i].vertexUVBuffer);
        }
    }
}

Vector2 InternalCubismRenderer2D_RD::get_size(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_size.X, vct_size.Y);
}

Vector2 InternalCubismRenderer2D_RD::get_origin(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return Vector2(vct_origin.X, vct_origin.Y);
}

float InternalCubismRenderer2D_RD::get_ppunit(const Csm::CubismModel *model) const
{
    Live2D::Cubism::Core::csmVector2 vct_size;
    Live2D::Cubism::Core::csmVector2 vct_origin;
    Csm::csmFloat32 ppunit;

    Live2D::Cubism::Core::csmReadCanvasInfo(model->GetModel(), &vct_size, &vct_origin, &ppunit);

    return ppunit;
}

void InternalCubismRenderer2D_RD::InitializeRenderer()
{
    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    _drawableIndexData.Resize(drawableCount);

    for (csmInt32 drawAssign = 0; drawAssign < drawableCount; drawAssign++)
    {
        const csmInt32 vcount = GetModel()->GetDrawableVertexCount(drawAssign);
        const csmInt32 icount = GetModel()->GetDrawableVertexIndexCount(drawAssign);
        if (vcount != 0 && icount != 0)
        {
            uint64_t posBufferSize = sizeof(ModelVertex::pos) * vcount; 
            uint64_t uvBufferSize = sizeof(ModelVertex::texCoord) * vcount; 
            RID vertexPositionBuffer = _renderingDevice->vertex_buffer_create(posBufferSize);
            RID vertexUVBuffer = _renderingDevice->vertex_buffer_create(uvBufferSize);
            TypedArray<RID> vertexBuffers;
            vertexBuffers.append(vertexPositionBuffer);
            vertexBuffers.append(vertexUVBuffer);
            RID vertexArray = _renderingDevice->vertex_array_create(vcount, InternalCubismPipelineMangerRD::GetInstance()->GetVertexFormat(), vertexBuffers);

            const csmUint16* indices = GetModel()->GetDrawableVertexIndices(drawAssign);
            PackedByteArray indicesArray;
            indicesArray.resize(icount * sizeof(csmUint16));
            csmUint16* p_indicesArray = reinterpret_cast<csmUint16*>(indicesArray.ptrw());
            for(int i = 0; i < icount; i++) 
            {
                p_indicesArray[i] = indices[i];
            }

            RID indexBuffer = _renderingDevice->index_buffer_create(icount, RenderingDevice::IndexBufferFormat::INDEX_BUFFER_FORMAT_UINT16, indicesArray);
            RID indexArray = _renderingDevice->index_array_create(indexBuffer, 0, icount);

            InternalCubismRenderer2D_RD::DrawableIndexData drawableData;
            drawableData.indexBuffer = indexBuffer;
            drawableData.indexArray = indexArray;
            drawableData.vertexPositionBuffer = vertexPositionBuffer;
            drawableData.vertexUVBuffer = vertexUVBuffer;
            drawableData.vertexArray = vertexArray;
            _drawableIndexData[drawAssign] = drawableData;
        }
    }

    Ref<RDSamplerState> samplerState;
    samplerState.instantiate();
    samplerState->set_repeat_u(RenderingDevice::SamplerRepeatMode::SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE);
    samplerState->set_repeat_v(RenderingDevice::SamplerRepeatMode::SAMPLER_REPEAT_MODE_CLAMP_TO_EDGE);
    samplerState->set_min_filter(RenderingDevice::SamplerFilter::SAMPLER_FILTER_LINEAR);
    samplerState->set_mag_filter(RenderingDevice::SamplerFilter::SAMPLER_FILTER_LINEAR);
	_sampler = _renderingDevice->sampler_create(samplerState);


    InitializeUBO();
    InitializeUniformSets();
}

void InternalCubismRenderer2D_RD::InitializeUBO()
{
    _modelUBO.projectionMatrix.resize(16);
    _modelUBO.clipMatrix.resize(16);
    _modelUBO.baseColor.resize(4);
    _modelUBO.multiplyColor.resize(4);
    _modelUBO.screenColor.resize(4);
    _modelUBO.channelFlag.resize(4);

    int32_t size = _modelUBO.projectionMatrix.to_byte_array().size() + 
        _modelUBO.clipMatrix.to_byte_array().size() + 
        _modelUBO.baseColor.to_byte_array().size() +
        _modelUBO.multiplyColor.to_byte_array().size() +
        _modelUBO.screenColor.to_byte_array().size() +
        _modelUBO.channelFlag.to_byte_array().size();

    _uniformBuffer = _renderingDevice->uniform_buffer_create(size);
}

//Initialize UniformSets for every Shader/Texture/MaskTexture (including dummy mask), because the Godot Uniform cache stops working after freeing resources (for example when switching models)
void InternalCubismRenderer2D_RD::InitializeUniformSets() 
{
    Ref<RDUniform> modelUniform;
    modelUniform.instantiate();
    modelUniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_UNIFORM_BUFFER);
    modelUniform->set_binding(0);
    modelUniform->add_id(_uniformBuffer);

    for(int i = 0; i < InternalCubismPipelineMangerRD::MeshShaders::nShaders; i++) {
        for(int j = 0; j < _textures.GetSize(); j++) {
            Ref<RDUniform> textureUniform;
            textureUniform.instantiate();
            textureUniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_SAMPLER_WITH_TEXTURE);
            textureUniform->add_id(_sampler);
            textureUniform->add_id(_textures[j]);
            textureUniform->set_binding(1);

            for(int k = 0; k < _offscreenSurfaces.GetSize(); k++) {
                Ref<RDUniform> maskUniform;
                maskUniform.instantiate();
                maskUniform->set_uniform_type(RenderingDevice::UniformType::UNIFORM_TYPE_SAMPLER_WITH_TEXTURE);
                maskUniform->add_id(_sampler);
                maskUniform->add_id(_offscreenSurfaces[k].GetTexture());
                maskUniform->set_binding(2);

                TypedArray<RDUniform> uniforms;
                uniforms.push_back(modelUniform);
                uniforms.push_back(textureUniform);
                uniforms.push_back(maskUniform);

                InternalCubismPipelineMangerRD::MeshShaders shaderID = static_cast<InternalCubismPipelineMangerRD::MeshShaders>(i);
                RID shader = InternalCubismPipelineMangerRD::GetInstance()->GetShader(shaderID);
                RID uniformSet = _renderingDevice->uniform_set_create(uniforms, shader, 0);
                _uniform_sets.PushBack(uniformSet);
            }
        }
    }
}

void InternalCubismRenderer2D_RD::Initialize(Csm::CubismModel *model, Csm::csmInt32 maskBufferCount)
{
    _renderingDevice = RenderingServer::get_singleton()->get_rendering_device();
    if (maskBufferCount < 1)
    {
        maskBufferCount = 1;
        CubismLogWarning("The number of render textures must be an integer greater than or equal to 1. Set the number of render textures to 1.");
    }

    _sortedDrawableIndexList.Resize(model->GetDrawableCount(), 0);

    if (model->IsUsingMasking())
    {
        _clippingManager = CSM_NEW InternalCubismClippingManager_RD();  
        _clippingManager->Initialize(
            *model,
            maskBufferCount
        );

        _offscreenSurfaces.Clear();
        for (csmInt32 i = 0; i < maskBufferCount; ++i)
        {
            InternalCubismOffscreenSurface_RD offscreenSurface;
            offscreenSurface.CreateOffscreenSurface(_clippingManager->GetClippingMaskBufferSize().X, _clippingManager->GetClippingMaskBufferSize().Y);
            _offscreenSurfaces.PushBack(offscreenSurface);
        }
    }

    //Dummy mask buffer so there's always a valid uniform set
    InternalCubismOffscreenSurface_RD dummyMask;
    dummyMask.CreateOffscreenSurface(1,1);
    _offscreenSurfaces.PushBack(dummyMask);

    CubismRenderer::Initialize(model, maskBufferCount);
}

void InternalCubismRenderer2D_RD::build_model(InternalCubismRendererResource &res, Node *target_node) 
{
    _outputTexture = CSM_NEW InternalCubismOffscreenSurface_RD();

    _outputTexture->CreateOffscreenSurface(GetModel()->GetCanvasWidthPixel(), GetModel()->GetCanvasHeightPixel());
    Texture2DRD* renderTexture = memnew(Texture2DRD);
    renderTexture->set_texture_rd_rid(_outputTexture->GetTexture());

    Sprite2D *sprite = memnew(Sprite2D);
    sprite->set_centered(true);
    sprite->set_texture(renderTexture);
    target_node->add_child(sprite);
    res.managed_nodes.append(sprite);

    for(int i = 0; i < res.ary_texture.size(); i++) 
    {
        Texture* texture = Object::cast_to<Texture>(res.ary_texture[i]);
        _textures.PushBack(RenderingServer::get_singleton()->texture_get_rd_texture(texture->get_rid()));
    }
    InitializeRenderer();
}


void InternalCubismRenderer2D_RD::update(InternalCubismRendererResource &res, int32_t viewport_size) 
{
    _outputTexture->Clear(0.0f, 0.0f, 0.0f, 0.0f);
    
    if (_clippingManager != NULL)
    {
        for (csmInt32 i = 0; i < _clippingManager->GetRenderTextureCount(); ++i)
        {
            if (_offscreenSurfaces[i].GetBufferWidth() != static_cast<csmUint32>(
                _clippingManager->
                GetClippingMaskBufferSize().X) ||
                _offscreenSurfaces[i].GetBufferHeight() != static_cast<csmUint32>(
                    _clippingManager->
                    GetClippingMaskBufferSize().Y))
            {
                _offscreenSurfaces[i].DestroyOffscreenSurface();
                _offscreenSurfaces[i].CreateOffscreenSurface(
                    static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X),
                    static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y));
            }
        }
        if (IsUsingHighPrecisionMask())
        {
            _clippingManager->SetupMatrixForHighPrecision(*GetModel(), false);
        }
        else
        {
            _clippingManager->SetupClippingContext(*GetModel(), this);
        }
    }

    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    const csmInt32* renderOrder = GetModel()->GetDrawableRenderOrders();


    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 order = renderOrder[i];
        _sortedDrawableIndexList[order] = i;
    }

    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 drawableIndex = _sortedDrawableIndexList[i];

        if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
        {
            continue;
        }

        InternalCubismClippingContext_RD* clipContext = (_clippingManager != NULL)
            ? (*_clippingManager->GetClippingContextListForDraw())[drawableIndex]
            : NULL;

        if (clipContext != NULL && IsUsingHighPrecisionMask())
        {
            InternalCubismOffscreenSurface_RD* renderSurface = _outputTexture;
            if(clipContext->_isUsing)
            {
                renderSurface = GetMaskBuffer(clipContext->_bufferIndex);
                renderSurface->Clear(1.0,1.0,1.0,1.0);
            }

            {
                const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                for (csmInt32 index = 0; index < clipDrawCount; index++)
                {
                    const csmInt32 clipDrawIndex = clipContext->_clippingIdList[index];

                    if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
                    {
                        continue;
                    }

                    IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0);
                    SetClippingContextBufferForMask(clipContext);
                    DrawMesh(*GetModel(), clipDrawIndex, renderSurface);
                }
            }

            {
                SetClippingContextBufferForMask(NULL);
            }
        }

        SetClippingContextBufferForDraw(clipContext);

        IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0);

        DrawMesh(*GetModel(), drawableIndex, _outputTexture);
    }
}

InternalCubismOffscreenSurface_RD* InternalCubismRenderer2D_RD::GetMaskBuffer(csmInt32 index)
{
    return &_offscreenSurfaces[index];
}

void InternalCubismRenderer2D_RD::SetClippingContextBufferForDraw(InternalCubismClippingContext_RD* clip)
{
    _clippingContextBufferForDraw = clip;
}

InternalCubismClippingContext_RD* InternalCubismRenderer2D_RD::GetClippingContextBufferForDraw() const
{
    return _clippingContextBufferForDraw;
}

void InternalCubismRenderer2D_RD::SetClippingContextBufferForMask(InternalCubismClippingContext_RD* clip)
{
    _clippingContextBufferForMask = clip;
}

InternalCubismClippingContext_RD* InternalCubismRenderer2D_RD::GetClippingContextBufferForMask() const
{
    return _clippingContextBufferForMask;
}

void InternalCubismRenderer2D_RD::DrawMesh(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface)
{
    if (model.GetDrawableVertexIndexCount(index) == 0) { return; }
    if (model.GetDrawableOpacity(index) <= 0.0f && GetClippingContextBufferForMask() == NULL) { return; }

    {
        csmInt32 textureIndex = model.GetDrawableTextureIndices(index);
        csmInt32 indexCount = model.GetDrawableVertexIndexCount(index);
        csmUint16* indexArray = const_cast<csmUint16*>(model.GetDrawableVertexIndices(index));

        CopyToBuffer(index, model.GetDrawableVertexCount(index),
                 const_cast<csmFloat32*>(model.GetDrawableVertices(index)),
                 reinterpret_cast<csmFloat32*>(const_cast<csmVector2*>(model.GetDrawableVertexUvs(index))));


     if (GetClippingContextBufferForMask() != NULL)  
    {
        ExecuteDrawForMask(model, index, surface);
    }
    else
    {
        ExecuteDrawForDraw(model, index, surface);
    }
    }

    SetClippingContextBufferForDraw(NULL);
    SetClippingContextBufferForMask(NULL);
}

void InternalCubismRenderer2D_RD::ExecuteDrawForDraw(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface) 
{
     InternalCubismPipelineMangerRD::RDPipeline rdPipeline = InternalCubismPipelineMangerRD::GetInstance()->GetPipelineForDraw(this, model, index);

    const csmBool masked = GetClippingContextBufferForDraw() != NULL;
    UpdateMatrix(_modelUBO.projectionMatrix, GetMvpMatrix());

    CubismTextureColor baseColor = GetModelColorWithOpacity(model.GetDrawableOpacity(index));
    CubismTextureColor multiplyColor = model.GetMultiplyColor(index);
    CubismTextureColor screenColor = model.GetScreenColor(index);
    SetColorUniformBuffer(_modelUBO, baseColor, multiplyColor, screenColor);

    uint32_t maskBufferIndex = _offscreenSurfaces.GetSize()-1; //Default buffer always exists
    if(masked) {
        UpdateMatrix(_modelUBO.clipMatrix, GetClippingContextBufferForDraw()->_matrixForDraw); 
        SetColorChannel(_modelUBO, GetClippingContextBufferForDraw());
        maskBufferIndex = GetClippingContextBufferForDraw()->_bufferIndex;
    }
    RID maskTexture = _offscreenSurfaces[maskBufferIndex].GetTexture();

    CopyUniformBuffer();

    csmInt32 textureIndex = model.GetDrawableTextureIndex(index);

    uint32_t uniformOffset = (rdPipeline.shaderID * _textures.GetSize() * _offscreenSurfaces.GetSize()) + (textureIndex * _offscreenSurfaces.GetSize()) + maskBufferIndex;
    RID uniformSet = _uniform_sets[uniformOffset];

    uint64_t drawList = _renderingDevice->draw_list_begin(surface->GetFrameBuffer(), 
                    RenderingDevice::InitialAction::INITIAL_ACTION_LOAD, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    RenderingDevice::InitialAction::INITIAL_ACTION_LOAD, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    PackedColorArray());

    _renderingDevice->draw_list_bind_render_pipeline(drawList, rdPipeline.pipeline);
	_renderingDevice->draw_list_bind_index_array(drawList, _drawableIndexData[index].indexArray);
	_renderingDevice->draw_list_bind_vertex_array(drawList, _drawableIndexData[index].vertexArray);
	_renderingDevice->draw_list_bind_uniform_set(drawList, uniformSet, 0);
	_renderingDevice->draw_list_draw(drawList, true, 1, 0);
    _renderingDevice->draw_list_end();
}


void InternalCubismRenderer2D_RD::ExecuteDrawForMask(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface)
{ 
    InternalCubismPipelineMangerRD::RDPipeline rdPipeline = InternalCubismPipelineMangerRD::GetInstance()->GetPipelineForMask(this, model, index);

    UpdateMatrix(_modelUBO.clipMatrix, GetClippingContextBufferForMask()->_matrixForMask);

    SetColorChannel(_modelUBO, GetClippingContextBufferForMask());

    csmRectF *rect = GetClippingContextBufferForMask()->_layoutBounds;
    CubismTextureColor baseColor = {rect->X * 2.0f - 1.0f, rect->Y * 2.0f - 1.0f, rect->GetRight() * 2.0f - 1.0f, rect->GetBottom() * 2.0f - 1.0f};
    CubismTextureColor multiplyColor = model.GetMultiplyColor(index);
    CubismTextureColor screenColor = model.GetScreenColor(index);
    SetColorUniformBuffer(_modelUBO, baseColor, multiplyColor, screenColor);

    CopyUniformBuffer();

    csmInt32 textureIndex = model.GetDrawableTextureIndex(index);
    uint32_t maskBufferIndex = _offscreenSurfaces.GetSize()-1; //Default buffer always exists

    uint32_t uniformOffset = (rdPipeline.shaderID * _textures.GetSize() * _offscreenSurfaces.GetSize()) + (textureIndex * _offscreenSurfaces.GetSize()) + maskBufferIndex;
    RID uniformSet = _uniform_sets[uniformOffset];

    uint64_t drawList = _renderingDevice->draw_list_begin(surface->GetFrameBuffer(), 
                    RenderingDevice::InitialAction::INITIAL_ACTION_LOAD, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    RenderingDevice::InitialAction::INITIAL_ACTION_LOAD, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    PackedColorArray());

    _renderingDevice->draw_list_bind_render_pipeline(drawList, rdPipeline.pipeline);
	_renderingDevice->draw_list_bind_index_array(drawList, _drawableIndexData[index].indexArray);
	_renderingDevice->draw_list_bind_vertex_array(drawList, _drawableIndexData[index].vertexArray);
	_renderingDevice->draw_list_bind_uniform_set(drawList, uniformSet, 0);
	_renderingDevice->draw_list_draw(drawList, true, 1, 0);
    _renderingDevice->draw_list_end();
}

void InternalCubismRenderer2D_RD::CopyToBuffer(csmInt32 index, const csmInt32 vcount, csmFloat32*varray, csmFloat32* uvarray) 
{
    PackedByteArray vertexPositionCopyBuffer;
    vertexPositionCopyBuffer.resize(vcount * sizeof(ModelVertex::pos));
    PackedByteArray vertexUVCopyBuffer;
    vertexUVCopyBuffer.resize(vcount * sizeof(ModelVertex::texCoord));
    csmVector2* p_PositionBuffer = reinterpret_cast<csmVector2*>(vertexPositionCopyBuffer.ptrw());
    csmVector2* p_UVBuffer = reinterpret_cast<csmVector2*>(vertexUVCopyBuffer.ptrw());
    for (csmInt32 i = 0; i < vcount; i ++)
    {
        p_PositionBuffer[i].X = varray[i*2+0];
        p_PositionBuffer[i].Y = varray[i*2+1];
        p_UVBuffer[i].X = uvarray[i*2+0];
        p_UVBuffer[i].Y = uvarray[i*2+1];
    }
   _renderingDevice->buffer_update(_drawableIndexData[index].vertexPositionBuffer, 0, vertexPositionCopyBuffer.size(), vertexPositionCopyBuffer);
   _renderingDevice->buffer_update(_drawableIndexData[index].vertexUVBuffer, 0, vertexUVCopyBuffer.size(), vertexUVCopyBuffer);
}

void InternalCubismRenderer2D_RD::CopyUniformBuffer()
{
    int offset = 0;
    PackedByteArray projectionMatrixBA = _modelUBO.projectionMatrix.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, projectionMatrixBA.size(), projectionMatrixBA);
    offset += projectionMatrixBA.size();

    PackedByteArray clipMatrixBA = _modelUBO.clipMatrix.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, clipMatrixBA.size(), clipMatrixBA);
    offset += clipMatrixBA.size();

    PackedByteArray baseColorBA = _modelUBO.baseColor.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, baseColorBA.size(), baseColorBA);
    offset += baseColorBA.size();

    PackedByteArray multiplyColorBA = _modelUBO.multiplyColor.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, multiplyColorBA.size(), multiplyColorBA);
    offset += multiplyColorBA.size();

    PackedByteArray screenColorBA = _modelUBO.screenColor.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, screenColorBA.size(), screenColorBA);
    offset += screenColorBA.size();

    PackedByteArray channelFlagBA = _modelUBO.channelFlag.to_byte_array();
    _renderingDevice->buffer_update(_uniformBuffer, offset, channelFlagBA.size(), channelFlagBA);
}

void InternalCubismRenderer2D_RD::UpdateMatrix(PackedFloat32Array& matrix, CubismMatrix44 cubismMat)
{
    for (int i = 0; i < 16; i++)
    {
        matrix[i] = cubismMat.GetArray()[i];
    }
}

void InternalCubismRenderer2D_RD::UpdateColor(PackedFloat32Array& color, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a)
{
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
}

void InternalCubismRenderer2D_RD::SetColorUniformBuffer(ModelUBO& ubo, const CubismTextureColor& baseColor,
                                                  const CubismTextureColor& multiplyColor, const CubismTextureColor& screenColor)
{
    UpdateColor(ubo.baseColor, baseColor.R, baseColor.G, baseColor.B, baseColor.A);
    UpdateColor(ubo.multiplyColor, multiplyColor.R, multiplyColor.G, multiplyColor.B, multiplyColor.A);
    UpdateColor(ubo.screenColor, screenColor.R, screenColor.G, screenColor.B, screenColor.A);
}

void InternalCubismRenderer2D_RD::SetColorChannel(ModelUBO& ubo, InternalCubismClippingContext_RD* contextBuffer)
{
    const csmInt32 channelIndex = contextBuffer->_layoutChannelIndex;
    CubismTextureColor *colorChannel = contextBuffer->GetClippingManager()->GetChannelFlagAsColor(channelIndex);
    UpdateColor(ubo.channelFlag, colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A);
}

csmBool InternalCubismOffscreenSurface_RD::CreateOffscreenSurface(csmUint32 displayBufferWidth, csmUint32 displayBufferHeight) 
{
    DestroyOffscreenSurface();

    RenderingDevice* rd = RenderingServer::get_singleton()->get_rendering_device();

    Ref<RDTextureFormat> textureFormat;
    textureFormat.instantiate();
    textureFormat->set_texture_type(RenderingDevice::TEXTURE_TYPE_2D);
    textureFormat->set_width(displayBufferWidth);
    textureFormat->set_height(displayBufferHeight);
    textureFormat->set_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
    textureFormat->set_usage_bits(RenderingDevice::TEXTURE_USAGE_COLOR_ATTACHMENT_BIT | RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice::TEXTURE_USAGE_CAN_COPY_FROM_BIT);

	Ref<RDAttachmentFormat> attachmentFormat; 
    attachmentFormat.instantiate();
	attachmentFormat->set_format(textureFormat->get_format());
	attachmentFormat->set_usage_flags(textureFormat->get_usage_bits());
	attachmentFormat->set_samples(RenderingDevice::TEXTURE_SAMPLES_1);

    TypedArray<RDAttachmentFormat> frameBufferFormatArray;
    frameBufferFormatArray.push_back(attachmentFormat);
    uint64_t frameBufferFormat = rd->framebuffer_format_create(frameBufferFormatArray);
    		
	Ref<RDTextureView> textureView;
    textureView.instantiate();
	_texture = rd->texture_create(textureFormat, textureView);


    TypedArray<RID> textureArray;
    textureArray.push_back(_texture);
	_frameBuffer = rd->framebuffer_create(textureArray, frameBufferFormat);

    _bufferWidth = displayBufferWidth;
    _bufferHeight = displayBufferHeight;

    _isValid = rd->texture_is_valid(_texture) && rd->framebuffer_is_valid(_frameBuffer);
    return true;
}

void InternalCubismOffscreenSurface_RD::DestroyOffscreenSurface()
{
    RenderingDevice* rd = RenderingServer::get_singleton()->get_rendering_device();
    if(_frameBuffer.is_valid())
    {
         rd->free_rid(_frameBuffer);
        _frameBuffer = RID();
    }
    if(_texture.is_valid()) 
    {
        rd->free_rid(_texture);
        _texture = RID();
    }
}

csmUint32 InternalCubismOffscreenSurface_RD::GetBufferWidth() const {return _bufferWidth;}
csmUint32 InternalCubismOffscreenSurface_RD::GetBufferHeight() const {return _bufferHeight;}
csmBool InternalCubismOffscreenSurface_RD::IsValid() const {return _isValid;};
RID InternalCubismOffscreenSurface_RD::GetFrameBuffer() const {return _frameBuffer;};
RID InternalCubismOffscreenSurface_RD::GetTexture() const {return _texture;};

void InternalCubismOffscreenSurface_RD::Clear(csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a) 
{ 
    RenderingDevice* rd = RenderingServer::get_singleton()->get_rendering_device();
    PackedColorArray clearColors;
    clearColors.append(Color(r,g,b,a));
    uint64_t drawList = rd->draw_list_begin(GetFrameBuffer(), 
                    RenderingDevice::InitialAction::INITIAL_ACTION_CLEAR, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    RenderingDevice::InitialAction::INITIAL_ACTION_CLEAR, 
                    RenderingDevice::FinalAction::FINAL_ACTION_STORE,
                    clearColors);
    rd->draw_list_end();
};


void InternalCubismClippingManager_RD::SetupClippingContext(CubismModel& model, InternalCubismRenderer2D_RD* renderer)
{
    RenderingDevice* rd = RenderingServer::get_singleton()->get_rendering_device();

    csmInt32 usingClipCount = 0;
    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        InternalCubismClippingContext_RD* cc = _clippingContextListForMask[clipIndex];
        CalcClippedDrawTotalBounds(model, cc);

        if (cc->_isUsing)
        {
            usingClipCount++; 
        }
    }

    if (usingClipCount <= 0)
    {
        return;
    }

    _currentMaskBuffer = renderer->GetMaskBuffer(0);
    _currentMaskBuffer->Clear(1.0,1.0,1.0,1.0);
    SetupLayoutBounds(usingClipCount);

    if (_clearedMaskBufferFlags.GetSize() != _renderTextureCount)
    {
        _clearedMaskBufferFlags.Clear();

        for (csmInt32 i = 0; i < _renderTextureCount; ++i)
        {
            _clearedMaskBufferFlags.PushBack(false);
        }
    }
    else
    {
        for (csmInt32 i = 0; i < _renderTextureCount; ++i)
        {
            _clearedMaskBufferFlags[i] = false;
        }
    }

    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        InternalCubismClippingContext_RD* clipContext = _clippingContextListForMask[clipIndex];
        csmRectF* allClippedDrawRect = clipContext->_allClippedDrawRect;
        csmRectF* layoutBoundsOnTex01 = clipContext->_layoutBounds;
        const csmFloat32 MARGIN = 0.05f;

        InternalCubismOffscreenSurface_RD* clipContextOffscreenSurface = renderer->GetMaskBuffer(clipContext->_bufferIndex);

        if (_currentMaskBuffer != clipContextOffscreenSurface)
        {
            _currentMaskBuffer = clipContextOffscreenSurface;
            _currentMaskBuffer->Clear(1.0f, 1.0f, 1.0f, 1.0f);
        }

        _tmpBoundsOnModel.SetRect(allClippedDrawRect);
        _tmpBoundsOnModel.Expand(allClippedDrawRect->Width * MARGIN, allClippedDrawRect->Height * MARGIN);
        csmFloat32 scaleX = layoutBoundsOnTex01->Width / _tmpBoundsOnModel.Width;
        csmFloat32 scaleY = layoutBoundsOnTex01->Height / _tmpBoundsOnModel.Height;

        createMatrixForMask(false, layoutBoundsOnTex01, scaleX, scaleY);

        clipContext->_matrixForMask.SetMatrix(_tmpMatrixForMask.GetArray());
        clipContext->_matrixForDraw.SetMatrix(_tmpMatrixForDraw.GetArray());

        const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
        for (csmInt32 i = 0; i < clipDrawCount; i++)
        {
            const csmInt32 clipDrawIndex = clipContext->_clippingIdList[i];

            if (!model.GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
            {
                continue;
            }

            renderer->IsCulling(model.GetDrawableCulling(clipDrawIndex) != 0);

            renderer->SetClippingContextBufferForMask(clipContext);
            renderer->DrawMesh(model, clipDrawIndex, _currentMaskBuffer);
        }
    }
    renderer->SetClippingContextBufferForMask(NULL);
}

//ClippingContext
InternalCubismClippingContext_RD::InternalCubismClippingContext_RD(
    CubismClippingManager<InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD>* manager, CubismModel& model,
    const csmInt32* clippingDrawableIndices, csmInt32 clipCount)
    : CubismClippingContext(clippingDrawableIndices, clipCount)
{
    _isUsing = false;
    _owner = manager;
}

InternalCubismClippingContext_RD::~InternalCubismClippingContext_RD() { }

CubismClippingManager<InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD> *InternalCubismClippingContext_RD::GetClippingManager()
{
    return _owner;
}

namespace {InternalCubismPipelineMangerRD  *_pipelineManagerInstance = NULL;}

InternalCubismPipelineMangerRD::~InternalCubismPipelineMangerRD()  
{
    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
    if(rd == NULL) {return;}
    {
        for(int i = 0; i < _pipelinesForMask.GetSize(); i++) 
        {
            rd->free_rid(_pipelinesForMask[i].pipeline);
        }
    }
    {
        for(int i = 0; i < _pipelinesForDraw.GetSize(); i++) 
        {
            rd->free_rid(_pipelinesForDraw[i].pipeline);
        }
    }
    {
        for(int i = 0; i < _shaders.GetSize(); i++) 
        {
            rd->free_rid(_shaders[i]);
        }
    }
}

InternalCubismPipelineMangerRD *InternalCubismPipelineMangerRD::GetInstance() 
{
    if(_pipelineManagerInstance == NULL) 
    {
        _pipelineManagerInstance = CSM_NEW InternalCubismPipelineMangerRD();
    }
    return _pipelineManagerInstance;
}

void InternalCubismPipelineMangerRD::Release()
{
    if(_pipelineManagerInstance != NULL) 
    {
        CsmDelete(_pipelineManagerInstance); 
        _pipelineManagerInstance = NULL;
    }
}

InternalCubismPipelineMangerRD::RDPipeline InternalCubismPipelineMangerRD::GetPipelineForMask(InternalCubismRenderer2D_RD* renderer, const CubismModel& model, const csmInt32 index) 
{
    int offset = renderer->IsCulling() ? 1 : 0;
    return _pipelinesForMask[offset];
}

InternalCubismPipelineMangerRD::RDPipeline InternalCubismPipelineMangerRD::GetPipelineForDraw(InternalCubismRenderer2D_RD* renderer, const CubismModel& model, const csmInt32 index) 
{
    const csmBool masked = renderer->GetClippingContextBufferForDraw() != NULL; 
    const csmBool invertedMask = model.GetDrawableInvertedMask(index);
    MaskingMode masking = masked ? (invertedMask ? MaskingMode::InvertedMask : MaskingMode::Masked) : MaskingMode::NoMask;
    CubismRenderer::CubismBlendMode blendMode = model.GetDrawableBlendMode(index);

    int culling = model.GetDrawableCulling(index) ? 1 : 0;
    int premultipliedAlpha = renderer->IsPremultipliedAlpha() ? 1 : 0;
    int nPremultipliedAlphaModes = 2;
    int nBlendModes = 3;
    int nMaskModes = 3;
    int cullingOffset = culling * (nPremultipliedAlphaModes * nMaskModes * nBlendModes);
    int premultipliedAlphaOffset = premultipliedAlpha * (nMaskModes * nBlendModes);
    int maskingOffset = masking * (nBlendModes);
    int blendModeOffset = blendMode;
    int offset = cullingOffset + premultipliedAlphaOffset + maskingOffset + blendModeOffset;

    return _pipelinesForDraw[offset];
}

uint64_t InternalCubismPipelineMangerRD::GetVertexFormat() const
{
    return _vertexFormat;
}

RID InternalCubismPipelineMangerRD::GetShader(MeshShaders shader) const
{
    return _shaders[shader];
}

InternalCubismPipelineMangerRD::InternalCubismPipelineMangerRD() 
{
    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
    if(rd == NULL) {return;}
    InitializeFrameBufferFormat();
    InitializeVertexFormat();
    InitializeShaders();
    InitializePipelines();
}

void InternalCubismPipelineMangerRD::InitializeVertexFormat() 
{
    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
    Ref<RDVertexAttribute> vertexAttributePosition;
    vertexAttributePosition.instantiate();
    vertexAttributePosition->set_format(RenderingDevice::DATA_FORMAT_R32G32_SFLOAT);
    vertexAttributePosition->set_offset(0);
    vertexAttributePosition->set_stride(2*sizeof(csmFloat32));
    vertexAttributePosition->set_location(0);

    Ref<RDVertexAttribute> vertexAttributeUV;
    vertexAttributeUV.instantiate();
    vertexAttributeUV->set_format(RenderingDevice::DATA_FORMAT_R32G32_SFLOAT);
    vertexAttributeUV->set_offset(0);
    vertexAttributeUV->set_stride(2*sizeof(csmFloat32));
    vertexAttributeUV->set_location(1);

    TypedArray<RDVertexAttribute> vertexAttributes;
    vertexAttributes.push_back(vertexAttributePosition);
    vertexAttributes.push_back(vertexAttributeUV);
    _vertexFormat = rd->vertex_format_create(vertexAttributes);
}

void InternalCubismPipelineMangerRD::InitializeFrameBufferFormat() 
{
    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
    Ref<RDTextureFormat> textureFormat;
    textureFormat.instantiate();
    textureFormat->set_texture_type(RenderingDevice::TEXTURE_TYPE_2D);
    textureFormat->set_width(1);
    textureFormat->set_height(1);
    textureFormat->set_format(RenderingDevice::DATA_FORMAT_R8G8B8A8_UNORM);
    textureFormat->set_usage_bits(RenderingDevice::TEXTURE_USAGE_COLOR_ATTACHMENT_BIT | RenderingDevice::TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice::TEXTURE_USAGE_CAN_COPY_FROM_BIT);

	Ref<RDAttachmentFormat> attachmentFormat; 
    attachmentFormat.instantiate();
	attachmentFormat->set_format(textureFormat->get_format());
	attachmentFormat->set_usage_flags(textureFormat->get_usage_bits());
	attachmentFormat->set_samples(RenderingDevice::TEXTURE_SAMPLES_1);

    TypedArray<RDAttachmentFormat> frameBufferFormatArray;
    frameBufferFormatArray.push_back(attachmentFormat);
    _frameBufferFormat = rd->framebuffer_format_create(frameBufferFormatArray);
}

void InternalCubismPipelineMangerRD::InitializePipelines()
{
    for(int isCulling = 0; isCulling <= 1; isCulling++)
    {
        _pipelinesForMask.PushBack(CreatePipelineForMask(isCulling));

        for(int isPremultipliedAlpha = 0; isPremultipliedAlpha <= 1; isPremultipliedAlpha++)
        {
            for(int nMaskingMode = 0; nMaskingMode <= MaskingMode::InvertedMask; nMaskingMode++)
            {
                for(int nBlendMode = 0; nBlendMode <= CubismRenderer::CubismBlendMode::CubismBlendMode_Multiplicative; nBlendMode++)
                {
                    CubismRenderer::CubismBlendMode blendMode = static_cast<CubismRenderer::CubismBlendMode>(nBlendMode);
                    MaskingMode maskingMode = static_cast<MaskingMode>(nMaskingMode);
                    _pipelinesForDraw.PushBack(CreatePipelineForDraw(isCulling, isPremultipliedAlpha, maskingMode, blendMode));
                }
            }
        }
    }
}
RID InternalCubismPipelineMangerRD::CompileShader(String vertSrc, String fragSrc) {
    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
    Ref<RDShaderSource> shaderSource; 
    shaderSource.instantiate();

	shaderSource->set_stage_source(RenderingDevice::ShaderStage::SHADER_STAGE_VERTEX, vertSrc);
	shaderSource->set_stage_source(RenderingDevice::ShaderStage::SHADER_STAGE_FRAGMENT, fragSrc);
	Ref<RDShaderSPIRV> spirvShader = rd->shader_compile_spirv_from_source(shaderSource);
	return rd->shader_create_from_spirv(spirvShader);
}

void InternalCubismPipelineMangerRD::InitializeShaders() 
{
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrcSetupMask(), InternalCubismRendererRDShaders::getFragShaderSrcSetupMask()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrc(), InternalCubismRendererRDShaders::getFragShaderSrc()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrcMasked(), InternalCubismRendererRDShaders::getFragShaderSrcMask()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrcMasked(), InternalCubismRendererRDShaders::getFragShaderSrcMaskInverted()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrc(), InternalCubismRendererRDShaders::getFragShaderSrcPremultipliedAlpha()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrcMasked(), InternalCubismRendererRDShaders::getFragShaderSrcMaskPremultipliedAlpha()));
     _shaders.PushBack(CompileShader(InternalCubismRendererRDShaders::getVertShaderSrcMasked(), InternalCubismRendererRDShaders::getFragShaderSrcMaskInvertedPremultipliedAlpha()));
}

InternalCubismPipelineMangerRD::RDPipeline InternalCubismPipelineMangerRD::CreatePipelineForMask(csmBool isCulling) 
{
    RID shader = _shaders[MeshShaders::SetupMaskShader];

    Ref<RDPipelineColorBlendStateAttachment> pipelineColorBlendStateAttachement;
    pipelineColorBlendStateAttachement.instantiate();
    pipelineColorBlendStateAttachement->set_enable_blend(true);
    pipelineColorBlendStateAttachement->set_src_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ZERO);
    pipelineColorBlendStateAttachement->set_dst_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_COLOR);
    pipelineColorBlendStateAttachement->set_src_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ZERO);
    pipelineColorBlendStateAttachement->set_dst_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
    pipelineColorBlendStateAttachement->set_color_blend_op(RenderingDevice::BlendOperation::BLEND_OP_ADD);
    pipelineColorBlendStateAttachement->set_alpha_blend_op(RenderingDevice::BlendOperation::BLEND_OP_ADD);


    Ref<RDPipelineRasterizationState> pipelineRasterizationState;
    pipelineRasterizationState.instantiate();
    pipelineRasterizationState->set_front_face(RenderingDevice::PolygonFrontFace::POLYGON_FRONT_FACE_COUNTER_CLOCKWISE);
    pipelineRasterizationState->set_cull_mode(isCulling ? RenderingDevice::PolygonCullMode::POLYGON_CULL_BACK : RenderingDevice::PolygonCullMode::POLYGON_CULL_DISABLED);

	Ref<RDPipelineMultisampleState> pipelineMultisampleState;
    pipelineMultisampleState.instantiate();
    pipelineMultisampleState->set_sample_count(RenderingDevice::TextureSamples::TEXTURE_SAMPLES_1);

	Ref<RDPipelineDepthStencilState> pipelineDepthStencilState;
    pipelineDepthStencilState.instantiate();

    Ref<RDPipelineColorBlendState> pipelineColorBlendState; 
    pipelineColorBlendState.instantiate();

    TypedArray<RDPipelineColorBlendStateAttachment> pipelineColorBlendStateAttachements;
    pipelineColorBlendStateAttachements.push_back(pipelineColorBlendStateAttachement);
    pipelineColorBlendState->set_attachments(pipelineColorBlendStateAttachements);

    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
	RID pipeline = rd->render_pipeline_create(
		shader,
		_frameBufferFormat,
		_vertexFormat,
		RenderingDevice::RENDER_PRIMITIVE_TRIANGLES,
        pipelineRasterizationState,
        pipelineMultisampleState,
		pipelineDepthStencilState,
		pipelineColorBlendState
	);
    return {pipeline, shader, isCulling ? PipelineConfiguration::SetupMaskCulling : PipelineConfiguration::SetupMaskNoCulling, MeshShaders::SetupMaskShader};
}

InternalCubismPipelineMangerRD::RDPipeline InternalCubismPipelineMangerRD::CreatePipelineForDraw(csmBool isCulling, csmBool isPremultipliedAlpha, MaskingMode masking, CubismRenderer::CubismBlendMode blendMode) 
{
    MeshShaders shaderID;
    if (masking == MaskingMode::NoMask) {
        if(isPremultipliedAlpha) { shaderID = MeshShaders::NoMaskShaderPremultipliedAlpha; } 
        else { shaderID = MeshShaders::NoMaskShader;}
    }
    if (masking == MaskingMode::Masked) {
        if(isPremultipliedAlpha) { shaderID = MeshShaders::MaskShaderPremultipliedAlpha; } 
        else { shaderID = MeshShaders::MaskShader;}
    }
    if (masking == MaskingMode::InvertedMask) {
        if(isPremultipliedAlpha) { shaderID = MeshShaders::InvertedMaskShaderPremultipliedAlpha; } 
        else { shaderID = MeshShaders::InvertedMaskShader;}
    }
    RID shader = _shaders[shaderID];

    Ref<RDPipelineColorBlendStateAttachment> pipelineColorBlendStateAttachement;
    pipelineColorBlendStateAttachement.instantiate();
    pipelineColorBlendStateAttachement->set_enable_blend(true);
    pipelineColorBlendStateAttachement->set_color_blend_op(RenderingDevice::BlendOperation::BLEND_OP_ADD);
    pipelineColorBlendStateAttachement->set_alpha_blend_op(RenderingDevice::BlendOperation::BLEND_OP_ADD);
    switch(blendMode){
        case CubismRenderer::CubismBlendMode::CubismBlendMode_Normal:
            pipelineColorBlendStateAttachement->set_src_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            pipelineColorBlendStateAttachement->set_dst_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
            pipelineColorBlendStateAttachement->set_src_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            pipelineColorBlendStateAttachement->set_dst_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
            break;
        case CubismRenderer::CubismBlendMode::CubismBlendMode_Additive:
            pipelineColorBlendStateAttachement->set_src_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            pipelineColorBlendStateAttachement->set_dst_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            pipelineColorBlendStateAttachement->set_src_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ZERO);
            pipelineColorBlendStateAttachement->set_dst_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            break;
        case CubismRenderer::CubismBlendMode::CubismBlendMode_Multiplicative:
            pipelineColorBlendStateAttachement->set_src_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_DST_COLOR);
            pipelineColorBlendStateAttachement->set_dst_color_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA);
            pipelineColorBlendStateAttachement->set_src_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ZERO);
            pipelineColorBlendStateAttachement->set_dst_alpha_blend_factor(RenderingDevice::BlendFactor::BLEND_FACTOR_ONE);
            break;
    }

    Ref<RDPipelineRasterizationState> pipelineRasterizationState;
    pipelineRasterizationState.instantiate();
    pipelineRasterizationState->set_front_face(RenderingDevice::PolygonFrontFace::POLYGON_FRONT_FACE_COUNTER_CLOCKWISE);
    pipelineRasterizationState->set_cull_mode(isCulling ? RenderingDevice::PolygonCullMode::POLYGON_CULL_BACK : RenderingDevice::PolygonCullMode::POLYGON_CULL_DISABLED);

	Ref<RDPipelineMultisampleState> pipelineMultisampleState;
    pipelineMultisampleState.instantiate();
    pipelineMultisampleState->set_sample_count(RenderingDevice::TextureSamples::TEXTURE_SAMPLES_1);

	Ref<RDPipelineDepthStencilState> pipelineDepthStencilState;
    pipelineDepthStencilState.instantiate();

    Ref<RDPipelineColorBlendState> pipelineColorBlendState; 
    pipelineColorBlendState.instantiate();

    TypedArray<RDPipelineColorBlendStateAttachment> pipelineColorBlendStateAttachements;
    pipelineColorBlendStateAttachements.push_back(pipelineColorBlendStateAttachement);
    pipelineColorBlendState->set_attachments(pipelineColorBlendStateAttachements);

    RenderingDevice *rd = RenderingServer::get_singleton()->get_rendering_device();
	RID pipeline = rd->render_pipeline_create(
		shader,
		_frameBufferFormat,
		_vertexFormat,
		RenderingDevice::RENDER_PRIMITIVE_TRIANGLES,
        pipelineRasterizationState,
        pipelineMultisampleState,
		pipelineDepthStencilState,
		pipelineColorBlendState
	);

    int culling = isCulling ? 1 : 0;
    int premultipliedAlpha = isPremultipliedAlpha ? 1 : 0;
    int nPremultipliedAlphaModes = 2;
    int nBlendModes = 3;
    int nMaskModes = 3;
    int cullingOffset = culling * (nPremultipliedAlphaModes * nMaskModes * nBlendModes);
    int premultipliedAlphaOffset = premultipliedAlpha * (nMaskModes * nBlendModes);
    int maskingOffset = masking * (nBlendModes);
    int blendModeOffset = blendMode;
    int offset = cullingOffset + premultipliedAlphaOffset + maskingOffset + blendModeOffset;
    PipelineConfiguration configuration = static_cast<PipelineConfiguration>(offset);

    return {pipeline, shader, configuration, shaderID};
}

#endif // GD_CUBISM_USE_RENDERER_2D
