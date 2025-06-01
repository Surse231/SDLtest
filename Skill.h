// Skill.h
#pragma once
class Player;
class Skill {
public:
    virtual ~Skill() = default;
    virtual void activate(Player* player) = 0;
    virtual void update(Player* player) {}
};
