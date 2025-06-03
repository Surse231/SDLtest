#pragma once
#include <SDL3/SDL.h>      // для SDL_Renderer и SDL_FRect
#include "Camera.h"        // твой класс Camera, который у тебя в проекте

class Dummy {
public:
    Dummy(SDL_Renderer* renderer, float x, float y);
    ~Dummy();

    void render(SDL_Renderer* renderer, Camera* camera);
    SDL_FRect getRect() const;
    void takeDamage(int amount);

    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;  // Текстура манекена
    SDL_FRect rect;
    int health;
    int maxHealth;
};
