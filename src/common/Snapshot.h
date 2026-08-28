#pragma once

#include <deque>
#include <string>
#include <vector>

#include "common/Common.h"

namespace game {

// 客户端视角的背包物品
struct ItemView {
    int slot = 0;      // 背包格 / 装备槽位
    char tag = '0';    // 'C' 消耗品 / 'E' 装备
    int itemId = 0;
    int count = 0;
};

// 客户端视角的玩家
struct PlayerView {
    int id = 0;
    std::string name;
    int level = 1;
    int gold = 0;
    float exp = 0.0f;
    float maxExp = 100.0f;
    Stats stats;
    int mapId = 0;
    Position pos;
    bool inCombat = false;
    int combatSlot = -1;
    int missionId = 0;
    MissionState missionState = MissionState::None;
    std::vector<ItemView> inventory;  // 仅“自己”完整
    std::vector<ItemView> equipment;
};

// 客户端视角的格子内怪物 / NPC
struct OccupantView {
    int id = 0;
    std::string name;
    int level = 0;
    bool isNpc = false;
    NpcRole role = NpcRole::None;
    int hp = 0;
    int maxHp = 0;
    ItemQuality quality = ItemQuality::Normal;
};

// 客户端视角的地图上怪物（用于把怪物画到地图格子上）
struct MapMonsterView {
    int x = 0;
    int y = 0;
    std::string name;
    int level = 0;
    ItemQuality quality = ItemQuality::Normal;
};

// 一次完整的世界快照（服务器 -> 客户端）
struct WorldSnapshot {
    int selfId = 0;
    std::vector<PlayerView> players;
    std::vector<OccupantView> occupants;      // 自己所在格子的怪物 / NPC
    std::vector<MapMonsterView> mapMonsters;  // 自己所在地图的所有怪物
};

// 解析 "S|..." 形式的快照字符串
bool parseSnapshot(const std::string& payload, WorldSnapshot& out);

// 找到自己
const PlayerView* findSelf(const WorldSnapshot& snap);
PlayerView* findSelf(WorldSnapshot& snap);

}  // namespace game
