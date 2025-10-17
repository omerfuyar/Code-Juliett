#version 330 core

in vec2 oVertUv;
in vec3 oVertPosition;
in vec3 oVertNormal;

uniform vec3 camPosition;
uniform vec3 camRotation;

uniform sampler2D matDiffuseMap;
uniform bool matHasDiffuseMap;

uniform vec3 matAmbientColor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matEmissiveColor;
uniform float matSpecularExponent;
uniform float matDissolve;

out vec4 FragColor;

void main()
{
    // Normalize the fragment normal
    vec3 normal = normalize(oVertNormal);
    
    // Hard-coded light direction (could be passed as uniform)
    vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
    
    // Hard-coded camera position (could be passed as uniform)
    vec3 viewDir = normalize(camPosition - oVertPosition);
    
    // Calculate the reflection direction
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // Calculate ambient component
    vec3 ambient = matAmbientColor * 1.0;
    
    // Calculate diffuse component
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * matDiffuseColor;
    
    // Calculate specular component
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), matSpecularExponent);
    vec3 specular = spec * matSpecularColor;
    
    // Get base color from texture or material
    vec4 baseColor;
    if (matHasDiffuseMap) {
        baseColor = texture(matDiffuseMap, oVertUv);
    } else {
        baseColor = vec4(matDiffuseColor, 1.0);
    }
    
    // Combine all lighting components
    vec3 result = (ambient + diffuse) * baseColor.rgb + specular;
    
    // Apply material opacity
    float alpha = matHasDiffuseMap ? baseColor.a : matDissolve;
    FragColor = vec4(result, alpha);

    // Display normals as colors (normalized from -1,1 to 0,1 range)
    //FragColor = vec4(normal * 0.5 + 0.5, 1.0);
}
