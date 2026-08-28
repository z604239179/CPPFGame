#pragma once

#include <string>
#include <vector>

namespace game {

class Player;

// 账号条目：用户名、密码、玩家序列化数据（避免 Player 因含 unique_ptr 不可拷贝）
struct Account {
    std::string username;
    std::string password;
    std::string playerData;  // Player::serialize() 的结果
};

// 账号管理器：负责账号持久化、注册、登录校验、玩家数据回写
// 文件格式（行式）：
//   <账号数>
//   <用户名>
//   <密码>
//   <Player 序列化数据多行>
//   <用户名>
//   ...
class AccountManager {
public:
    // 从文件加载所有账号；文件不存在时返回 true（空账号库）
    bool load(const std::string& path);
    // 保存所有账号到文件
    bool save(const std::string& path) const;

    // 注册新账号：用户名唯一则创建，返回对应账号指针；已存在返回 nullptr
    Account* registerAccount(const std::string& username,
                             const std::string& password,
                             const std::string& charName);
    // 按用户名查找账号
    Account* find(const std::string& username);

    // 把玩家最新数据序列化后写回账号（玩家下线时调用）
    bool updatePlayer(const std::string& username, const Player& p);

    int accountCount() const { return static_cast<int>(accounts_.size()); }

private:
    std::vector<Account> accounts_;
};

}  // namespace game
