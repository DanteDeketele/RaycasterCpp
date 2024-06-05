#version 330 core
#extension GL_NV_shader_buffer_load : enable

out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 uResolution;
uniform vec2 uPlayerPos;
uniform float uPlayerAngle;

const float FOV = 1.0;
const float MAP_SIZE = 8.0;
const int MAP_WIDTH = 8;
const int MAP_HEIGHT = 8;

int map[64] = int[64](
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 1, 0, 0, 1,
    1, 0, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1
);

bool isWall(float x, float y) {
    int gridX = int(floor(x));
    int gridY = int(floor(y));
    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
        return true;
    }
    return map[gridY * MAP_WIDTH + gridX] == 1;
}

void main()
{
    // color that will be used to filter the final image
    vec3 filterColor = vec3(0.717647, 0.647059, 0.560784);

    float screenX = (TexCoord.x - 0.5) * 2.0;
    float rayAngle = uPlayerAngle + screenX * FOV * 0.5;

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

    while (!hitWall && distToWall < MAP_SIZE) {
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
    float wallHeight = (1.0 / perpendicularDist) * (uResolution.y / 2.0);

    vec3 color;
    if (TexCoord.y < (0.5 - wallHeight / uResolution.y)) {
        color = vec3(0.2, 0.2, 0.2);
    } else if (TexCoord.y > (0.5 + wallHeight / uResolution.y)) {
        color = vec3(0.7, 0.7, 0.7);
    } else {
        float shade = 1.0 - distToWall / MAP_SIZE;
        color = vec3(shade);
    }

    FragColor = vec4(color * filterColor, 1.0);
}