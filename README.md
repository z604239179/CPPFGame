# 冒险大陆（MVC 重构 + MMO 联机版）

这是对学生时代项目的完整重构：按 **MVC 架构**分层重写，采用 **纯 MMO 架构**——
所有玩家必须连接服务器，通过账号密码登录，玩家数据由服务器持久化保存。

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
│   ├── Player.h/cpp    （玩家状态，含序列化/反序列化）
│   ├── Inventory.h/cpp （背包 + 装备栏）
│   ├── Item / Equipment / Consumable / Monster
├── view/               View 层：控制台渲染器（双缓冲无闪烁），只画不决策
│   └── ConsoleRenderer.h/cpp
├── controller/         Controller 层：输入 -> 指令 -> 渲染服务器同步回来的快照
│   └── GameController.h/cpp
└── net/                Network 层：权威服务器 + 客户端 + 账号管理
    ├── NetServer.h/cpp （持有模型，处理指令，广播快照，集成账号系统）
    ├── NetClient.h/cpp （连接、发送指令、接收快照）
    └── AccountManager.h/cpp （账号持久化、注册、登录校验）
```

两种运行模式：

| 模式 | 说明 |
| --- | --- |
| 服务器 | 权威服务器，持有世界状态与账号数据库，等待客户端连接 |
| 客户端 | 连接服务器，登录/注册账号，发送指令并渲染同步快照 |

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
服务器启动时会自动读取同目录下的 `accounts.txt` 账号数据库（不存在则视为空）。

### 2. 玩家连接

```bat
Release\MyGame.exe                              # 默认连接本机 127.0.0.1:8888
Release\MyGame.exe --client 127.0.0.1 8888      # 显式指定本机
Release\MyGame.exe --client 192.168.1.100 8888  # 局域网，填房主 IP
```

局域网联机时，服务器电脑需要知道自己的局域网 IP（`ipconfig` 查看 IPv4 地址），
并且防火墙需要允许 `MyGame.exe` 通信。

### 3. 登录 / 注册

连接后选择：

| 选项 | 说明 |
| --- | --- |
| `1` | 登录（输入账号、密码） |
| `2` | 注册（输入账号≥3位、密码≥3位、角色名） |
| `3` | 关于制作 |
| `4` | 退出游戏 |

注册成功后回到菜单，用 `1` 登录即可进入世界。账号数据自动保存在服务器目录的
`accounts.txt`，玩家下线时进度自动回写，下次登录继续上次的进度。

### 4. 游戏操作

| 按键 | 功能 |
| --- | --- |
| `W A S D` | 移动 |
| `1 2 3` | 与当前格子里的怪物战斗 / 与 NPC 对话 |
| `I` | 背包（`0-9` 查看物品，`J` 使用，`K` 丢弃，`S` 出售，`L` 返回） |
| `C` | 角色状态 |
| `M` | 任务 |
| `P` | 保存（请求服务器立即写入账号数据） |
| `Q` | 退出（下线，数据自动保存） |

战斗中：`1` 攻击，`2` 技能，`3` 使用物品，`4` 逃跑。
地图上的 `O` 是传送门，可以往返「村庄」和「城镇」。

> 所有玩家的状态由服务器统一计算并广播，同一个格子的怪物血量是共享的，
> 两个玩家可以一起打同一只怪。

## 账号数据格式

`accounts.txt`（行式文本）：

```
<账号数>
<用户名>
<密码>
PLAYERDATA_BEGIN
<玩家序列化数据多行>
PLAYERDATA_END
<用户名>
...
```

玩家数据格式见 `Player::serialize()`，与原 `GameState` 存档单玩家段兼容。

## 网络协议（简要）

- 传输：TCP，每条消息 = 4 字节小端长度 + 负载文本。
- 客户端 -> 服务器：`LOGIN <账号> <密码>`、`REGISTER <账号> <密码> <角色名>`、
  `MOVE <u/d/l/r>`、`FIGHT <槽位>`、`TALK <槽位>`、`ATK`、`SKILL`、`FLEE`、
  `USE <格子>`、`DROP <格子>`、`SELL <格子>`、`BUY <商店序号>`、`SAVE`、`QUIT`。
- 服务器 -> 客户端：`MSG <文本>`、`STATE <世界快照>`、`ERR <文本>`。

快照格式见 `src/common/Snapshot.cpp`，帧协议见 `src/common/Protocol.cpp`。
