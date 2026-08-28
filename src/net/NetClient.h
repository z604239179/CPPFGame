#pragma once

#include <deque>
#include <string>

#include <winsock2.h>

#include "common/Protocol.h"
#include "common/Snapshot.h"

namespace game {

enum class PumpResult {
    Ok,           // 已连接，没有新数据
    Updated,      // 收到新的 MSG / STATE / ERR
    Disconnected, // 连接已断开
};

// 联机客户端（Network）：连接服务器、发送指令、接收快照
class NetClient {
public:
    NetClient();
    ~NetClient();

    bool connectTo(const std::string& host, int port);
    bool sendCommand(const std::string& cmd);

    // 拉取网络数据：MSG 追加到 messages，STATE 更新快照
    PumpResult pump(std::deque<std::string>& messages, WorldSnapshot& snap);

private:
    WsaInit wsa_;
    SOCKET sock_ = INVALID_SOCKET;
    std::string recvBuf_;
};

}  // namespace game
