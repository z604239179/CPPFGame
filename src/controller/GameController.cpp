#include "controller/GameController.h"

#include <climits>
#include <conio.h>
#include <iostream>
#include <string>

#include <windows.h>

#include "net/NetClient.h"

namespace game {

namespace {
std::vector<std::string> toVector(const std::deque<std::string>& d) {
    return std::vector<std::string>(d.begin(), d.end());
}
}  // namespace

GameController::GameController() = default;
GameController::~GameController() = default;

// ---------------- 登录 / 注册 ----------------

int GameController::runClient(const std::string& host, int port) {
    NetClient client;
    if (!client.connectTo(host, port)) {
        renderer_.printMessage("无法连接到服务器 " + host + ":" + std::to_string(port));
        return 1;
    }

    std::deque<std::string> messages;
    WorldSnapshot snap;

    // 登录注册循环，直到登录成功或用户退出
    if (!loginLoop(client, messages, snap)) {
        return 0;
    }

    // ---- 游戏主循环 ----
    bool running = true;
    bool dirty = true;
    while (running) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) {
            renderer_.printMessage("与服务器断开连接。");
            return 1;
        }
        if (result == PumpResult::Updated) dirty = true;

        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            const PlayerView* self = findSelf(snap);
            const bool inCombat = self && self->inCombat;
            dirty = true;

            if (inCombat) {
                // ---- 战斗中 ----
                if (key == 'q' || key == 'Q') {
                    client.sendCommand("SKILL 0");       // 强击
                } else if (key == 'w' || key == 'W') {
                    client.sendCommand("SKILL 1");       // 连斩
                } else if (key == 'e' || key == 'E') {
                    client.sendCommand("SKILL 2");       // 治疗术
                } else if (key == 'r' || key == 'R') {
                    client.sendCommand("SKILL 3");       // 必杀技
                } else if (key == '1') {
                    client.sendCommand("CUSE 0");       // 消耗品格1
                } else if (key == '2') {
                    client.sendCommand("CUSE 1");       // 消耗品格2
                } else if (key == '3') {
                    client.sendCommand("CUSE 2");       // 消耗品格3
                } else if (key == '4') {
                    client.sendCommand("CUSE 3");       // 消耗品格4
                } else if (key == 'f' || key == 'F') {
                    client.sendCommand("FLEE");        // 逃跑
                }
            } else {
                // ---- 探索中 ----
                if (key == 'w' || key == 'a' || key == 's' || key == 'd') {
                    const char* dir = key == 'w' ? "u" : key == 's' ? "d" : key == 'a' ? "l" : "r";
                    client.sendCommand(std::string("MOVE ") + dir);
                } else if (key == '1' && !snap.occupants.empty() && snap.occupants[0].isNpc) {
                    client.sendCommand("TALK 0");
                    clientDialogueMode(client, messages, snap, 0);
                } else if (key == 'i') {
                    clientInventoryMode(client, messages, snap);
                } else if (key == 'c') {
                    clientStatusMode(client, messages, snap);
                } else if (key == 'm') {
                    clientMissionMode(client, messages, snap);
                } else if (key == 'p') {
                    client.sendCommand("SAVE");
                } else if (key == 27) {  // Esc 退出
                    client.sendCommand("QUIT");
                    running = false;
                }
            }
        }

        if (dirty) {
            renderer_.printMain(snap, toVector(messages));
            dirty = false;
        }
        Sleep(10);
    }
    return 0;
}

