// DashSkill.cpp
#include "DashSkill.h"

void DashSkill::activate(Player* player) {
    SDL_FRect pos = player->gedDest();
    if (player->isFlipped()) // налево
        pos.x -= dashDistance;
    else
        pos.x += dashDistance;
    player->setPosition(pos.x, pos.y);
}
