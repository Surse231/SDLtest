#include "FireballSkill.h"

FireballSkill::FireballSkill()
    : active(false),
    fireballRect{ 0, 0, WIDTH, HEIGHT },
    fireballSpeed(600.0f), // скорость пикселей в секунду
    fireballDirection(1.0f)
{
}

void FireballSkill::activate(Player* player) {
    if (active) return;

    SDL_FRect playerRect = player->getDest();

    float fireballX = playerRect.x + (playerRect.w / 2) - (WIDTH / 2);
    float fireballY = playerRect.y + (playerRect.h / 2) - (HEIGHT / 2);

    fireballRect = { fireballX, fireballY, WIDTH, HEIGHT };
    fireballDirection = player->isFlipped() ? -1.0f : 1.0f;

    traveledDistance = 0.0f; // 🔥 сбрасываем
    active = true;
}


void FireballSkill::update(Player* player, float deltaTime)
{
    update(player, deltaTime, 5000.0f); // или возьми real mapWidth из player
}

void FireballSkill::update(Player* player, float deltaTime, float mapWidth)
{
    if (!active) return;

    float dx = fireballSpeed * fireballDirection * deltaTime;
    fireballRect.x += dx;

    traveledDistance += std::abs(dx);  // 🔥 суммируем дистанцию

    // Прекращаем, если пролетели слишком много
    if (traveledDistance >= maxDistance || fireballRect.x < 0 || fireballRect.x > mapWidth) {
        active = false;
    }
}





void FireballSkill::render(SDL_Renderer* renderer, Camera* camera) {
    if (!active) return;

    SDL_FRect screenRect = camera->apply(fireballRect);

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderFillRect(renderer, &screenRect);
}