// 登录注册主菜单循环，返回 true 表示已进入游戏，false 表示用户选择退出
bool GameController::loginLoop(NetClient& client, std::deque<std::string>& messages,
                               WorldSnapshot& snap) {
    while (true) {
        // 先消费掉服务器推送的欢迎/提示消息，显示在登录菜单下方
        bool gotMsg = false;
        while (pumpOnce(client, messages, snap)) {
            if (!snap.players.empty()) return true;  // 已登录（收到 STATE）
            gotMsg = true;
        }

        renderer_.clear();
        renderer_.setColor(ConsoleColor::Yellow);
        std::cout << "================ 冒险大陆 ================\n";
        std::cout << "+           1. 登录账号                 +\n";
        std::cout << "+           2. 注册账号                 +\n";
        std::cout << "+           3. 关于制作                 +\n";
        std::cout << "+           4. 退出游戏                 +\n";
        std::cout << "==========================================\n";
        renderer_.setColor(ConsoleColor::Normal);
        if (gotMsg) {
            // 显示最近一条服务器消息
            for (const auto& m : messages) {
                std::cout << "> " << m << "\n";
            }
        }
        renderer_.present();

        const char key = static_cast<char>(_getch());
        if (key == '1') {
            if (doLoginInput(client, messages, snap)) return true;
        } else if (key == '2') {
            doRegisterInput(client, messages, snap);
        } else if (key == '3') {
            renderer_.printAbout();
        } else if (key == '4' || key == 'q' || key == 'Q') {
            client.sendCommand("QUIT");
            return false;
        }
    }
}

bool GameController::doLoginInput(NetClient& client, std::deque<std::string>& messages,
                                  WorldSnapshot& snap) {
    renderer_.clear();
    renderer_.setColor(ConsoleColor::Yellow);
    std::cout << "===== 登录 =====\n";
    renderer_.setColor(ConsoleColor::Normal);
    std::cout << "账号: ";
    renderer_.present();
    std::string user;
    std::cin >> user;
    std::cin.ignore(INT_MAX, '\n');

    renderer_.clear();
    std::cout << "密码: ";
    renderer_.present();
    std::string pass;
    std::cin >> pass;
    std::cin.ignore(INT_MAX, '\n');

    // 清除控制台输入缓冲中残留的事件，避免干扰游戏循环的 _kbhit()
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    client.sendCommand("LOGIN " + user + " " + pass);

    // 等待服务器响应：持续拉取，收到 STATE 视为成功，收到 ERR 视为失败，超时报错
    for (int i = 0; i < 200; ++i) {  // 约 2 秒超时
        const size_t before = messages.size();
        if (pumpOnce(client, messages, snap)) {
            if (!snap.players.empty()) return true;  // 收到 STATE，登录成功
            // 检查是否收到 ERR（NetClient 把 ERR 转成 "[系统] ..."）
            bool gotErr = false;
            for (size_t j = before; j < messages.size(); ++j) {
                if (messages[j].rfind("[系统]", 0) == 0) { gotErr = true; break; }
            }
            if (gotErr) {
                renderer_.clear();
                renderer_.setColor(ConsoleColor::Red);
                for (const auto& m : messages) std::cout << "> " << m << "\n";
                renderer_.setColor(ConsoleColor::Normal);
                std::cout << "\n按任意键返回...";
                renderer_.present();
                messages.clear();
                _getch();
                return false;
            }
            // 仅收到 MSG（如欢迎语），继续等待 STATE
        }
        Sleep(10);
    }
    renderer_.printMessage("登录超时，请重试。");
    return false;
}

bool GameController::doRegisterInput(NetClient& client, std::deque<std::string>& messages,
                                     WorldSnapshot& snap) {
    renderer_.clear();
    renderer_.setColor(ConsoleColor::Yellow);
    std::cout << "===== 注册 =====\n";
    renderer_.setColor(ConsoleColor::Normal);
    std::cout << "账号(>=3位,无空格): ";
    renderer_.present();
    std::string user;
    std::cin >> user;
    std::cin.ignore(INT_MAX, '\n');

    renderer_.clear();
    std::cout << "密码(>=3位,无空格): ";
    renderer_.present();
    std::string pass;
    std::cin >> pass;
    std::cin.ignore(INT_MAX, '\n');

    renderer_.clear();
    std::cout << "角色名: ";
    renderer_.present();
    std::string charName;
    std::cin >> charName;
    std::cin.ignore(INT_MAX, '\n');

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));

    client.sendCommand("REGISTER " + user + " " + pass + " " + charName);

    for (int i = 0; i < 200; ++i) {
        if (pumpOnce(client, messages, snap)) {
            renderer_.clear();
            renderer_.setColor(ConsoleColor::Green);
            for (const auto& m : messages) std::cout << "> " << m << "\n";
            renderer_.setColor(ConsoleColor::Normal);
            std::cout << "\n按任意键返回登录菜单...";
            renderer_.present();
            messages.clear();
            _getch();
            return false;  // 注册不自动登录
        }
        Sleep(10);
    }
    renderer_.printMessage("注册超时，请重试。");
    return false;
}

