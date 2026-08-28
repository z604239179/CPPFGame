#pragma once

#include <string>

#include "common/Common.h"

namespace game {

// 物品基类（Model）
class Item {
public:
    Item(int id, std::string name, int price, ItemQuality quality);
    virtual ~Item() = default;

    int id = 0;
    std::string name;
    int price = 0;
    ItemQuality quality = ItemQuality::Normal;
    int count = 1;

    // 类型标签：'C' 消耗品 / 'E' 装备（用于序列化与动态类型判断）
    virtual char typeTag() const = 0;
};

}  // namespace game
