#include "Game.h"
#include <vector>
#include <string>
#include "FireballSkill.h"


bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return !(a.x + a.w < b.x || b.x + b.w < a.x ||
        a.y + a.h < b.y || b.y + b.h < a.y);
}

Game::Game() {}

Game::~Game()
{
    delete dummy;
    delete player;
    delete menu;
    delete camera;

    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

SDL_AppResult Game::SDL_AppInit()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    SDL_CreateWindowAndRenderer("SDL3 Game", 1920, 1080, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

  

    font = TTF_OpenFont("assets/fonts/Orbitron-VariableFont_wght.ttf", 32);
    camera = new Camera(1920, 1080, 400, 200);
    player = new Player(renderer, font, camera);
    dummy = new Dummy(renderer, 600, 250);

    menu = new MainMenu(renderer, font, window);  // 🔧 добавлено

    return SDL_AppResult();
}


SDL_AppResult Game::SDL_AppEvent(SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE)
    {
        if (showMenu) {
            if (!menu || !menu->isInSettings()) {
                showMenu = false;
            }
        }
        else {
            showMenu = true;
        }
    }

    if (showMenu) {
        bool resume = false;
        if (menu)
            menu->handleEvent(*event, resume, quit);
        if (resume) showMenu = false;
    }
    else {
        player->obrabotkaklavish(event);
    }

    return quit ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

SDL_AppResult Game::SDL_AppIterate()
{
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    if (showMenu) {
        menu->render();
    }
    else {
        camera->update(player->gedDest());

        player->otrisovka();
        player->obnovleniepersa();

        // Здесь проверяем столкновение с огненным шаром:
        for (Skill* skill : player->getSkills()) {
            FireballSkill* fireball = dynamic_cast<FireballSkill*>(skill);
            if(fireball && fireball->isActive()) {
                if (checkCollision(fireball->getRect(), dummy->getRect())) {
                    dummy->takeDamage(10);    // Наносим урон 10
                    fireball->deactivate();  // Отключаем огненный шар после попадания
                }
            }
        }


        dummy->render(renderer, camera);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);

    return quit ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

SDL_FRect Game::getWindowSize() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return SDL_FRect{ 0, 0, static_cast<float>(w), static_cast<float>(h) };
}



void Game::SDL_AppQuit(SDL_AppResult result)
{
    // Всё удаляется в деструкторе
}
