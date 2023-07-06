#version 410 core
layout(points) in;
layout(points, max_vertices = 1) out;


in VS_OUT {
    vec3 v_vertexColor1;
    vec3 v_vertexColor2;
    vec2 v_vertexTexture1;
    vec2 v_vertexTexture2;
    vec3 v_vertexNormal;
    mat4 rectBasis;
} gs_in[];

uniform mat4 u_Projection;
uniform mat4 u_WorldIT;
uniform mat4 u_World;
uniform mat4 u_ModelMatrix;



out vec3 fColor1;
out vec3 fColor2;
out vec2 texCoord1;
out vec2 texCoord2;



void main()
{
   
    fColor1 = gs_in[0].v_vertexColor1;
    fColor2 = gs_in[0].v_vertexColor2;
    
    texCoord1 = gs_in[0].v_vertexTexture1;
    texCoord2 = gs_in[0].v_vertexTexture2;
    
    gl_PointSize = 10.0f;
    
    vec3 p = gl_in[0].gl_Position.xyz;
    float w = gl_in[0].gl_Position.w;
    
    vec4 convertBacktoWorld = u_World * vec4(gl_in[0].gl_Position.xyz, 0.0f);
    
    gl_Position = u_Projection * vec4(convertBacktoWorld.xyz, 1.0f);
    //gl_Position = u_WorldIT * u_Projection * vec4(gl_in[0].gl_Position.xyz, 1.0f);
    EmitVertex();
    
    EndPrimitive();
}
