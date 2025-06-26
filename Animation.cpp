#include "Animation.h"

Animation::Animation() {}
Animation::~Animation() {}

void Animation::update(const AnimationSet& anim, SDL_FRect& src, int frameWidth, bool loop) {
    frameCount = anim.frameCount;  // ÎÁßÇÀÒÅËÜÍÎ îáíîâëÿòü frameCount äëÿ êîððåêòíîé ðàáîòû

    Uint32 now = SDL_GetTicks();

    if (lastUpdate == 0) {
        lastUpdate = now;
        return; // ïåðâûé âûçîâ — íå äâèãàåì êàäðû
    }

    Uint64 delta = now - lastUpdate;
    elapsedTime += static_cast<int>(delta);
    lastUpdate = now;

    if (elapsedTime >= anim.frameDelay) {
        elapsedTime = 0;
        currentFrame++;
        if (currentFrame >= anim.frameCount) {
            if (loop) {
                currentFrame = 0; // ñáðîñ äëÿ öèêëà
            }
            else {
                currentFrame = anim.frameCount - 1; // îñòàíîâêà íà ïîñëåäíåì êàäðå
            }
        }
    }

    src.x = static_cast<float>(currentFrame * frameWidth);
    src.y = 0; // åñëè ó âàñ îäíà ñòðîêà êàäðîâ, ôèêñèðóåì Y íà 0
}


void Animation::reset() {
    currentFrame = 0;
    elapsedTime = 0;
    lastUpdate = 0;
    // Ëó÷øå ñáðîñ src äåëàòü â âûçûâàþùåì êîäå (Player)
}

bool Animation::isFinished() const {
    return currentFrame >= frameCount - 1;
}