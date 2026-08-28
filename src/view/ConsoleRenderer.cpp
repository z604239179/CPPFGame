#include "view/ConsoleRenderer.h"

#include <algorithm>
#include <iostream>
#include <streambuf>
#include <string>

#include <windows.h>

#include "model/GameData.h"

namespace game {

// ---- 自定义 streambuf：将 std::cout 输出重定向到指定控制台屏幕缓冲区 ----

class ConsoleRenderer::ConsoleStreamBuf : public std::streambuf {
public:
    explicit ConsoleStreamBuf(HANDLE h) : handle_(h) {}

    void setHandle(HANDLE h) { handle_ = h; }

protected:
    std::streamsize xsputn(const char* s, std::streamsize n) override {
        if (n <= 0 || !handle_ || handle_ == INVALID_HANDLE_VALUE) return 0;
        writeText(s, static_cast<int>(n));
        return n;
    }

    int overflow(int c) override {
        if (c == traits_type::eof()) return traits_type::eof();
        char ch = static_cast<char>(c);
        writeText(&ch, 1);
        return c;
    }

    int sync() override { return 0; }  // WriteConsole 即时写入，无需刷新

private:
    void writeText(const char* s, int len) {
        if (len <= 0) return;
        // UTF-8 转 UTF-16 后用 WriteConsoleW 写入，保证中文等字符正确显示
        int wlen = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
        if (wlen <= 0) return;
        std::wstring wstr(static_cast<size_t>(wlen), L'\0');
        MultiByteToWideChar(CP_UTF8, 0, s, len, wstr.data(), wlen);
        DWORD written = 0;
        WriteConsoleW(handle_, wstr.data(), static_cast<DWORD>(wlen), &written, nullptr);
    }

    HANDLE handle_;
};

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

// 按终端显示宽度截断字符串（ASCII 记 1 列，中文等记 2 列），防止日志过长换行打乱布局
std::string truncateByWidth(const std::string& s, int maxCols) {
    if (maxCols <= 0) return "";
    std::string out;
    int width = 0;
    size_t i = 0;
    while (i < s.size()) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        size_t len = 1;
        if (c >= 0xF0)      len = 4;
        else if (c >= 0xE0) len = 3;
        else if (c >= 0xC0) len = 2;
        if (i + len > s.size()) len = s.size() - i;
        const int w = (c < 0x80) ? 1 : 2;
        if (width + w > maxCols) break;
        out.append(s, i, len);
        width += w;
        i += len;
    }
    return out;
}
}  // namespace

// ---- 构造 / 析构 ----

ConsoleRenderer::ConsoleRenderer() {
    ensureInit();
}

ConsoleRenderer::~ConsoleRenderer() {
    // 恢复 std::cout 原始 streambuf
    if (originalCoutBuf_) {
        std::cout.rdbuf(originalCoutBuf_);
    }
}

// ---- 双缓冲初始化 ----

void ConsoleRenderer::ensureInit() const {
    if (initialized_) return;
    initialized_ = true;

    originalStdout_ = GetStdHandle(STD_OUTPUT_HANDLE);
    frontBuffer_ = originalStdout_;

    backBuffer_ = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        CONSOLE_TEXTMODE_BUFFER,
        nullptr);

    if (backBuffer_ == INVALID_HANDLE_VALUE) {
        // 创建失败，回退到单缓冲（仍有闪烁，但至少能正常工作）
        backBuffer_ = originalStdout_;
        return;
    }

    // 将后台缓冲区的尺寸和窗口对齐到前台缓冲区
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(frontBuffer_, &info)) {
        // 先放大缓冲区，确保窗口能放下
        COORD tempSize = {100, 100};
        SetConsoleScreenBufferSize(backBuffer_, tempSize);
        SetConsoleWindowInfo(backBuffer_, TRUE, &info.srWindow);
        SetConsoleScreenBufferSize(backBuffer_, info.dwSize);
    }

    // 启用控制字符处理（\n 换行、\t 制表等）
    SetConsoleMode(backBuffer_, ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

    // 将 std::cout 重定向到后台缓冲区
    backBuf_ = std::make_unique<ConsoleStreamBuf>(backBuffer_);
    originalCoutBuf_ = std::cout.rdbuf(backBuf_.get());
}

