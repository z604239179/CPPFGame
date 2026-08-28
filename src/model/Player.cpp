#include "model/Player.h"

namespace game {

Player::Player(int id, std::string name)
    : id(id), name(std::move(name)) {
    stats.maxHp = 100;
    stats.hp = 100;
    stats.maxMp = 100;
    stats.mp = 100;
    stats.atk = 10;
    stats.def = 0;
}

}  // namespace game
