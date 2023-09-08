// GDCubism shader: Mask
// author: MizunagiKB
shader_type canvas_item;
render_mode blend_mix, unshaded;

uniform vec4 channel;
uniform sampler2D tex_main;

void vertex() {
    UV.y = 1.0 - UV.y;
}

void fragment() {
    COLOR = channel * texture(tex_main, UV).a;
}
