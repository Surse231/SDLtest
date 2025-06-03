#include "Animation.h"

Animation::Animation() {}
Animation::~Animation() {}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop) {
    frameCount = anim.frameCount; // добавь сюда!

    Uint64 now = SDL_GetTicks();
    if (lastUpdate == 0) {
        lastUpdate = now;
    }

    Uint64 delta = now - lastUpdate;
    elapsedTime += static_cast<int>(delta);
    lastUpdate = now;

    if (elapsedTime >= anim.frameDelay) {
        elapsedTime = 0;
        currentFrame++;
        if (currentFrame >= anim.frameCount) {
            if (loop) {
                currentFrame = 0; // сброс для цикла
            }
            else {
                currentFrame = anim.frameCount - 1; // остановка на последнем кадре
            }
        }
    }

Animation::~Animation()
{
}


void Animation::reset() {
    currentFrame = 0;
    lastUpdate = SDL_GetTicks();
}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth) {
    Uint64 now = SDL_GetTicks();
    if (now - lastUpdate >= anim.frameDelay) {
        currentFrame = (currentFrame + 1) % anim.frameCount;
        src.x = currentFrame * frameWidth;
        src.y = 0;
        lastUpdate = now;
    }
}