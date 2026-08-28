#pragma once

#include <string>

#include "common/Common.h"
#include "model/Inventory.h"

namespace game {

// 玩家（Model）：只包含状态与数据，不包含输入/输出逻辑
class Player {
public:
    Player(int id, std::string name);

    int id = 0;
    std::string name;
    int level = 1;
    int gold = 0;
    float exp = 0.0f;
    float maxExp = 100.0f;
    Stats stats;
    int mapId = 0;
    Position pos{1, 2};
    Inventory inventory;

    int missionId = 0;
    MissionState missionState = MissionState::None;

    bool inCombat = false;
    int combatSlot = -1;
};

}  // namespace game
