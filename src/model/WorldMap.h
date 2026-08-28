#pragma once

#include <vector>

#include "common/Common.h"
#include "model/Monster.h"

namespace game {

// 地图（Model）：静态地形 + 每格的动态怪物/NPC + 刷怪调度
class WorldMap {
public:
    struct Tile {
        TileType type = TileType::Space;
        std::vector<Monster> occupants;
        long long respawnAtMs = 0;  // 怪物被击杀后计划刷新的时间戳（0 = 无待刷新）
    };

    explicit WorldMap(int mapId);

    int mapId = 0;
    Tile tiles_[kMapSize][kMapSize];

    bool isWalkable(int x, int y) const;
    Monster* occupantAt(int x, int y, int slot);
    const Monster* occupantAt(int x, int y, int slot) const;
    void spawnMonsterAt(int x, int y);

    // 时间系统：刷新到期的怪物，返回本帧是否有怪物刷新
    bool tick(long long nowMs);

private:
    void buildLayout();
    void spawnOccupants();
};

}  // namespace game
