#include "model/WorldMap.h"

#include <algorithm>
#include <random>
#include <utility>

#include "common/Common.h"
#include "model/GameData.h"

namespace game {

WorldMap::WorldMap(int mapId) : mapId(mapId) {
    buildLayout();
    spawnOccupants();
}

void WorldMap::buildLayout() {
    const auto& layout = GameData::layouts()[mapId];
    for (int x = 0; x < kMapSize; ++x) {
        for (int y = 0; y < kMapSize; ++y) {
            tiles_[x][y].type = static_cast<TileType>(layout[x][y]);
        }
    }
}

void WorldMap::spawnOccupants() {
    int npcIndex = 0;
    const auto& npcs = GameData::npcsForMap(mapId);

    // 收集所有空地，洗牌后取前 kMonstersPerMap 块刷怪（均匀分布、固定数量）
    std::vector<std::pair<int, int>> emptySpaces;
    for (int x = 0; x < kMapSize; ++x) {
        for (int y = 0; y < kMapSize; ++y) {
            if (tiles_[x][y].type == TileType::Space) {
                emptySpaces.emplace_back(x, y);
            } else if (tiles_[x][y].type == TileType::People &&
                       npcIndex < static_cast<int>(npcs.size())) {
                const auto& def = npcs[npcIndex++];
                Monster npc(def.id, def.name, def.level, ItemQuality::Legend);
                npc.isNpc = true;
                npc.role = def.role;
                tiles_[x][y].occupants.push_back(std::move(npc));
            }
        }
    }

    std::shuffle(emptySpaces.begin(), emptySpaces.end(),
                 std::mt19937(std::random_device{}()));
    const int spawnCount = std::min(kMonstersPerMap,
                                    static_cast<int>(emptySpaces.size()));
    for (int i = 0; i < spawnCount; ++i) {
        spawnMonsterAt(emptySpaces[i].first, emptySpaces[i].second);
    }
}

bool WorldMap::isWalkable(int x, int y) const {
    if (x < 0 || x >= kMapSize || y < 0 || y >= kMapSize) return false;
    const TileType t = tiles_[x][y].type;
    return t != TileType::Wall && t != TileType::Tree;
}

Monster* WorldMap::occupantAt(int x, int y, int slot) {
    if (x < 0 || x >= kMapSize || y < 0 || y >= kMapSize) return nullptr;
    auto& occs = tiles_[x][y].occupants;
    return (slot >= 0 && slot < static_cast<int>(occs.size())) ? &occs[slot] : nullptr;
}

const Monster* WorldMap::occupantAt(int x, int y, int slot) const {
    if (x < 0 || x >= kMapSize || y < 0 || y >= kMapSize) return nullptr;
    const auto& occs = tiles_[x][y].occupants;
    return (slot >= 0 && slot < static_cast<int>(occs.size())) ? &occs[slot] : nullptr;
}

void WorldMap::spawnMonsterAt(int x, int y) {
    if (x < 0 || x >= kMapSize || y < 0 || y >= kMapSize) return;
    Tile& tile = tiles_[x][y];
    if (tile.type != TileType::Space) return;  // 只在空地刷怪，不覆盖 NPC
    const auto* def = GameData::randomMonster();
    if (tile.occupants.empty()) {
        tile.occupants.emplace_back(def->id, def->name, def->level, def->quality);
    } else if (!tile.occupants[0].isNpc) {
        tile.occupants[0] = Monster(def->id, def->name, def->level, def->quality);
    }
}

bool WorldMap::tick(long long nowMsValue) {
    bool changed = false;
    for (int x = 0; x < kMapSize; ++x) {
        for (int y = 0; y < kMapSize; ++y) {
            Tile& tile = tiles_[x][y];
            if (tile.respawnAtMs != 0 && nowMsValue >= tile.respawnAtMs) {
                tile.respawnAtMs = 0;
                spawnMonsterAt(x, y);
                changed = true;
            }
        }
    }
    return changed;
}

}  // namespace game
