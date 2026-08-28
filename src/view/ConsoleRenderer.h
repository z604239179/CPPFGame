#pragma once

#include <string>
#include <vector>

#include "common/Common.h"
#include "common/Snapshot.h"

namespace game {

// 控制台渲染器（View）：只负责把快照画出来，不包含任何规则
class ConsoleRenderer {
public:
    void clear() const;
    void setColor(ConsoleColor color) const;

    void printLoginMenu() const;
    void printAbout() const;

    void printMain(const WorldSnapshot& snap, const std::vector<std::string>& messages) const;
    void printInventory(const WorldSnapshot& snap, int page) const;
    void printItemDetail(const WorldSnapshot& snap, int slot) const;
    void printStatus(const WorldSnapshot& snap) const;
    void printMission(const WorldSnapshot& snap) const;
    void printCombat(const WorldSnapshot& snap) const;
    void printShop(const WorldSnapshot& snap, NpcRole role) const;
    void printNpcMenu(const std::vector<std::string>& messages, bool hasShop) const;
    void printMessage(const std::string& text) const;

private:
    void printBar(const char* label, int cur, int max, ConsoleColor color) const;
    void printMap(const WorldSnapshot& snap) const;
    void printOccupants(const WorldSnapshot& snap) const;
    void printPlayers(const WorldSnapshot& snap) const;
    void printMessages(const std::vector<std::string>& messages) const;
    void printItemName(char tag, int id) const;
    static const char* qualityText(ItemQuality q);
};

}  // namespace game
