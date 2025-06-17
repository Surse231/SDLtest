    #include <SDL3_image/SDL_image.h>
    #include <vector>
    #include <SDL3/SDL_events.h>
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_keyboard.h>
    #include "DashSkill.h"          
    #include "FireballSkill.h"
    #include "Enemy.h"  // если класс называется Ennemy, тогда "Ennemy.h"
    #include <iostream> 
    #include <fstream>

Player::Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera)
    : renderer(renderer), font(font), camera(camera),
    inventoryOpen(false), inventory(new Inventory(renderer)),
    dashIconTexture(nullptr), animationHandler(),
    speed(5), currentHealth(100), TotalHealth(100),
    money(0), flip(SDL_FLIP_NONE),
    oldX(0), velocityY(0), gravity(1.0f), sila_prizhka(-15.0f),
    currentLoop(true), isWalk(false), isAttack(false),
    isjump(false), isRunning(false), hasDealtDamage(false),
    isSkillActive(false), lastDashTime(0), lastAttackTime(0)
    
{
    initAnimations();
    src = { 0, 0, 48, 48 };
    dest = { 400.0f, 300.0f, 64.0f, 64.0f };
    rect = dest;

    interface = new Interface(renderer, font, currentHealth, TotalHealth);
    skillHUD = new SkillHUD(renderer);

    inventory->addItem("Topor", "assets/MoiInventory/Topor.png");
    inventory->addItem("eda", "assets/MoiInventory/eda.png");

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
}


Player::~Player() {
    delete interface;
    delete skillHUD;
    for (auto s : skills) delete s;
    delete inventory;
    if (dashIconTexture) SDL_DestroyTexture(dashIconTexture);
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
 
        // Прямоугольник здоровья
        SDL_FRect hpBg = { dest.x, dest.y - 20, 100, 10 };
        SDL_FRect hpBar = { dest.x, dest.y - 20, currentHealth, 10 };
        if (inventoryOpen) {
            inventory->render();
        }
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_RenderFillRect(renderer, &hpBg);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &hpBar);


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

        // Хитбокс — красный прямоугольник
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);
        SDL_FRect screenHitbox = camera->apply(hitbox);
        SDL_RenderRect(renderer, &screenHitbox);
    }

    void Player::setEnemies(const std::vector<Enemy*>& enemiesList) {
        enemies = enemiesList;
    }   

    void Player::renderInventory() {
        if (inventoryOpen && inventory) {
            inventory->render();
        }
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

    SDL_FRect Player::getRect() const {
        return rect; // rect - текущий прямоугольник игрока, меняется при движении
    }




    void Player::addMoney(int addedMoney) {
        money += addedMoney;
        interface->setMoney(money);
    }
    

    void Player::updateHitbox() {
        hitbox.w = dest.w - 50.0f;
        hitbox.h = dest.h - 25.0f;

        hitbox.y = dest.y + 25.1f - 1.f; // 🔥 смещение вверх на 1 пиксель, чтобы не цепляться за пол

        // Легкий сдвиг в сторону в зависимости от направления, но с запасом
        hitbox.x = (flip == SDL_FLIP_HORIZONTAL)
            ? dest.x + 30.0f
            : dest.x + 18.0f;
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

   


    void Player::updateInventory() {
        if (inventoryOpen && inventory) {
            // Например:
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                inventory->handleEvent(&e);
            }
        }
    }

    void Player::moveHandler(const bool* keys) {
        isWalk = false;
        isRunning = false;

        oldX = dest.x;
        float oldY = dest.y;

        int actualSpeed = speed;
        if (keys[SDL_SCANCODE_LSHIFT]) {
            isRunning = true;
            actualSpeed *= 2;
        }

        // === Движение по X ===
        float dx = 0.0f;

        if (keys[SDL_SCANCODE_A]) {
            dx = -actualSpeed;
            flip = SDL_FLIP_HORIZONTAL;
            isWalk = true;
        }
        if (keys[SDL_SCANCODE_D]) {
            dx = actualSpeed;
            flip = SDL_FLIP_NONE;
            isWalk = true;
        }

        dest.x += dx;
        updateHitbox();

        for (const auto& rect : collisionRects) {
            if (SDL_HasRectIntersectionFloat(&hitbox, &rect)) {
                dest.x -= dx;  // откат на тот же шаг, не на старую позицию
                updateHitbox();
                break;
            }
        }

       

        // === Движение по Y ===
        dest.y += velocityY;
        velocityY += gravity;
        updateHitbox();

        // === Проверка коллизий по Y ===
        for (const auto& rect : collisionRects) {
            if (SDL_HasRectIntersectionFloat(&hitbox, &rect)) {
                if (hitbox.y + hitbox.h > rect.y && hitbox.y < rect.y && velocityY >= 0) {
                    dest.y = rect.y - dest.h;
                    velocityY = 0;
                    isjump = false;
                    isOnGround = true;
                }
                else if (velocityY < 0 && hitbox.y - velocityY >= rect.y + rect.h) {
                    // Сверху ударились
                    dest.y = rect.y + rect.h;
                    velocityY = 0;
                }
                updateHitbox();
                break;
            }
        }

        // === Прыжок ===
        if (keys[SDL_SCANCODE_SPACE] && isOnGround) {
            velocityY = sila_prizhka;
            isjump = true;
            isOnGround = false; // сбрасываем после прыжка
        }


        // === Анимации ===
        if (isAttack) {
            // не трогаем
        }
        else if (!isOnGround || isjump) {
            setAnimation("jump", true);
        }
        else if (isWalk) {
            setAnimation(isRunning ? "run" : "walk", true);
        }
        else {
            setAnimation("idle", true);
        }
        updateHitbox();
    }




    void Player::setPosition(float x, float y) {
        dest.x = x - dest.w / 2;  // по центру
        dest.y = y - dest.h;      // над тайлом
        updateHitbox();
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

        for (Skill* skill : skills) {
            FireballSkill* fire = dynamic_cast<FireballSkill*>(skill);
            if (fire) {
                fire->update(this, deltaTime, mapWidth);
            }
            else {
                skill->update(this, deltaTime);
            }
        }


        rect = dest;  // чтобы getRect() возвращал актуальные координаты

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


    void Player::takeDamage(int amount) {
        if (currentHealth <= 0) return;
        currentHealth -= amount;
        if (currentHealth < 0) currentHealth = 0;
    }

    void Player::setDest(const SDL_FRect& d)
    {
        dest = d;
        updateHitbox();
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

        if (inventoryOpen) {
            inventory->handleEvent(event);  // Перетаскивание работает
        }

        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT) {
            if (!inventoryOpen) { // 🚫 Не атакуем, если инвентарь открыт
                isAttack = true;
                animationHandler.reset();
            }
        }
        if (event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT) {
            isAttack = false;
            currentAnim = "idle";
        }
        // ✅ Переключение инвентаря по клавише I
        if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_I) {
            if (!inventoryTogglePressed) {
                inventoryOpen = !inventoryOpen;
                inventoryTogglePressed = true;
                std::cout << "📦 Inventory status: " << (inventoryOpen ? "OPEN" : "CLOSED") << std::endl;
            }
        }

        if (event->type == SDL_EVENT_KEY_UP && event->key.key == SDLK_I) {
            inventoryTogglePressed = false;
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



