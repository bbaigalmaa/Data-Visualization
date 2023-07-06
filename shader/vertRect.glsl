#version 410 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 vertexColor1;
layout(location=2) in vec3 vertexColor2;
layout(location=3) in vec2 vertexTexture1;
layout(location=4) in vec2 vertexTexture2;
layout(location=5) in vec3 vertexNormal;

//uniform float u_Offset; // Cannot initialized on CPU becaue it should pass to GPU


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
} vs_out;

out vec3 normal;
out vec3 basis1;
out vec3 basis2;
out mat4 rectBasis;

void main()
{
    vs_out.v_vertexColor1 = vertexColor1;
    vs_out.v_vertexColor2 = vertexColor2;
    vs_out.v_vertexTexture1 = vertexTexture1;
    vs_out.v_vertexTexture2 = vertexTexture2;
    vs_out.v_vertexNormal = vertexNormal;
   
    mat4 w      = u_World;
    mat4 wIT    = u_WorldIT;
    mat4 model  = u_ModelMatrix;
    mat4 proj   = u_Projection;
    
    normal = vertexNormal;
    
    
    
    
    vec3 yaxis = vec3(0.0f, 1.0f, 0.0f);
    basis1    = normalize(cross(yaxis, vertexNormal));
    basis2   = normalize(cross(vertexNormal, basis1));
    
    rectBasis[0] = vec4(basis1, position.x);
    rectBasis[1] = vec4(normalize(vertexNormal), position.y);
    rectBasis[2] = vec4(basis2, position.z);
    rectBasis[3] = vec4(vec3(0.0f), 1.0f);
    
    rectBasis = inverse(transpose(rectBasis));
    
    vec4 posRectBasis = rectBasis * vec4(position, 0);
    vec4 newPosition =   u_Projection * vec4(posRectBasis.xyz, 1);
    
    //vec4 newPosition =  u_Projection * vec4(position, 1.0f);
    
 	gl_Position = newPosition;  //Do not forget 'w'
    
}
