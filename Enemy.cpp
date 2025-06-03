#include "Enemy.h"
#include <SDL3_image/SDL_image.h>

Enemy::Enemy(SDL_Renderer* renderer, float x, float y)
    : renderer(renderer), health(100), maxHealth(100),
    damageTimer(0.0f), damageDuration(0.3f) // инициализация таймера
{
    rect = { x, y, 64, 64 }; // размер манекена
    texture = IMG_LoadTexture(renderer, "assets/enemies/dummy.png");  // путь к PNG-файлу
    if (!texture) {
        SDL_Log("Failed to load texture: %s", SDL_GetError());
    }
}

Enemy::~Enemy()
{
    if (texture) SDL_DestroyTexture(texture);
}

SDL_FRect Enemy::getHitbox() const
{
    return SDL_FRect();
}

void Enemy::takeDamage(int amount) {
    if (isDead) return;

    health -= amount;
    damageTimer = 0.2f; // краснеет 0.1 секунды

    if (health <= 0) {
        health = 0;
        isDead = true;
    }
}

SDL_FRect Enemy::getRect() const {
    return rect;
}

bool Enemy::isMarkedForDeletion() const {
    return isDead && alpha <= 0.0f;
}

void Enemy::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_FRect screenRect = camera->apply(rect);

    if (texture) {
        // Эффект урона — красный оттенок
        if (damageTimer > 0.0f) {
            SDL_SetTextureColorMod(texture, 255, 100, 100);
        }
        else {
            SDL_SetTextureColorMod(texture, 255, 255, 255);
        }

        SDL_SetTextureAlphaMod(texture, (Uint8)alpha); // Применяем альфу
        SDL_RenderTexture(renderer, texture, nullptr, &screenRect);
        SDL_SetTextureAlphaMod(texture, 255); // Сброс альфы
        SDL_SetTextureColorMod(texture, 255, 255, 255); // Сброс цвета
    }
    else {
        // Если текстура не загружена — нарисовать прямоугольник
        SDL_SetRenderDrawColor(renderer, 150, 50, 50, 255);
        SDL_RenderFillRect(renderer, &screenRect);
    }

    // Полоска здоровья
    float barWidth = screenRect.w;
    float barHeight = 6;
    float healthPercent = (float)health / maxHealth;

    if (healthPercent < 0.0f) healthPercent = 0.0f;
    if (healthPercent > 1.0f) healthPercent = 1.0f;

    SDL_FRect healthBarBg = { screenRect.x, screenRect.y - barHeight - 6, barWidth, barHeight };
    SDL_FRect healthBarFg = { screenRect.x, screenRect.y - barHeight - 6, barWidth * healthPercent, barHeight };

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);  // фон
    SDL_RenderFillRect(renderer, &healthBarBg);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);      // здоровье
    SDL_RenderFillRect(renderer, &healthBarFg);
}

void Enemy::update(float deltaTime) {
    if (damageTimer > 0.0f) {
        damageTimer -= deltaTime;
        if (damageTimer < 0.0f)
            damageTimer = 0.0f;
    }

    if (isDead) {
        alpha -= fadeSpeed * deltaTime;
        if (alpha < 0.0f) alpha = 0.0f;
    }
}
