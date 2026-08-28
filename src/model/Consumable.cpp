#include "model/Consumable.h"

namespace game {

Consumable::Consumable(int id, std::string name, int price, ItemQuality quality,
                       int restoreHp, int restoreMp)
    : Item(id, std::move(name), price, quality),
      restoreHp(restoreHp), restoreMp(restoreMp) {}

}  // namespace game
