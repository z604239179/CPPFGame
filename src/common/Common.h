#pragma once

#include <string>
#include <vector>

namespace game {

// 物品品质
enum class ItemQuality { Normal = 0, Unusual = 1, Epic = 2, Legend = 3 };

// 装备部位
enum class EquipmentSlot { Head = 0, Body = 1, Weapon = 2, Legs = 3, Boots = 4, Count = 5 };

// 地图块类型（数值与静态地图数据中的编码一致）
enum class TileType { Space = 0, Wall = 1, Tree = 2, People = 3, Safe = 4, Portal = 5 };

// NPC 职能
enum class NpcRole { None = 0, Merchant = 1, Blacksmith = 2, Chief = 3, Shepherd = 4, Mayor = 5 };

// 任务状态 / 类型
enum class MissionState { None = 0, Accepted = 1, Completed = 2 };
enum class MissionType { Talk = 0, Kill = 1, Collect = 2 };

// 移动 / 战斗指令
enum class Direction { Up, Down, Left, Right };
enum class FightAction { Attack, Skill, Flee };

// 世界常量
constexpr int kMapSize = 20;
constexpr int kMapCount = 2;
constexpr int kInventorySlots = 30;  // 3 页 x 10 格
constexpr int kEquipmentSlots = 5;   // 与 EquipmentSlot::Count 一致
constexpr int kMaxRecentMessages = 20;

// 控制台颜色
enum class ConsoleColor { Normal, Red, Blue, Green, Yellow, Purple, Cyan };

// 基础数据结构
struct Position {
    int x = 0;
    int y = 0;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

struct Stats {
    int hp = 0;
    int maxHp = 0;
    int mp = 0;
    int maxMp = 0;
    int atk = 0;
    int def = 0;
};

// 地图名称
inline const char* mapName(int mapId) { return mapId == 0 ? "村庄" : "城镇"; }

// 通用字符串分割（内联，供多文件复用）
inline std::vector<std::string> splitString(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::string cur;
    for (char c : s) {
        if (c == delim) {
            out.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

// 简易清理玩家名（防止破坏协议分隔符）
inline std::string sanitizeName(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        if (c == '|' || c == ':' || c == ';' || c == '\n' || c == '\r') continue;
        out.push_back(c);
        if (out.size() >= 12) break;
    }
    return out.empty() ? "玩家" : out;
}

}  // namespace game
