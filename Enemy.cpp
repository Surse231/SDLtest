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
    loadTexture("slime-walk", "assets/1 Enemies/animals/slime_monster_spritesheet.png");
    loadTexture("deer-run", "assets/1 Enemies/animals/reindeer-m-run.png");
    loadTexture("ovca-walk", "assets/1 Enemies/animals/ram_walk.png");
    loadTexture("ovca-eat", "assets/1 Enemies/animals/ram eat.png");
    loadTexture("polar-bear", "assets/1 Enemies/animals/polar_bear.png");
    loadTexture("fox", "assets/1 Enemies/animals/fox-SWEN-bright.png");
    loadTexture("boar-walk", "assets/1 Enemies/animals/Boar Walk.png");
    loadTexture("boar-stand", "assets/1 Enemies/animals/Boar Stand.png");
    loadTexture("boar-die", "assets/1 Enemies/animals/Boar Die.png");
    loadTexture("boar-attack", "assets/1 Enemies/animals/Boar Attack.png");
    loadTexture("black-bear", "assets/1 Enemies/animals/black_bear.png");
    loadTexture("bird_spparow", "assets/1 Enemies/animals/bird_3_sparrow.png");
    loadTexture("bird_white", "assets/1 Enemies/animals/bird_2_white.png");
    loadTexture("bird_eagle", "assets/1 Enemies/animals/bird_2_eagle.png");
    loadTexture("bird_brown", "assets/1 Enemies/animals/bird_2_brown_1.png");
    loadTexture("bird_blue", "assets/1 Enemies/animals/bird_2_blue.png");
    loadTexture("bear", "assets/1 Enemies/animals/bear.png");
    loadTexture("bat", "assets/1 Enemies/animals/bat-orig.png");
    loadTexture("goat_walk", "assets/1 Enemies/animals/goat_walk.png");
    loadTexture("goat_eat", "assets/1 Enemies/animals/goat_eat.png");

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
    state ? this->state = EnemyState::Aggro : this->state = EnemyState::Idle;
}

void Enemy::setAnimation(const std::string& anim) {
    if (!textures.count(anim) || !textures[anim]) return;
    if (currentAnim == anim && totalFrames > 1) return;


    currentAnim = anim;
    if (frameCounts.count(anim) == 0) return;
    totalFrames = frameCounts[anim];


    SDL_Texture* tex = textures[anim];
    if (!tex) return;

    currentAnim = anim;

    if (frameCounts.count(anim) == 0) return;
    totalFrames = frameCounts[anim];

    float w = 0.0f, h = 0.0f;
    SDL_GetTextureSize(tex, &w, &h);

    // Сохраняем размеры кадра
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

        // Не вызываем setAnimation("death")

        deathTimer = 0.0f;  // Запускаем таймер эффекта смерти
    }
    else {
        hitStartTime = SDL_GetTicks(); // момент получения урона
    }
}





void Enemy::initRectSize() {
    switch (type) {
    case EnemyType::Fox: rect.w = 96; rect.h = 64; break;
    case EnemyType::Boar: rect.w = 96; rect.h = 64; break;
    case EnemyType::Goat: rect.w = 96; rect.h = 64; break;
    case EnemyType::Bird: rect.w = 48; rect.h = 36; break;
    default: rect.w = 90; rect.h = 90; break;
    }
}


SDL_FRect Enemy::getHitbox() const {
    return SDL_FRect{ rect.x, rect.y, rect.w * 1.2f, rect.h * 1.2f };
}

SDL_FRect Enemy::getRect() const {
    return rect;
}

bool Enemy::isMarkedForDeletion() const {
    return markedForDeletion;
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

    Uint64 now = SDL_GetTicks();
    bool recentlyHit = (now - hitStartTime) < 150;

    // Эффект получения урона (покраснение)
    if (recentlyHit) {
        SDL_SetTextureColorMod(tex, 255, 100, 100);
    }

    // Прозрачность при смерти
    if (isDead) {
        SDL_SetTextureAlphaMod(tex, static_cast<Uint8>(deathAlpha));
    }

    SDL_RenderTextureRotated(renderer, tex, &srcRect, &screenRect, 0.0, nullptr, flip);

    // Сброс цвета и альфы
    if (recentlyHit) {
        SDL_SetTextureColorMod(tex, 255, 255, 255);
    }
    if (isDead) {
        SDL_SetTextureAlphaMod(tex, 255);
    }
}




void Enemy::update(float deltaTime, Player* player) {
    if (state == EnemyState::Dead) {
        deathTimer += deltaTime;

        // Плавное исчезновение (альфа от 255 до 0)
        float fadeRate = 255.0f / deathDuration;  // например, исчезает за deathDuration секунд
        deathAlpha -= fadeRate * deltaTime;
        if (deathAlpha < 0.0f) deathAlpha = 0.0f;

        if (deathTimer >= deathDuration) {
            markedForDeletion = true;
        }
        return;
    }


    timeSinceLastAttack += deltaTime;

    SDL_FPoint playerPos = { player->getRect().x, player->getRect().y };
    SDL_FPoint myPos = { rect.x, rect.y };

    float dx = playerPos.x - myPos.x;
    float dy = playerPos.y - myPos.y;
    float distance = sqrtf(dx * dx + dy * dy);

    // Распространение агрессии
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

    // Поведение по типу врага
    switch (type) {
    case EnemyType::Boar:
        if (distance < boarAggroRadius) {
            setAnimation("boar-walk");
            boarChargeTimer += deltaTime;

            if (boarChargeTimer > chargeDelay) {
                setAnimation("boar-attack");
                if (timeSinceLastAttack >= attackCooldown) {
                    player->takeDamage(20);
                    timeSinceLastAttack = 0.0f;
                }
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
        facingRight = patrolDirection >= 0;
        setAnimation("goat_walk");
        break;

    case EnemyType::Bird:
        movementTimer += deltaTime;
        if (movementTimer > movementDelay) {
            float direction = (rand() % 2 == 0) ? 1.0f : -1.0f;
            rect.x += direction * speed * deltaTime;
            facingRight = direction >= 0;
            movementTimer = 0.0f;
        }
        setAnimation("bird_blue");
        break;

    default:
        // Поведение по состоянию
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
                setAnimation("walk");
                float dirX = dx / distance;
                rect.x += dirX * speed * deltaTime;
                facingRight = dirX >= 0;
            }

            if (distance > aggroRadius * 1.5f) {
                state = EnemyState::Returning;
            }
            break;

        case EnemyState::Returning: {
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

                                  break;
        }
    }

    // Анимация (кроме Dead)
    animationTimer += deltaTime;
    if (animationTimer >= 0.15f) {
        animationTimer = 0.0f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }

    // Проверка на окончание эффекта краснения
    if (isFlashingRed) {
        Uint64 now = SDL_GetTicks();
        if (now - flashStartTime > flashDuration) {
            isFlashingRed = false;
        }
    }
}
