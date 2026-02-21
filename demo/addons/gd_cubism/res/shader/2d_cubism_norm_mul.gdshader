// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2023 MizunagiKB <mizukb@live.jp>
// GDCubism shader: Mix
shader_type canvas_item;
render_mode blend_mul, unshaded;

instance uniform vec4 color_base: source_color;
instance uniform vec4 color_screen: source_color;
instance uniform vec4 color_multiply: source_color;
instance uniform bool color_override;

#include "cubism_mask.gdshaderinc"

void fragment() {
	if (discard_mask()) {
		discard;
	}
	
    vec4 color_tex = texture(TEXTURE, UV);
    color_tex.rgba = color_tex.rgba * color_multiply.rgba;

    // premul alpha
    color_tex.rgb = color_tex.rgb + color_screen.rgb - (color_tex.rgb * color_screen.rgb);

	if (has_mask) {
	    vec4 color_for_mask = sample_mask(color_tex, color_base);
	    COLOR = vec4(
            color_for_mask.r + (1.0 - color_for_mask.a),
            color_for_mask.g + (1.0 - color_for_mask.a),
            color_for_mask.b + (1.0 - color_for_mask.a),
            1.0
        ) * modulate;
	} else {
		// premul alpha
	    vec4 color = color_tex * color_base * modulate;
        COLOR = vec4(color.rgb * color.a, color.a);
	}
}
