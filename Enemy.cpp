// Enemy.cpp
#include "Enemy.h"
#include <iostream>
#include <cmath>

std::vector<Enemy*> Enemy::allEnemies;

static float distanceBetween(const SDL_FRect& a, const SDL_FRect& b) {
    float dx = (a.x + a.w / 2.0f) - (b.x + b.w / 2.0f);
    float dy = (a.y + a.h / 2.0f) - (b.y + b.h / 2.0f);
    return sqrtf(dx * dx + dy * dy);
}

Enemy::Enemy(SDL_Renderer* renderer, float x, float y, EnemyType type)
    : renderer(renderer), health(100), maxHealth(100), type(type)
{
    spawnPoint = { x, y };
    rect = { x, y, 90, 90 };
    allEnemies.push_back(this);

    auto loadTexture = [&](const std::string& name, const std::string& path) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (!tex) std::cerr << "Failed to load " << path << ": " << SDL_GetError() << std::endl;
        textures[name] = tex;
        };

    loadTexture("idle", "assets/1 Enemies/2/idle.png");
    loadTexture("walk", "assets/1 Enemies/2/walk.png");
    loadTexture("attack", "assets/1 Enemies/2/attack.png");
    loadTexture("death", "assets/1 Enemies/2/death.png");
    loadTexture("fox", "assets/1 Enemies/animals/fox-SWEN-bright.png");
    loadTexture("boar-walk", "assets/1 Enemies/animals/Boar Walk.png");
    loadTexture("boar-stand", "assets/1 Enemies/animals/Boar Stand.png");
    loadTexture("boar-attack", "assets/1 Enemies/animals/Boar Attack.png");
    loadTexture("goat_walk", "assets/1 Enemies/animals/goat_walk.png");
    loadTexture("bird_blue", "assets/1 Enemies/animals/bird_2_blue.png");

    initRectSize();
    setAnimation("idle");
}

Enemy::~Enemy() {
    for (auto& pair : textures) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}

void Enemy::setAggroState(bool state) {
    this->state = state ? EnemyState::Aggro : EnemyState::Idle;
}

void Enemy::setAnimation(const std::string& anim) {
    if (!textures.count(anim) || !textures[anim]) return;
    if (currentAnim == anim && totalFrames > 1) return;
    currentAnim = anim;
    if (frameCounts.count(anim) == 0) return;
    totalFrames = frameCounts[anim];

    SDL_Texture* tex = textures[anim];
    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(tex, &w, &h);
    frameWidth = static_cast<int>(w) / totalFrames;
    frameHeight = static_cast<int>(h);
    currentFrame = 0;
    animationTimer = 0.0f;
}

void Enemy::takeDamage(int amount) {
    if (isDead) return;
    health -= amount;
    if (health <= 0) {
        health = 0;
        isDead = true;
        state = EnemyState::Dead;
        deathTimer = 0.0f;
    }
}

void Enemy::initRectSize() {
    switch (type) {
    case EnemyType::Fox:
    case EnemyType::Boar:
    case EnemyType::Goat:
        rect.w = 96; rect.h = 64; break;
    case EnemyType::Bird:
        rect.w = 48; rect.h = 36; break;
    default:
        rect.w = 70; rect.h = 70; break;
    }
}

void Enemy::setCollisionRects(const std::vector<SDL_FRect>& rects) {
    collisionRects = rects;
}

SDL_FRect Enemy::getHitbox() const {
    return { rect.x, rect.y, rect.w * 1.2f, rect.h * 1.2f };
}

SDL_FRect Enemy::getRect() const {
    return rect;
}

bool Enemy::isMarkedForDeletion() const {
    return markedForDeletion;
}

void Enemy::setPosition(float x, float y) {
    rect.x = x;
    rect.y = y;
}

void Enemy::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_Texture* tex = textures[currentAnim];
    if (!tex) return;

    SDL_FRect srcRect = { static_cast<float>(currentFrame * frameWidth), 0.0f, (float)frameWidth, (float)frameHeight };
    SDL_FRect screenRect = camera->apply(rect);
    SDL_FlipMode flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

    if (isDead)
        SDL_SetTextureAlphaMod(tex, static_cast<Uint8>(deathAlpha));

    SDL_RenderTextureRotated(renderer, tex, &srcRect, &screenRect, 0.0, nullptr, flip);

    if (isDead)
        SDL_SetTextureAlphaMod(tex, 255);
}

