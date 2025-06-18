#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <vector>  // добавь для вектора
#include "Player.h"
#include "MainMenu.h"
#include "Camera.h"
#include "TileMap.h"
#include "Enemy.h"   // заменили Dummy на Enemy
#include "TileMap.h"

struct FloatingText {
    std::string text;
    SDL_FPoint position;
    Uint64 startTime;    // èçìåíèòü ñ Uint32 íà Uint64
    Uint32 duration;     // ìîæíî îñòàâèòü Uint32

    FloatingText(const std::string& t, SDL_FPoint pos, Uint64 start, Uint32 dur)
        : text(t), position(pos), startTime(start), duration(dur) {
    }
};

extern std::vector<FloatingText> floatingTexts;

class Game
{
public:
    Game();
    ~Game();

    SDL_AppResult SDL_AppInit();
    SDL_AppResult SDL_AppEvent(SDL_Event* event);
    SDL_AppResult SDL_AppIterate();
    void SDL_AppQuit(SDL_AppResult result);

private:
    bool showMenu = false;
    bool quit = false;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    Player* player = nullptr;
    TTF_Font* font;
    MainMenu* menu = nullptr;
    Camera* camera = nullptr;

    TileMap* tileMap = nullptr;

    std::vector<Enemy*> enemies;  // вместо одного dummy — вектор врагов

    SDL_FRect getWindowSize();
};
