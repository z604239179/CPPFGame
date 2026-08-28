#pragma once

#include <cstdint>
#include <string>

#include <winsock2.h>

namespace game {

// Winsock 生命周期管理（RAII）
class WsaInit {
public:
    WsaInit();
    ~WsaInit();
};

// 给负载加上 4 字节长度前缀
std::string encodeFrame(const std::string& payload);

// 从接收缓冲区中提取一帧完整消息；不足一帧时返回 false
bool tryReadFrame(std::string& buffer, std::string& out);

// 尽量发送完整数据，失败返回 false
bool sendAll(SOCKET sock, const std::string& data);

}  // namespace game
