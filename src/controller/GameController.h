#pragma once

#include <deque>
#include <memory>
#include <string>

#include "common/Common.h"
#include "common/Snapshot.h"
#include "view/ConsoleRenderer.h"

namespace game {

class NetClient;

// 控制器（Controller）：把输入翻译成指令发送给服务器，并驱动视图渲染
class GameController {
public:
    GameController();
    ~GameController();
    int runClient(const std::string& host, int port);

private:
    // ---- 登录 / 注册交互（返回是否已进入游戏）----
    bool loginLoop(NetClient& client, std::deque<std::string>& messages, WorldSnapshot& snap);
    bool doLoginInput(NetClient& client, std::deque<std::string>& messages, WorldSnapshot& snap);
    bool doRegisterInput(NetClient& client, std::deque<std::string>& messages, WorldSnapshot& snap);
    // 拉取一次服务器消息，若收到 STATE 则视为登录成功
    bool pumpOnce(NetClient& client, std::deque<std::string>& messages, WorldSnapshot& snap);

    // ---- 联机游戏各界面 ----
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

    ConsoleRenderer renderer_;
};

}  // namespace game
