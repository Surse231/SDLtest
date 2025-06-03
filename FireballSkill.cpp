#include "FireballSkill.h"

FireballSkill::FireballSkill()
    : active(false),
    fireballRect{ 0, 0, WIDTH, HEIGHT },
    fireballSpeed(600.0f), // �������� �������� � �������
    fireballDirection(1.0f)
{
}

void FireballSkill::activate(Player* player) {
    if (active) return;

    SDL_FRect playerRect = player->gedDest();

    float fireballX = playerRect.x + (playerRect.w / 2) - (WIDTH / 2);
    float fireballY = playerRect.y + (playerRect.h / 2) - (HEIGHT / 2);

    fireballRect = { fireballX, fireballY, WIDTH, HEIGHT };

    fireballDirection = player->isFlipped() ? -1.0f : 1.0f;

    active = true;
}
int windowWidth = 800;  // ��� ������ ��������� ��������

void FireballSkill::update(Player* player, float deltaTime) {
    if (!active) return;

    fireballRect.x += fireballSpeed * fireballDirection * deltaTime;

    // ����� windowWidth � ������ ����, � �����    �������� ����� (��������, ���������� ���������� ��� ��������)
    extern int windowWidth;

    if (fireballRect.x < 0 || fireballRect.x > windowWidth) {
        active = false;
    }
}

void FireballSkill::render(SDL_Renderer* renderer, Camera* camera) {
    if (!active) return;

    SDL_FRect screenRect = camera->apply(fireballRect);

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_RenderFillRect(renderer, &screenRect);
}
