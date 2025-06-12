#include "Game.h"
#include <vector>
#include <string>
#include "FireballSkill.h"
#include "SDL3_ttf/SDL_ttf.h"

bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y;
}


Game::Game() {}

Game::~Game()
{
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
    SDL_FRect playerRect = player->getRect();

    // Создаем врагов
 // Создаем врагов
    enemies.push_back(new Enemy(renderer, 600, 250, EnemyType::Default));
    enemies.push_back(new Enemy(renderer, 800, 250, EnemyType::Default));

    enemies.push_back(new Enemy(renderer, 600, 250, EnemyType::Boar));
    enemies.push_back(new Enemy(renderer, 600, 250, EnemyType::Fox));
    enemies.push_back(new Enemy(renderer, 400, 250, EnemyType::Goat));
    enemies.push_back(new Enemy(renderer, 1000, 250, EnemyType::Bird));



    // Передаем врагов игроку
    player->setEnemies(enemies);

    menu = new MainMenu(renderer, font, window);  // 🔧 добавлено

    // Проверяем агр-зону и устанавливаем состояние для всех врагов
    for (Enemy* enemy : enemies) {
        double distance = sqrt(pow(playerRect.x - enemy->getRect().x, 2) +
            pow(playerRect.y - enemy->getRect().y, 2));

        enemy->setAggroState(distance < enemy->getAggroRadius());

    }

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

    if (player->isDead()) {
        SDL_Color red = { 255, 0, 0, 255 };

        SDL_Surface* surface = TTF_RenderText_Blended(font, "GAME OVER", strlen("GAME OVER"), red);

        if (!surface) {
            SDL_Log("Failed to create text surface: %s", SDL_GetError());
            return SDL_APP_CONTINUE;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!texture) {
            SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
            return SDL_APP_CONTINUE;
        }

        float w, h;
        SDL_GetTextureSize(texture, &w, &h);

        SDL_FRect dst = { 1920 / 2.0f - w / 2.0f, 1080 / 2.0f - h / 2.0f, w, h };
        SDL_RenderTexture(renderer, texture, nullptr, &dst);

        SDL_DestroyTexture(texture);

        SDL_RenderPresent(renderer);
        SDL_Delay(100);

        return SDL_APP_CONTINUE;
    }


    if (showMenu) {
        menu->render();
    }
    else {
        camera->update(player->getDest());

        player->otrisovka();
        player->obnovleniepersa();

        // Проверка попадания снаряда Fireball по всем врагам
        for (Skill* skill : player->getSkills()) {
            FireballSkill* fireball = dynamic_cast<FireballSkill*>(skill);
            if (fireball && fireball->isActive()) {
                for (Enemy* enemy : enemies) {
                    if (checkCollision(fireball->getRect(), enemy->getRect())) {
                        enemy->takeDamage(10);
                        fireball->deactivate();
                        break; // чтобы не нанести урон нескольким врагам одним шаром за кадр
                    }
                }
            }
        }

        // Проверка урона от ближней атаки игрока
        if (player->getIsAttack() && player->getCurrentAttackFrame() == 5) {
            SDL_FRect attackBox = player->getAttackHitbox();
            for (Enemy* enemy : enemies) {
                if (checkCollision(attackBox, enemy->getRect())) {
                    enemy->takeDamage(10);
                }
            }
        }

        // Обновление и отрисовка всех врагов с удалением мёртвых
        for (auto it = enemies.begin(); it != enemies.end();) {
            Enemy* enemy = *it;
            enemy->update(0.016f, player);
            enemy->render(renderer, camera);

            if (enemy->isMarkedForDeletion()) {
                delete enemy;
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }
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
    for (Enemy* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

}
