// DashSkill.h
#pragma once
#include "Skill.h"
#include "Player.h"
#include <SDL3/SDL.h>

class DashSkill : public Skill {
public:
    void activate(Player* player) override;

private:
    const float dashDistance = 100.0f;
};
