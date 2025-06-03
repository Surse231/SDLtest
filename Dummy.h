#pragma once
#include <SDL3/SDL.h>
#include "Camera.h"

class Camera;

class Dummy {
public:
    Dummy(SDL_Renderer* renderer, float x, float y);
    ~Dummy();

    void takeDamage(int amount);
    void render(SDL_Renderer* renderer, Camera* camera);
    void update(float deltaTime);

    SDL_FRect getRect() const;
    bool isMarkedForDeletion() const;  // <-- ���������

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture = nullptr;
    SDL_FRect rect;

    int health;
    int maxHealth;

    bool isDead = false;               // <-- ���������
    float alpha = 255.0f;              // <-- ��� ������������
    float fadeSpeed = 320.0f;          // <-- ��������/���
    float damageTimer = 0.0f;
    float damageDuration = 0.3f;
};
