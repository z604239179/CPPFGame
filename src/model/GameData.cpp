#include "model/GameData.h"

#include <cstdlib>
#include <utility>

#include "model/Consumable.h"
#include "model/Equipment.h"

namespace game {

const std::vector<EquipmentDef>& GameData::equipmentCatalog() {
    static const std::vector<EquipmentDef> list = {
        {1,  "头巾", 1,   ItemQuality::Normal, EquipmentSlot::Head,   0,  1,  0,  0},
        {2,  "布衣", 1,   ItemQuality::Normal, EquipmentSlot::Body,   0,  1,  0,  0},
        {3,  "树枝", 1,   ItemQuality::Normal, EquipmentSlot::Weapon, 1,  1,  0,  0},
        {4,  "遮羞布", 1, ItemQuality::Normal, EquipmentSlot::Legs,   0,  1,  0,  0},
        {5,  "布鞋", 1,   ItemQuality::Normal, EquipmentSlot::Boots,  0,  1,  0,  0},
        {6,  "木盔", 5,   ItemQuality::Normal, EquipmentSlot::Head,   0,  2,  0,  0},
        {7,  "木甲", 5,   ItemQuality::Normal, EquipmentSlot::Body,   0,  2,  0,  0},
        {8,  "木剑", 5,   ItemQuality::Normal, EquipmentSlot::Weapon, 3,  0,  0,  0},
        {9,  "木裤", 5,   ItemQuality::Normal, EquipmentSlot::Legs,   0,  2,  0,  0},
        {10, "木鞋", 5,   ItemQuality::Normal, EquipmentSlot::Boots,  0,  2,  0,  0},
        {11, "铜盔", 20,  ItemQuality::Unusual, EquipmentSlot::Head,  0,  4,  5,  0},
        {12, "铜甲", 20,  ItemQuality::Unusual, EquipmentSlot::Body,  0,  4,  5,  0},
        {13, "铜剑", 20,  ItemQuality::Unusual, EquipmentSlot::Weapon, 5, 0,  0,  0},
        {14, "铜裤", 20,  ItemQuality::Unusual, EquipmentSlot::Legs,  0,  4,  5,  0},
        {15, "铜靴", 20,  ItemQuality::Unusual, EquipmentSlot::Boots, 0,  4,  5,  0},
        {16, "银盔", 50,  ItemQuality::Unusual, EquipmentSlot::Head,  0, 10, 10,  0},
        {17, "银甲", 50,  ItemQuality::Unusual, EquipmentSlot::Body,  0, 10, 10,  0},
        {18, "银剑", 50,  ItemQuality::Unusual, EquipmentSlot::Weapon, 0, 12, 10, 0},
        {19, "银裤", 50,  ItemQuality::Unusual, EquipmentSlot::Legs,  0, 10, 10,  0},
        {20, "银靴", 50,  ItemQuality::Unusual, EquipmentSlot::Boots, 0, 10, 10,  0},
        {21, "金盔", 100, ItemQuality::Unusual, EquipmentSlot::Head,  3, 15, 10,  0},
        {22, "金甲", 100, ItemQuality::Unusual, EquipmentSlot::Body,  3, 15, 10,  0},
        {23, "金剑", 100, ItemQuality::Unusual, EquipmentSlot::Weapon, 23, 0, 0, 0},
        {24, "金裤", 100, ItemQuality::Unusual, EquipmentSlot::Legs,  3, 15, 10,  0},
        {25, "金靴", 100, ItemQuality::Unusual, EquipmentSlot::Boots, 3, 15, 10,  0},
    };
    return list;
}

const std::vector<ConsumableDef>& GameData::consumableCatalog() {
    static const std::vector<ConsumableDef> list = {
        {1, "微型生命药剂", 5,  ItemQuality::Normal, 10, 0},
        {2, "微型魔法药剂", 5,  ItemQuality::Normal, 0,  10},
        {3, "小型生命药剂", 10, ItemQuality::Unusual, 50, 0},
        {4, "小型魔法药剂", 10, ItemQuality::Unusual, 0,  50},
        {5, "生命药剂", 30, ItemQuality::Epic, 100, 0},
        {6, "魔法药剂", 30, ItemQuality::Epic, 0,  100},
        {7, "强力生命药剂", 50, ItemQuality::Legend, 200, 0},
        {8, "强力魔法药剂", 50, ItemQuality::Epic, 0, 200},
    };
    return list;
}

const std::vector<MonsterDef>& GameData::monsterCatalog() {
    static const std::vector<MonsterDef> list = {
        {0, "史莱姆", 1,   ItemQuality::Normal},
        {1, "哥布林", 5,   ItemQuality::Normal},
        {2, "野猪", 10,    ItemQuality::Normal},
        {3, "野狼", 30,    ItemQuality::Unusual},
        {4, "骷髅", 50,    ItemQuality::Unusual},
        {5, "僵尸", 70,    ItemQuality::Epic},
        {6, "屠夫", 100,   ItemQuality::Epic},
        {7, "精灵龙", 150, ItemQuality::Legend},
    };
    return list;
}

const std::vector<NpcDef>& GameData::npcCatalog() {
    static const std::vector<NpcDef> list = {
        {0, "商人", 10, NpcRole::Merchant},
        {1, "村长", 5,  NpcRole::Chief},
        {2, "铁匠", 10, NpcRole::Blacksmith},
        {3, "牧童", 1,  NpcRole::Shepherd},
        {4, "士兵", 25, NpcRole::None},
        {5, "商人", 10, NpcRole::Merchant},
        {6, "铁匠", 10, NpcRole::Blacksmith},
        {7, "镇长", 5,  NpcRole::Mayor},
    };
    return list;
}

const std::vector<NpcDef>& GameData::npcsForMap(int mapId) {
    static const std::vector<NpcDef> village = {npcCatalog()[0], npcCatalog()[1], npcCatalog()[2], npcCatalog()[3]};
    static const std::vector<NpcDef> town = {npcCatalog()[4], npcCatalog()[5], npcCatalog()[6], npcCatalog()[7]};
    return mapId == 0 ? village : town;
}

const std::vector<MissionDef>& GameData::missionCatalog() {
    static const std::vector<MissionDef> list = {
        {0, "诞生", "去找村长谈谈话。", 1, 5.0f, 1},
    };
    return list;
}

const std::vector<EquipmentDef>& GameData::shopEquipment() {
    static const std::vector<EquipmentDef> list = {
        equipmentCatalog()[0],  // 头巾
        equipmentCatalog()[1],  // 布衣
        equipmentCatalog()[2],  // 树枝
        equipmentCatalog()[3],  // 遮羞布
        equipmentCatalog()[4],  // 布鞋
    };
    return list;
}

const std::vector<ConsumableDef>& GameData::shopConsumables() {
    static const std::vector<ConsumableDef> list = {
        consumableCatalog()[0],  // 微型生命药剂
        consumableCatalog()[1],  // 微型魔法药剂
    };
    return list;
}

const std::array<std::array<std::array<int, kMapSize>, kMapSize>, kMapCount>& GameData::layouts() {
    static const std::array<std::array<std::array<int, kMapSize>, kMapSize>, kMapCount> data = {{
        // 地图 0：村庄（20x20，中央是带围墙的村核心，南侧有出口通向野外）
        {{
            {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
            {{1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,1}},
            {{1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,2,0,0,0,0,0,0,0,0,2,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,5,4,4,4,4,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,4,3,2,4,4,1,0,0,0,2,0,0,1}},
            {{1,0,0,0,0,0,1,4,2,2,3,4,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,4,3,4,4,4,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,4,4,3,4,4,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,1,1,1,4,1,1,1,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,2,0,0,0,0,0,0,0,0,0,2,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,1}},
            {{1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,1}},
            {{1,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,2,1}},
            {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
        }},
        // 地图 1：城镇
        {{
            {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
            {{1,5,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,2,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,2,2,2,0,0,0,0,0,0,0,2,0,0,1}},
            {{1,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,2,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            // 中央 3x3 安全区（(9,9) 为死亡复活点，安全区不刷怪）
            {{1,0,0,0,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,2,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,2,0,0,0,0,4,4,4,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}},
            {{1,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,1}},
            {{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}},
        }},
    }};
    return data;
}

const EquipmentDef* GameData::equipmentById(int id) {
    for (const auto& d : equipmentCatalog()) {
        if (d.id == id) return &d;
    }
    return nullptr;
}

const ConsumableDef* GameData::consumableById(int id) {
    for (const auto& d : consumableCatalog()) {
        if (d.id == id) return &d;
    }
    return nullptr;
}

const MonsterDef* GameData::monsterById(int id) {
    for (const auto& d : monsterCatalog()) {
        if (d.id == id) return &d;
    }
    return nullptr;
}

const NpcDef* GameData::npcById(int id) {
    for (const auto& d : npcCatalog()) {
        if (d.id == id) return &d;
    }
    return nullptr;
}

const MissionDef* GameData::missionById(int id) {
    for (const auto& d : missionCatalog()) {
        if (d.id == id) return &d;
    }
    return nullptr;
}

const MonsterDef* GameData::randomMonster() {
    const auto& list = monsterCatalog();
    return &list[rand() % list.size()];
}

std::unique_ptr<Item> GameData::createItem(char tag, int id, int count) {
    if (tag == 'C') {
        const auto* def = consumableById(id);
        if (!def) return nullptr;
        auto item = std::make_unique<Consumable>(def->id, def->name, def->price, def->quality,
                                                 def->restoreHp, def->restoreMp);
        item->count = count;
        return item;
    }
    if (tag == 'E') {
        const auto* def = equipmentById(id);
        if (!def) return nullptr;
        auto item = std::make_unique<Equipment>(def->id, def->name, def->price, def->quality,
                                                def->slot, def->atk, def->def, def->hp, def->mp);
        item->count = 1;
        return item;
    }
    return nullptr;
}

}  // namespace game
