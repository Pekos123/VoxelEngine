#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_ViewProj;
uniform vec3 blockPos;
uniform float u_OutlineThickness; // Try a value like 0.005

void main() {
    // 1. Calculate the standard world position
    vec4 worldPos = vec4(aPos + blockPos, 1.0);
    
    // 2. Project to Clip Space
    vec4 projectedPos = u_ViewProj * worldPos;

    // 3. Get the "Normal" direction for expansion
    // Since it's a cube centered at 0.5, (aPos - 0.5) gives us the direction out from center
    vec3 direction = normalize(aPos - 0.5);

    // 4. Scale expansion by gl_Position.w
    // This makes the offset constant in screen-space pixels regardless of FOV/Distance
    float offset = u_OutlineThickness * projectedPos.w;
    
    // Apply the offset in world space before the final projection
    // OR apply it directly to the projected position
    gl_Position = u_ViewProj * vec4(worldPos.xyz + (direction * offset), 1.0);
}