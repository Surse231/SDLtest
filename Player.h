
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

class Player {
public:
    Player(SDL_Renderer* renderer, TTF_Font* font, Camera* camera);
    ~Player();

    void obrabotkaklavish(SDL_Event* event);
    void obnovleniepersa();
    void otrisovka();

    void setPosition(float x, float y);
    void setCollisions(const std::vector<SDL_FRect>& rects);

    SDL_FRect gedDest() { return dest; }
    void addMoney(int addedMoney);

    bool getIsSkillActive() const { return isSkillActive; }

    bool isFlipped() const { return flip == SDL_FLIP_HORIZONTAL; }


    void setSkillActive(bool active);
    void setLastDashTime(Uint64 t);
    Uint64 getLastDashTime() const;

    int getDirection() const { return isFlipped() ? -1 : 1; }

    const std::vector<Skill*>& getSkills() const {
        return skills;  // теперь фигурна€ скобка закрыта!
    }

private:

    std::vector<Skill*> skills;  // только один раз!

    void defineLook(const bool* keys);
    void attackHandler();
    void moveHandler(const bool* keys);
    void initAnimations();

    SDL_Renderer* renderer;
    TTF_Font* font;
    Interface* interface;
    Camera* camera;

    std::vector<SDL_FRect> collisionRects;

    SkillHUD* skillHUD;

    SDL_Texture* dashIconTexture = nullptr;

    std::map<std::string, AnimationSet> animations;
    std::string currentAnim = "idle";
    Animation animationHandler;

    SDL_FRect src;
    SDL_FRect dest;
    SDL_FlipMode flip;

    int speed;
    int currentHealth;
    int TotalHealth;
    int money = 0;

    bool isWalk = false;
    bool isAttack = false;
    bool isjump = false;
    bool isRunning = false;

    int velocityY = 0;
    static const int gravity = 1;
    static const int sila_prizhka = -15;

    bool isSkillActive = false;

    Uint64 lastDashTime = 0;
    const Uint64 dashCooldown = 2000;

    Uint32 lastAttackTime = 0;     // Ёто поле в классе Player или глобально, если нужно
    const Uint32 attackCooldown = 300;  // «адержка в миллисекундах

};