#pragma once
#include "Skill.h"
#include "Camera.h"
    
class DashSkill : public Skill {
public:
    void activate(Player* player) override;
    void update(Player* player) override;
    void render(SDL_Renderer* renderer, Camera* camera) override;

};
