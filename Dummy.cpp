#include "Dummy.h"
#include <SDL3_image/SDL_image.h>
Dummy::Dummy(SDL_Renderer* renderer, float x, float y)
    : renderer(renderer), health(100), maxHealth(100)
{
    rect = { x, y, 64, 64 }; // ������ ��������
    texture = IMG_LoadTexture(renderer, "assets/dummy.png");  // ���� � PNG-�����
    if (!texture) {
        SDL_Log("Failed to load texture: %s", SDL_GetError());
    }
}

Dummy::~Dummy()
{
    if (texture) SDL_DestroyTexture(texture);
}

void Dummy::takeDamage(int amount) {
    health -= amount;
    if (health < 0) health = 0;
}

SDL_FRect Dummy::getRect() const {
    return rect;
}

void Dummy::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_FRect screenRect = camera->apply(rect);

    if (texture) {
        SDL_RenderTexture(renderer, texture, nullptr, &screenRect);
    }
    else {
        // ���� �������� �� �����������, ������ ������� �������
        SDL_SetRenderDrawColor(renderer, 150, 50, 50, 255);
        SDL_RenderFillRect(renderer, &screenRect);
    }

    // ������� ��������
    float barWidth = screenRect.w;
    float barHeight = 6;
    float healthPercent = (float)health / maxHealth;

    SDL_FRect healthBarBg = { screenRect.x, screenRect.y - barHeight - 6, barWidth, barHeight }; // ������ ���� �� 4 �������
    SDL_FRect healthBarFg = { screenRect.x, screenRect.y - barHeight - 6, barWidth * healthPercent, barHeight };

    // ��� ������� � �����
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &healthBarBg);

    // ����������� ����� � ������
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &healthBarFg);
}
