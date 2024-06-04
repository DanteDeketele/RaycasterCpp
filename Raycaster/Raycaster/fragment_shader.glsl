#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform int verticalPixels; // Number of vertical pixels
uniform int horizontalPixels; // Number of horizontal pixels

void main()
{
    // Calculate the pixel position within the grid
    int pixelX = int(TexCoords.x * float(horizontalPixels));
    int pixelY = int(TexCoords.y * float(verticalPixels));

    // Create a checkerboard pattern based on the pixel position
    int checker = (pixelX + pixelY) % 2;

    // Set color based on the checker value
    if (checker == 0)
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Black
    else
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White
}
