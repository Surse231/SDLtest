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

Enemy::Enemy(SDL_Renderer* renderer, float x, float y)
    : renderer(renderer), health(100), maxHealth(100)
{
    spawnPoint = { x, y };
    rect = { x, y, 90, 90 };
    allEnemies.push_back(this);

    auto loadTexture = [&](const std::string& name, const std::string& path) {
        SDL_Texture* tex = IMG_LoadTexture(renderer, path.c_str());
        if (!tex) {
            std::cerr << "Не удалось загрузить " << path << ": " << SDL_GetError() << std::endl;
        }
        textures[name] = tex;
        };

    loadTexture("idle", "assets/1 Enemies/2/idle.png");
    loadTexture("walk", "assets/1 Enemies/2/walk.png");
    loadTexture("attack", "assets/1 Enemies/2/attack.png");
    loadTexture("hurt", "assets/1 Enemies/2/hurt.png");
    loadTexture("death", "assets/1 Enemies/2/death.png");

    setAnimation("idle");
}

Enemy::~Enemy() {
    for (auto& pair : textures) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    textures.clear();
}

void Enemy::setAggroState(bool state) {
    state ? this->state = EnemyState::Aggro : this->state = EnemyState::Idle;
}

void Enemy::setAnimation(const std::string& anim) {
    if (!textures.count(anim) || !textures[anim]) return;
    if (currentAnim == anim && totalFrames > 1) return;


    currentAnim = anim;
    totalFrames = frameCounts[anim];

    float texW = 0.0f, texH = 0.0f;
    SDL_GetTextureSize(textures[anim], &texW, &texH);

    frameWidth = static_cast<int>(texW) / totalFrames;
    frameHeight = static_cast<int>(texH);

    currentFrame = 0;
    animationTimer = 0.0f;
}

void Enemy::takeDamage(int amount) {
    if (isDead) return;
    health -= amount;
    if (health <= 0) {
        health = 0;
        isDead = true;
        setAnimation("death");
        state = EnemyState::Dead;
    }
    else {
        setAnimation("hurt");
        hitStartTime = SDL_GetTicks();
    }
}

SDL_FRect Enemy::getHitbox() const {
    return SDL_FRect{ rect.x, rect.y, rect.w * 1.2f, rect.h * 1.2f };
}

SDL_FRect Enemy::getRect() const {
    return rect;
}

bool Enemy::isMarkedForDeletion() const {
    return isDead && currentAnim == "death" && currentFrame >= frameCounts.at("death") - 1;
}

void Enemy::render(SDL_Renderer* renderer, Camera* camera) {
    SDL_Texture* tex = textures[currentAnim];
    if (!tex) return;

    SDL_FRect srcRect = {
        static_cast<float>(currentFrame * frameWidth),
        0.0f,
        static_cast<float>(frameWidth),
        static_cast<float>(frameHeight)
    };

    SDL_FRect screenRect = camera->apply(rect);
    SDL_FlipMode flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderTextureRotated(renderer, tex, &srcRect, &screenRect, 0.0, nullptr, flip);
}

void Enemy::update(float deltaTime, Player* player) {
    if (state == EnemyState::Dead) {
        animationTimer += deltaTime;
        if (animationTimer >= 0.15f) {
            animationTimer = 0.0f;
            currentFrame = std::min(currentFrame + 1, totalFrames - 1);
        }
        return;
    }

    timeSinceLastAttack += deltaTime;

    SDL_FPoint playerPos = { player->getRect().x, player->getRect().y };
    SDL_FPoint myPos = { rect.x, rect.y };

    float dx = playerPos.x - myPos.x;
    float dy = playerPos.y - myPos.y;
    float distance = sqrtf(dx * dx + dy * dy);

    if (state == EnemyState::Aggro) {
        for (Enemy* other : Enemy::allEnemies) {
            if (other != this && !other->isDeadNow()) {
                float distToOther = distanceBetween(this->rect, other->getRect());
                if (distToOther < 150.0f) {
                    other->state = EnemyState::Aggro;
                }
            }
        }
    }
    
    switch (state) {
    case EnemyState::Idle:
        setAnimation("idle");
        if (distance < aggroRadius) {
            suspicionTimer += deltaTime;
            state = (suspicionTimer > suspicionThreshold) ? EnemyState::Aggro : EnemyState::Suspicious;
        }
        break;

    case EnemyState::Suspicious:
        setAnimation("idle");
        if (distance < aggroRadius) {
            suspicionTimer += deltaTime;
            if (suspicionTimer >= suspicionThreshold) {
                state = EnemyState::Aggro;
            }
        }
        else {
            suspicionTimer = 0;
            state = EnemyState::Idle;
        }
        break;

    case EnemyState::Aggro:
        if (distance < 50.0f) {
            setAnimation("attack");
            if (timeSinceLastAttack >= attackCooldown) {
                int damage = 12 + rand() % 4;
                player->takeDamage(damage);
                timeSinceLastAttack = 0.0f;
            }
        }
        else {
            setAnimation("walk");  // 🔧 ОБЯЗАТЕЛЬНО!
            float dirX = dx / distance;
            rect.x += dirX * speed * deltaTime;
            facingRight = dirX >= 0;
        }

        if (distance > aggroRadius * 1.5f) {
            state = EnemyState::Returning;
        }
        break;

    case EnemyState::Returning:
        setAnimation("walk");
        float dxBack = spawnPoint.x - rect.x;
        float distBack = fabsf(dxBack);
        if (distBack > 5.0f) {
            float dirX = dxBack / distBack;
            rect.x += dirX * speed * deltaTime;
            facingRight = dirX >= 0;
        }
        else {
            rect.x = spawnPoint.x;
            state = EnemyState::Idle;
        }
        break;
    }

    // 🔧 АНИМАЦИЯ ВСЕГДА ОБНОВЛЯЕТСЯ (кроме смерти)
    animationTimer += deltaTime;
    if (animationTimer >= 0.15f) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }
}
