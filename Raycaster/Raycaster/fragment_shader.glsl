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

const float FOV = 1;

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
    bool wallVertical = false;

    while (!hitWall && distToWall < textureSize(map, 0).x) {
        if (rayLength1D.x < rayLength1D.y) {
            mapCheck.x += step.x;
            distToWall = rayLength1D.x;
            rayLength1D.x += stepSize.x;
            wallVertical = true;
        } else {
            mapCheck.y += step.y;
            distToWall = rayLength1D.y;
            rayLength1D.y += stepSize.y;
            wallVertical = false;
        }

        if (isWall(mapCheck.x, mapCheck.y)) {
            hitWall = true;
        }
    }

    float perpendicularDist = distToWall * cos(rayAngle - uPlayerAngle);
    float wallHeight = (1.0 / perpendicularDist) * (uResolution.y / 2.0) * cos(atan(0.5, perpendicularDist));

    vec3 color;
    if (TexCoord.y < (0.5 - wallHeight / uResolution.y)) {
        color = vec3(1.0, 1.0, 1.0);
    } else if (TexCoord.y > (0.5 + wallHeight / uResolution.y)) {
        color = texture(skybox, vec2(3 * rayAngle / (2 * 3.14159265359), TexCoord.y)).rgb;
        color = vec3(1, 1, 1) * color.r;
    } else {
        float shade = 1.0 - distToWall / textureSize(map, 0).x / 2;

        vec2 texCoord;
        if (wallVertical) {
            texCoord.x = fract(rayPos.y + distToWall * rayDir.y);
        } else {
            texCoord.x = fract(rayPos.x + distToWall * rayDir.x);
        }
        texCoord.y = fract((TexCoord.y - 0.5 + wallHeight) * (uResolution.y / wallHeight) / 2 - 0.5);

        int subdivisionIndexX = int(0) % texturesX;
        int subdivisionIndexY = int(18) % texturesY;

        float subdivisionWidth = 1.0 / float(texturesX);
        float subdivisionHeight = 1.0 / float(texturesY - 1);
        vec2 subdivisionStart = vec2(subdivisionWidth * subdivisionIndexX, subdivisionHeight * subdivisionIndexY);
        vec2 subdivisionTexCoord = texCoord * vec2(subdivisionWidth, subdivisionHeight);
        vec2 finalTexCoord = subdivisionStart + subdivisionTexCoord;
        vec4 texColor = texture(textures, finalTexCoord);
        color = vec3(shade) * texColor.rgb;
    }

    // Floor rendering
    if (TexCoord.y < 0.5 - wallHeight / uResolution.y) {
        // Calculate the distance to the floor
        float floorDist = (0.5 * uResolution.y) / ((TexCoord.y - 0.5) *cos(rayAngle - uPlayerAngle));

        // Calculate the position of the floor intersection
        vec2 floorPos = uPlayerPos + rayDir * floorDist;

        // Calculate the texture coordinates for the floor
        vec2 floorTexCoords = floorPos/uResolution.y - vec2(floor(floorPos.x), floor(floorPos.y));

        // Get the texture color for the floor
        vec4 floorColor = texture(textures, - uPlayerPos + floorTexCoords);

        // Mix the floor color with the wall color
        color *= floorColor.r * (1 -TexCoord.y);
    }


    FragColor = vec4(color * filterColor, 1.0);
}
