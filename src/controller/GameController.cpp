#include "controller/GameController.h"

#include <conio.h>
#include <iostream>

#include <windows.h>

#include "model/GameState.h"
#include "net/NetClient.h"

namespace game {

namespace {
std::vector<std::string> toVector(const std::deque<std::string>& d) {
    return std::vector<std::string>(d.begin(), d.end());
}

WorldSnapshot refreshSnapshot(GameState& state, int selfId) {
    WorldSnapshot snap;
    parseSnapshot(state.serializeSnapshot(selfId), snap);
    return snap;
}
}  // namespace

int GameController::runLocal() {
    state_ = std::make_unique<GameState>();
    const int selfId = doLocalLogin();
    if (selfId < 0) return 0;
    localMainLoop(selfId);
    return 0;
}

GameController::GameController() = default;
GameController::~GameController() = default;

int GameController::doLocalLogin() {
    while (true) {
        renderer_.printLoginMenu();
        const char key = static_cast<char>(_getch());
        switch (key) {
            case '1': {
                renderer_.clear();
                std::cout << "请输入角色名称:" << std::endl;
                std::string name;
                std::cin >> name;
                const auto& p = state_->addPlayer(name);
                state_->addMessage("欢迎来到冒险大陆，" + p.name + "！");
                return p.id;
            }
            case '2': {
                if (state_->loadFromFile("savegame.txt")) {
                    state_->addMessage("读取存档成功。");
                    return state_->players().front().id;
                }
                renderer_.printMessage("未找到存档文件。");
                break;
            }
            case '3':
                renderer_.printAbout();
                break;
            case '4':
                return -1;
            default:
                break;
        }
    }
}

void GameController::localMainLoop(int selfId) {
    bool running = true;
    while (running) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printMain(snap, state_->messages());

        const char key = static_cast<char>(_getch());
        switch (key) {
            case 'w': state_->movePlayer(selfId, Direction::Up); break;
            case 's': state_->movePlayer(selfId, Direction::Down); break;
            case 'a': state_->movePlayer(selfId, Direction::Left); break;
            case 'd': state_->movePlayer(selfId, Direction::Right); break;
            case '1':
            case '2':
            case '3': {
                const int slot = key - '1';
                if (slot >= 0 && slot < static_cast<int>(snap.occupants.size())) {
                    if (snap.occupants[slot].isNpc) {
                        localDialogueLoop(selfId, slot);
                    } else if (state_->startFight(selfId, slot)) {
                        localCombatLoop(selfId);
                    }
                }
                break;
            }
            case 'i': localInventoryLoop(selfId); break;
            case 'c': localStatusLoop(selfId); break;
            case 'm': localMissionLoop(selfId); break;
            case 'p':
                if (state_->saveToFile("savegame.txt")) {
                    state_->addMessage("游戏已保存。");
                } else {
                    state_->addMessage("保存失败！");
                }
                break;
            case 'q':
            case 'Q':
                running = false;
                break;
            default:
                break;
        }
    }
}

void GameController::localCombatLoop(int selfId) {
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        const PlayerView* self = findSelf(snap);
        if (!self || !self->inCombat) return;

        renderer_.printCombat(snap);
        const char key = static_cast<char>(_getch());
        switch (key) {
            case '1': state_->fightRound(selfId, FightAction::Attack); break;
            case '2': state_->fightRound(selfId, FightAction::Skill); break;
            case '3': localInventoryLoop(selfId); break;
            case '4': state_->fightRound(selfId, FightAction::Flee); break;
            default: break;
        }
    }
}

void GameController::localDialogueLoop(int selfId, int slot) {
    state_->talkToNpc(selfId, slot);

    auto snap = refreshSnapshot(*state_, selfId);
    NpcRole role = NpcRole::None;
    if (slot >= 0 && slot < static_cast<int>(snap.occupants.size())) {
        role = snap.occupants[slot].role;
    }
    const bool hasShop = role == NpcRole::Merchant || role == NpcRole::Blacksmith;

    while (true) {
        renderer_.printNpcMenu(state_->messages(), hasShop);
        const char key = static_cast<char>(_getch());
        if (key == '1') {
            localMissionLoop(selfId);
        } else if (key == '2' && hasShop) {
            localShopLoop(selfId, role);
        } else if (key == '3' || key == 'l' || key == 'L') {
            return;
        }
        snap = refreshSnapshot(*state_, selfId);
    }
}

