#version 460 core

layout(triangles_adjacency) in;          // Recebe 6 vértices
layout(triangle_strip, max_vertices = 18) out; // Cospe o volume lateral e tampas

uniform vec3 u_LightPos; // Em Object Space
uniform mat4 u_VP;       // View-Projection Matrix

// Função auxiliar para verificar se um triângulo encara a luz
bool FacesLight(vec3 a, vec3 b, vec3 c) {
    vec3 normal = cross(b - a, c - a);
    vec3 lightDir = u_LightPos - a;
    return dot(normal, lightDir) > 0.0;
}

void EmitEdgeQuad(vec3 start, vec3 end) {
    // Projeta os vértices para o "infinito" (W=0) ou uma distância muito grande
    vec3 lightDirStart = normalize(start - u_LightPos);
    vec3 lightDirEnd = normalize(end - u_LightPos);

    gl_Position = u_VP * vec4(start, 1.0); EmitVertex();
    gl_Position = u_VP * vec4(start + lightDirStart * 1000.0, 1.0); EmitVertex();
    gl_Position = u_VP * vec4(end, 1.0); EmitVertex();
    gl_Position = u_VP * vec4(end + lightDirEnd * 1000.0, 1.0); EmitVertex();
    EndPrimitive();
}

void main() {
    // Vértices do triângulo atual (0, 2, 4 são os principais no modo adjacency)
    vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;
    vec3 v3 = gl_in[3].gl_Position.xyz;
    vec3 v4 = gl_in[4].gl_Position.xyz;
    vec3 v5 = gl_in[5].gl_Position.xyz;

    if (FacesLight(v0, v2, v4)) {
        // Se a face vizinha NÃO encara a luz, a aresta comum é uma silhueta
        if (!FacesLight(v0, v1, v2)) EmitEdgeQuad(v0, v2);
        if (!FacesLight(v2, v3, v4)) EmitEdgeQuad(v2, v4);
        if (!FacesLight(v4, v5, v0)) EmitEdgeQuad(v4, v0);
        
        // Opcional: Renderizar a "tampa" frontal e traseira para fechar o volume (Z-Fail/Carmack's Reverse)
    }
}