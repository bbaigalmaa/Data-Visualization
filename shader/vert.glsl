#version 410 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColor1;
layout(location=2) in vec3 vertexColor2;
layout(location=3) in vec2 vertexTexture1;
layout(location=4) in vec2 vertexTexture2;
layout(location=5) in vec3 vertexNormal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection; //will use a perspective projection
uniform mat4 u_World;
uniform mat4 u_WorldIT;

out VS_OUT {
    vec3 v_vertexColor1;
    vec3 v_vertexColor2;
    vec2 v_vertexTexture1;
    vec2 v_vertexTexture2;
    vec3 v_vertexNormal;
    mat4 rectBasis;
} vs_out;



void main()
{
    vs_out.v_vertexColor1 = vertexColor1;
    vs_out.v_vertexColor2 = vertexColor2;
    vs_out.v_vertexTexture1 = vertexTexture1;
    vs_out.v_vertexTexture2 = vertexTexture2;
    vs_out.v_vertexNormal = vertexNormal;
    
   
    mat4 w = u_World;
    mat4 wIT = u_WorldIT;
    mat4 model = u_ModelMatrix;
    mat4 proj = u_Projection;
    
    vec3 yaxis = vec3(0.0f, 1.0f, 0.0f);
    vec3 basis1    = normalize(cross(yaxis, vertexNormal));
    vec3 basis2   = normalize(cross(vertexNormal, basis1));
    
    
    vs_out.rectBasis[0] = vec4(basis1, position.x);
    vs_out.rectBasis[1] = vec4(normalize(vertexNormal), position.y);
    vs_out.rectBasis[2] = vec4(basis2, position.z);
    vs_out.rectBasis[3] = vec4(vec3(0.0f), 1.0f);
    
    vs_out.rectBasis = inverse(transpose(vs_out.rectBasis));
    
    vec4 posRectBasis = vs_out.rectBasis * vec4(position, 0);
    vec4 newPosition = u_Projection * vec4(posRectBasis.xyz, 1);
    //vec4 newPosition = vec4(position, 1.0f);
    
 	gl_Position = newPosition;  //Do not forget 'w'
    
}
