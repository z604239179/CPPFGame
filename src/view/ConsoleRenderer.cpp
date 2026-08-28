#include "view/ConsoleRenderer.h"

#include <algorithm>
#include <iostream>

#include <windows.h>

#include "model/GameData.h"

namespace game {

namespace {
WORD toWindowsColor(ConsoleColor color) {
    switch (color) {
        case ConsoleColor::Red:    return FOREGROUND_RED | FOREGROUND_INTENSITY;
        case ConsoleColor::Blue:   return FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case ConsoleColor::Green:  return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case ConsoleColor::Yellow: return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        case ConsoleColor::Purple: return FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case ConsoleColor::Cyan:   return FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        case ConsoleColor::Normal:
        default:                   return 7;
    }
}
}  // namespace

void ConsoleRenderer::clear() const { system("cls"); }

void ConsoleRenderer::setColor(ConsoleColor color) const {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), toWindowsColor(color));
}

void ConsoleRenderer::printLoginMenu() const {
    clear();
    setColor(ConsoleColor::Yellow);
    std::cout << "\n\n\n";
    std::cout << "================ 冒险大陆 ================\n";
    std::cout << "+           1. 开始新游戏               +\n";
    std::cout << "+           2. 读取存档                 +\n";
    std::cout << "+           3. 关于制作                 +\n";
    std::cout << "+           4. 退出游戏                 +\n";
    std::cout << "==========================================\n";
    std::cout << "\n                              Version: Beta";
    setColor(ConsoleColor::Normal);
}

void ConsoleRenderer::printAbout() const {
    clear();
    setColor(ConsoleColor::Green);
    std::cout << "▲\t\t树木\n";
    setColor(ConsoleColor::Normal);
    std::cout << "█\t\t围墙\n";
    setColor(ConsoleColor::Yellow);
    std::cout << "●\t\t玩家\n";
    setColor(ConsoleColor::Purple);
    std::cout << "◆\t\tNPC\n";
    setColor(ConsoleColor::Normal);
    std::cout << "■\t\t安全区\n";
    setColor(ConsoleColor::Cyan);
    std::cout << "O\t\t传送门\n\n\n";

    setColor(ConsoleColor::Yellow);
    std::cout << "\t制作人：Ssaturday（重构版）\n";
    setColor(ConsoleColor::Normal);
    system("pause");
}

void ConsoleRenderer::printBar(const char* label, int cur, int max, ConsoleColor color) const {
    std::cout << label << ": " << cur << "/" << max << " [";
    int filled = 0;
    if (max > 0) {
        filled = static_cast<int>(static_cast<long long>(cur) * 20 / max);
    }
    filled = std::clamp(filled, 0, 20);
    setColor(color);
    for (int i = 0; i < filled; ++i) std::cout << "█";
    setColor(ConsoleColor::Normal);
    for (int i = filled; i < 20; ++i) std::cout << "░";
    std::cout << "]" << std::endl;
}

void ConsoleRenderer::printMain(const WorldSnapshot& snap,
                                const std::vector<std::string>& messages) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) {
        std::cout << "正在等待服务器同步……" << std::endl;
        return;
    }

    std::cout << "===== " << self->name << "  Lv." << self->level
              << "  金币:" << self->gold << " =====" << std::endl;
    printBar("HP", self->stats.hp, self->stats.maxHp, ConsoleColor::Red);
    printBar("MP", self->stats.mp, self->stats.maxMp, ConsoleColor::Blue);
    std::cout << "攻击:" << self->stats.atk << "  防御:" << self->stats.def
              << "  经验:" << static_cast<int>(self->exp) << "/"
              << static_cast<int>(self->maxExp) << std::endl;
    std::cout << "位置:" << mapName(self->mapId) << " (" << self->pos.x << "," << self->pos.y << ")";
    if (self->inCombat) std::cout << "  [战斗中]";
    std::cout << std::endl << std::endl;

    printMap(snap);
    printOccupants(snap);
    printPlayers(snap);
    printMessages(messages);
    std::cout << "WASD移动  1/2/3互动  I背包  C状态  M任务  P保存  Q退出" << std::endl;
}

