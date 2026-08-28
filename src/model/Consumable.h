#pragma once

#include "model/Item.h"

namespace game {

class Consumable : public Item {
public:
    Consumable(int id, std::string name, int price, ItemQuality quality,
               int restoreHp, int restoreMp);

    int restoreHp = 0;
    int restoreMp = 0;

    char typeTag() const override { return 'C'; }
};

}  // namespace game