// 拉取一帧服务器消息；返回 true 表示收到了任意 MSG/STATE/ERR
bool GameController::pumpOnce(NetClient& client, std::deque<std::string>& messages,
                              WorldSnapshot& snap) {
    const PumpResult r = client.pump(messages, snap);
    if (r == PumpResult::Disconnected) {
        renderer_.printMessage("与服务器断开连接。");
        std::exit(1);
    }
    return r == PumpResult::Updated;
}

// ---------------- 各游戏界面（联机） ----------------

void GameController::clientDialogueMode(NetClient& client, std::deque<std::string>& messages,
                                        WorldSnapshot& snap, int slot) {
    NpcRole role = NpcRole::None;
    if (slot >= 0 && slot < static_cast<int>(snap.occupants.size())) {
        role = snap.occupants[slot].role;
    }
    const bool hasShop = role == NpcRole::Merchant || role == NpcRole::Blacksmith;

    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key == '1') {
                clientMissionMode(client, messages, snap);
            } else if (key == '2' && hasShop) {
                clientShopMode(client, messages, snap, role);
            } else if (key == '3' || key == 'l' || key == 'L') {
                inMenu = false;
            }
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printNpcMenu(toVector(messages), hasShop);
            dirty = false;
        }
        Sleep(10);
    }
}

void GameController::clientShopMode(NetClient& client, std::deque<std::string>& messages,
                                    WorldSnapshot& snap, NpcRole role) {
    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key >= '1' && key <= '9') {
                client.sendCommand("BUY " + std::to_string(key - '1'));
            } else if (key == 'l' || key == 'L') {
                inMenu = false;
            }
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printShop(snap, role);
            dirty = false;
        }
        Sleep(10);
    }
}

void GameController::clientInventoryMode(NetClient& client, std::deque<std::string>& messages,
                                         WorldSnapshot& snap) {
    int page = 0;
    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key >= '0' && key <= '9') {
                clientItemDetailMode(client, messages, snap, page * 10 + (key - '0'));
            } else if (key == 'j' || key == 'J') {
                if (page < 2) ++page;
            } else if (key == 'k' || key == 'K') {
                if (page > 0) --page;
            } else if (key == 'l' || key == 'L') {
                inMenu = false;
            }
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printInventory(snap, page);
            dirty = false;
        }
        Sleep(10);
    }
}

void GameController::clientItemDetailMode(NetClient& client, std::deque<std::string>& messages,
                                          WorldSnapshot& snap, int slot) {
    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key == 'j' || key == 'J') {
                client.sendCommand("USE " + std::to_string(slot));
                inMenu = false;
            } else if (key == 'k' || key == 'K') {
                client.sendCommand("DROP " + std::to_string(slot));
                inMenu = false;
            } else if (key == 's' || key == 'S') {
                client.sendCommand("SELL " + std::to_string(slot));
                inMenu = false;
            } else if (key == 'l' || key == 'L') {
                inMenu = false;
            }
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printItemDetail(snap, slot);
            dirty = false;
        }
        Sleep(10);
    }
}

void GameController::clientStatusMode(NetClient& client, std::deque<std::string>& messages,
                                      WorldSnapshot& snap) {
    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key == 'l' || key == 'L') inMenu = false;
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printStatus(snap);
            dirty = false;
        }
        Sleep(10);
    }
}

void GameController::clientMissionMode(NetClient& client, std::deque<std::string>& messages,
                                       WorldSnapshot& snap) {
    bool inMenu = true;
    bool dirty = true;
    while (inMenu) {
        const PumpResult result = client.pump(messages, snap);
        if (result == PumpResult::Disconnected) return;
        if (result == PumpResult::Updated) dirty = true;
        if (_kbhit()) {
            const char key = static_cast<char>(_getch());
            if (key == 'l' || key == 'L') inMenu = false;
            dirty = true;
        }
        if (dirty && inMenu) {
            renderer_.printMission(snap);
            dirty = false;
        }
        Sleep(10);
    }
}

}  // namespace game
