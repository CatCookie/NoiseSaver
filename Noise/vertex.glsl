#version 400

in vec2 texCoord;
in vec2 position;
in vec3 color;

out vec3 Color;
out vec2 TexCoord;

void main()
{
	TexCoord = texCoord;
	Color = color;
    gl_Position = vec4(position, 0.0, 1.0);
}