void ConsoleRenderer::printMap(const WorldSnapshot& snap) const {
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    const auto& layout = GameData::layouts()[self->mapId];
    for (int x = 0; x < kMapSize; ++x) {
        for (int y = 0; y < kMapSize; ++y) {
            const PlayerView* playerHere = nullptr;
            for (const auto& pl : snap.players) {
                if (pl.mapId == self->mapId && pl.pos.x == x && pl.pos.y == y) {
                    playerHere = &pl;
                    break;
                }
            }
            if (playerHere) {
                const bool isSelf = playerHere->id == snap.selfId;
                setColor(isSelf ? ConsoleColor::Yellow : ConsoleColor::Cyan);
                std::cout << (isSelf ? "●" : "@");
                continue;
            }
            switch (static_cast<TileType>(layout[x][y])) {
                case TileType::Space:
                    std::cout << " ";
                    break;
                case TileType::Wall:
                    setColor(ConsoleColor::Normal);
                    std::cout << "█";
                    break;
                case TileType::Tree:
                    setColor(ConsoleColor::Green);
                    std::cout << "▲";
                    break;
                case TileType::Safe:
                    setColor(ConsoleColor::Normal);
                    std::cout << "■";
                    break;
                case TileType::People:
                    setColor(ConsoleColor::Purple);
                    std::cout << "◆";
                    break;
                case TileType::Portal:
                    setColor(ConsoleColor::Cyan);
                    std::cout << "O";
                    break;
            }
        }
        std::cout << std::endl;
    }
    setColor(ConsoleColor::Normal);
}

void ConsoleRenderer::printOccupants(const WorldSnapshot& snap) const {
    if (snap.occupants.empty()) {
        std::cout << "此地空无一人。" << std::endl;
        return;
    }
    std::cout << "此地: ";
    for (size_t i = 0; i < snap.occupants.size(); ++i) {
        const auto& o = snap.occupants[i];
        std::cout << (i + 1) << ".";
        if (o.isNpc) {
            setColor(ConsoleColor::Purple);
        } else {
            switch (o.quality) {
                case ItemQuality::Unusual: setColor(ConsoleColor::Blue); break;
                case ItemQuality::Epic:    setColor(ConsoleColor::Purple); break;
                case ItemQuality::Legend:  setColor(ConsoleColor::Yellow); break;
                default:                   setColor(ConsoleColor::Normal); break;
            }
        }
        std::cout << o.name;
        setColor(ConsoleColor::Normal);
        std::cout << " Lv." << o.level;
        if (!o.isNpc && o.maxHp > 0) std::cout << " HP:" << o.hp << "/" << o.maxHp;
        std::cout << "   ";
    }
    std::cout << std::endl;
}

void ConsoleRenderer::printPlayers(const WorldSnapshot& snap) const {
    std::cout << "[玩家] ";
    for (const auto& pl : snap.players) {
        std::cout << pl.name << "(" << pl.pos.x << "," << pl.pos.y << ")  ";
    }
    std::cout << std::endl;
}

void ConsoleRenderer::printMessages(const std::vector<std::string>& messages) const {
    const size_t start = messages.size() > 3 ? messages.size() - 3 : 0;
    for (size_t i = start; i < messages.size(); ++i) {
        std::cout << "> " << messages[i] << std::endl;
    }
}

const char* ConsoleRenderer::qualityText(ItemQuality q) {
    switch (q) {
        case ItemQuality::Normal:  return "普通";
        case ItemQuality::Unusual: return "稀有";
        case ItemQuality::Epic:    return "史诗";
        case ItemQuality::Legend:  return "传说";
    }
    return "普通";
}

