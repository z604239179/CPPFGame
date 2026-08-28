#include "common/Snapshot.h"

#include <cstdlib>

namespace game {

const PlayerView* findSelf(const WorldSnapshot& snap) {
    for (const auto& p : snap.players) {
        if (p.id == snap.selfId) return &p;
    }
    return nullptr;
}

PlayerView* findSelf(WorldSnapshot& snap) {
    for (auto& p : snap.players) {
        if (p.id == snap.selfId) return &p;
    }
    return nullptr;
}

bool parseSnapshot(const std::string& payload, WorldSnapshot& out) {
    const auto parts = splitString(payload, '|');
    if (parts.size() < 4 || parts[0] != "S") return false;

    out.selfId = std::atoi(parts[1].c_str());
    const int playerCount = std::atoi(parts[2].c_str());
    const int occCountIndex = 3 + playerCount;
    if (occCountIndex >= static_cast<int>(parts.size())) return false;

    out.players.clear();
    out.players.reserve(playerCount);

    for (int i = 0; i < playerCount; ++i) {
        const auto f = splitString(parts[3 + i], ':');
        if (f.size() < 22) return false;

        PlayerView pv;
        pv.id = std::atoi(f[0].c_str());
        pv.name = f[1];
        pv.level = std::atoi(f[2].c_str());
        pv.gold = std::atoi(f[3].c_str());
        pv.exp = static_cast<float>(std::atof(f[4].c_str()));
        pv.maxExp = static_cast<float>(std::atof(f[5].c_str()));
        pv.stats.hp = std::atoi(f[6].c_str());
        pv.stats.maxHp = std::atoi(f[7].c_str());
        pv.stats.mp = std::atoi(f[8].c_str());
        pv.stats.maxMp = std::atoi(f[9].c_str());
        pv.stats.atk = std::atoi(f[10].c_str());
        pv.stats.def = std::atoi(f[11].c_str());
        pv.mapId = std::atoi(f[12].c_str());
        pv.pos.x = std::atoi(f[13].c_str());
        pv.pos.y = std::atoi(f[14].c_str());
        pv.inCombat = std::atoi(f[15].c_str()) != 0;
        pv.combatSlot = std::atoi(f[16].c_str());
        pv.missionId = std::atoi(f[17].c_str());
        pv.missionState = static_cast<MissionState>(std::atoi(f[18].c_str()));

        const int invCount = std::atoi(f[19].c_str());
        if (invCount > 0 && !f[20].empty()) {
            for (const auto& seg : splitString(f[20], ',')) {
                const auto s2 = splitString(seg, ':');
                if (s2.size() != 4) continue;
                ItemView iv;
                iv.slot = std::atoi(s2[0].c_str());
                iv.tag = s2[1].empty() ? '0' : s2[1][0];
                iv.itemId = std::atoi(s2[2].c_str());
                iv.count = std::atoi(s2[3].c_str());
                pv.inventory.push_back(iv);
            }
        }

        if (!f[21].empty()) {
            for (const auto& seg : splitString(f[21], ',')) {
                const auto s2 = splitString(seg, ':');
                if (s2.size() != 3 || s2[0].empty() || s2[1].empty() || s2[0][0] == '0') continue;
                ItemView iv;
                iv.slot = std::atoi(s2[0].c_str());
                iv.tag = s2[1][0];
                iv.itemId = std::atoi(s2[2].c_str());
                iv.count = 1;
                pv.equipment.push_back(iv);
            }
        }

        out.players.push_back(std::move(pv));
    }

    const int occCount = std::atoi(parts[occCountIndex].c_str());
    out.occupants.clear();
    out.occupants.reserve(occCount);
    for (int i = 0; i < occCount; ++i) {
        const int idx = occCountIndex + 1 + i;
        if (idx >= static_cast<int>(parts.size())) break;
        const auto f = splitString(parts[idx], ':');
        if (f.size() < 8) continue;
        OccupantView ov;
        ov.id = std::atoi(f[0].c_str());
        ov.name = f[1];
        ov.level = std::atoi(f[2].c_str());
        ov.isNpc = std::atoi(f[3].c_str()) != 0;
        ov.role = static_cast<NpcRole>(std::atoi(f[4].c_str()));
        ov.hp = std::atoi(f[5].c_str());
        ov.maxHp = std::atoi(f[6].c_str());
        ov.quality = static_cast<ItemQuality>(std::atoi(f[7].c_str()));
        out.occupants.push_back(ov);
    }
    return true;
}

}  // namespace game
