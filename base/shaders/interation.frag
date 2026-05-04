#version 460 core

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec3 v_LightDir;
layout(location = 2) in vec3 v_ViewDir;

layout(binding = 0) sampler2D u_DiffuseMap;
layout(binding = 1) sampler2D u_NormalMap;
layout(binding = 2) sampler2D u_SpecularMap;
layout(binding = 3) sampler2D u_AttenuationMap; // A famosa "light falloff" do Doom 3

layout(location = 0) out vec4 o_Color;

void main() {
    // Normal vinda da textura (Normal Map)
    vec3 normal = texture(u_NormalMap, v_TexCoord).xyz * 2.0 - 1.0;
    normal = normalize(normal);

    vec3 L = normalize(v_LightDir);
    vec3 V = normalize(v_ViewDir);
    vec3 H = normalize(L + V);

    // Componentes de Iluminação
    float diff = max(dot(normal, L), 0.0);
    float spec = pow(max(dot(normal, H), 0.0), 16.0); // Brilho fixo ou do Specular Map

    // Amostragem das texturas
    vec4 diffuseColor  = texture(u_DiffuseMap, v_TexCoord);
    vec4 specularColor = texture(u_SpecularMap, v_TexCoord);
    
    // Atenuação (Doom 3 usa uma textura de projeção 1D ou 2D para o decaimento)
    // Aqui simplificamos, mas você deve manter a lógica do interaction.vfp original
    float attenuation = 1.0 / dot(v_LightDir, v_LightDir); 

    vec3 finalDiffuse  = diff * diffuseColor.rgb;
    vec3 finalSpecular = spec * specularColor.rgb;

    o_Color = vec4((finalDiffuse + finalSpecular) * attenuation, diffuseColor.a);
}