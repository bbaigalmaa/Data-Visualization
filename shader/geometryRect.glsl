#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;
//triangle_strip

in vec3 normal[];
in vec3 basis1[];
in vec3 basis2[];
in mat4 rectBasis[];

in VS_OUT {
    vec3 v_vertexColor1;
    vec3 v_vertexColor2;
    vec2 v_vertexTexture1;
    vec2 v_vertexTexture2;
    vec3 v_vertexNormal;
} gs_in[];

uniform mat4 u_Projection;
uniform mat4 u_World;
uniform mat4 u_WorldIT;
uniform float u_rectSize;

out vec3 fColor1;
out vec3 fColor2;
out vec2 leftTop;
out vec2 leftBottom;
out vec2 rightBottom;
out vec2 rightTop;



void main()
{
    fColor1 = gs_in[0].v_vertexColor1;
    fColor2 = gs_in[0].v_vertexColor2;
    
    vec3 P = gl_in[0].gl_Position.xyz;
    float w = gl_in[0].gl_Position.w;
    
    leftTop = vec2(-u_rectSize, u_rectSize);
    leftBottom = vec2(-u_rectSize, -u_rectSize);
    rightBottom = vec2(u_rectSize, -u_rectSize);
    rightTop = vec2(u_rectSize, u_rectSize);
    
    
    vec4 p1 = gl_in[0].gl_Position + rectBasis[0] * vec4(vec3(leftTop.x, 0.0f, leftTop.y), 0.0f);
    vec4 p2 = gl_in[0].gl_Position + rectBasis[0] * vec4(vec3(leftBottom.x, 0.0f, leftBottom.y), 0.0f);
    vec4 p3 = gl_in[0].gl_Position + rectBasis[0] * vec4(vec3(rightBottom.x, 0.0f, rightBottom.y), 0.0f);
    vec4 p4 = gl_in[0].gl_Position + rectBasis[0] * vec4(vec3(rightTop.x, 0.0f, rightTop.y), 0.0f);
    
    
    gl_Position = u_Projection * vec4(p1.xyz, 1.0f);
    EmitVertex();

    gl_Position = u_Projection * vec4(p2.xyz, 1.0f);
    EmitVertex();

    gl_Position = u_Projection * vec4(p3.xyz, 1.0f);
    EmitVertex();

    gl_Position = u_Projection * vec4(p4.xyz, 1.0f);
    EmitVertex();

    gl_Position = u_Projection * vec4(p1.xyz, 1.0f);
    EmitVertex();

    gl_Position = u_Projection * vec4(p3.xyz, 1.0f);
    EmitVertex();
        
        EndPrimitive();
    
}
