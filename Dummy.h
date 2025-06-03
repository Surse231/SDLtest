#pragma once
#include <SDL3/SDL.h>      // ��� SDL_Renderer � SDL_FRect
#include "Camera.h"        // ���� ����� Camera, ������� � ���� � �������

class Dummy {
public:
    Dummy(SDL_Renderer* renderer, float x, float y);
    ~Dummy();

    void render(SDL_Renderer* renderer, Camera* camera);
    SDL_FRect getRect() const;
    void takeDamage(int amount);

    int getHealth() const { return health; }
    int getMaxHealth() const { return maxHealth; }

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;  // �������� ��������
    SDL_FRect rect;
    int health;
    int maxHealth;
};
