#include "model/GameState.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>

#include "model/Equipment.h"
#include "model/GameData.h"

namespace game {

namespace {

// 在目标地图上找传送门旁边的空地作为落点
Position findPortalExit(const WorldMap& dest, int x, int y, Direction cameFrom) {
    auto isGoodExit = [&dest](int px, int py) {
        if (!dest.isWalkable(px, py)) return false;
        const TileType t = dest.tiles_[px][py].type;
        return t == TileType::Space || t == TileType::Safe;
    };

    // 1) 优先“穿过去”的位置：从传送门另一侧出来
    Position preferred{x, y};
    switch (cameFrom) {
        case Direction::Up:    preferred.x -= 1; break;
        case Direction::Down:  preferred.x += 1; break;
        case Direction::Left:  preferred.y -= 1; break;
        case Direction::Right: preferred.y += 1; break;
    }
    if (isGoodExit(preferred.x, preferred.y)) return preferred;

    // 2) 传送门四邻域里的任意空地
    const Position candidates[] = {
        {x + 1, y}, {x - 1, y}, {x, y + 1}, {x, y - 1},
    };
    for (const auto& c : candidates) {
        if (isGoodExit(c.x, c.y)) return c;
    }

    // 3) 兜底：目标地图上任意一块空地
    for (int i = 0; i < kMapSize; ++i) {
        for (int j = 0; j < kMapSize; ++j) {
            const TileType t = dest.tiles_[i][j].type;
            if (t == TileType::Space || t == TileType::Safe) return {i, j};
        }
    }
    return {5, 5};
}

}  // namespace

GameState::GameState() {
    maps_.emplace_back(0);
    maps_.emplace_back(1);
}

Player* GameState::findPlayer(int id) {
    for (auto& p : players_) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

const Player* GameState::findPlayer(int id) const {
    for (const auto& p : players_) {
        if (p.id == id) return &p;
    }
    return nullptr;
}

Player& GameState::addPlayer(const std::string& name) {
    players_.emplace_back(nextPlayerId_++, sanitizeName(name));
    return players_.back();
}

Player& GameState::addPlayer(Player p) {
    // 登录复用存档数据，但 id 由世界重新分配（存档 id 仅用于账号内部，无全局意义）
    p.id = nextPlayerId_++;
    p.inCombat = false;
    p.combatSlot = -1;
    players_.push_back(std::move(p));
    return players_.back();
}

void GameState::removePlayer(int id) {
    players_.erase(std::remove_if(players_.begin(), players_.end(),
                                  [id](const Player& p) { return p.id == id; }),
                   players_.end());
}

int GameState::playerCount() const { return static_cast<int>(players_.size()); }

WorldMap& GameState::mapOf(Player& p) { return maps_[p.mapId]; }
const WorldMap& GameState::mapOf(const Player& p) const { return maps_[p.mapId]; }

bool GameState::movePlayer(int id, Direction dir) {
    Player* p = findPlayer(id);
    if (!p || p->inCombat) return false;

    Position target = p->pos;
    switch (dir) {
        case Direction::Up:    --target.x; break;
        case Direction::Down:  ++target.x; break;
        case Direction::Left:  --target.y; break;
        case Direction::Right: ++target.y; break;
    }

    WorldMap& map = mapOf(*p);
    if (!map.isWalkable(target.x, target.y)) return false;

    if (map.tiles_[target.x][target.y].type == TileType::Portal) {
        const int destMapId = 1 - p->mapId;
        p->mapId = destMapId;
        p->pos = findPortalExit(maps_[destMapId], target.x, target.y, dir);
        addMessage("你穿过传送门，来到了" + std::string(mapName(p->mapId)) + "（" +
                   std::to_string(p->pos.x) + "," + std::to_string(p->pos.y) + "）。");
        return true;
    }

    p->pos = target;

    // 走上怪物所在格子时自动进入战斗
    const auto& occs = map.tiles_[target.x][target.y].occupants;
    if (!occs.empty() && !occs[0].isNpc) {
        startFight(id, 0);
    }
    return true;
}

bool GameState::startFight(int id, int slot) {
    Player* p = findPlayer(id);
    if (!p || p->inCombat) return false;

    Monster* occ = mapOf(*p).occupantAt(p->pos.x, p->pos.y, slot);
    if (!occ || occ->isNpc) return false;

    p->inCombat = true;
    p->combatSlot = slot;
    p->lastCombatAtMs = nowMs();
    addMessage("你遭遇了" + occ->name + "（Lv." + std::to_string(occ->level) + "）！");
    return true;
}

Monster* GameState::combatMonster(Player& p) {
    return mapOf(p).occupantAt(p.pos.x, p.pos.y, p.combatSlot);
}

// 对怪物造成伤害；返回 true 若怪物死亡
bool GameState::applyCombatDamage(Player& attacker, Monster& target, float damageMult) {
    const int raw = static_cast<int>(attacker.stats.atk * damageMult);
    const int damage = std::max(1, raw - target.stats.def);
    target.stats.hp -= damage;
    addMessage(attacker.name + "对" + target.name + "造成了 " + std::to_string(damage) + " 点伤害。");
    return target.stats.hp <= 0;
}

// 怪物反击
void GameState::counterAttack(Player& p, Monster& m) {
    const int mDamage = std::max(1, m.stats.atk - p.stats.def);
    p.stats.hp -= mDamage;
    addMessage(m.name + "对" + p.name + "造成了 " + std::to_string(mDamage) + " 点伤害。");
    if (p.stats.hp <= 0) {
        p.stats.hp = p.stats.maxHp / 2;
        p.stats.mp = p.stats.maxMp / 2;
        p.inCombat = false;
        p.combatSlot = -1;
        // 死亡：传送到城镇复活点（安全区）半血复活
        p.mapId = kReviveMapId;
        p.pos = kRevivePos;
        addMessage(p.name + "战斗失败，已在城镇复活点复活。");
    }
}

// 怪物死亡：给击杀者奖励 + 清同格所有战斗玩家 + 调度重生
void GameState::endMonsterCombat(const Player& killer) {
    WorldMap& wmap = maps_[killer.mapId];
    const int kx = killer.pos.x;
    const int ky = killer.pos.y;
    // 可能怪物已被其他玩家抢先击杀
    if (!wmap.tiles_[kx][ky].occupants.empty() && !wmap.tiles_[kx][ky].occupants[0].isNpc) {
        // 给最后一击者发奖励；多人击杀可改为均分，这里保持谁最后一下谁拿
        Player* pk = findPlayer(killer.id);
        if (pk) grantRewards(*pk, wmap.tiles_[kx][ky].occupants[0]);
        wmap.tiles_[kx][ky].occupants.clear();
        wmap.tiles_[kx][ky].respawnAtMs = nowMs() + kMonsterRespawnMs;
    }
    // 清除所有在同格的战斗状态
    for (auto& other : players_) {
        if (other.inCombat && other.mapId == killer.mapId &&
            other.pos.x == kx && other.pos.y == ky) {
            other.inCombat = false;
            other.combatSlot = -1;
        }
    }
}

bool GameState::normalAttack(int id) {
    Player* p = findPlayer(id);
    if (!p || !p->inCombat) return false;
    Monster* m = combatMonster(*p);
    if (!m) { p->inCombat = false; p->combatSlot = -1; return false; }

    p->lastCombatAtMs = nowMs();
    if (applyCombatDamage(*p, *m, 1.0f)) {
        endMonsterCombat(*p);
        return true;
    }
    counterAttack(*p, *m);
    return true;
}

bool GameState::useSkill(int id, int skillIndex) {
    Player* p = findPlayer(id);
    if (!p || !p->inCombat) return false;
    if (skillIndex < 0 || skillIndex >= kSkillCount) return false;

    const SkillDef& sk = skillDef(skillIndex);
    if (p->stats.mp < sk.mpCost) {
        // MP 不足：降级为普通攻击（不给提示，用户不输入时默认普攻也有同样行为）
        return normalAttack(id);
    }
    p->stats.mp -= sk.mpCost;
    p->lastCombatAtMs = nowMs();

    Monster* m = combatMonster(*p);

    // 治疗术
    if (sk.healPercent > 0) {
        const int heal = p->stats.maxHp * sk.healPercent / 100;
        p->stats.hp = std::min(p->stats.hp + heal, p->stats.maxHp);
        addMessage(p->name + "施放" + std::string(sk.name) + "，恢复 " + std::to_string(heal) + " 点生命。");
        if (m) counterAttack(*p, *m);
        return true;
    }

    if (!m) { p->inCombat = false; p->combatSlot = -1; return false; }

    if (applyCombatDamage(*p, *m, sk.damageMult)) {
        endMonsterCombat(*p);
        return true;
    }
    counterAttack(*p, *m);
    return true;
}

bool GameState::useCombatItem(int id, int invSlot) {
    Player* p = findPlayer(id);
    if (!p || !p->inCombat) return false;

    Item* item = p->inventory.itemAt(invSlot);
    if (!item || item->typeTag() != 'C') {
        addMessage("该格位没有可用的消耗品！");
        // 无物品不消耗回合，不重置计时
        return true;
    }
    const std::string itemName = item->name;
    int restoreHp = 0, restoreMp = 0;
    if (!p->inventory.useConsumable(invSlot, restoreHp, restoreMp)) {
        addMessage("使用失败！");
        return true;
    }
    p->stats.hp = std::min(p->stats.hp + restoreHp, p->stats.maxHp);
    p->stats.mp = std::min(p->stats.mp + restoreMp, p->stats.maxMp);
    addMessage(p->name + "使用了 " + itemName + "，恢复 " + std::to_string(restoreHp) +
              " 生命、" + std::to_string(restoreMp) + " 法力。");

    p->lastCombatAtMs = nowMs();
    Monster* m = combatMonster(*p);
    if (m) counterAttack(*p, *m);
    return true;
}

bool GameState::fleeCombat(int id) {
    Player* p = findPlayer(id);
    if (!p || !p->inCombat) return false;
    p->inCombat = false;
    p->combatSlot = -1;
    addMessage(p->name + "逃跑了！");
    return true;
}

// 自动战斗 Tick：每秒对 inCombat 玩家自动普攻一次
bool GameState::tickCombat(long long nowMsValue) {
    bool changed = false;
    for (auto& p : players_) {
        if (!p.inCombat) continue;
        if (nowMsValue - p.lastCombatAtMs < kCombatRoundMs) continue;
        // 可能怪物已被其他玩家击杀
        Monster* m = combatMonster(p);
        if (!m) {
            p.inCombat = false;
            p.combatSlot = -1;
            changed = true;
            continue;
        }
        if (normalAttack(p.id)) changed = true;
    }
    return changed;
}

void GameState::grantRewards(Player& p, Monster& m) {
    p.gold += m.goldReward;
    p.exp += static_cast<float>(m.expReward);
    addMessage("胜利！获得 " + std::to_string(m.goldReward) + " 金币、" +
               std::to_string(m.expReward) + " 经验。");

    auto drop = rollDrop(m.level);
    if (drop) {
        if (p.inventory.addItem(std::move(drop))) {
            addMessage("掉落物品已收入背包。");
        } else {
            addMessage("背包已满，无法拾取掉落物品。");
        }
    }
    levelUp(p);
}

std::unique_ptr<Item> GameState::rollDrop(int monsterLevel) {
    if (rand() % 100 < 30) return nullptr;  // 30% 掉落概率

    if (rand() % 2 == 0) {  // 消耗品
        int lo = 1, hi = 2;
        if (monsterLevel > 70) { lo = 7; hi = 8; }
        else if (monsterLevel > 30) { lo = 5; hi = 6; }
        else if (monsterLevel > 5) { lo = 3; hi = 4; }
        return GameData::createItem('C', lo + rand() % (hi - lo + 1), 1);
    }
    // 装备
    int lo = 1, hi = 5;
    if (monsterLevel > 70) { lo = 21; hi = 25; }
    else if (monsterLevel > 30) { lo = 11; hi = 20; }
    else if (monsterLevel > 5) { lo = 6; hi = 10; }
    return GameData::createItem('E', lo + rand() % (hi - lo + 1), 1);
}

void GameState::levelUp(Player& p) {
    while (p.exp >= p.maxExp) {
        p.exp -= p.maxExp;
        ++p.level;
        p.maxExp *= 1.5f;
        p.stats.atk += 1;
        p.stats.def += 1;
        p.stats.maxHp += 5;
        p.stats.hp = p.stats.maxHp;
        p.stats.maxMp += 5;
        p.stats.mp = p.stats.maxMp;
        addMessage("恭喜升级！当前等级 Lv." + std::to_string(p.level));
    }
}

void GameState::talkToNpc(int id, int slot) {
    Player* p = findPlayer(id);
    if (!p) return;

    Monster* occ = mapOf(*p).occupantAt(p->pos.x, p->pos.y, slot);
    if (!occ || !occ->isNpc) return;

    switch (occ->role) {
        case NpcRole::Merchant:
            addMessage("商人：嘿！我这里有些宝贝，要不要看看？");
            break;
        case NpcRole::Blacksmith:
            addMessage("铁匠：年轻人，买些新装备吧。");
            break;
        case NpcRole::Shepherd:
            addMessage("牧童：你是新来的吗？");
            break;
        case NpcRole::Mayor:
            addMessage("镇长：欢迎来到我们的城镇。");
            break;
        case NpcRole::Chief: {
            const auto* mission = GameData::missionById(p->missionId);
            if (mission && p->missionState == MissionState::None) {
                p->missionState = MissionState::Accepted;
                addMessage("村长：年轻人，先去找村长谈谈话吧。");
                addMessage("[任务] 诞生：去找村长谈谈话。");
            } else if (mission && p->missionState == MissionState::Accepted) {
                p->missionState = MissionState::Completed;
                p->gold += mission->rewardGold;
                p->exp += mission->rewardExp;
                addMessage("村长：干得好！这是你的奖励。");
                addMessage("[任务完成] 诞生  +" + std::to_string(mission->rewardGold) + " 金币 +" +
                           std::to_string(static_cast<int>(mission->rewardExp)) + " 经验");
                levelUp(*p);
            } else {
                addMessage("村长：年轻人，有何贵干啊。");
            }
            break;
        }
        default:
            addMessage("士兵：站住！没事不要乱跑。");
            break;
    }
}

bool GameState::useItem(int id, int slot) {
    Player* p = findPlayer(id);
    if (!p) return false;

    Item* item = p->inventory.itemAt(slot);
    if (!item) return false;
    const std::string itemName = item->name;

    if (item->typeTag() == 'C') {
        int restoreHp = 0, restoreMp = 0;
        if (!p->inventory.useConsumable(slot, restoreHp, restoreMp)) return false;
        p->stats.hp = std::min(p->stats.hp + restoreHp, p->stats.maxHp);
        p->stats.mp = std::min(p->stats.mp + restoreMp, p->stats.maxMp);
        addMessage("你使用了 " + itemName + "。");
        return true;
    }

    if (item->typeTag() == 'E') {
        const auto* eq = static_cast<const Equipment*>(item);
        const Item* cur = p->inventory.equippedAt(eq->slot);
        const auto* curEq = cur ? static_cast<const Equipment*>(cur) : nullptr;

        p->stats.atk += eq->atk - (curEq ? curEq->atk : 0);
        p->stats.def += eq->def - (curEq ? curEq->def : 0);
        p->stats.maxHp += eq->hp - (curEq ? curEq->hp : 0);
        p->stats.maxMp += eq->mp - (curEq ? curEq->mp : 0);
        p->stats.hp = std::min(p->stats.hp, p->stats.maxHp);
        p->stats.mp = std::min(p->stats.mp, p->stats.maxMp);

        if (p->inventory.equip(slot)) {
            addMessage("你装备了 " + itemName + "。");
            return true;
        }
    }
    return false;
}

bool GameState::sellItem(int id, int slot) {
    Player* p = findPlayer(id);
    if (!p) return false;

    Item* item = p->inventory.itemAt(slot);
    if (!item) return false;
    const std::string name = item->name;

    int gain = 0;
    if (!p->inventory.sell(slot, gain)) return false;
    p->gold += gain;
    addMessage("你出售了 " + name + "，获得 " + std::to_string(gain) + " 金币。");
    return true;
}

bool GameState::dropItem(int id, int slot) {
    Player* p = findPlayer(id);
    if (!p) return false;

    Item* item = p->inventory.itemAt(slot);
    if (!item) return false;
    const std::string name = item->name;

    if (!p->inventory.drop(slot)) return false;
    addMessage("你丢弃了 " + name + "。");
    return true;
}

bool GameState::buyItem(int id, int shopIndex) {
    Player* p = findPlayer(id);
    if (!p) return false;

    const auto& occs = mapOf(*p).tiles_[p->pos.x][p->pos.y].occupants;
    NpcRole role = NpcRole::None;
    for (const auto& occ : occs) {
        if (occ.isNpc && (occ.role == NpcRole::Merchant || occ.role == NpcRole::Blacksmith)) {
            role = occ.role;
            break;
        }
    }
    if (role == NpcRole::None) return false;

    if (role == NpcRole::Merchant) {
        const auto& list = GameData::shopConsumables();
        if (shopIndex < 0 || shopIndex >= static_cast<int>(list.size())) return false;
        const auto& def = list[shopIndex];
        const int price = static_cast<int>(def.price * 1.5f);
        if (p->gold < price) {
            addMessage("金币不足，买不起 " + def.name + "。");
            return false;
        }
        auto item = GameData::createItem('C', def.id, 1);
        if (!item || !p->inventory.addItem(std::move(item))) {
            addMessage("背包已满！");
            return false;
        }
        p->gold -= price;
        addMessage("你购买了 " + def.name + "，花费 " + std::to_string(price) + " 金币。");
        return true;
    }

    const auto& list = GameData::shopEquipment();
    if (shopIndex < 0 || shopIndex >= static_cast<int>(list.size())) return false;
    const auto& def = list[shopIndex];
    const int price = static_cast<int>(def.price * 1.5f);
    if (p->gold < price) {
        addMessage("金币不足，买不起 " + def.name + "。");
        return false;
    }
    auto item = GameData::createItem('E', def.id, 1);
    if (!item || !p->inventory.addItem(std::move(item))) {
        addMessage("背包已满！");
        return false;
    }
    p->gold -= price;
    addMessage("你购买了 " + def.name + "，花费 " + std::to_string(price) + " 金币。");
    return true;
}

bool GameState::tick(long long nowMsValue) {
    bool changed = false;
    for (auto& map : maps_) {
        if (map.tick(nowMsValue)) changed = true;
    }
    return changed;
}

void GameState::addMessage(const std::string& msg) {
    messages_.push_back(msg);
    if (messages_.size() > kMaxRecentMessages) {
        messages_.erase(messages_.begin());
    }
}

const std::vector<std::string>& GameState::messages() const { return messages_; }

std::vector<std::string> GameState::takePendingMessages() {
    std::vector<std::string> out = std::move(messages_);
    messages_.clear();
    return out;
}

std::string GameState::playerSerialize(const Player& p, bool full) const {
    std::string s;
    s += std::to_string(p.id) + ":" + p.name + ":";
    s += std::to_string(p.level) + ":" + std::to_string(p.gold) + ":";
    s += std::to_string(static_cast<int>(p.exp)) + ":" + std::to_string(static_cast<int>(p.maxExp)) + ":";
    s += std::to_string(p.stats.hp) + ":" + std::to_string(p.stats.maxHp) + ":";
    s += std::to_string(p.stats.mp) + ":" + std::to_string(p.stats.maxMp) + ":";
    s += std::to_string(p.stats.atk) + ":" + std::to_string(p.stats.def) + ":";
    s += std::to_string(p.mapId) + ":" + std::to_string(p.pos.x) + ":" + std::to_string(p.pos.y) + ":";
    s += std::to_string(p.inCombat ? 1 : 0) + ":" + std::to_string(p.combatSlot) + ":";
    s += std::to_string(p.missionId) + ":" + std::to_string(static_cast<int>(p.missionState)) + ":";

    if (full) {
        s += std::to_string(p.inventory.itemCount()) + ":" + p.inventory.serializeItems() + ":"
             + p.inventory.serializeEquipment();
    } else {
        s += "0::";
    }
    return s;
}

std::string GameState::serializeSnapshot(int selfId) const {
    std::string s = "S|" + std::to_string(selfId) + "|" + std::to_string(players_.size());
    for (const auto& p : players_) {
        s += "|" + playerSerialize(p, p.id == selfId);
    }

    const Player* self = findPlayer(selfId);
    if (self) {
        const auto& occs = maps_[self->mapId].tiles_[self->pos.x][self->pos.y].occupants;
        s += "|" + std::to_string(occs.size());
        for (const auto& occ : occs) {
            s += "|" + std::to_string(occ.id) + ":" + occ.name + ":" + std::to_string(occ.level) + ":"
                 + (occ.isNpc ? "1" : "0") + ":" + std::to_string(static_cast<int>(occ.role)) + ":"
                 + std::to_string(occ.stats.hp) + ":" + std::to_string(occ.stats.maxHp) + ":"
                 + std::to_string(static_cast<int>(occ.quality));
        }
    } else {
        s += "|0";
    }

    // 本地图上所有活着的怪物（用于客户端把怪物画到地图上）
    if (self) {
        const WorldMap& map = maps_[self->mapId];
        int monCount = 0;
        for (int x = 0; x < kMapSize; ++x) {
            for (int y = 0; y < kMapSize; ++y) {
                for (const auto& occ : map.tiles_[x][y].occupants) {
                    if (!occ.isNpc) ++monCount;
                }
            }
        }
        s += "|" + std::to_string(monCount);
        for (int x = 0; x < kMapSize; ++x) {
            for (int y = 0; y < kMapSize; ++y) {
                for (const auto& occ : map.tiles_[x][y].occupants) {
                    if (occ.isNpc) continue;
                    s += "|" + std::to_string(x) + ":" + std::to_string(y) + ":" + occ.name + ":"
                         + std::to_string(occ.level) + ":" + std::to_string(static_cast<int>(occ.quality));
                }
            }
        }
    } else {
        s += "|0";
    }
    return s;
}

bool GameState::saveToFile(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;

    out << players_.size() << "\n";
    for (const auto& p : players_) {
        out << p.serialize();
    }
    return true;
}

bool GameState::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;

    players_.clear();
    int count = 0;
    in >> count;
    for (int i = 0; i < count; ++i) {
        Player p;
        if (!p.deserialize(in)) break;
        nextPlayerId_ = std::max(nextPlayerId_, p.id + 1);
        players_.push_back(std::move(p));
    }
    return !players_.empty();
}

}  // namespace game
