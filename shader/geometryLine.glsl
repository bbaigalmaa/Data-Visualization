#version 410 core
layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 normal[];
in mat4 rectBasis[];
in vec3 basis1[];
in vec3 basis2[];

uniform float u_normalLength;
uniform mat4 u_Projection;
uniform mat4 u_WorldIT;
uniform mat4 u_World;

out vec3 fColor;

void main()
{
    vec3 P = gl_in[0].gl_Position.xyz;
    float w = gl_in[0].gl_Position.w;
    fColor = vec3(1.0f, 0.0f, 0.0f);
    vec4 n = rectBasis[0] * vec4(normal[0], 0.0f);
    vec4 b2 = rectBasis[0] * vec4(basis2[0], 1.0f);
    vec4 b1 = rectBasis[0] * vec4(basis1[0], 1.0f);
    
   //vec4 nRectBais = rectBasis[0] * vec4(P, 0.0f);

    vec4 pWorld = u_World * vec4(P, 0.0f);
    vec4 nWorld = u_World * vec4(P + n.xyz*u_normalLength, 0.0f);

    gl_Position = u_Projection * vec4(pWorld.xyz, 1.0f);
    EmitVertex();
    gl_Position = u_Projection * vec4(nWorld.xyz, 1.0f);
    EmitVertex();
    
    
    EndPrimitive();
}
