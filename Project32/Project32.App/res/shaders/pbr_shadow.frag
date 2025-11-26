#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 Color;
    vec4 FragPosLightSpace;
    mat3 TBN;
    vec3 Normal;
} fs_in;

// Material properties
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

// Textures
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D shadowMap;

// Texture flags
uniform bool useAlbedoMap;
uniform bool useNormalMap;
uniform bool useMetallicMap;
uniform bool useRoughnessMap;
uniform bool useAOMap;

// Simple lighting (backward compatible)
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 color;

const float PI = 3.14159265359;

// Shadow calculation
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

// Normal Distribution Function
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

// Geometry Function
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

// Fresnel Equation
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // Sample material properties
    vec3 albedoValue = useAlbedoMap ? 
        pow(texture(albedoMap, fs_in.TexCoord).rgb, vec3(2.2)) : 
        albedo * color * fs_in.Color;
    
    float metallicValue = useMetallicMap ? 
        texture(metallicMap, fs_in.TexCoord).r : 
        metallic;
    
    float roughnessValue = useRoughnessMap ? 
        texture(roughnessMap, fs_in.TexCoord).r : 
        roughness;
    
    float aoValue = useAOMap ? 
        texture(aoMap, fs_in.TexCoord).r : 
        ao;
    
    // Sample normal map and transform to world space
    vec3 N;
    if(useNormalMap) {
        N = texture(normalMap, fs_in.TexCoord).rgb;
        N = N * 2.0 - 1.0;
        N = normalize(fs_in.TBN * N);
    } else {
        N = normalize(fs_in.Normal);
    }
    
    vec3 V = normalize(viewPos - fs_in.FragPos);
    
    // Calculate reflectance at normal incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoValue, metallicValue);
    
    // Single light calculation
    vec3 L = normalize(lightPos - fs_in.FragPos);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - fs_in.FragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = vec3(3.0) * attenuation; // White light with intensity 3.0
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughnessValue);
    float G = GeometrySmith(N, V, L, roughnessValue);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallicValue;
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    
    // Shadow calculation
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, N, L);
    
    vec3 Lo = (kD * albedoValue / PI + specular) * radiance * NdotL * (1.0 - shadow);
    
    // Ambient lighting
    vec3 ambient = vec3(0.03) * albedoValue * aoValue;
    vec3 finalColor = ambient + Lo;
    
    // HDR tonemapping
    finalColor = finalColor / (finalColor + vec3(1.0));
    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    FragColor = vec4(finalColor, 1.0);
}