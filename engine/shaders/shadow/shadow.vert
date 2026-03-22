#version 330 core
layout (location = 0) in uint a_Data; // All data packed into one 32-bit int

uniform mat4 u_LightSpaceMatrix;
uniform vec3 u_ChunkPos;

void main()
{
    // UNPACKING (Matches the C++ and obj.vert packing logic)
    uint x = a_Data & 0x1Fu;
    uint y = (a_Data >> 5u) & 0xFFu;
    uint z = (a_Data >> 13u) & 0x1Fu;
    uint normalID = (a_Data >> 20u) & 0x7u;
    uint vIdx = (a_Data >> 23u) & 0x3u;

    // Offsets for the 4 corners of a face quad
    vec3 vOffsets[24] = vec3[](
        // TOP (+Y)
        vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), vec3(1, 1, 0),
        // BOTTOM (-Y)
        vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1),
        // FRONT (+Z)
        vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(0, 1, 1),
        // BACK (-Z)
        vec3(1, 0, 0), vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0),
        // LEFT (-X)
        vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1), vec3(0, 1, 0),
        // RIGHT (+X)
        vec3(1, 0, 1), vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1)
    );

    vec3 localPos = vec3(float(x), float(y), float(z)) + vOffsets[normalID * 4u + vIdx];
    vec3 worldPos = u_ChunkPos + localPos; 
    
    gl_Position = u_LightSpaceMatrix * vec4(worldPos, 1.0);
}
