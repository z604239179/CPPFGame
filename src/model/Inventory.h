#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "common/Common.h"
#include "model/Item.h"

namespace game {

class Equipment;

// 玩家背包 + 装备栏（Model）
class Inventory {
public:
    Inventory();

    // 拾取物品：消耗品自动堆叠，否则放入空格
    bool addItem(std::unique_ptr<Item> item);

    // 使用消耗品：返回恢复值并扣减数量；失败返回 false
    bool useConsumable(int slot, int& restoreHp, int& restoreMp);

    // 装备物品：与对应装备栏交换
    bool equip(int slot);

    // 出售 / 丢弃
    bool sell(int slot, int& goldGain);
    bool drop(int slot);

    Item* itemAt(int slot);
    const Item* itemAt(int slot) const;
    Item* equippedAt(EquipmentSlot slot);
    const Item* equippedAt(EquipmentSlot slot) const;

    int itemCount() const;
    int firstEmptySlot() const;
    bool isFull() const;

    // 序列化（网络快照与存档通用）：slot:tag:id:count / slot:tag:id
    std::string serializeItems() const;
    std::string serializeEquipment() const;
    bool loadItems(const std::string& data);
    bool loadEquipment(const std::string& data);

private:
    std::vector<std::unique_ptr<Item>> slots_;
    std::array<std::unique_ptr<Item>, kEquipmentSlots> equipment_;
};

}  // namespace game
