#pragma once
#include "Skill.h"
#include "Camera.h"
    
class DashSkill : public Skill {
public:
    DashSkill();
    void activate(Player* player) override;
    void update(Player* player) override;
    void render(SDL_Renderer*, Camera*) override {} // Пусто, если ничего не рисуешь

private:
    bool isDashing = false;
    float dashTime = 0.0f;
    float dashSpeed = 1500.0f;      // скорость рывка в пикселях в секунду
    float dashDuration = 200.0f;   // длительность рывка в миллисекундах
    int direction = 1;
    Uint64 lastTime = 0;
};



