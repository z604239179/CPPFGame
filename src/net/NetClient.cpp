#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "net/NetClient.h"

#include <cstring>

#include <ws2tcpip.h>

namespace game {

NetClient::NetClient() = default;

NetClient::~NetClient() {
    if (sock_ != INVALID_SOCKET) {
        closesocket(sock_);
    }
}

bool NetClient::connectTo(const std::string& host, int port) {
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_ == INVALID_SOCKET) return false;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<u_short>(port));

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        hostent* he = gethostbyname(host.c_str());
        if (!he) {
            closesocket(sock_);
            sock_ = INVALID_SOCKET;
            return false;
        }
        std::memcpy(&addr.sin_addr, he->h_addr, he->h_length);
    }

    if (connect(sock_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
        closesocket(sock_);
        sock_ = INVALID_SOCKET;
        return false;
    }
    return true;
}

bool NetClient::sendCommand(const std::string& cmd) {
    if (sock_ == INVALID_SOCKET) return false;
    return sendAll(sock_, encodeFrame(cmd));
}

PumpResult NetClient::pump(std::deque<std::string>& messages, WorldSnapshot& snap) {
    if (sock_ == INVALID_SOCKET) return PumpResult::Disconnected;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sock_, &readfds);
    timeval tv{0, 0};
    if (select(0, &readfds, nullptr, nullptr, &tv) <= 0) return PumpResult::Ok;

    char buf[4096];
    const int n = recv(sock_, buf, sizeof(buf), 0);
    if (n <= 0) return PumpResult::Disconnected;
    recvBuf_.append(buf, n);

    bool updated = false;
    std::string frame;
    while (tryReadFrame(recvBuf_, frame)) {
        if (frame.rfind("MSG ", 0) == 0) {
            messages.push_back(frame.substr(4));
            updated = true;
        } else if (frame.rfind("STATE ", 0) == 0) {
            if (!parseSnapshot(frame.substr(6), snap)) return PumpResult::Disconnected;
            updated = true;
        } else if (frame.rfind("ERR ", 0) == 0) {
            messages.push_back("[系统] " + frame.substr(4));
            updated = true;
        }
        while (messages.size() > kMaxRecentMessages) messages.pop_front();
    }
    return updated ? PumpResult::Updated : PumpResult::Ok;
}

}  // namespace game
