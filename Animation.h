// Animation.h
#pragma once
#include <SDL3/SDL.h>
#include "Structs.h"

class Animation {
public:
    Animation();
    ~Animation();
    void update(const AnimationSet& anim, SDL_FRect& src, int frameWidth);
    void reset();
    bool isFinished() const;

    int getCurrentFrame() const { return currentFrame; }
    bool isAnimationFinished() const { return finished; }

private:
    int currentFrame = 0;
    int elapsedTime = 0;
    int frameCount = 1;          // <-- вот оно
    Uint64 lastUpdate = 0;
};