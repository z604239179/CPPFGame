#include "model/Equipment.h"

namespace game {

Equipment::Equipment(int id, std::string name, int price, ItemQuality quality,
                     EquipmentSlot slot, int atk, int def, int hp, int mp)
    : Item(id, std::move(name), price, quality),
      slot(slot), atk(atk), def(def), hp(hp), mp(mp) {}

}  // namespace game
