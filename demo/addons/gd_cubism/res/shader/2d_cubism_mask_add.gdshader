// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// GDCubism shader: Mask + Add
shader_type canvas_item;
render_mode blend_premul_alpha, unshaded;

uniform vec4 color_base;
uniform vec4 color_screen;
uniform vec4 color_multiply;
uniform vec4 channel;
uniform sampler2D tex_main : filter_linear_mipmap;
uniform sampler2D tex_mask : filter_linear_mipmap;

uniform vec2 mask_adjust_pos;
uniform float mask_adjust_scale;

void vertex() {
    UV.y = 1.0 - UV.y;
}

void fragment() {
    vec4 color_tex = texture(tex_main, UV);
    color_tex.rgb = color_tex.rgb * color_multiply.rgb;

    // premul alpha
    color_tex.rgb = color_tex.rgb + color_screen.rgb - (color_tex.rgb * color_screen.rgb);
    vec4 color_for_mask = color_tex * color_base;
    color_for_mask.rgb = color_for_mask.rgb * color_for_mask.a;

    float screen_scale = 1.0 / mask_adjust_scale;
    vec4 clip_mask = texture(tex_mask, ((SCREEN_UV - 0.5) * screen_scale) + 0.50 ) * channel;
    float mask_val = clip_mask.r + clip_mask.g + clip_mask.b + clip_mask.a;
    color_for_mask.rgb = color_for_mask.rgb * mask_val;
    COLOR = vec4(color_for_mask.rgb, 0.0);
}
