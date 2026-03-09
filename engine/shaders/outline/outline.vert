#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 u_ViewProj;
uniform vec3 blockPos;

void main() {
    // Expand the cube by 1% and center it
    // 1.01 makes it slightly larger than the 1.0 block
    // -0.005 centers that expansion (0.5 * 0.01)
    vec3 expandedPos = (aPos * 1.01) - 0.005; 
    
    gl_Position = u_ViewProj * vec4(expandedPos + blockPos, 1.0);
}