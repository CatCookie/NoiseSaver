#version 400

in vec3 Color;
in vec2 TexCoord;
uniform sampler2D tex;

out vec4 outColor;



void main()
{
    outColor =  texture2D(tex, TexCoord);  //*vec4(Color, 1.0);
}