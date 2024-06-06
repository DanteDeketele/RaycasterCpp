#version 330 core
#extension GL_NV_shader_buffer_load : enable

out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 uResolution;
uniform vec2 uPlayerPos;
uniform float uPlayerAngle;

uniform sampler2D map;
uniform sampler2D textures;
uniform int texturesX;
uniform int texturesY;

uniform sampler2D overlay;
uniform sampler2D skybox;

const float FOV = 2;

bool isAir(int value)
{
    return (value == 0);
}

bool isWall(float x, float y) {
    ivec2 bufferSize = textureSize(map, 0);
    int gridX = int(floor(x));
    int gridY = int(floor(y));
    if (gridX < 0 || gridX >= bufferSize.x || gridY < 0 || gridY >= bufferSize.y) {
        return true;
    }
    float value = texelFetch(map, ivec2(gridY, gridX), 0).r * 255;
    return !isAir((int)value);
}

void main()
{
    vec3 filterColor = vec3(0.817647, 0.747059, 0.660784);

    vec3 overlayColor = texture(overlay, TexCoord).rgb;
    filterColor *= overlayColor.r;

    float screenX = (TexCoord.x - 0.5) * 2.0;
    float rayAngle = uPlayerAngle + atan(screenX, 1.0);

    vec2 rayDir = vec2(cos(rayAngle), sin(rayAngle));
    vec2 rayPos = uPlayerPos;

    vec2 stepSize = abs(vec2(1.0 / rayDir.x, 1.0 / rayDir.y));

    vec2 mapCheck = floor(rayPos);
    vec2 rayLength1D = vec2(0.0);
    vec2 step = vec2(0.0);

    if (rayDir.x < 0) {
        step.x = -1;
        rayLength1D.x = (rayPos.x - mapCheck.x) * stepSize.x;
    } else {
        step.x = 1;
        rayLength1D.x = (mapCheck.x + 1 - rayPos.x) * stepSize.x;
    }

    if (rayDir.y < 0) {
        step.y = -1;
        rayLength1D.y = (rayPos.y - mapCheck.y) * stepSize.y;
    } else {
        step.y = 1;
        rayLength1D.y = (mapCheck.y + 1 - rayPos.y) * stepSize.y;
    }

    float distToWall = 0.0;
    bool hitWall = false;
    bool wallVariant = false;

    while (!hitWall && distToWall < textureSize(map, 0).x) {
        if (rayLength1D.x < rayLength1D.y) {
            mapCheck.x += step.x;
            distToWall = rayLength1D.x;
            rayLength1D.x += stepSize.x;
        } else {
            mapCheck.y += step.y;
            distToWall = rayLength1D.y;
            rayLength1D.y += stepSize.y;
        }

        if (isWall(mapCheck.x, mapCheck.y)) {
            hitWall = true;
        }
    }

    float perpendicularDist = distToWall * cos(rayAngle - uPlayerAngle);
    float wallHeight = (1.0 / perpendicularDist) * (uResolution.y / 2.0) * cos(atan(0.5, perpendicularDist));

    vec3 color;
    if (TexCoord.y < (0.5 - wallHeight / uResolution.y)) {
        color = vec3(0.2, 0.2, 0.2) * (1 - TexCoord.y);
    } else if (TexCoord.y > (0.5 + wallHeight / uResolution.y)) {

        color = texture(skybox, vec2(3*rayAngle / (2 * 3.14159265359), TexCoord.y)).rgb;
        color = vec3(1,1,1) * color.r;
    } else {
        float shade = 1.0 - distToWall / textureSize(map, 0).x;

        // Calculate texture coordinates
        bool isWallFacingNorthSouth = abs(rayDir.y) > abs(rayDir.x);
        bool isWallFacingWestEast = !isWallFacingNorthSouth;

        vec2 texCoord;
        if (isWallFacingWestEast) {
            texCoord.x = fract(rayPos.y + distToWall * sin(rayAngle));
        } else {
            texCoord.x = fract(rayPos.x + distToWall * cos(rayAngle));
        }
        texCoord.y = fract((TexCoord.y - 0.5 + wallHeight) * (uResolution.y / wallHeight) / 2 - 0.5);

        // Determine tile index dynamically
        // Assuming some logic to determine the correct subdivisionIndexX and subdivisionIndexY based on the map position
        int subdivisionIndexX = int(0) % texturesX;
        int subdivisionIndexY = int(18) % texturesY;

        // Calculate the width and height of each subdivision
        float subdivisionWidth = 1.0 / float(texturesX);
        float subdivisionHeight = 1.0 / float(texturesY-1);

        // Calculate the starting UV position of the desired subdivision
        vec2 subdivisionStart = vec2(subdivisionWidth * subdivisionIndexX, subdivisionHeight * subdivisionIndexY);

        // Calculate the UV position within the subdivision
        vec2 subdivisionTexCoord = texCoord * vec2(subdivisionWidth, subdivisionHeight);

        // Calculate the final texCoord by adding the subdivisionStart and subdivisionTexCoord
        vec2 finalTexCoord = subdivisionStart + subdivisionTexCoord;

        // Sample the texture using the finalTexCoord
        vec4 texColor = texture(textures, finalTexCoord);
        color = vec3(shade) * texColor.rgb;
    }

    FragColor = vec4(color * filterColor, 1.0);
}
