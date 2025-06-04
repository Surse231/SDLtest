    #include <SDL3_image/SDL_image.h>
    #include <vector>
    #include <SDL3/SDL_events.h>
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_keyboard.h>
    #include "DashSkill.h"          
    #include "FireballSkill.h"
    //#include "SunBeamSkill.h"
    #include "Enemy.h"  // если класс называется Ennemy, тогда "Ennemy.h"
#include <iostream> 
#include <fstream>





    Player::Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera)
        : renderer(renderer), font(font), camera(camera)
    {
        initAnimations();
        src = { 0, 0, 48, 48 };
        dest = { 200, 250, 96, 96 }; // Вернули персонажа выше

        speed = 5;
        currentHealth = 50;
        TotalHealth = 100;
        interface = new Interface(renderer, font, currentHealth, TotalHealth);
        skillHUD = new SkillHUD(renderer);

        dashIconTexture = IMG_LoadTexture(renderer, "assets/icons/dash.png");
        if (dashIconTexture) {
            skillHUD->addSkillIcon(dashIconTexture, "Dash");
            skills.push_back(new DashSkill());
        }

        SDL_Texture* fireIcon = IMG_LoadTexture(renderer, "assets/icons/fireball.png");
        if (fireIcon) {
            skillHUD->addSkillIcon(fireIcon, "Fireball");
            skills.push_back(new FireballSkill());
        }

        /*SDL_Texture* sunBeamIcon = IMG_LoadTexture(renderer, "assets/icons/sunbeam.png");
        if (sunBeamIcon) {
            skillHUD->addSkillIcon(sunBeamIcon, "SunBeam");
            skills.push_back(new SunBeamSkill());
        }*/


    }

    Player::~Player() {
        if (dashIconTexture) {
            SDL_DestroyTexture(dashIconTexture);
        }

        delete interface;
        delete skillHUD;
        for (Skill* skill : skills) {
            delete skill;
        }
        skills.clear();

    }

    void Player::initAnimations() {
        SDL_Texture* tex = nullptr;

        tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_idle.png");
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations["idle"] = { tex, 4, 200 };
        }

        tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_walk.png");
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations["walk"] = { tex, 6, 100 };
        }

        tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_run.png");
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations["run"] = { tex, 6, 100 };
        }

        tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_attack2.png");
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations["attack"] = { tex, 6, 150 }; // 150 мс на кадр, чтобы дать время обновиться
        }

        tex = IMG_LoadTexture(renderer, "assets/1 Woodcutter/Woodcutter_jump.png");
        if (tex) {
            SDL_SetTextureScaleMode(tex, SDL_SCALEMODE_NEAREST);
            animations["jump"] = { tex, 6, 150 };
        }
    }


    void Player::otrisovka() {
        const AnimationSet& anim = animations[currentAnim];
        SDL_FRect screenDest = camera->apply(dest);
        SDL_RenderTextureRotated(renderer, anim.texture, &src, &screenDest, 0, nullptr, flip);
        interface->otrisovka();
 
        // отрисовка интерфейса с иконками скиллов
        skillHUD->render();

        static Uint64 lastTime = SDL_GetTicks();
        Uint64 now = SDL_GetTicks();
        Uint64 deltaTimeMs = now - lastTime;
        lastTime = now;

        float deltaTime = deltaTimeMs / 1000.0f; // миллисекунды в секунды

        for (Skill* skill : skills) {
            skill->render(renderer, camera);
        }
    }

    void Player::setEnemies(const std::vector<Enemy*>& enemiesList) {
        enemies = enemiesList;
    }   



    void Player::setAnimation(const std::string& animName, bool loop) {
        if (currentAnim != animName) {
            currentAnim = animName;
            animationHandler.reset();

            src.x = 0;
            src.y = 0;
            currentLoop = loop;
        }
    }


    void Player::addMoney(int addedMoney) {
        money += addedMoney;
        interface->setMoney(money);
    }
    

    void Player::defineLook(const bool* keys) {
        if (keys[SDL_SCANCODE_A]) {
            flip = SDL_FLIP_HORIZONTAL;
            currentAnim = "walk";
        }
        else if (keys[SDL_SCANCODE_D]) {
            flip = SDL_FLIP_NONE;
            currentAnim = "walk";
        }
        else {
            currentAnim = "idle";
        }
    }


    void Player::attackHandler() {
        if (!isAttack) return;

        int currentFrame = animationHandler.getCurrentFrame();
        std::cout << "Enemies count in Player: " << enemies.size() << std::endl;

        if (currentFrame >= 4 && !hasDealtDamage) {
            SDL_FRect atkBox = getAttackHitbox();
            std::cout << "Attack hitbox: x=" << atkBox.x << " y=" << atkBox.y
                << " w=" << atkBox.w << " h=" << atkBox.h << std::endl;

            for (Enemy* enemy : enemies) {
                if (!enemy) continue;

                SDL_FRect enemyBox = enemy->getHitbox();
                std::cout << "Enemy hitbox: x=" << enemyBox.x << " y=" << enemyBox.y
                    << " w=" << enemyBox.w << " h=" << enemyBox.h << std::endl;

                if (checkCollision(atkBox, enemyBox)) {
                    std::cout << "HIT DETECTED" << std::endl;
                    enemy->takeDamage(10);
                    hasDealtDamage = true;
                    break;
                }
            }
        }

        if (animationHandler.isFinished()) {
            isAttack = false;
            hasDealtDamage = false;
            currentAnim = "idle";
            animationHandler.reset();
        }
    }







    void Player::moveHandler(const bool* keys) {
        isWalk = false;
        isRunning = false;

        // Определяем направление и состояние
        if (keys[SDL_SCANCODE_A]) {
            flip = SDL_FLIP_HORIZONTAL;
            isWalk = true;
        }
        else if (keys[SDL_SCANCODE_D]) {
            flip = SDL_FLIP_NONE;
            isWalk = true;
        }

        // Прыжок
        if (keys[SDL_SCANCODE_SPACE] && !isjump) {
            velocityY = sila_prizhka;
            isjump = true;
        }

        if (isjump) {
            dest.y += velocityY;
            velocityY += gravity;
            if (dest.y >= 250) {
                dest.y = 250;
                isjump = false;
                velocityY = 0;
            }
        }

        int actualSpeed = speed;
        if (keys[SDL_SCANCODE_LSHIFT]) {
            isRunning = true;
            actualSpeed = speed * 2;
        }

        if (keys[SDL_SCANCODE_A]) {
            dest.x -= actualSpeed;
        }
        if (keys[SDL_SCANCODE_D]) {
            dest.x += actualSpeed;
        }

        // Устанавливаем анимацию в зависимости от состояния
        if (isAttack) {
            // Не меняем анимацию при атаке, она будет обновляться отдельно
        }
        else if (isjump) {
            setAnimation("jump", true);
        }
        else if (isWalk) {
            setAnimation(isRunning ? "run" : "walk", true);
        }
        else {
            setAnimation("idle", true);
        }
    }



    void Player::setPosition(float x, float y) {
        dest.x = x;
        dest.y = y;
    }
    void Player::setCollisions(const std::vector<SDL_FRect>& rects) {
        collisionRects = rects;
    }


    // Возвращает true, если игрок сейчас атакует
    bool Player::getIsAttack() const {
        return isAttack;
    }

    SDL_FRect Player::getAttackHitbox() const {
        float attackWidth = 110.0f;  // Расширяем ещё больше (с 90 → 110)
        float attackHeight = dest.h * 1.2f; // Делаем выше (с 1.1 → 1.2)
        float attackX;

        if (flip == SDL_FLIP_NONE) {
            attackX = dest.x + dest.w;
        }
        else {
            attackX = dest.x - attackWidth;
        }

        float attackY = dest.y;

        return SDL_FRect{ attackX, attackY, attackWidth, attackHeight };
    }


    void Player::obnovleniepersa() {
        static Uint64 lastTime = SDL_GetTicks();

        Uint64 now = SDL_GetTicks();
        Uint64 deltaTimeMs = now - lastTime;
        lastTime = now;

        float deltaTime = deltaTimeMs / 1000.0f;

        const bool* keys = SDL_GetKeyboardState(nullptr);

        // Всегда вызываем движение
        moveHandler(keys);

        interface->obnovlenieHUD();

        const AnimationSet& currentAnimation = animations[currentAnim];

        // loop=false для анимации атаки, чтобы она дошла до конца
        bool loopAnim = (currentAnim != "attack");

        animationHandler.update(currentAnimation, src, 48 /* ширина кадра для ваших спрайтов */, loopAnim);


        attackHandler();

        for (Skill* skill : skills)
            skill->update(this, deltaTime);
    }


    bool Player::checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
        float rightA = a.x + a.w;
        float rightB = b.x + b.w;
        float bottomA = a.y + a.h;
        float bottomB = b.y + b.h;

        bool collidingX = (a.x < rightB) && (rightA > b.x);
        bool collidingY = (a.y < bottomB) && (bottomA > b.y);

        return collidingX && collidingY;
    }   





    void Player::setSkillActive(bool active) {
        isSkillActive = active;
    }

    void Player::setLastDashTime(Uint64 t) {
        lastDashTime = t;
    }

    Uint64 Player::getLastDashTime() const {
        return lastDashTime;
    }

    void Player::obrabotkaklavish(SDL_Event* event) {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
            if (!isAttack) {
                currentAnim = "attack";
                animationHandler.reset();

                isAttack = true;
                hasDealtDamage = false; // <<< ВАЖНО!
            }
        }


        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_Q) {
            if (!skills.empty()) {
                skills[0]->activate(this);  // DashSkill
            }
        }
        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_E) {
            if (skills.size() > 1) {
                skills[1]->activate(this);  // FireballSkill
            }
        }
        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_R) {
            if (skills.size() > 2) {
                skills[2]->activate(this);  // SunBeamSkill
            }
        }
    }



