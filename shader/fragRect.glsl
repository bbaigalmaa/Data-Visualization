#version 410 core
in vec3 fColor1;
in vec3 fColor2;
in vec2 leftTop;
in vec2 leftBottom;
in vec2 rightBottom;
in vec2 rightTop;



out vec4 color;

uniform sampler2D u_sampler2D; //for texture color
uniform int u_Color1;
uniform int u_Color2;
uniform int u_TexLeftTop;
uniform int u_TexLeftBottom;
uniform int u_TexRightTop;
uniform int u_TexRightBottom;

void main()
{
    //color = vec4(fColor, 1.0f);
    vec2 centerOftexture = vec2(0.5f, 0.5f);

    
    
    if(u_Color1 == 1)
        color = vec4(fColor1, 1.0f);
    else if(u_Color2 == 1)
        color = vec4(fColor2, 1.0f);
    else if(u_TexLeftTop == 1)
        color = texture(u_sampler2D, leftTop + centerOftexture);
    else if(u_TexLeftBottom == 1)
        color = texture(u_sampler2D, leftBottom + centerOftexture);
    else if(u_TexRightTop == 1)
        color = texture(u_sampler2D, rightTop + centerOftexture);
    else if(u_TexRightBottom == 1)
        color = texture(u_sampler2D, rightBottom + centerOftexture);
//    else
//        color = vec4(fColor1, 1.0f);
}

