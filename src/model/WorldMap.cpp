#include "model/WorldMap.h"

#include <cstdlib>

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

    for (int x = 0; x < kMapSize; ++x) {
        for (int y = 0; y < kMapSize; ++y) {
            Tile& tile = tiles_[x][y];
            if (tile.type == TileType::Space) {
                // 每块空地放 3 只怪物（与原版一致）
                for (int k = 0; k < 3; ++k) {
                    const auto* def = GameData::randomMonster();
                    tile.occupants.emplace_back(def->id, def->name, def->level, def->quality);
                }
            } else if (tile.type == TileType::People && npcIndex < static_cast<int>(npcs.size())) {
                const auto& def = npcs[npcIndex++];
                Monster npc(def.id, def.name, def.level, ItemQuality::Legend);
                npc.isNpc = true;
                npc.role = def.role;
                tile.occupants.push_back(std::move(npc));
            }
        }
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

void WorldMap::respawnMonsterAt(int x, int y, int slot) {
    auto* occ = occupantAt(x, y, slot);
    if (!occ || occ->isNpc) return;
    const auto* def = GameData::randomMonster();
    *occ = Monster(def->id, def->name, def->level, def->quality);
}

}  // namespace game
