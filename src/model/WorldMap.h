#pragma once

#include <vector>

#include "common/Common.h"
#include "model/Monster.h"

namespace game {

// 地图（Model）：静态地形 + 每格的动态怪物/NPC
class WorldMap {
public:
    struct Tile {
        TileType type = TileType::Space;
        std::vector<Monster> occupants;
    };

    explicit WorldMap(int mapId);

    int mapId = 0;
    Tile tiles_[kMapSize][kMapSize];

    bool isWalkable(int x, int y) const;
    Monster* occupantAt(int x, int y, int slot);
    const Monster* occupantAt(int x, int y, int slot) const;
    void respawnMonsterAt(int x, int y, int slot);

private:
    void buildLayout();
    void spawnOccupants();
};

}  // namespace game
