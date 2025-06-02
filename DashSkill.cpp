#include "DashSkill.h"
#include "Player.h"

void DashSkill::update(Player* player) {
    if (!isDashing) return;

    Uint64 now = SDL_GetTicks();
    float delta = static_cast<float>(now - lastTime);
    lastTime = now;

    dashTime += delta;

    // Смещение за этот кадр
    float dx = static_cast<float>(direction) * dashSpeed * (delta / 1000.0f);

    SDL_FRect dest = player->gedDest();
    dest.x += dx;
    player->setPosition(dest.x, dest.y);

    if (dashTime >= dashDuration) {
        isDashing = false;
    }
}



DashSkill::DashSkill()
    : isDashing(false), dashTime(0.0f), dashDuration(200.0f), dashSpeed(1000.0f), direction(1), lastTime(0)
{
}


void DashSkill::activate(Player* player) {
    Uint64 now = SDL_GetTicks();
    if (now - lastUsedTime < cooldown || isDashing)
        return;

    isDashing = true;
    dashTime = 0;
    lastTime = now;   // сброс времени для плавного обновления
    direction = player->getDirection();
    lastUsedTime = now;
}







