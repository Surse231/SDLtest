// Animation.cpp
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

    src.x = currentFrame * frameWidth;
}


void Animation::reset() {
    currentFrame = 0;
    elapsedTime = 0;
    lastUpdate = 0;
}

bool Animation::isFinished() const {
    return currentFrame >= frameCount - 1;
}
