#pragma once

#include <chrono>
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

// 移动方向
enum class Direction { Up, Down, Left, Right };

// 战斗技能（对应按键 Q/W/E/R）
struct SkillDef {
    const char* name;
    const char* key;
    float damageMult;   // 攻击倍率（0 = 非伤害技能）
    int mpCost;         // 法力消耗
    int healPercent;    // 恢复最大生命百分比（0 = 非治疗技能）
};

inline constexpr int kSkillCount = 4;
inline const SkillDef& skillDef(int index) {
    static const SkillDef skills[kSkillCount] = {
        {"强击",   "Q", 1.5f, 10,  0},
        {"连斩",   "W", 2.0f, 20,  0},
        {"治疗术", "E", 0.0f, 15, 30},
        {"必杀技", "R", 3.0f, 30,  0},
    };
    return skills[index];
}

// 世界常量
constexpr int kMapSize = 20;
constexpr int kMapCount = 2;
constexpr int kInventorySlots = 30;  // 3 页 x 10 格
constexpr int kEquipmentSlots = 5;   // 与 EquipmentSlot::Count 一致
constexpr int kMaxRecentMessages = 20;

// 刷怪系统：每张地图刷怪数量与击杀后的刷新间隔
constexpr int kMonstersPerMap = 45;              // 每张地图固定刷 45 只怪物
constexpr long long kMonsterRespawnMs = 15000;   // 击杀后 15 秒刷新

// 自动战斗回合：每秒一次自动普通攻击
constexpr long long kCombatRoundMs = 1000;       // 战斗出手间隔（1 秒/回合）

// 单调时钟（毫秒），供时间系统 / 刷怪调度使用
inline long long nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

// 控制台颜色
enum class ConsoleColor { Normal, Red, Blue, Green, Yellow, Purple, Cyan };

// 基础数据结构
struct Position {
    int x = 0;
    int y = 0;
    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
};

// 死亡复活点：城镇（地图1）中央安全区 Safe 格（与 GameData 城镇布局保持一致）
constexpr int kReviveMapId = 1;
constexpr Position kRevivePos{9, 9};

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
