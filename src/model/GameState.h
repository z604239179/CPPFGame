#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "common/Common.h"
#include "model/Player.h"
#include "model/WorldMap.h"

namespace game {

// 世界状态（Model 核心）：玩家、地图、规则。不依赖 View / 网络。
class GameState {
public:
    GameState();

    Player* findPlayer(int id);
    const Player* findPlayer(int id) const;
    Player& addPlayer(const std::string& name);
    void removePlayer(int id);
    int playerCount() const;
    const std::vector<Player>& players() const { return players_; }

    // 玩家动作（单机与服务器共用同一套规则）
    bool movePlayer(int id, Direction dir);
    bool startFight(int id, int slot);
    bool fightRound(int id, FightAction action);
    void talkToNpc(int id, int slot);
    bool useItem(int id, int slot);
    bool sellItem(int id, int slot);
    bool dropItem(int id, int slot);
    bool buyItem(int id, int shopIndex);

    // 消息队列
    void addMessage(const std::string& msg);
    const std::vector<std::string>& messages() const;
    std::vector<std::string> takePendingMessages();

    // 网络快照 / 存档
    std::string serializeSnapshot(int selfId) const;
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

private:
    WorldMap& mapOf(Player& p);
    const WorldMap& mapOf(const Player& p) const;
    Monster* combatMonster(Player& p);
    void levelUp(Player& p);
    void grantRewards(Player& p, Monster& m);
    std::unique_ptr<Item> rollDrop(int monsterLevel);
    std::string playerSerialize(const Player& p, bool full) const;

    std::vector<Player> players_;
    std::vector<WorldMap> maps_;
    std::vector<std::string> messages_;
    int nextPlayerId_ = 1;
};

}  // namespace game
