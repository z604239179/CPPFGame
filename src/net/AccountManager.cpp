#include "net/AccountManager.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#include "model/Player.h"

namespace game {

namespace {
// 凭据清洗：去掉空格与协议/存档分隔符，限制长度，避免破坏文件格式
std::string sanitizeCredential(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
            c == ':' || c == '|' || c == ';') continue;
        out.push_back(c);
        if (out.size() >= 16) break;
    }
    return out;
}
}  // namespace

bool AccountManager::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return true;  // 文件不存在视为空账号库

    accounts_.clear();
    int count = 0;
    in >> count;
    for (int i = 0; i < count; ++i) {
        Account acc;
        if (!(in >> acc.username >> acc.password)) break;

        std::string tag;
        if (!(in >> tag) || tag != "PLAYERDATA_BEGIN") break;
        // 逐行读取直到 PLAYERDATA_END
        std::string data;
        std::string line;
        // 读完上一行的换行，进入 getline 循环
        std::getline(in, line);
        while (std::getline(in, line)) {
            if (line == "PLAYERDATA_END") break;
            if (!data.empty()) data += "\n";
            data += line;
        }
        acc.playerData = data;
        accounts_.push_back(std::move(acc));
    }
    return true;
}

bool AccountManager::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;

    out << accounts_.size() << "\n";
    for (const auto& acc : accounts_) {
        out << acc.username << "\n" << acc.password << "\n";
        out << "PLAYERDATA_BEGIN\n" << acc.playerData << "\nPLAYERDATA_END\n";
    }
    return true;
}

Account* AccountManager::registerAccount(const std::string& username,
                                         const std::string& password,
                                         const std::string& charName) {
    const std::string user = sanitizeCredential(username);
    const std::string pass = sanitizeCredential(password);
    if (user.size() < 3 || pass.size() < 3) return nullptr;
    if (find(user) != nullptr) return nullptr;

    Account acc;
    acc.username = user;
    acc.password = pass;
    // 新玩家：临时 id=0，加入世界时由 GameState 重新分配；出生在村庄安全区
    Player fresh(0, sanitizeName(charName));
    fresh.mapId = 0;
    fresh.pos = {8, 10};
    acc.playerData = fresh.serialize();
    accounts_.push_back(std::move(acc));
    return &accounts_.back();
}

Account* AccountManager::find(const std::string& username) {
    for (auto& acc : accounts_) {
        if (acc.username == username) return &acc;
    }
    return nullptr;
}

bool AccountManager::updatePlayer(const std::string& username, const Player& p) {
    Account* acc = find(username);
    if (!acc) return false;
    acc->playerData = p.serialize();
    return true;
}

}  // namespace game
