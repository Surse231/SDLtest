#include "Animation.h"

Animation::Animation() {}

Animation::~Animation() {}

void Animation::reset() {
    currentFrame = 0;
    finished = false;
    elapsedTime = 0;
    lastUpdate = SDL_GetTicks();
}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, int frameHeight) {
    if (finished) return;

    Uint64 now = SDL_GetTicks();
    if (now - lastUpdate >= anim.frameDelay) {
        lastUpdate = now;

        currentFrame++;
        if (currentFrame >= anim.frameCount) {
            currentFrame = anim.frameCount - 1;
            finished = true;
        }

        src.x = static_cast<float>(currentFrame) * frameWidth;
        src.y = 0;  // фиксируем y (если анимация 1-рядная)
        src.w = static_cast<float>(frameWidth);
        src.h = static_cast<float>(frameHeight);
    }
}