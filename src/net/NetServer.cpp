#include "net/NetServer.h"

#include <conio.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include <windows.h>

#include "model/GameState.h"

namespace game {

NetServer::NetServer(int port) : port_(port) {}

NetServer::~NetServer() = default;

int NetServer::run() {
    WsaInit wsa;

    listenSock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock_ == INVALID_SOCKET) {
        std::cout << "[服务器] 创建套接字失败。" << std::endl;
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<u_short>(port_));

    if (bind(listenSock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        std::cout << "[服务器] 端口 " << port_ << " 绑定失败。" << std::endl;
        closesocket(listenSock_);
        return 1;
    }
    if (listen(listenSock_, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "[服务器] 监听失败。" << std::endl;
        closesocket(listenSock_);
        return 1;
    }

    state_ = std::make_unique<GameState>();
    accounts_.load("accounts.txt");
    std::cout << "[服务器] 冒险大陆服务器已启动，端口 " << port_
              << "，已加载账号 " << accounts_.accountCount() << " 个（按 Q 退出）" << std::endl;

    while (running_) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(listenSock_, &readfds);
        for (const auto& c : clients_) {
            if (!c.closed) FD_SET(c.sock, &readfds);
        }

        timeval tv{0, 100000};  // 100ms
        const int rc = select(0, &readfds, nullptr, nullptr, &tv);
        if (rc == SOCKET_ERROR) break;

        if (FD_ISSET(listenSock_, &readfds)) acceptNew();

        for (auto& c : clients_) {
            if (!c.closed && FD_ISSET(c.sock, &readfds)) handleReadable(c);
        }
        removeClosedClients();

        // 时间系统：怪物重生 tick + 自动战斗 tickCombat。任一有变化都广播消息与快照
        bool worldChanged = false;
        const long long nowMsValue = nowMs();
        if (state_->tick(nowMsValue)) worldChanged = true;
        if (state_->tickCombat(nowMsValue)) worldChanged = true;
        if (worldChanged) {
            for (const auto& msg : state_->takePendingMessages()) broadcastText("MSG " + msg);
            broadcastState();
        }

        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key == 'q' || key == 'Q') running_ = false;
        }
        Sleep(10);
    }

    for (auto& c : clients_) closesocket(c.sock);
    closesocket(listenSock_);
    std::cout << "[服务器] 已关闭。" << std::endl;
    return 0;
}

void NetServer::acceptNew() {
    sockaddr_in clientAddr{};
    int addrLen = sizeof(clientAddr);
    const SOCKET s = accept(listenSock_, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);
    if (s == INVALID_SOCKET) return;

    ClientConn c;
    c.sock = s;
    clients_.push_back(std::move(c));
    std::cout << "[服务器] 新连接加入，等待登录。" << std::endl;
    sendTo(clients_.back(), "MSG 已连接到服务器，请登录或注册。");
}

void NetServer::handleReadable(ClientConn& c) {
    char buf[4096];
    const int n = recv(c.sock, buf, sizeof(buf), 0);
    if (n <= 0) {
        c.closed = true;
        return;
    }
    c.recvBuf.append(buf, n);

    std::string frame;
    while (tryReadFrame(c.recvBuf, frame)) {
        handleCommand(c, frame);
        if (c.closed) break;
    }
}

