#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "common/Common.h"
#include "model/Item.h"

namespace game {

struct EquipmentDef {
    int id = 0;
    std::string name;
    int price = 0;
    ItemQuality quality = ItemQuality::Normal;
    EquipmentSlot slot = EquipmentSlot::Head;
    int atk = 0;
    int def = 0;
    int hp = 0;
    int mp = 0;
};

struct ConsumableDef {
    int id = 0;
    std::string name;
    int price = 0;
    ItemQuality quality = ItemQuality::Normal;
    int restoreHp = 0;
    int restoreMp = 0;
};

struct MonsterDef {
    int id = 0;
    std::string name;
    int level = 1;
    ItemQuality quality = ItemQuality::Normal;
};

struct NpcDef {
    int id = 0;
    std::string name;
    int level = 1;
    NpcRole role = NpcRole::None;
};

struct MissionDef {
    int id = 0;
    std::string name;
    std::string description;
    int rewardGold = 0;
    float rewardExp = 0.0f;
    int targetNpcId = -1;
};

// 静态游戏数据目录（Model）
class GameData {
public:
    static const std::vector<EquipmentDef>& equipmentCatalog();
    static const std::vector<ConsumableDef>& consumableCatalog();
    static const std::vector<MonsterDef>& monsterCatalog();
    static const std::vector<NpcDef>& npcCatalog();
    static const std::vector<NpcDef>& npcsForMap(int mapId);
    static const std::vector<MissionDef>& missionCatalog();

    // 商店货物
    static const std::vector<EquipmentDef>& shopEquipment();
    static const std::vector<ConsumableDef>& shopConsumables();

    // 静态地图（0=空地 1=墙 2=树 3=NPC 4=安全区 5=传送门）
    static const std::array<std::array<std::array<int, kMapSize>, kMapSize>, kMapCount>& layouts();

    static const EquipmentDef* equipmentById(int id);
    static const ConsumableDef* consumableById(int id);
    static const MonsterDef* monsterById(int id);
    static const NpcDef* npcById(int id);
    static const MissionDef* missionById(int id);
    static const MonsterDef* randomMonster();

    static std::unique_ptr<Item> createItem(char tag, int id, int count);
};

}  // namespace game
