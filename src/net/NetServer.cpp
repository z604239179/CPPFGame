#include "net/NetServer.h"

#include <conio.h>
#include <cstdlib>
#include <iostream>

#include <windows.h>

#include "model/GameState.h"

namespace game {

NetServer::NetServer(int port, bool loadSave) : port_(port), loadSave_(loadSave) {}

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
    if (loadSave_ && state_->loadFromFile("server_save.txt")) {
        std::cout << "[服务器] 已读取存档，当前玩家 " << state_->playerCount() << " 人。" << std::endl;
    }
    std::cout << "[服务器] 冒险大陆服务器已启动，端口 " << port_ << "（按 Q 退出）" << std::endl;

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
    sendTo(clients_.back(), "MSG 已连接到服务器，请输入角色名称。");
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

    if (verb == "LOGIN" && parts.size() >= 2 && !c.joined) {
        const std::string name = sanitizeName(parts[1]);
        Player& p = state_->addPlayer(name);
        c.playerId = p.id;
        c.joined = true;
        std::cout << "[服务器] 玩家 " << p.name << " (ID " << p.id << ") 加入游戏。" << std::endl;
        sendTo(c, "MSG 欢迎来到多人冒险大陆，" + p.name + "！");
        sendTo(c, "MSG WASD 移动，1/2/3 互动，I 背包，C 状态，M 任务，Q 退出。");
        for (const auto& msg : state_->takePendingMessages()) broadcastText("MSG " + msg);
        broadcastState();
        return;
    }

    if (!c.joined) {
        sendTo(c, "ERR 请先登录: LOGIN <名字>");
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
    } else if (verb == "FIGHT" && parts.size() >= 2) {
        state_->startFight(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "TALK" && parts.size() >= 2) {
        state_->talkToNpc(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "ATK") {
        state_->fightRound(pid, FightAction::Attack);
    } else if (verb == "SKILL") {
        state_->fightRound(pid, FightAction::Skill);
    } else if (verb == "FLEE") {
        state_->fightRound(pid, FightAction::Flee);
    } else if (verb == "USE" && parts.size() >= 2) {
        state_->useItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "DROP" && parts.size() >= 2) {
        state_->dropItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "SELL" && parts.size() >= 2) {
        state_->sellItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "BUY" && parts.size() >= 2) {
        state_->buyItem(pid, std::atoi(parts[1].c_str()));
    } else if (verb == "SAVE") {
        if (state_->saveToFile("server_save.txt")) sendTo(c, "MSG 服务器存档已保存。");
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
