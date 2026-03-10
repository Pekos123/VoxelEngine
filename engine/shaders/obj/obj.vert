#version 330 core
layout (location = 0) in uint a_Data; // All data packed into one 32-bit int

out vec3 Normal;
out vec3 FragPos;
out float vAO;
out vec2 vTexCoord;
flat out uint vTextureID;

uniform mat4 u_ViewProj;
uniform vec3 u_ChunkPos; // Pass the chunk's world position (e.g., 16, 0, 32)

void main()
{
    // UNPACKING (Matches the C++ packing logic)
    // Bits 0-4: X (0-31)
    // Bits 5-12: Y (0-255)
    // Bits 13-17: Z (0-31)
    uint x = a_Data & 0x1Fu;
    uint y = (a_Data >> 5u) & 0xFFu;
    uint z = (a_Data >> 13u) & 0x1Fu;
    
    // Bits 18-19: AO (0-3)
    uint ao = (a_Data >> 18u) & 0x3u;
    vAO = float(ao) / 3.0; // Convert to 0.0 - 1.0 range

    // Bits 20-22: Normal ID (0-5)
    uint normalID = (a_Data >> 20u) & 0x7u;
    
    // Bits 23-24: Vertex Index (0-3)
    uint vIdx = (a_Data >> 23u) & 0x3u;
    
    // Bits 25-31: Block Id (0-127)
    uint blockID = (a_Data >> 25u) & 0x7Fu;

    vAO = float(ao) / 3.0;
    vTextureID = blockID - 1u; // Adjust if blockID 1 is the first texture (index 0)

    vec3 normals[6] = vec3[](
        vec3(0, 1, 0),  // TOP
        vec3(0, -1, 0), // BOTTOM
        vec3(0, 0, 1),  // FRONT
        vec3(0, 0, -1), // BACK
        vec3(-1, 0, 0), // LEFT
        vec3(1, 0, 0)   // RIGHT
    );
    Normal = normals[normalID];

    vec2 uvs[4] = vec2[](
        vec2(0.0, 0.0),
        vec2(1.0, 0.0),
        vec2(1.0, 1.0),
        vec2(0.0, 1.0)
    );
    vTexCoord = uvs[vIdx];

    // Offsets for the 4 corners of a face quad
    // Order: Bottom-Left, Bottom-Right, Top-Right, Top-Left
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

    // Calculate Positions
    vec3 localPos = vec3(float(x), float(y), float(z)) + vOffsets[normalID * 4u + vIdx];
    FragPos = u_ChunkPos + localPos; 
    
    gl_Position = u_ViewProj * vec4(FragPos, 1.0);
}
