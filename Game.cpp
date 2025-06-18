#include "Game.h"
#include <vector>
#include <string>
#include "FireballSkill.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "TileMap.h"
#include "Enemy.h"
#include <cstdlib>
#include <ctime>

bool checkCollision(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w && a.x + a.w > b.x &&
        a.y < b.y + b.h && a.y + a.h > b.y;
}


Game::Game() : font(nullptr), window(nullptr), renderer(nullptr), camera(nullptr), player(nullptr), menu(nullptr)
{
    // остальные инициализации, если нужны
}


Game::~Game()
{
    // Очистка ресурсов игры

    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    // Уничтожение окна и рендерера
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

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
    camera = new Camera(1920.0f, 1080.0f);


    player = new Player(renderer, font, camera);
    SDL_FRect playerRect = player->getRect();

    tileMap = new TileMap(renderer);
    tileMap->loadFromFile("assets/map/MEGATEST.json");

 
    float groundY = tileMap->getSpawnPoint().y;

    // Создаем врагов
 // Создаем врагов
    enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Default));
    enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Default));


    // Game::SDL_AppInit()

    enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Boar));
    enemies.push_back(new Enemy(renderer, 800.0f, 250.0f, EnemyType::Fox));
    enemies.push_back(new Enemy(renderer, 1200.0f, 230.0f, EnemyType::Bird));


    for (Enemy* enemy : enemies) {
        enemy->setCollisionRects(tileMap->getCollisionRects());
    }

    // Передаем врагов игроку
    player->setEnemies(enemies);

    player->setCollisions(tileMap->getCollisionRects());
    player->setPosition(tileMap->getSpawnPoint().x, tileMap->getSpawnPoint().y);

    SDL_FPoint spawn = tileMap->getSpawnPoint();
    player->setPosition(spawn.x, spawn.y);

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


    tileMap->renderLayer(renderer, camera, "Tile Layer 1");
    tileMap->renderLayer(renderer, camera, "Tile Layer 2");
    tileMap->renderLayer(renderer, camera, "Tile Layer 3");
    tileMap->renderLayer(renderer, camera, "Tile Layer 4");
    tileMap->renderLayer(renderer, camera, "Tile Layer 5");
    tileMap->renderLayer(renderer, camera, "Tile Layer 6");

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

        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); // жёлтые прямоугольники
        for (const auto& rect : tileMap->getCollisionRects()) {
            SDL_FRect screen = camera->apply(rect);
            SDL_RenderFillRect(renderer, &screen);
        }
        for (Enemy* enemy : enemies) {
            enemy->setCollisionRects(tileMap->getCollisionRects());
        }


        player->otrisovka();
        player->obnovleniepersa();

        return SDL_APP_CONTINUE;
    }


    if (showMenu) {
        menu->render();
    }
    else {
        const bool* keys = SDL_GetKeyboardState(nullptr);
        camera->update(player->getDest(), tileMap->getMapWidth(), tileMap->getMapHeight());
        if (keys[SDL_SCANCODE_EQUALS]) camera->zoom += 0.01f;
        if (keys[SDL_SCANCODE_MINUS]) camera->zoom -= 0.01f;
        if (camera->zoom < 0.5f) camera->zoom = 0.5f;
        if (camera->zoom > 4.0f) camera->zoom = 4.0f;


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
        player->renderInventory();

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
