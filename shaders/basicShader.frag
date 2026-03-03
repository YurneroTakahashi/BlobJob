#version 430 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 glPos;
uniform vec4 ourColor;
void main()
{
    FragColor = ourColor;

}
