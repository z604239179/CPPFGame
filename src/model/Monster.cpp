#include "model/Monster.h"

namespace game {

Monster::Monster(int id, std::string name, int level, ItemQuality quality)
    : id(id), name(std::move(name)), level(level), quality(quality) {
    stats.maxHp = level * 5;
    stats.hp = stats.maxHp;
    stats.maxMp = 0;
    stats.mp = 0;
    stats.atk = level;
    stats.def = level;
    expReward = level * 100;
    goldReward = level * 10;
}

}  // namespace game
