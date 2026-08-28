#include "model/Item.h"

namespace game {

Item::Item(int id, std::string name, int price, ItemQuality quality)
    : id(id), name(std::move(name)), price(price), quality(quality) {}

}  // namespace game
