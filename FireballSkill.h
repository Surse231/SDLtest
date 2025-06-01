#pragma once
#include <SDL3/SDL.h>
#include "Player.h"
#include "Skill.h"
#include "Camera.h"

class FireballSkill : public Skill {
public:
    FireballSkill();

    void activate(Player* player) override;
    void update(Player* player) override;
    void render(SDL_Renderer* renderer, Camera* camera);

private:
    bool active;
    SDL_FRect fireballRect;
    float fireballSpeed;
    float fireballDirection;

    static constexpr float WIDTH = 20.0f;
    static constexpr float HEIGHT = 20.0f;
};
