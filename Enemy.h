// Enemy.h
#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <string>
#include "Camera.h"
#include "Player.h"

enum class EnemyType { Default, Boar, Fox, Goat, Bird };

class Enemy {
public:
    Enemy(SDL_Renderer* renderer, float x, float y, EnemyType type);
    ~Enemy();

    void takeDamage(int amount);
    void render(SDL_Renderer* renderer, Camera* camera);
    void update(float deltaTime, Player* player);

    void initRectSize();
    void setCollisionRects(const std::vector<SDL_FRect>& rects);
    SDL_FRect getRect() const;
    SDL_FRect getHitbox() const;
    bool isMarkedForDeletion() const;
    float getAggroRadius() const { return aggroRadius; }
    void setPosition(float x, float y);
    void setAggroState(bool state);
    bool isDeadNow() const { return isDead; }

private:
    void setAnimation(const std::string& anim);

    bool markedForDeletion = false;

    enum class EnemyState { Idle, Suspicious, Aggro, Returning, Hurt, Dead };
    EnemyState state = EnemyState::Idle;

    EnemyType type = EnemyType::Default;

    SDL_Renderer* renderer;
    SDL_FRect rect;
    SDL_FPoint spawnPoint;

    std::vector<SDL_FRect> collisionRects;

    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, int> frameCounts = {
        {"idle", 4}, {"walk", 6}, {"attack", 6}, {"hurt", 2}, {"death", 6},
        {"boar-walk", 4}, {"boar-stand", 4}, {"boar-attack", 6}, {"boar-die", 4},
        {"slime-walk", 3}, {"deer-run", 3}, {"ovca-walk", 4}, {"ovca-eat", 4},
        {"polar-bear", 3}, {"fox", 3}, {"black-bear", 3}, {"bird_spparow", 3},
        {"bird_white", 3}, {"bird_eagle", 3}, {"bird_brown", 3}, {"bird_blue", 3},
        {"bear", 3}, {"bat", 4}, {"goat_walk", 4}, {"goat_eat", 4}
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

    float boarAggroRadius = 200.0f;
    float boarChargeTimer = 0.0f;
    float chargeDelay = 0.6f;

    float movementTimer = 0.0f;
    float movementDelay = 2.0f;

    bool facingRight = true;
    bool isDead = false;
    std::string currentAnim = "idle";

    int health;
    int maxHealth;
    Uint64 hitStartTime = 0;

    float attackCooldown = 1.0f;
    float timeSinceLastAttack = 0.0f;

    float velocityY = 0.0f;
    float gravity = 1.0f;
    bool isOnGround = false;

    float deathTimer = 0.0f;
    float deathDuration = 1.5f;

    bool isFlashingRed = false;
    Uint64 flashStartTime = 0;
    Uint64 flashDuration = 200;

    static std::vector<Enemy*> allEnemies;

    float patrolDirection = 1.0f;
    float deathAlpha = 255.0f;
};