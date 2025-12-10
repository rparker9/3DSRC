#version 330

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 finalColor;

// Light
uniform vec3 u_lightDir;
uniform vec3 u_lightColor;
uniform vec3 u_ambientColor;
uniform vec3 u_viewPos;

// Shadow
uniform sampler2D u_shadowMap;
uniform mat4 u_lightSpaceMatrix;

// Raylib material
uniform sampler2D texture0;
uniform vec4 colDiffuse;

float ComputeShadowFactor(vec3 worldPos, vec3 N, vec3 L)
{
    // Transform fragment position to light space
    vec4 fragPosLightSpace = u_lightSpaceMatrix * vec4(worldPos, 1.0);
    
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range (NDC to texture coordinates)
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if fragment is outside light frustum
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z > 1.0)
    {
        return 0.0; // No shadow outside light frustum
    }
    
    // Get depth from shadow map (stored in R channel)
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Calculate bias to prevent shadow acne - increased and based on surface angle
    float bias = max(0.01 * (1.0 - dot(N, L)), 0.005);
    
    // PCF (Percentage Closer Filtering) for softer edges and less artifacts
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(u_shadowMap, 0));
    
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    // Normalize the interpolated normal
    vec3 N = normalize(fragNormal);
    
    // Light direction (negated because u_lightDir points towards the light)
    vec3 L = normalize(-u_lightDir);
    
    // Diffuse lighting
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = u_lightColor * NdotL;
    
    // Specular lighting (Blinn-Phong)
    vec3 V = normalize(u_viewPos - fragPos);
    vec3 H = normalize(L + V);
    float NdotH = max(dot(N, H), 0.0);
    float spec = pow(NdotH, 32.0);
    vec3 specular = u_lightColor * spec * 0.3;
    
    // Sample texture
    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse;
    
    // Calculate shadow factor
    float shadowFactor = ComputeShadowFactor(fragPos, N, L);
    
    // Combine lighting components
    vec3 ambient = u_ambientColor;
    vec3 lighting = ambient + (1.0 - shadowFactor) * (diffuse + specular);
    
    // Clamp to valid range
    lighting = clamp(lighting, 0.0, 2.0);
    
    // Final color
    finalColor = vec4(lighting * texColor.rgb, texColor.a);
}