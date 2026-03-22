#version 330 core

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 
in vec4 vFragPosLightSpace;
in float vAO;
in vec2 vTexCoord;
flat in uint vTextureID;

uniform sampler2DArray u_Textures;
uniform sampler2D u_ShadowMap;

uniform vec3 lightPos;  
uniform vec3 viewPos;  
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform vec3 sunPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLightSpace as coords)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Check whether current frag pos is in shadow
    // Use dynamic bias based on surface normal and light direction
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(sunPos);
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
    
    // PCF (Percentage Closer Filtering)
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- 1. GLOBAL SUN LIGHT ---
    vec3 sunDir = normalize(sunPos); 
    vec3 sunColor = vec3(1.0, 0.98, 0.9); // Warm sunlight
    
    float sunDiff = max(dot(norm, sunDir), 0.0);
    vec3 sunDiffuse = sunDiff * sunColor;

    // --- 2. LOCAL POINT LIGHT (Torch) ---
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * (distance * distance));

    vec3 lightDir = normalize(lightPos - FragPos);
    float pointDiff = max(dot(norm, lightDir), 0.0);
    vec3 pointDiffuse = (pointDiff * lightColor) * attenuation;

    // --- 3. AMBIENT ---
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * sunColor; 

    // --- 4. SHADOW ---
    float shadow = ShadowCalculation(vFragPosLightSpace);

    // --- 5. TEXTURE ---
    vec4 texColor = texture(u_Textures, vec3(vTexCoord, float(vTextureID)));
    if(texColor.a < 0.1)
        discard;

    // --- 6. COMBINE ---
    // Apply shadow to sun diffuse only
    vec3 lighting = (ambient + (1.0 - shadow) * sunDiffuse + pointDiffuse);
    vec3 result = lighting * texColor.rgb * objectColor;
    
    float aoFactor = mix(0.4, 1.0, vAO); 
    result *= aoFactor;

    FragColor = vec4(result, 1.0);
}