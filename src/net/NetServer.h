#pragma once

#include <memory>
#include <string>
#include <vector>

#include <winsock2.h>

#include "common/Protocol.h"
#include "net/AccountManager.h"

namespace game {

class GameState;

// 权威服务器（Network）：持有世界模型，处理客户端指令并广播状态快照
class NetServer {
public:
    explicit NetServer(int port);
    ~NetServer();
    int run();

private:
    struct ClientConn {
        SOCKET sock = INVALID_SOCKET;
        std::string recvBuf;
        int playerId = -1;
        std::string username;  // 登录后绑定的账号
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

    // 登录/注册：成功返回 true，并把玩家加入世界
    bool doLogin(ClientConn& c, const std::string& user, const std::string& pass);
    bool doRegister(ClientConn& c, const std::string& user,
                    const std::string& pass, const std::string& charName);

    int port_ = 8888;
    SOCKET listenSock_ = INVALID_SOCKET;
    bool running_ = true;
    std::unique_ptr<GameState> state_;
    AccountManager accounts_;
    std::vector<ClientConn> clients_;
};

}  // namespace game
