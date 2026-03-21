#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_Texture;
uniform vec2 u_UVOffset;
uniform float u_UVScale;

void main() {
    vec4 color = texture(u_Texture, TexCoord);
    
    if(color.a < 0.1) discard;
    FragColor = color;
}
