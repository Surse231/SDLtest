#include "DashSkill.h"
#include "Player.h"

void DashSkill::update(Player* player) {
    if (!isDashing) return;

    Uint64 now = SDL_GetTicks();
    float delta = static_cast<float>(now - lastTime);
    lastTime = now;

    dashTime += delta;

    float dx = dashSpeed * (delta / 1000.0f); // в пикселях
    if (direction == -1)
        dx = -dx;

    SDL_FRect dest = player->gedDest();
    dest.x += dx;
    player->setPosition(dest.x, dest.y);

    if (dashTime >= dashDuration) {
        isDashing = false;
    }
}


DashSkill::DashSkill()
    : isDashing(false), dashTime(0.0f), dashDuration(200.0f), dashSpeed(1500.0f), direction(1), lastTime(0)
{
}


void DashSkill::activate(Player* player) {
    Uint64 now = SDL_GetTicks();
    if (now - player->getLastDashTime() < 2000) return;

    isDashing = true;
    dashTime = 0.0f;
    lastTime = SDL_GetTicks();
    direction = player->isFlipped() ? -1 : 1;

    player->setSkillActive(true);
    player->setLastDashTime(now);
}


