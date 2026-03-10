#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2DArray u_Texture;
uniform vec2 u_UVOffset;
uniform float u_UVScale;
uniform int u_Layer;

void main() {
    // For TextureArray, we sample using (u, v, layer)
    // We can still use UVOffset/Scale if the array layers themselves are atlases,
    // but usually in a TextureArray, 1 layer = 1 block.
    // Assuming 1 layer per block for now:
    vec4 color = texture(u_Texture, vec3(TexCoord, float(u_Layer)));
    
    if(color.a < 0.1) discard;
    FragColor = color;
}
