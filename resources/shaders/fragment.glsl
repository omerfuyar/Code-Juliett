#version 330 core

in vec2 oVertUv;
in vec3 oVertPosition;
in vec3 oVertNormal;

uniform vec3 camPosition;
uniform vec3 camRotation;
uniform float camSize;
uniform bool camIsPerspective;

uniform vec4 matBaseColorFactor;
uniform float matMetallicFactor;
uniform float matRoughnessFactor;
uniform vec3 matEmissiveFactor;
uniform sampler2D matBaseColorMap;
uniform bool matHasBaseColorMap;
uniform sampler2D matMetallicRoughnessMap;
uniform bool matHasMetallicRoughnessMap;

out vec4 FragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return nom / (denom + 0.000001);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 N = normalize(oVertNormal);
    vec3 V = normalize(camPosition - oVertPosition);

    vec4 baseColor = matBaseColorFactor;
    if (matHasBaseColorMap) {
        vec4 texColor = texture(matBaseColorMap, oVertUv);
        baseColor *= vec4(pow(texColor.rgb, vec3(2.2)), texColor.a);
    }

    float metallic = matMetallicFactor;
    float roughness = matRoughnessFactor;
    if (matHasMetallicRoughnessMap) {
        vec4 mr = texture(matMetallicRoughnessMap, oVertUv);
        roughness *= mr.g;
        metallic *= mr.b;
    }

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, baseColor.rgb, metallic);

    vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    vec3 H = normalize(V + L);

    vec3 radiance = vec3(5.0);

    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  

    float NdotL = max(dot(N, L), 0.0);        
    vec3 Lo = (kD * baseColor.rgb / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.1) * baseColor.rgb;
    vec3 color = ambient + Lo + matEmissiveFactor;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, baseColor.a);
}
