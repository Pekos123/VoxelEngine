#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos; 
in float vAO;

uniform vec3 lightPos;  
uniform vec3 viewPos;  
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- 1. GLOBAL SUN LIGHT ---
    vec3 sunDir = normalize(vec3(0.5, 1.0, 0.3)); 
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
    // Increase this slightly so the "unlit" areas aren't pitch black
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * sunColor; 

    // --- 4. SPECULAR (Optional for Voxel) ---
    // Most voxel games skip specular because dirt/grass isn't shiny
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor * attenuation; 

    // --- 5. COMBINE ---
    // Add sun and point light together, then multiply by color and AO
    vec3 lighting = (ambient + sunDiffuse + pointDiffuse + specular);
    vec3 result = lighting * objectColor;
    
    // Apply Ambient Occlusion at the very end
    // Use mix to ensure AO doesn't turn the block 100% black in corners
    float aoFactor = mix(0.4, 1.0, vAO); 
    result *= aoFactor;

    FragColor = vec4(result, 1.0);
}