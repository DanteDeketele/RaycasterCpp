#version 330 core

layout(location = 0) in vec3 aPos;
out vec2 TexCoords;

void main()
{
    TexCoords = (aPos.xy + 1.0) / 2.0; // Convert from [-1, 1] range to [0, 1] range
    gl_Position = vec4(aPos, 1.0);
}
