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

    std::string mode = "local";
    std::string host;
    int port = 8888;
    bool loadSave = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--server") {
            mode = "server";
            if (i + 1 < argc) port = std::atoi(argv[++i]);
        } else if (arg == "--client") {
            mode = "client";
            if (i + 1 < argc) host = argv[++i];
            if (i + 1 < argc) port = std::atoi(argv[++i]);
        } else if (arg == "--local") {
            mode = "local";
        } else if (arg == "--load") {
            loadSave = true;
        } else if (arg == "--help") {
            std::cout << "用法:" << std::endl;
            std::cout << "  MyGame                       单机模式" << std::endl;
            std::cout << "  MyGame --server [端口]       开启联机服务器（默认 8888）" << std::endl;
            std::cout << "  MyGame --server 8888 --load  开服并读取 server_save.txt 存档" << std::endl;
            std::cout << "  MyGame --client <主机> [端口] 连接服务器" << std::endl;
            return 0;
        }
    }

    if (mode == "server") {
        game::NetServer server(port, loadSave);
        return server.run();
    }
    if (mode == "client") {
        if (host.empty()) {
            std::cout << "请指定服务器地址: MyGame --client <主机> [端口]" << std::endl;
            return 1;
        }
        game::GameController ctrl;
        return ctrl.runClient(host, port);
    }

    game::GameController ctrl;
    return ctrl.runLocal();
}
