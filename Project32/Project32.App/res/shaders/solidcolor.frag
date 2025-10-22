#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool useTexture;
uniform sampler2D uTexture;

void main() {
    // Ambient lighting - ensures minimum brightness
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * vec3(1.0);
    
    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0);
    
    // Specular lighting (optional, adds shine)
    float specularStrength = 0.3;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0);
    
    // Combine lighting
    vec3 lighting = ambient + diffuse + specular;
    
    // Apply texture or color
    vec3 objectColor;
    if (useTexture) {
        objectColor = texture(uTexture, TexCoords).rgb * color;
    } else {
        objectColor = color;
    }
    
    // Final color with lighting
    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}