#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 UV;

uniform sampler2D uTexture;   // The texture sampler
uniform bool useTexture;      // Toggle to use texture or not
uniform vec3 color;           // solid color

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 lightColor = vec3(1.0, 1.0, 0.9);

    // Ambient lighting
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Calculate final lighting result
    vec3 lighting = ambient + diffuse + specular;

    // Sample texture color or fallback to solid color
    vec3 objectColor = useTexture ? texture(uTexture, UV).rgb : color;

    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}
