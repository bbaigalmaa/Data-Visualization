#version 410 core
//in vec3 v_vertexColors;
in vec3 fColor1;
in vec3 fColor2;
in vec2 texCoord1;
in vec2 texCoord2;

uniform sampler2D u_sampler2D; //for texture color

uniform int u_Tex1;
uniform int u_Tex2;
uniform int u_Color1;
uniform int u_Color2;


out vec4 color;

void main()
{
    if(u_Color1 == 1)
        color = vec4(fColor1, 1.0f);
    else if(u_Color2 == 1)
        color = vec4(fColor2, 1.0f);
    else if(u_Tex1 == 1)
        color = texture(u_sampler2D, texCoord1);
    else if(u_Tex2 == 1)
        color = texture(u_sampler2D, texCoord2);
    else
        color = vec4(fColor1, 1.0f);
}

