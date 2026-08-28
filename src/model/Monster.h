#pragma once

#include <string>

#include "common/Common.h"

namespace game {

// 怪物 / NPC（Model）。NPC 通过 isNpc + role 标识
class Monster {
public:
    Monster(int id, std::string name, int level, ItemQuality quality);

    int id = 0;
    std::string name;
    int level = 1;
    ItemQuality quality = ItemQuality::Normal;
    Stats stats;
    int expReward = 0;
    int goldReward = 0;

    bool isNpc = false;
    NpcRole role = NpcRole::None;
};

}  // namespace game
