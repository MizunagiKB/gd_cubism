// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
#ifndef INTERNAL_CUBISM_RENDERER_2D_RD
#define INTERNAL_CUBISM_RENDERER_2D_RD

// ----------------------------------------------------------------- include(s)
#include <gd_cubism.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/mesh_instance2d.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/rendering_device.hpp>

#include <Rendering/CubismRenderer.hpp>
#include <Rendering/CubismClippingManager.hpp>
#include <Type/csmVector.hpp>

#include <private/internal_cubism_renderer_resource.hpp>
#include <gd_cubism_user_model.hpp>


using namespace Live2D::Cubism::Framework;
using namespace Live2D::Cubism::Framework::Rendering;
using namespace godot;


class InternalCubismOffscreenSurface_RD;
class InternalCubismClippingManager_RD;
class InternalCubismClippingContext_RD;


class InternalCubismRenderer2D_RD : public CubismRenderer
{
    friend class CubismRenderer;
    friend class InternalCubismClippingManager_RD;

public:
    InternalCubismRenderer2D_RD();
    virtual ~InternalCubismRenderer2D_RD();

private:

    struct ModelUBO
    {
        PackedFloat32Array projectionMatrix; //16
        PackedFloat32Array clipMatrix; //16
        PackedFloat32Array baseColor; //4
        PackedFloat32Array multiplyColor; //4
        PackedFloat32Array screenColor; //4
        PackedFloat32Array channelFlag; //4
    };

    struct DrawableIndexData
    {
        RID vertexPositionBuffer;
        RID vertexUVBuffer;
        RID indexBuffer;
        RID vertexArray;
        RID indexArray;
    };

    RenderingDevice *_renderingDevice; 
    InternalCubismOffscreenSurface_RD* _outputTexture;
    InternalCubismClippingManager_RD *_clippingManager;
    InternalCubismClippingContext_RD*_clippingContextBufferForMask;
    InternalCubismClippingContext_RD*_clippingContextBufferForDraw;
    ModelUBO _modelUBO;

    csmVector<InternalCubismOffscreenSurface_RD> _offscreenSurfaces;
    csmVector<DrawableIndexData> _drawableIndexData; 

    csmVector<RID> _textures; 
    csmVector<csmInt32> _sortedDrawableIndexList; 
    RID _sampler;
    RID _uniformBuffer;
    csmBool _modelInitialized;

    void InitializeRenderer();
    void InitializeUBO();
    void DrawMesh(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface);
    void ExecuteDrawForDraw(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface);
    void ExecuteDrawForMask(const CubismModel& model, const csmInt32 index, InternalCubismOffscreenSurface_RD* surface);
    void CopyToBuffer(csmInt32 index, const csmInt32 vcount, csmFloat32*varray, csmFloat32* uvarray);
    void CopyUniformBuffer();

    void UpdateMatrix(PackedFloat32Array& vkMat16, CubismMatrix44 cubismMat);
    void UpdateColor(PackedFloat32Array&, csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a);
    void SetColorUniformBuffer(ModelUBO& ubo, const CubismTextureColor& baseColor, const CubismTextureColor& multiplyColor, const CubismTextureColor& screenColor);
    void SetColorChannel(ModelUBO& ubo, InternalCubismClippingContext_RD* contextBuffer);

public:
    Vector2 get_size(const Csm::CubismModel *model) const;
    Vector2 get_origin(const Csm::CubismModel *model) const;
    float get_ppunit(const Csm::CubismModel *model) const;

    void build_model(InternalCubismRendererResource &res, Node *target_node);
    void update(InternalCubismRendererResource &res, int32_t viewport_size = 0);

    virtual void Initialize(Csm::CubismModel *model, Csm::csmInt32 maskBufferCount);
    virtual void DoDrawModel() override {};
    virtual void SaveProfile() override {};
    virtual void RestoreProfile() override {};

    InternalCubismOffscreenSurface_RD* InternalCubismRenderer2D_RD::GetMaskBuffer(csmInt32 index);
    InternalCubismClippingContext_RD* GetClippingContextBufferForDraw() const;
    void SetClippingContextBufferForDraw(InternalCubismClippingContext_RD* clip);
    InternalCubismClippingContext_RD* GetClippingContextBufferForMask() const;
    void SetClippingContextBufferForMask(InternalCubismClippingContext_RD* clip);
};

class InternalCubismOffscreenSurface_RD {
public:
    csmBool CreateOffscreenSurface(csmUint32 displayBufferWidth, csmUint32 displayBufferHeight);
    void DestroyOffscreenSurface();

    csmUint32 GetBufferWidth() const;
    csmUint32 GetBufferHeight() const;

    csmBool IsValid() const;
    RID GetFrameBuffer() const;
    RID GetTexture() const;

    void Clear(csmFloat32 r, csmFloat32 g, csmFloat32 b, csmFloat32 a);

private:
    csmUint32   _bufferWidth;
    csmUint32   _bufferHeight;
    RID _frameBuffer;
    RID _texture;
    csmBool _isValid;
};

class InternalCubismClippingManager_RD : public CubismClippingManager<
            InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD>
{
public:
    void SetupClippingContext(CubismModel& model, InternalCubismRenderer2D_RD* renderer);
};