void Enemy::update(float deltaTime, Player* player) {
    if (state == EnemyState::Dead) {
        deathTimer += deltaTime;
        deathAlpha -= (255.0f / deathDuration) * deltaTime;
        if (deathAlpha < 0.0f) deathAlpha = 0.0f;
        if (deathTimer >= deathDuration) markedForDeletion = true;
        return;
    }

    timeSinceLastAttack += deltaTime;

    float dx = player->getRect().x - rect.x;
    float dy = player->getRect().y - rect.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (state == EnemyState::Aggro) {
        for (Enemy* other : Enemy::allEnemies) {
            if (other != this && !other->isDeadNow()) {
                if (distanceBetween(this->rect, other->getRect()) < 150.0f)
                    other->state = EnemyState::Aggro;
            }
        }
    }

    switch (type) {
    case EnemyType::Boar:
        if (distance < boarAggroRadius) {
            setAnimation("boar-walk");
            boarChargeTimer += deltaTime;
            if (boarChargeTimer > chargeDelay && timeSinceLastAttack >= attackCooldown) {
                setAnimation("boar-attack");
                player->takeDamage(20);
                timeSinceLastAttack = 0.0f;
            }
        }
        else {
            setAnimation("boar-stand");
            boarChargeTimer = 0.0f;
        }
        break;

    case EnemyType::Fox:
        movementTimer += deltaTime;
        if (movementTimer > movementDelay) {
            patrolDirection *= -1.0f;
            movementTimer = 0.0f;
        }
        rect.x += patrolDirection * speed * deltaTime;
        for (const auto& wall : collisionRects) {
            if (SDL_HasRectIntersectionFloat(&rect, &wall)) {
                rect.x -= patrolDirection * speed * deltaTime;
                patrolDirection *= -1.0f;
                break;
            }
        }
        facingRight = patrolDirection >= 0;
        setAnimation("fox");
        break;

    case EnemyType::Goat:
        movementTimer += deltaTime;
        if (movementTimer > movementDelay) {
            patrolDirection *= -1.0f;
            movementTimer = 0.0f;
        }
        rect.x += patrolDirection * speed * deltaTime;
        for (const auto& wall : collisionRects) {
            if (SDL_HasRectIntersectionFloat(&rect, &wall)) {
                rect.x -= patrolDirection * speed * deltaTime;
                break;
            }
        }
        facingRight = patrolDirection >= 0;
        setAnimation("goat_walk");
        break;

    case EnemyType::Bird:
        movementTimer += deltaTime;
        if (movementTimer > movementDelay) {
            float dir = (rand() % 2 == 0) ? 1.0f : -1.0f;
            rect.x += dir * speed * deltaTime;
            facingRight = dir >= 0;
            movementTimer = 0.0f;
        }
        setAnimation("bird_blue");
        break;

    default:
        if (state == EnemyState::Idle && distance < aggroRadius) {
            suspicionTimer += deltaTime;
            state = (suspicionTimer > suspicionThreshold) ? EnemyState::Aggro : EnemyState::Suspicious;
        }
        else if (state == EnemyState::Suspicious) {
            if (distance < aggroRadius) {
                suspicionTimer += deltaTime;
                if (suspicionTimer >= suspicionThreshold) state = EnemyState::Aggro;
            }
            else {
                suspicionTimer = 0;
                state = EnemyState::Idle;
            }
        }
        else if (state == EnemyState::Aggro) {
            if (distance < 50.0f && timeSinceLastAttack >= attackCooldown) {
                setAnimation("attack");
                player->takeDamage(12 + rand() % 4);
                timeSinceLastAttack = 0.0f;
            }
            else {
                setAnimation("walk");
                rect.x += (dx / distance) * speed * deltaTime;
                facingRight = dx >= 0;
            }
            if (distance > aggroRadius * 1.5f)
                state = EnemyState::Returning;
        }
        else if (state == EnemyState::Returning) {
            setAnimation("walk");
            float back = spawnPoint.x - rect.x;
            if (fabsf(back) > 5.0f) {
                rect.x += (back / fabsf(back)) * speed * deltaTime;
                facingRight = back >= 0;
            }
            else {
                rect.x = spawnPoint.x;
                state = EnemyState::Idle;
            }
        }
        break;
    }

    velocityY += gravity;
    rect.y += velocityY;
    isOnGround = false;

    for (const auto& wall : collisionRects) {
        SDL_FRect inter;
        if (SDL_GetRectIntersectionFloat(&rect, &wall, &inter)) {
            if (velocityY > 0) {
                rect.y = wall.y - rect.h;
                velocityY = 0;
                isOnGround = true;
            }
            else if (velocityY < 0) {
                rect.y = wall.y + wall.h;
                velocityY = 0;
            }
            break;
        }
    }

    animationTimer += deltaTime;
    if (animationTimer >= 0.15f) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }
}