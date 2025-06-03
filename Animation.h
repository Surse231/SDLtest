// Animation.h
#pragma once
#include <SDL3/SDL.h>
#include "Structs.h"

class Animation {
public:
    Animation();
    ~Animation();

    void update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop = true);

    void reset();
    bool isFinished() const;

private:
    int currentFrame = 0;
    int elapsedTime = 0;
    int frameCount = 1;          // <-- ��� ���
    Uint64 lastUpdate = 0;
};
