#version 430 core
out vec4 FragColor;
in vec2 TexCoord;
in vec3 glPos;
void main()
{
    FragColor = vec4(glPos,1.0);
}
