#version 460 core

layout(location = 0) in vec3 attr_Position;
layout(location = 1) in vec2 attr_TexCoord;
layout(location = 2) in vec3 attr_Normal;
layout(location = 3) in vec3 attr_Tangent;
layout(location = 4) in vec3 attr_Binormal;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec3 v_LightDir; // Em Tangent Space
layout(location = 2) out vec3 v_ViewDir;  // Em Tangent Space

layout(std140, binding = 0) uniform SceneData {
    mat4 u_ProjectionMatrix;
    mat4 u_ModelViewMatrix;
    vec3 u_LightPos; // Posição da luz em Eye Space
    vec3 u_ViewPos;  // Posição da câmera (geralmente 0,0,0 em Eye Space)
};

void main() {
    vec4 eyePos = u_ModelViewMatrix * vec4(attr_Position, 1.0);
    
    // Construção da Matriz TBN (Eye Space)
    mat3 normalMatrix = mat3(u_ModelViewMatrix);
    vec3 T = normalize(normalMatrix * attr_Tangent);
    vec3 B = normalize(normalMatrix * attr_Binormal);
    vec3 N = normalize(normalMatrix * attr_Normal);
    mat3 invTBN = transpose(mat3(T, B, N));

    // Vetores de Luz e Visão transformados para Tangent Space
    v_LightDir = invTBN * (u_LightPos - eyePos.xyz);
    v_ViewDir  = invTBN * (u_ViewPos - eyePos.xyz);
    
    v_TexCoord = attr_TexCoord;
    gl_Position = u_ProjectionMatrix * eyePos;
}