// ---- 清屏：在后台缓冲区上操作（不可见，无闪烁）----

void ConsoleRenderer::clear() const {
    ensureInit();
    COORD home = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(backBuffer_, &info)) {
        DWORD cells = static_cast<DWORD>(info.dwSize.X) * static_cast<DWORD>(info.dwSize.Y);
        DWORD written = 0;
        FillConsoleOutputCharacterW(backBuffer_, L' ', cells, home, &written);
        FillConsoleOutputAttribute(backBuffer_, toWindowsColor(ConsoleColor::Normal), cells, home, &written);
        // 重置窗口位置到左上角，避免 cin 回显导致窗口滚动后内容不可见
        SMALL_RECT win = info.srWindow;
        const SHORT w = win.Right - win.Left;
        const SHORT h = win.Bottom - win.Top;
        win.Left = 0;
        win.Top = 0;
        win.Right = w;
        win.Bottom = h;
        SetConsoleWindowInfo(backBuffer_, TRUE, &win);
    }
    SetConsoleCursorPosition(backBuffer_, home);
}

void ConsoleRenderer::setColor(ConsoleColor color) const {
    ensureInit();
    SetConsoleTextAttribute(backBuffer_, toWindowsColor(color));
}

// ---- 切换缓冲区：将后台内容一次性显示到屏幕 ----

void ConsoleRenderer::present() const {
    ensureInit();
    if (backBuffer_ == frontBuffer_) return;  // 单缓冲回退模式，无需切换
    SetConsoleActiveScreenBuffer(backBuffer_);
    std::swap(frontBuffer_, backBuffer_);
    if (backBuf_) backBuf_->setHandle(backBuffer_);
}

void ConsoleRenderer::printLoginMenu() const {
    clear();
    setColor(ConsoleColor::Yellow);
    std::cout << "\n\n\n";
    std::cout << "================ 冒险大陆 ================\n";
    std::cout << "+           1. 登录账号                 +\n";
    std::cout << "+           2. 注册账号                 +\n";
    std::cout << "+           3. 关于制作                 +\n";
    std::cout << "+           4. 退出游戏                 +\n";
    std::cout << "==========================================\n";
    std::cout << "\n                              Version: MMO";
    setColor(ConsoleColor::Normal);
    present();
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
    setColor(ConsoleColor::Red);
    std::cout << "M\t\t怪物（走上自动战斗，击杀后定时刷新）\n";
    setColor(ConsoleColor::Normal);
    std::cout << "■\t\t安全区 / 复活点\n";
    setColor(ConsoleColor::Cyan);
    std::cout << "O\t\t传送门\n\n\n";

    setColor(ConsoleColor::Yellow);
    std::cout << "\t制作人：Ssaturday（重构版）\n";
    setColor(ConsoleColor::Normal);
    present();
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

// 将光标定位到当前行的指定列（用于地图右侧日志面板对齐）
void ConsoleRenderer::setCursorCol(int col) const {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (!GetConsoleScreenBufferInfo(backBuffer_, &info)) return;
    COORD pos;
    pos.X = static_cast<SHORT>(col);
    pos.Y = info.dwCursorPosition.Y;
    SetConsoleCursorPosition(backBuffer_, pos);
}

void ConsoleRenderer::printMain(const WorldSnapshot& snap,
                                const std::vector<std::string>& messages) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) {
        std::cout << "正在等待服务器同步……" << std::endl;
        present();
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

    printMap(snap, messages);  // 地图 + 右侧日志面板
    printOccupants(snap);
    printPlayers(snap);
    if (self->inCombat) {
        std::cout << "【战斗中】 ";
        for (int i = 0; i < kSkillCount; ++i) {
            const SkillDef& sk = skillDef(i);
            setColor(i == 2 ? ConsoleColor::Green : ConsoleColor::Yellow);
            std::cout << sk.key << "." << sk.name << "(MP" << sk.mpCost << ") ";
        }
        setColor(ConsoleColor::Normal);
        std::cout << "| 1-4使用消耗品 F逃跑" << std::endl;
    } else {
        std::cout << "WASD移动  走上怪物格自动战斗  1对话NPC  I背包  C状态  M任务  P保存  Esc退出" << std::endl;
    }
    present();
}

