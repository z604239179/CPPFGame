#include "model/Player.h"

#include <istream>
#include <sstream>

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

// 单玩家存档序列化：多行文本，与原 GameState::saveToFile 单玩家段格式兼容
std::string Player::serialize() const {
    std::ostringstream out;
    out << id << "\n" << name << "\n";
    out << level << " " << gold << " " << exp << " " << maxExp << "\n";
    out << stats.hp << " " << stats.maxHp << " " << stats.mp << " "
        << stats.maxMp << " " << stats.atk << " " << stats.def << "\n";
    out << mapId << " " << pos.x << " " << pos.y << "\n";
    out << missionId << " " << static_cast<int>(missionState) << "\n";
    out << "INV " << inventory.serializeItems() << "\n";
    out << "EQUIP " << inventory.serializeEquipment() << "\n";
    return out.str();
}

bool Player::deserialize(std::istream& in) {
    int idValue = 0;
    std::string nameValue;
    if (!(in >> idValue >> nameValue)) return false;
    id = idValue;
    name = nameValue;

    int missionStateValue = 0;
    if (!(in >> level >> gold >> exp >> maxExp)) return false;
    if (!(in >> stats.hp >> stats.maxHp >> stats.mp >> stats.maxMp
            >> stats.atk >> stats.def)) return false;
    if (!(in >> mapId >> pos.x >> pos.y)) return false;
    if (!(in >> missionId >> missionStateValue)) return false;
    missionState = static_cast<MissionState>(missionStateValue);

    std::string tag;
    if (!(in >> tag) || tag != "INV") return false;
    std::string invData;
    std::getline(in, invData);
    if (!invData.empty() && invData[0] == ' ') invData.erase(0, 1);
    inventory.loadItems(invData);

    if (!(in >> tag) || tag != "EQUIP") return false;
    std::string equipData;
    std::getline(in, equipData);
    if (!equipData.empty() && equipData[0] == ' ') equipData.erase(0, 1);
    inventory.loadEquipment(equipData);

    inCombat = false;
    combatSlot = -1;
    return true;
}

}  // namespace game
