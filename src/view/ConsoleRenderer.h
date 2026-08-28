#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "common/Common.h"
#include "common/Snapshot.h"

namespace game {

// 控制台渲染器（View）：只负责把快照画出来，不包含任何规则
// 使用双缓冲：先渲染到后台缓冲区，再一次性切换显示，消除 cls 闪烁
class ConsoleRenderer {
public:
    ConsoleRenderer();
    ~ConsoleRenderer();
    ConsoleRenderer(const ConsoleRenderer&) = delete;
    ConsoleRenderer& operator=(const ConsoleRenderer&) = delete;

    void clear() const;
    void setColor(ConsoleColor color) const;
    void present() const;  // 将后台缓冲区切换到屏幕显示

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
    class ConsoleStreamBuf;

    void ensureInit() const;
    void printBar(const char* label, int cur, int max, ConsoleColor color) const;
    void printMap(const WorldSnapshot& snap) const;
    void printOccupants(const WorldSnapshot& snap) const;
    void printPlayers(const WorldSnapshot& snap) const;
    void printMessages(const std::vector<std::string>& messages) const;
    void printItemName(char tag, int id) const;
    static const char* qualityText(ItemQuality q);

    // 双缓冲相关（void* 即 Windows HANDLE，避免在头文件引入 windows.h）
    mutable void* frontBuffer_ = nullptr;   // 当前可见的屏幕缓冲区
    mutable void* backBuffer_ = nullptr;    // 后台渲染缓冲区
    mutable void* originalStdout_ = nullptr;
    mutable bool initialized_ = false;
    mutable std::unique_ptr<ConsoleStreamBuf> backBuf_;
    mutable std::streambuf* originalCoutBuf_ = nullptr;
};

}  // namespace game
