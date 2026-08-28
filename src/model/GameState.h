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
    Player& addPlayer(Player p);  // 加入已存在的玩家（登录时复用存档数据）
    void removePlayer(int id);
    int playerCount() const;
    const std::vector<Player>& players() const { return players_; }

    // 玩家动作（单机与服务器共用同一套规则）
    bool movePlayer(int id, Direction dir);
    bool startFight(int id, int slot);
    bool normalAttack(int id);                    // 无消耗普通攻击
    bool useSkill(int id, int skillIndex);        // 0-3 对应 Q/W/E/R
    bool useCombatItem(int id, int invSlot);       // 战斗中使用消耗品
    bool fleeCombat(int id);
    void talkToNpc(int id, int slot);
    bool useItem(int id, int slot);
    bool sellItem(int id, int slot);
    bool dropItem(int id, int slot);
    bool buyItem(int id, int shopIndex);

    // 时间系统：tick=怪物重生；tickCombat=自动战斗回合。返回是否有变化需要广播
    bool tick(long long nowMs);
    bool tickCombat(long long nowMs);

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
    bool applyCombatDamage(Player& attacker, Monster& target, float damageMult);
    void counterAttack(Player& p, Monster& m);
    void endMonsterCombat(const Player& killer);
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
