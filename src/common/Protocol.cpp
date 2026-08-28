#include "common/Protocol.h"

#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

namespace game {

WsaInit::WsaInit() {
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
}

WsaInit::~WsaInit() {
    WSACleanup();
}

std::string encodeFrame(const std::string& payload) {
    std::string out;
    const std::uint32_t len = static_cast<std::uint32_t>(payload.size());
    out.push_back(static_cast<char>(len & 0xFF));
    out.push_back(static_cast<char>((len >> 8) & 0xFF));
    out.push_back(static_cast<char>((len >> 16) & 0xFF));
    out.push_back(static_cast<char>((len >> 24) & 0xFF));
    out += payload;
    return out;
}

bool tryReadFrame(std::string& buffer, std::string& out) {
    if (buffer.size() < 4) return false;
    const auto b0 = static_cast<unsigned char>(buffer[0]);
    const auto b1 = static_cast<unsigned char>(buffer[1]);
    const auto b2 = static_cast<unsigned char>(buffer[2]);
    const auto b3 = static_cast<unsigned char>(buffer[3]);
    std::uint32_t len = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
    if (len > 1024 * 1024) {  // 单帧上限 1MB，防异常
        buffer.clear();
        return false;
    }
    if (buffer.size() < 4u + len) return false;
    out.assign(buffer, 4, len);
    buffer.erase(0, 4u + len);
    return true;
}

bool sendAll(SOCKET sock, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        const int n = send(sock, data.data() + sent, static_cast<int>(data.size() - sent), 0);
        if (n == SOCKET_ERROR || n == 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

}  // namespace game
