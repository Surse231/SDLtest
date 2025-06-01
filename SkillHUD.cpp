// SkillHUD.cpp
#include "SkillHUD.h"

SkillHUD::SkillHUD(SDL_Renderer* renderer) : renderer(renderer) {}

SkillHUD::~SkillHUD() {}

void SkillHUD::addSkillIcon(SDL_Texture* tex, const std::string& name) {
    SDL_FRect rect = { 20 + (float)icons.size() * 0, 20, 100, 100 };
    icons.push_back({ tex, rect, name });
}

void SkillHUD::render() const {
    for (const auto& icon : icons) {
        SDL_RenderTexture(renderer, icon.icon, nullptr, &icon.rect);
    }
}
