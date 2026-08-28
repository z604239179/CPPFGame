#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include <windows.h>

#include "controller/GameController.h"
#include "net/NetServer.h"

int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);
    system("mode con cols=56 lines=40");
    srand(static_cast<unsigned>(time(nullptr)));

    std::string mode = "client";  // 默认启动客户端
    std::string host = "127.0.0.1";
    int port = 8888;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--server") {
            mode = "server";
            if (i + 1 < argc) port = std::atoi(argv[++i]);
        } else if (arg == "--client") {
            mode = "client";
            if (i + 1 < argc) host = argv[++i];
            if (i + 1 < argc) port = std::atoi(argv[++i]);
        } else if (arg == "--help") {
            std::cout << "用法:" << std::endl;
            std::cout << "  MyGame                       连接本机服务器（127.0.0.1:8888）" << std::endl;
            std::cout << "  MyGame --server [端口]       开启 MMO 服务器（默认 8888）" << std::endl;
            std::cout << "  MyGame --client <主机> [端口] 连接指定服务器" << std::endl;
            std::cout << "  账号数据自动保存在服务器目录的 accounts.txt" << std::endl;
            return 0;
        }
    }

    if (mode == "server") {
        game::NetServer server(port);
        return server.run();
    }

    game::GameController ctrl;
    return ctrl.runClient(host, port);
}
