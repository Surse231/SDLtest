#pragma once
#include <SDL3/SDL.h>
#include "Structs.h"

class Animation {
public:
    Animation();
    ~Animation();

    void update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, int frameHeight);



    void reset();

    int getCurrentFrame() const { return currentFrame; }
    bool isAnimationFinished() const { return finished; }

private:
    int currentFrame = 0;
    Uint64 lastUpdate = 0;
    bool finished = false;
    Uint32 elapsedTime = 0;
};