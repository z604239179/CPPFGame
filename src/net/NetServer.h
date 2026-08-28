#pragma once

#include <memory>
#include <string>
#include <vector>

#include <winsock2.h>

#include "common/Protocol.h"

namespace game {

class GameState;

// 权威服务器（Network）：持有世界模型，处理客户端指令并广播状态快照
class NetServer {
public:
    NetServer(int port, bool loadSave);
    ~NetServer();
    int run();

private:
    struct ClientConn {
        SOCKET sock = INVALID_SOCKET;
        std::string recvBuf;
        int playerId = -1;
        bool joined = false;
        bool closed = false;
    };

    void acceptNew();
    void handleReadable(ClientConn& c);
    void handleCommand(ClientConn& c, const std::string& cmd);
    void sendTo(ClientConn& c, const std::string& text);
    void broadcastText(const std::string& text);
    void broadcastState();
    void removeClosedClients();

    int port_ = 8888;
    bool loadSave_ = false;
    SOCKET listenSock_ = INVALID_SOCKET;
    bool running_ = true;
    std::unique_ptr<GameState> state_;
    std::vector<ClientConn> clients_;
};

}  // namespace game
