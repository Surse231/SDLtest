#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <map>
#include <string>
#include <vector>
#include "Structs.h"    
#include "Animation.h"
#include "Interface.h"
#include "Camera.h"
#include "Skill.h"
#include "SkillHUD.h"

class Enemy;


class Player {
public:
    Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera);
    ~Player();

    void setEnemies(const std::vector<Enemy*>& enemiesList);
    void obrabotkaklavish(SDL_Event* event);
    void obnovleniepersa();
    void otrisovka();


    void setPosition(float x, float y);
    void setCollisions(const std::vector<SDL_FRect>& rects);

    SDL_FRect getDest() const { return dest; }

    void addMoney(int addedMoney);

    bool getIsSkillActive() const { return isSkillActive; }
    bool isFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }
    bool checkCollision(const SDL_FRect& a, const SDL_FRect& b);

    void setSkillActive(bool active);
    void setLastDashTime(Uint64 t);
    Uint64 getLastDashTime() const;

    int getDirection() const { return isFlipped() ? -1 : 1; }
    int getCurrentAttackFrame() const { return animationHandler.getCurrentFrame(); }

    const std::vector<Skill*>& getSkills() const {
        return skills;
    }

    bool getIsAttack() const;
    SDL_FRect getAttackHitbox() const;

    void setAnimation(const std::string& animName, bool loop);
    SDL_FRect getRect() const;

    void takeDamage(int amount);

    bool isDead() const { return currentHealth <= 0; }
    int getHealth() const { return currentHealth; }

private:
    std::map<std::string, AnimationSet> animations;
    std::string currentAnim = "idle";
    std::vector<Enemy*> enemies;
    std::vector<Skill*> skills;
    std::vector<SDL_FRect> collisionRects;

    void defineLook(const bool* keys);
    void attackHandler();
    void moveHandler(const bool* keys);
    void initAnimations();

    SDL_Renderer* renderer;
    TTF_Font* font;
    Interface* interface;
    Camera* camera;


    SkillHUD* skillHUD;

    SDL_Texture* dashIconTexture = nullptr;

    Animation animationHandler;

    SDL_FRect rect;
    SDL_FRect src;
    SDL_FRect dest;
    SDL_FlipMode flip;

    int previousFrame = -1; // Добавь это в private-секцию Player
    int speed;
    int currentHealth;
    int TotalHealth;
    int money = 0;

    bool currentLoop = true;
    bool isWalk = false;
    bool isAttack = false;
    bool isjump = false;
    bool isRunning = false;
    bool damageDone = false;
    bool hasDealtDamage = false;

    int velocityY = 0;
    static const int gravity = 1;
    static const int sila_prizhka = -15;    

    bool isSkillActive = false;

    Uint64 lastDashTime = 0;
    const Uint64 dashCooldown = 2000;

    Uint32 lastAttackTime = 0;
    const Uint32 attackCooldown = 300;

};
