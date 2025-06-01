#pragma once
#include <SDL3/SDL.h>
#include "Camera.h"

class Player;

class Skill {
public:
    virtual ~Skill() = default;

    virtual void activate(Player* player) = 0;
    virtual void update(Player* player) = 0;
    virtual void render(SDL_Renderer* renderer, Camera* camera) = 0;
};
