#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D map;

void main()
{
    int value = int(texture(map, TexCoord).r);
    if (value == 1) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White for walls
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black for empty spaces
    }
}