void ConsoleRenderer::printItemName(char tag, int id) const {
    if (tag == 'C') {
        const auto* def = GameData::consumableById(id);
        if (!def) return;
        switch (def->quality) {
            case ItemQuality::Unusual: setColor(ConsoleColor::Blue); break;
            case ItemQuality::Epic:    setColor(ConsoleColor::Purple); break;
            case ItemQuality::Legend:  setColor(ConsoleColor::Yellow); break;
            default:                   setColor(ConsoleColor::Normal); break;
        }
        std::cout << def->name;
    } else if (tag == 'E') {
        const auto* def = GameData::equipmentById(id);
        if (!def) return;
        switch (def->quality) {
            case ItemQuality::Unusual: setColor(ConsoleColor::Blue); break;
            case ItemQuality::Epic:    setColor(ConsoleColor::Purple); break;
            case ItemQuality::Legend:  setColor(ConsoleColor::Yellow); break;
            default:                   setColor(ConsoleColor::Normal); break;
        }
        std::cout << def->name;
    }
    setColor(ConsoleColor::Normal);
}

void ConsoleRenderer::printInventory(const WorldSnapshot& snap, int page) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    std::cout << "===== 背包 第" << (page + 1) << "/3 页 =====" << std::endl;
    for (int i = 0; i < 10; ++i) {
        const int slot = page * 10 + i;
        const ItemView* found = nullptr;
        for (const auto& it : self->inventory) {
            if (it.slot == slot) { found = &it; break; }
        }
        std::cout << i << ".";
        if (found) {
            printItemName(found->tag, found->itemId);
            std::cout << "  x" << found->count << std::endl;
        } else {
            std::cout << " 空" << std::endl;
        }
    }
    std::cout << std::endl << "0-9 查看物品  J下一页  K上一页  L返回" << std::endl;
}

void ConsoleRenderer::printItemDetail(const WorldSnapshot& snap, int slot) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    const ItemView* found = nullptr;
    for (const auto& it : self->inventory) {
        if (it.slot == slot) { found = &it; break; }
    }
    if (!found) {
        std::cout << "该格为空。" << std::endl;
        return;
    }

    std::cout << "===== 物品详情 =====" << std::endl;
    printItemName(found->tag, found->itemId);
    std::cout << std::endl;
    if (found->tag == 'C') {
        const auto* def = GameData::consumableById(found->itemId);
        if (def) std::cout << "品质: " << qualityText(def->quality) << std::endl;
    } else {
        const auto* def = GameData::equipmentById(found->itemId);
        if (def) {
            std::cout << "品质: " << qualityText(def->quality) << std::endl;
            std::cout << "部位: " << static_cast<int>(def->slot) << "  攻击+"
                      << def->atk << " 防御+" << def->def << " HP+" << def->hp
                      << " MP+" << def->mp << std::endl;
        }
    }
    std::cout << "数量: " << found->count << "  售价: ";
    if (found->tag == 'C') {
        const auto* def = GameData::consumableById(found->itemId);
        if (def) std::cout << def->price;
    } else {
        const auto* def = GameData::equipmentById(found->itemId);
        if (def) std::cout << def->price;
    }
    std::cout << std::endl << std::endl;
    std::cout << "J 使用/装备  K 丢弃  S 出售  L 返回" << std::endl;
}

void ConsoleRenderer::printStatus(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    std::cout << "名称:" << self->name << "  等级:" << self->level
              << "  金币:" << self->gold << std::endl;
    std::cout << "生命值:" << self->stats.hp << "/" << self->stats.maxHp
              << "  魔法值:" << self->stats.mp << "/" << self->stats.maxMp << std::endl;
    std::cout << "攻击力:" << self->stats.atk << "  防御力:" << self->stats.def << std::endl << std::endl;

    static const char* slotNames[] = {"头部", "上衣", "武器", "裤子", "鞋子"};
    for (int i = 0; i < kEquipmentSlots; ++i) {
        std::cout << slotNames[i] << ":";
        bool found = false;
        for (const auto& it : self->equipment) {
            if (it.slot == i) {
                printItemName(it.tag, it.itemId);
                found = true;
                break;
            }
        }
        if (!found) std::cout << "无";
        std::cout << std::endl;
    }
    std::cout << std::endl << "                                    L--返回" << std::endl;
}

