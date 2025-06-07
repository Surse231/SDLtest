// Enemy.h
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Camera.h"
#include "Player.h"

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, float x, float y);
    ~Enemy();

    void takeDamage(int amount);
    void render(SDL_Renderer* renderer, Camera* camera);
    void update(float deltaTime, Player* player);

    SDL_FRect getRect() const;
    SDL_FRect getHitbox() const;
    bool isMarkedForDeletion() const;
    float getAggroRadius() const { return aggroRadius; }

    void setAggroState(bool state);
    bool isDeadNow() const { return isDead; } // 👈 используется для проверки

private:
    void setAnimation(const std::string& anim);

    enum class EnemyState { Idle, Suspicious, Aggro, Returning, Dead };
    EnemyState state = EnemyState::Idle;

    SDL_Renderer* renderer;
    SDL_FRect rect;
    SDL_FPoint spawnPoint;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, int> frameCounts = {
        {"idle", 4}, {"walk", 6}, {"attack", 6}, {"hurt", 2}, {"death", 6}
    };

    int currentFrame = 0;
    int totalFrames = 1;
    float animationTimer = 0.0f;
    int frameWidth = 64;
    int frameHeight = 64;

    float speed = 70.0f;
    float aggroRadius = 300.0f;
    float suspicionTimer = 0.0f;
    float suspicionThreshold = 0.5f;

    bool facingRight = true;
    bool isDead = false;
    std::string currentAnim = "idle";

    int health;
    int maxHealth;
    Uint64 hitStartTime = 0;
    bool isPlayingOnce = false;

    float attackCooldown = 1.0f;
    float timeSinceLastAttack = 0.0f;

    static std::vector<Enemy*> allEnemies;
};
