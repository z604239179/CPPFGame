# 冒险大陆（MVC 重构 + 多人联机版）

这是对学生时代项目的完整重构：按 **MVC 架构**分层重写，并加入 **TCP 多人联机**能力。

## 架构

```
src/
├── main.cpp            入口：解析命令行，选择运行模式
├── common/             公共层：类型定义、世界快照、网络帧协议
│   ├── Common.h        （枚举、常量、Position/Stats 等基础结构）
│   ├── Snapshot.h/cpp  （服务器 -> 客户端的世界快照）
│   └── Protocol.h/cpp  （长度前缀帧协议 + Winsock RAII）
├── model/              Model 层：纯游戏状态与规则，不依赖 UI/网络
│   ├── GameState.h/cpp （世界核心：玩家、地图、移动/战斗/商店/任务规则）
│   ├── GameData.h/cpp  （静态数据目录：装备/药剂/怪物/NPC/任务/地图）
│   ├── WorldMap.h/cpp  （地形 + 每格动态怪物/NPC）
│   ├── Player.h/cpp    （玩家状态）
│   ├── Inventory.h/cpp （背包 + 装备栏）
│   ├── Item / Equipment / Consumable / Monster
├── view/               View 层：控制台渲染器，只画不决策
│   └── ConsoleRenderer.h/cpp
├── controller/         Controller 层：输入 -> 模型动作 -> 刷新视图
│   └── GameController.h/cpp
└── net/                Network 层：权威服务器 + 客户端
    ├── NetServer.h/cpp （持有模型，处理指令，广播快照）
    └── NetClient.h/cpp （连接、发送指令、接收快照）
```

三种运行模式共用同一套 `GameState` 规则：

| 模式 | 说明 |
| --- | --- |
| 单机 | 本地直接操作模型，无需网络 |
| 服务器 | 权威服务器，持有世界状态，等待客户端连接 |
| 客户端 | 连接服务器，发送指令并渲染服务器同步回来的快照 |

## 构建

使用 Visual Studio 打开 `MyGame.vcxproj` 直接编译（Win32 / x64 均可），或在命令行：

```bat
"C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe" MyGame.vcxproj /p:Configuration=Release /p:Platform=Win32
```

生成的可执行文件：`Release\MyGame.exe`（或 `Debug\MyGame.exe`）。

## 联机玩法

### 1. 先开服务器（房主）

```bat
Release\MyGame.exe --server 8888
```

`8888` 是端口号，可以随便换。开服后按 `Q` 关闭服务器。
想读取上一次的服务器存档，加上 `--load`：

```bat
Release\MyGame.exe --server 8888 --load
```

### 2. 其他玩家连接

在**同一台电脑**或**同一个局域网**的其他电脑上运行：

```bat
Release\MyGame.exe --client 127.0.0.1 8888        # 本机测试
Release\MyGame.exe --client 192.168.1.100 8888    # 局域网，填房主的 IP
```

局域网联机时，服务器电脑需要知道自己的局域网 IP（`ipconfig` 查看 IPv4 地址），
并且防火墙需要允许 `MyGame.exe` 通信。

### 3. 开始游戏

连接后输入角色名即可进入世界：

| 按键 | 功能 |
| --- | --- |
| `W A S D` | 移动 |
| `1 2 3` | 与当前格子里的怪物战斗 / 与 NPC 对话 |
| `I` | 背包（`0-9` 查看物品，`J` 使用，`K` 丢弃，`S` 出售，`L` 返回） |
| `C` | 角色状态 |
| `M` | 任务 |
| `P` | 服务器存档（联机时由服务器保存） |
| `Q` | 退出 |

战斗中：`1` 攻击，`2` 技能，`3` 使用物品，`4` 逃跑。
地图上的 `O` 是传送门，可以往返「村庄」和「城镇」。

> 所有玩家的状态由服务器统一计算并广播，同一个格子的怪物血量是共享的，
> 两个玩家可以一起打同一只怪。

## 单机模式

不带参数直接运行就是单机：

```bat
Release\MyGame.exe
```

## 网络协议（简要）

- 传输：TCP，每条消息 = 4 字节小端长度 + 负载文本。
- 客户端 -> 服务器：`LOGIN <名字>`、`MOVE <u/d/l/r>`、`FIGHT <槽位>`、
  `TALK <槽位>`、`ATK`、`SKILL`、`FLEE`、`USE <格子>`、`DROP <格子>`、
  `SELL <格子>`、`BUY <商店序号>`、`SAVE`、`QUIT`。
- 服务器 -> 客户端：`MSG <文本>`、`STATE <世界快照>`、`ERR <文本>`。

快照格式见 `src/common/Snapshot.cpp`，帧协议见 `src/common/Protocol.cpp`。