void NetServer::handleCommand(ClientConn& c, const std::string& cmd) {
    const auto parts = splitString(cmd, ' ');
    const std::string verb = parts.empty() ? "" : parts[0];

    // ---- 登录 / 注册（未加入游戏时只接受这两条指令）----

    if (verb == "LOGIN" && parts.size() >= 3 && !c.joined) {
        if (doLogin(c, parts[1], parts[2])) {
            sendTo(c, "MSG 欢迎来到多人冒险大陆，" + state_->findPlayer(c.playerId)->name + "！");
            sendTo(c, "MSG WASD移动 走上怪物格自动战斗  QWER技能 1234消耗品 F逃跑  I背包 C状态 M任务 P保存(退游戏按Esc)");
            for (const auto& msg : state_->takePendingMessages()) broadcastText("MSG " + msg);
            broadcastState();
        }
        return;
    }

    if (verb == "REGISTER" && parts.size() >= 4 && !c.joined) {
        doRegister(c, parts[1], parts[2], parts[3]);
        return;
    }

    if (!c.joined) {
        sendTo(c, "ERR 请先登录: LOGIN <账号> <密码> 或 REGISTER <账号> <密码> <角色名>");
        return;
    }

    const int pid = c.playerId;
    if (verb == "MOVE" && parts.size() >= 2) {
        Direction dir;
        if (parts[1] == "u") dir = Direction::Up;
        else if (parts[1] == "d") dir = Direction::Down;
        else if (parts[1] == "l") dir = Direction::Left;
        else if (parts[1] == "r") dir = Direction::Right;
        else return;
        state_->movePlayer(pid, dir);
    } else if (verb == "TALK" && parts.size() >= 2) {
        state_->talkToNpc(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "SKILL" && parts.size() >= 2) {
        state_->useSkill(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "CUSE" && parts.size() >= 2) {
        state_->useCombatItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "FLEE") {
        state_->fleeCombat(pid);
    } else if (verb == "USE" && parts.size() >= 2) {
        state_->useItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "DROP" && parts.size() >= 2) {
        state_->dropItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "SELL" && parts.size() >= 2) {
        state_->sellItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "BUY" && parts.size() >= 2) {
        state_->buyItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "SAVE") {
        if (accounts_.save("accounts.txt")) sendTo(c, "MSG 账号数据已保存到服务器。");
        else sendTo(c, "ERR 服务器存档失败。");
        return;
    } else if (verb == "QUIT") {
        c.closed = true;
        return;
    } else {
        sendTo(c, "ERR 未知指令: " + verb);
        return;
    }

    for (const auto& msg : state_->takePendingMessages()) broadcastText("MSG " + msg);
    broadcastState();
}

bool NetServer::doLogin(ClientConn& c, const std::string& user, const std::string& pass) {
    Account* acc = accounts_.find(user);
    if (!acc) {
        sendTo(c, "ERR 账号不存在。");
        return false;
    }
    if (acc->password != pass) {
        sendTo(c, "ERR 密码错误。");
        return false;
    }
    // 反序列化账号存档玩家数据，加入世界（避免 Player 不可拷贝）
    Player loaded;
    std::istringstream iss(acc->playerData);
    if (!loaded.deserialize(iss)) {
        sendTo(c, "ERR 账号数据损坏，无法登录。");
        return false;
    }
    Player& p = state_->addPlayer(std::move(loaded));
    c.playerId = p.id;
    c.username = acc->username;
    c.joined = true;
    std::cout << "[服务器] 账号 " << acc->username << " 登录，玩家 " << p.name
              << " (ID " << p.id << ") 加入游戏。" << std::endl;
    return true;
}

bool NetServer::doRegister(ClientConn& c, const std::string& user,
                           const std::string& pass, const std::string& charName) {
    Account* acc = accounts_.registerAccount(user, pass, charName);
    if (!acc) {
        sendTo(c, "ERR 注册失败：账号已存在或账号/密码长度不足3位。");
        return false;
    }
    accounts_.save("accounts.txt");
    sendTo(c, "MSG 注册成功，请使用 LOGIN 登录。");
    std::cout << "[服务器] 新账号 " << acc->username << " 注册。" << std::endl;
    return false;  // 注册不自动登录，客户端回到登录菜单
}

void NetServer::sendTo(ClientConn& c, const std::string& text) {
    if (c.closed) return;
    if (!sendAll(c.sock, encodeFrame(text))) c.closed = true;
}

void NetServer::broadcastText(const std::string& text) {
    for (auto& c : clients_) {
        if (c.joined) sendTo(c, text);
    }
}

void NetServer::broadcastState() {
    for (auto& c : clients_) {
        if (c.joined) sendTo(c, "STATE " + state_->serializeSnapshot(c.playerId));
    }
}

void NetServer::removeClosedClients() {
    bool changed = false;
    for (auto it = clients_.begin(); it != clients_.end();) {
        if (it->closed) {
            if (it->joined) {
                // 把玩家最新数据写回账号并保存
                const Player* p = state_->findPlayer(it->playerId);
                if (p && !it->username.empty()) {
                    accounts_.updatePlayer(it->username, *p);
                    accounts_.save("accounts.txt");
                    std::cout << "[服务器] 账号 " << it->username << " 的玩家数据已保存。" << std::endl;
                }
                std::cout << "[服务器] 玩家 ID " << it->playerId << " 已断开。" << std::endl;
                state_->removePlayer(it->playerId);
                changed = true;
            }
            closesocket(it->sock);
            it = clients_.erase(it);
        } else {
            ++it;
        }
    }
    if (changed) {
        for (const auto& msg : state_->takePendingMessages()) broadcastText("MSG " + msg);
        broadcastState();
    }
}

}  // namespace game
