#include "Camera.h"
#include <cmath>

Camera::Camera(float screenWidth, float screenHeight)
    : screenW(screenWidth), screenH(screenHeight)
{
    camera = { 0.0f, 0.0f, screenWidth, screenHeight };
}

Camera::~Camera() {}

void Camera::update(const SDL_FRect& player, float mapWidth, float mapHeight) {
    float smoothing = 0.08f;

    float targetX = player.x + player.w / 2 - camera.w / 2;
    float targetY = player.y + player.h / 2 - camera.h / 2;

    float maxX = mapWidth * 32.0f - camera.w;
    float maxY = mapHeight * 32.0f - camera.h;

    camera.x += (targetX - camera.x) * smoothing;
    camera.y += (targetY - camera.y) * smoothing;

    if (camera.x < 0) camera.x = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.x > maxX) camera.x = maxX;
    if (camera.y > maxY) camera.y = maxY;
}

SDL_FRect Camera::getView() const {
    return camera;
}

SDL_FRect Camera::apply(const SDL_FRect& worldRect) const {
    return SDL_FRect{
        worldRect.x - camera.x,
        worldRect.y - camera.y,
        worldRect.w,
        worldRect.h
    };
}