class InternalCubismClippingContext_RD : public CubismClippingContext
{
    friend class InternalCubismClippingManager_RD;
    friend class InternalCubismRenderer2D_RD;

public:
    InternalCubismClippingContext_RD(
        CubismClippingManager<InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD>* manager, CubismModel& model,
        const csmInt32* clippingDrawableIndices, csmInt32 clipCount);

    virtual ~InternalCubismClippingContext_RD();

    CubismClippingManager<InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD>* GetClippingManager();
    CubismClippingManager<InternalCubismClippingContext_RD, InternalCubismOffscreenSurface_RD>* _owner;
};

struct ModelVertex
{
    CubismVector2 pos; 
    CubismVector2 texCoord; 
};

/*
Manages all permutations of rendering pipelines.
Since the RenderingDevice doesn't support the SetCullMode extension yet, the number of pipelines is
SetupMask + SetupMaskCulling + (Culling + NoCulling) * (BlendNormal + BlendAdditive + BlendMultiplicative) * (Unmasked + Masked + MaskedInverted + UnmaskedPremultipliedAlpha + MaskedUnmaskedPremultipliedAlpha + MaskedInvertedPremultipliedAlpha)
= 37 pipelines
*/
class InternalCubismPipelineMangerRD 
{
    public:
    enum MaskingMode 
    {
        NoMask,
        Masked,
        InvertedMask
    };

    enum MeshShaders {
        SetupMaskShader,
        NoMaskShader,
        MaskShader,
        InvertedMaskShader,
        NoMaskShaderPremultipliedAlpha,
        MaskShaderPremultipliedAlpha,
        InvertedMaskShaderPremultipliedAlpha
    };

    enum PipelineConfiguration {
        NoCulling_NoPremAlpha_NoMask_Normal,
        NoCulling_NoPremAlpha_NoMask_Add,
        NoCulling_NoPremAlpha_NoMask_Mult,
        NoCulling_NoPremAlpha_Mask_Normal,
        NoCulling_NoPremAlpha_Mask_Add,
        NoCulling_NoPremAlpha_Mask_Mult,
        NoCulling_NoPremAlpha_Inverted_Normal,
        NoCulling_NoPremAlpha_Inverted_Add,
        NoCulling_NoPremAlpha_Inverted_Mult,
        NoCulling_PremAlpha_NoMask_Normal,
        NoCulling_PremAlpha_NoMask_Add,
        NoCulling_PremAlpha_NoMask_Mult,
        NoCulling_PremAlpha_Mask_Normal,
        NoCulling_PremAlpha_Mask_Add,
        NoCulling_PremAlpha_Mask_Mult,
        NoCulling_PremAlpha_Inverted_Normal,
        NoCulling_PremAlpha_Inverted_Add,
        NoCulling_PremAlpha_Inverted,
        Culling_NoPremAlpha_NoMask_Normal,
        Culling_NoPremAlpha_NoMask_Add,
        Culling_NoPremAlpha_NoMask_Mult,
        Culling_NoPremAlpha_Mask_Normal,
        Culling_NoPremAlpha_Mask_Add,
        Culling_NoPremAlpha_Mask_Mult,
        Culling_NoPremAlpha_Inverted_Normal,
        Culling_NoPremAlpha_Inverted_Add,
        Culling_NoPremAlpha_Inverted_Mult,
        Culling_PremAlpha_NoMask_Normal,
        Culling_PremAlpha_NoMask_Add,
        Culling_PremAlpha_NoMask_Mult,
        Culling_PremAlpha_Mask_Normal,
        Culling_PremAlpha_Mask_Add,
        Culling_PremAlpha_Mask_Mult,
        Culling_PremAlpha_Inverted_Normal,
        Culling_PremAlpha_Inverted_Add,
        Culling_PremAlpha_Inverted_Mult,
        SetupMaskNoCulling,
        SetupMaskCulling
    };

    struct RDPipeline 
    {
        RID pipeline;
        RID shader;
        PipelineConfiguration configuration;
    };

    static InternalCubismPipelineMangerRD  *GetInstance();
    static void  Release();
    ~InternalCubismPipelineMangerRD();
    RDPipeline GetPipelineForMask(InternalCubismRenderer2D_RD* renderer, const CubismModel& model, const csmInt32 index);
    RDPipeline GetPipelineForDraw(InternalCubismRenderer2D_RD* renderer, const CubismModel& model, const csmInt32 index);
    RID GetEmptyTexture() const;
    uint64_t GetVertexFormat() const;
    

    private:
    InternalCubismPipelineMangerRD();
    void InitializeVertexFormat();
    void InitializeFrameBufferFormat();
    void InitializeShaders();
    void InitializePipelines();
    void InitializeEmptyTexture();
    RID CompileShader(String vertSrc, String fragSrc);
    RDPipeline CreatePipelineForMask(csmBool isCulling);
    RDPipeline CreatePipelineForDraw(csmBool isCulling, csmBool isPremultipliedAlpha, MaskingMode masking, CubismRenderer::CubismBlendMode blendMode);

    RID _emptyTexture;
    csmVector<RDPipeline> _pipelinesForMask;
    csmVector<RDPipeline> _pipelinesForDraw;
    csmVector<RID> _shaders;
    uint64_t _vertexFormat;
    uint64_t _frameBufferFormat;

};

#endif // INTERNAL_CUBISM_RENDERER_2D_RD
