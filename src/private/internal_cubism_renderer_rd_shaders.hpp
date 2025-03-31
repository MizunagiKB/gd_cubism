#ifndef INTERNAL_CUBISM_RENDERER_RD_SHADERS
#define INTERNAL_CUBISM_RENDERER_RD_SHADERS

#include <godot_cpp/variant/string.hpp>

using namespace godot;

class InternalCubismRendererRDShaders
{
public:
    static godot::String getCommon()
    {
        return R"(
#version 460
layout(set = 0, binding = 0) uniform UBO 
{
    mat4 u_matrix;
    mat4 u_clipMatrix;
    vec4 u_baseColor;
    vec4 u_multiplyColor;
    vec4 u_screenColor;
    vec4 u_channelFlag;
}ubo;

layout(set = 0, binding = 1) uniform sampler2D s_texture0;
layout(set = 0, binding = 2) uniform sampler2D s_texture1;
)";
    };

    static godot::String getFragShaderSrc()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0, v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + ubo.u_screenColor.rgb - (texColor.rgb * ubo.u_screenColor.rgb);
    vec4 color = texColor *  ubo.u_baseColor;
    outColor = vec4(color.rgb * color.a, color.a);
}
)";
    }

    static godot::String getFragShaderSrcMask()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_clipPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0, v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + ubo.u_screenColor.rgb - (texColor.rgb * ubo.u_screenColor.rgb);
    vec4 col_formask = texColor * ubo.u_baseColor;
    col_formask.rgb = col_formask.rgb  * col_formask.a;
    vec4 clipMask = (1.0 - texture(s_texture1, v_clipPos.xy / v_clipPos.w)) * ubo.u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    col_formask = col_formask * maskVal;
    outColor = col_formask;
}
)";
    }

    static godot::String getFragShaderSrcMaskInverted()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_clipPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0, v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = texColor.rgb + ubo.u_screenColor.rgb - (texColor.rgb * ubo.u_screenColor.rgb);
    vec4 col_formask = texColor * ubo.u_baseColor;
    col_formask.rgb = col_formask.rgb  * col_formask.a ;
    vec4 clipMask = (1.0 - texture(s_texture1, v_clipPos.xy / v_clipPos.w)) * ubo.u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    col_formask = col_formask * (1.0 - maskVal);
    outColor = col_formask;
}
)";
    }

    static godot::String getFragShaderSrcMaskInvertedPremultipliedAlpha()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_clipPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0, v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + ubo.u_screenColor.rgb * texColor.a) - (texColor.rgb * ubo.u_screenColor.rgb);
    vec4 col_formask = texColor * ubo.u_baseColor;
    vec4 clipMask = (1.0 - texture(s_texture1, v_clipPos.xy / v_clipPos.w)) * ubo.u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    col_formask = col_formask * (1.0 - maskVal);
    outColor = col_formask;
}
)";
    }

    static godot::String getFragShaderSrcMaskPremultipliedAlpha()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_clipPos;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0, v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + ubo.u_screenColor.rgb * texColor.a) - (texColor.rgb * ubo.u_screenColor.rgb);
    vec4 col_formask = texColor * ubo.u_baseColor;
    vec4 clipMask = (1.0 - texture(s_texture1, v_clipPos.xy / v_clipPos.w)) * ubo.u_channelFlag;
    float maskVal = clipMask.r + clipMask.g + clipMask.b + clipMask.a;
    col_formask = col_formask * maskVal;
    outColor = col_formask;
}
)";
    }

    static godot::String getFragShaderSrcPremultipliedAlpha()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;

void main()
{
    vec4 texColor = texture(s_texture0 , v_texCoord);
    texColor.rgb = texColor.rgb * ubo.u_multiplyColor.rgb;
    texColor.rgb = (texColor.rgb + ubo.u_screenColor.rgb * texColor.a) - (texColor.rgb * ubo.u_screenColor.rgb);
    outColor = texColor * ubo.u_baseColor;
}
)";
    }

    static godot::String getFragShaderSrcSetupMask()
    {
        return getCommon() + R"(
layout(location = 0) in vec2 v_texCoord;
layout(location = 1) in vec4 v_myPos;
layout(location = 0) out vec4 outColor;

void main()
{
    float isInside = step(ubo.u_baseColor.x, v_myPos.x/v_myPos.w)*
    step(ubo.u_baseColor.y, v_myPos.y/v_myPos.w)*
    step(v_myPos.x/v_myPos.w, ubo.u_baseColor.z)*
    step(v_myPos.y/v_myPos.w, ubo.u_baseColor.w);

    outColor = ubo.u_channelFlag * texture(s_texture0 , v_texCoord).a * isInside;
}
)";
    }

    static godot::String getVertShaderSrc()
    {
        return getCommon() + R"(
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 v_texCoord;

void main()
{
    vec4 pos = ubo.u_matrix * a_position;
    pos.y = -pos.y;
    gl_Position = pos;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}
)";
    }

    static godot::String getVertShaderSrcMasked()
    {
        return getCommon() + R"(
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 v_texCoord;
layout(location = 1) out vec4 v_clipPos;

void main()
{
    vec4 pos = ubo.u_matrix * a_position;
    pos.y = -pos.y;
    gl_Position = pos;
    v_clipPos = ubo.u_clipMatrix * a_position;
    v_clipPos.y = 1.0 - v_clipPos.y;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}


)";
    }

    static godot::String getVertShaderSrcSetupMask()
    {
        return getCommon() + R"(
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

layout(location = 0) out vec2 v_texCoord;
layout(location = 1) out vec4 v_myPos;

void main()
{
    vec4 pos = ubo.u_clipMatrix * a_position;
    pos.y = -pos.y;
    gl_Position = pos;
    v_myPos = ubo.u_clipMatrix * a_position;
    v_texCoord = a_texCoord;
    v_texCoord.y = 1.0 - v_texCoord.y;
}


)";
    }
};

#endif // INTERNAL_CUBISM_RENDERER_RD_SHADERS
