#pragma once

#include <deque>
#include <memory>
#include <string>

#include "common/Common.h"
#include "common/Snapshot.h"
#include "view/ConsoleRenderer.h"

namespace game {

class GameState;
class NetClient;

// 控制器（Controller）：把输入翻译成模型动作，并驱动视图渲染
class GameController {
public:
    GameController();
    ~GameController();
    int runLocal();
    int runClient(const std::string& host, int port);

private:
    // ---- 单机模式 ----
    int doLocalLogin();
    void localMainLoop(int selfId);
    void localInventoryLoop(int selfId);
    void localItemDetail(int selfId, int slot);
    void localStatusLoop(int selfId);
    void localMissionLoop(int selfId);
    void localCombatLoop(int selfId);
    void localDialogueLoop(int selfId, int slot);
    void localShopLoop(int selfId, NpcRole role);

    // ---- 联机客户端模式 ----
    void clientDialogueMode(NetClient& client, std::deque<std::string>& messages,
                            WorldSnapshot& snap, int slot);
    void clientShopMode(NetClient& client, std::deque<std::string>& messages,
                        WorldSnapshot& snap, NpcRole role);
    void clientInventoryMode(NetClient& client, std::deque<std::string>& messages,
                             WorldSnapshot& snap);
    void clientItemDetailMode(NetClient& client, std::deque<std::string>& messages,
                              WorldSnapshot& snap, int slot);
    void clientStatusMode(NetClient& client, std::deque<std::string>& messages,
                          WorldSnapshot& snap);
    void clientMissionMode(NetClient& client, std::deque<std::string>& messages,
                           WorldSnapshot& snap);

    std::unique_ptr<GameState> state_;
    ConsoleRenderer renderer_;
};

}  // namespace game