void ConsoleRenderer::printMission(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    const auto* mission = GameData::missionById(self->missionId);
    std::cout << "===== 任务 =====" << std::endl;
    if (!mission) {
        std::cout << "暂无任务。" << std::endl;
    } else {
        std::cout << "任务: " << mission->name << std::endl;
        std::cout << "描述: " << mission->description << std::endl;
        std::cout << "奖励: " << mission->rewardGold << " 金币, "
                  << static_cast<int>(mission->rewardExp) << " 经验" << std::endl;
        switch (self->missionState) {
            case MissionState::None:      std::cout << "状态: 未接取" << std::endl; break;
            case MissionState::Accepted:  std::cout << "状态: 进行中" << std::endl; break;
            case MissionState::Completed: std::cout << "状态: 已完成" << std::endl; break;
        }
    }
    std::cout << std::endl << "                                    L--返回" << std::endl;
}

void ConsoleRenderer::printCombat(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    std::cout << "===== 战斗 =====" << std::endl;
    printBar("HP", self->stats.hp, self->stats.maxHp, ConsoleColor::Red);
    printBar("MP", self->stats.mp, self->stats.maxMp, ConsoleColor::Blue);

    if (self->combatSlot >= 0 &&
        self->combatSlot < static_cast<int>(snap.occupants.size())) {
        const auto& o = snap.occupants[self->combatSlot];
        std::cout << "对手: ";
        setColor(ConsoleColor::Red);
        std::cout << o.name;
        setColor(ConsoleColor::Normal);
        std::cout << " Lv." << o.level << std::endl;
        printBar("怪HP", o.hp, o.maxHp, ConsoleColor::Red);
    }
    std::cout << std::endl << "1.攻击  2.技能攻击  3.使用物品  4.逃跑" << std::endl;
}

void ConsoleRenderer::printShop(const WorldSnapshot& snap, NpcRole role) const {
    clear();
    std::cout << "===== 商店 =====" << std::endl;

    if (role == NpcRole::Merchant) {
        const auto& list = GameData::shopConsumables();
        for (size_t i = 0; i < list.size(); ++i) {
            const auto& d = list[i];
            std::cout << (i + 1) << ".";
            printItemName('C', d.id);
            std::cout << "  价格:" << static_cast<int>(d.price * 1.5f) << " 金币  品质:"
                      << qualityText(d.quality) << std::endl;
        }
    } else if (role == NpcRole::Blacksmith) {
        const auto& list = GameData::shopEquipment();
        for (size_t i = 0; i < list.size(); ++i) {
            const auto& d = list[i];
            std::cout << (i + 1) << ".";
            printItemName('E', d.id);
            std::cout << "  价格:" << static_cast<int>(d.price * 1.5f) << " 金币  品质:"
                      << qualityText(d.quality) << std::endl;
        }
    } else {
        std::cout << "这里没有商店。" << std::endl;
    }
    std::cout << std::endl << "输入数字购买, L 返回" << std::endl;
}

void ConsoleRenderer::printNpcMenu(const std::vector<std::string>& messages, bool hasShop) const {
    clear();
    printMessages(messages);
    std::cout << std::endl;
    std::cout << "1.任务  2.交易  3.离开" << std::endl;
    if (!hasShop) std::cout << "(这位 NPC 没有商店)" << std::endl;
}

void ConsoleRenderer::printMessage(const std::string& text) const {
    clear();
    std::cout << text << std::endl;
    Sleep(1500);
}

}  // namespace game
