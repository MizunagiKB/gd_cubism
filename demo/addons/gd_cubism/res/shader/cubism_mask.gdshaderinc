// masking
instance uniform bool has_mask = false;
instance uniform bool invert;
uniform sampler2D tex_mask : filter_linear_mipmap, repeat_disable;
uniform float mask_scale = 1.0;
uniform vec2 canvas_size = vec2(2,2);
uniform vec2 mesh_offset = vec2(0,0);

varying vec2 MASK_UV;
varying vec4 modulate;

bool discard_mask() {
	if(has_mask) {
		if (MASK_UV.x > 1.0 || MASK_UV.y > 1.0 || MASK_UV.x < 0.0 || MASK_UV.y < 0.0) {
			return true;
		}
	}
	return false;
}

vec2 mask_uv(vec2 vtx) {
	if (!has_mask) {
		return vec2(0,0);
	}
	
    vec2 mask_size = vec2(textureSize(tex_mask, 0)) / mask_scale;
    vec2 mask_vtx = vtx - mesh_offset;
    return mask_vtx / mask_size;
}

vec4 sample_mask(vec4 color_tex, vec4 base) {
    vec4 color_for_mask = color_tex * base;
    color_for_mask.rgb = color_for_mask.rgb * color_for_mask.a;

    vec4 clip_mask = texture(tex_mask, MASK_UV) * vec4(0,0,0,1);

    float mask_val = clip_mask.r + clip_mask.g + clip_mask.b + clip_mask.a;
    if (invert) {
        mask_val = 1.0 - mask_val;
    }
    return color_for_mask * mask_val;
}

void vertex() {
	MASK_UV = mask_uv(VERTEX.xy);
	modulate = COLOR;
}