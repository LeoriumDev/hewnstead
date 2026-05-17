#version 410 core

in vec2 v_uv;
in float v_layer;

uniform sampler2DArray u_blockTextures;

out vec4 frag_color;

void main() {
    frag_color = texture(u_blockTextures, vec3(v_uv, v_layer));
}
