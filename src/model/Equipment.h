#pragma once

#include "model/Item.h"

namespace game {

class Equipment : public Item {
public:
    Equipment(int id, std::string name, int price, ItemQuality quality,
              EquipmentSlot slot, int atk, int def, int hp, int mp);

    EquipmentSlot slot = EquipmentSlot::Head;
    int atk = 0;
    int def = 0;
    int hp = 0;
    int mp = 0;

    char typeTag() const override { return 'E'; }
};

}  // namespace game