void GameController::localShopLoop(int selfId, NpcRole role) {
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printShop(snap, role);
        const char key = static_cast<char>(_getch());
        if (key >= '1' && key <= '9') {
            state_->buyItem(selfId, key - '1');
        } else if (key == 'l' || key == 'L') {
            return;
        }
    }
}

void GameController::localInventoryLoop(int selfId) {
    int page = 0;
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printInventory(snap, page);
        const char key = static_cast<char>(_getch());
        if (key >= '0' && key <= '9') {
            localItemDetail(selfId, page * 10 + (key - '0'));
        } else if (key == 'j' || key == 'J') {
            if (page < 2) ++page;
        } else if (key == 'k' || key == 'K') {
            if (page > 0) --page;
        } else if (key == 'l' || key == 'L') {
            return;
        }
    }
}

void GameController::localItemDetail(int selfId, int slot) {
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printItemDetail(snap, slot);
        const char key = static_cast<char>(_getch());
        if (key == 'j' || key == 'J') {
            state_->useItem(selfId, slot);
            return;
        }
        if (key == 'k' || key == 'K') {
            state_->dropItem(selfId, slot);
            return;
        }
        if (key == 's' || key == 'S') {
            state_->sellItem(selfId, slot);
            return;
        }
        if (key == 'l' || key == 'L') return;
    }
}

void GameController::localStatusLoop(int selfId) {
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printStatus(snap);
        const char key = static_cast<char>(_getch());
        if (key == 'l' || key == 'L') return;
    }
}

void GameController::localMissionLoop(int selfId) {
    while (true) {
        const auto snap = refreshSnapshot(*state_, selfId);
        renderer_.printMission(snap);
        const char key = static_cast<char>(_getch());
        if (key == 'l' || key == 'L') return;
    }
}

// ---------------- 联机客户端 ----------------

int GameController::runClient(const std::string& host, int port) {
    NetClient client;
    if (!client.connectTo(host, port)) {
        renderer_.printMessage("无法连接到服务器 " + host + ":" + std::to_string(port));
        return 1;
    }

    renderer_.clear();
    std::cout << "请输入角色名称:" << std::endl;
    std::string name;
    std::cin >> name;
    client.sendCommand("LOGIN " + sanitizeName(name));

    std::deque<std::string> messages;
    WorldSnapshot snap;
    bool running = true;
    bool dirty = true;  // 只在“有新数据”或“按了键”时重绘，避免无操作时闪屏
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

            if (key == 'w' || key == 'a' || key == 's' || key == 'd') {
                const char* dir = key == 'w' ? "u" : key == 's' ? "d" : key == 'a' ? "l" : "r";
                client.sendCommand(std::string("MOVE ") + dir);
            } else if (key == '1' || key == '2' || key == '3' || key == '4') {
                const int slot = key - '1';
                if (inCombat) {
                    if (key == '1') client.sendCommand("ATK");
                    else if (key == '2') client.sendCommand("SKILL");
                    else if (key == '3') clientInventoryMode(client, messages, snap);
                    else if (key == '4') client.sendCommand("FLEE");
                } else if (key != '4' && slot >= 0 && slot < static_cast<int>(snap.occupants.size())) {
                    if (snap.occupants[slot].isNpc) {
                        client.sendCommand("TALK " + std::to_string(slot));
                        clientDialogueMode(client, messages, snap, slot);
                    } else {
                        client.sendCommand("FIGHT " + std::to_string(slot));
                    }
                }
            } else if (key == 'i') {
                clientInventoryMode(client, messages, snap);
            } else if (key == 'c') {
                clientStatusMode(client, messages, snap);
            } else if (key == 'm') {
                clientMissionMode(client, messages, snap);
            } else if (key == 'p') {
                client.sendCommand("SAVE");
            } else if (key == 'q' || key == 'Q') {
                client.sendCommand("QUIT");
                running = false;
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
