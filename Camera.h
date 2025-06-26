#pragma once
#include <SDL3/SDL.h>

class Camera {
    SDL_FRect view;
public:
    Camera(float screenWidth, float screenHeight);
    ~Camera();

    // Добавляем параметры размеров тайлов
    void update(const SDL_FRect& player, float mapWidth, float mapHeight, int tileWidth, int tileHeight);

    SDL_FRect getView() const { return camera; }
    SDL_FRect apply(const SDL_FRect& worldRect) const;

    float zoom = 3.0f;
    float getX() const { return view.x; }
    float getY() const { return view.y; }

private:
    SDL_FRect camera;
    float screenW, screenH;
};