void ConsoleRenderer::printMap(const WorldSnapshot& snap,
                               const std::vector<std::string>& messages) const {
    const PlayerView* self = findSelf(snap);
    if (!self) return;

    // 日志面板起始列（地图最宽约 40 列）与面板可容纳的最大显示宽度
    CONSOLE_SCREEN_BUFFER_INFO sbInfo = {};
    int bufWidth = 100;
    if (GetConsoleScreenBufferInfo(backBuffer_, &sbInfo)) bufWidth = sbInfo.dwSize.X;
    constexpr int kLogCol = 42;
    const int logMaxCols = bufWidth > kLogCol + 4 ? bufWidth - kLogCol - 4 : 30;

    // 右侧日志面板：第 0 行为标题，其后按顺序显示最近日志（最多占满地图 20 行）
    std::string panel[kMapSize];
    panel[0] = "—— 消息日志 ——";
    const size_t logCount = (std::min)(messages.size(), static_cast<size_t>(kMapSize - 1));
    for (size_t i = 0; i < logCount; ++i) {
        panel[1 + i] = "> " + messages[messages.size() - logCount + i];
    }

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
            // 怪物直接显示在地图格子上（按品质着色），走上该格自动进入战斗
            bool monsterHere = false;
            for (const auto& m : snap.mapMonsters) {
                if (m.x == x && m.y == y) {
                    switch (m.quality) {
                        case ItemQuality::Unusual: setColor(ConsoleColor::Blue);   break;
                        case ItemQuality::Epic:    setColor(ConsoleColor::Purple); break;
                        case ItemQuality::Legend:  setColor(ConsoleColor::Yellow); break;
                        default:                   setColor(ConsoleColor::Red);    break;
                    }
                    std::cout << "M";
                    monsterHere = true;
                    break;
                }
            }
            if (monsterHere) continue;
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
        // 行尾：定位到右侧日志列，输出该行对应的日志（过长自动截断，避免换行打乱布局）
        setColor(ConsoleColor::Normal);
        setCursorCol(kLogCol);
        std::cout << "│ ";
        if (!panel[x].empty()) {
            if (x == 0) setColor(ConsoleColor::Yellow);
            std::cout << truncateByWidth(panel[x], logMaxCols);
            setColor(ConsoleColor::Normal);
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
    if (!self) { present(); return; }

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
    present();
}

void ConsoleRenderer::printItemDetail(const WorldSnapshot& snap, int slot) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) { present(); return; }

    const ItemView* found = nullptr;
    for (const auto& it : self->inventory) {
        if (it.slot == slot) { found = &it; break; }
    }
    if (!found) {
        std::cout << "该格为空。" << std::endl;
        present();
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
    present();
}

void ConsoleRenderer::printStatus(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) { present(); return; }

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
    present();
}

void ConsoleRenderer::printMission(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) { present(); return; }

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
    present();
}

void ConsoleRenderer::printCombat(const WorldSnapshot& snap) const {
    clear();
    const PlayerView* self = findSelf(snap);
    if (!self) { present(); return; }

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
    present();
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
    present();
}

void ConsoleRenderer::printNpcMenu(const std::vector<std::string>& messages, bool hasShop) const {
    clear();
    printMessages(messages);
    std::cout << std::endl;
    std::cout << "1.任务  2.交易  3.离开" << std::endl;
    if (!hasShop) std::cout << "(这位 NPC 没有商店)" << std::endl;
    present();
}

void ConsoleRenderer::printMessage(const std::string& text) const {
    clear();
    std::cout << text << std::endl;
    present();
    Sleep(1500);
}

}  // namespace game
