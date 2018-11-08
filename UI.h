#pragma once
#include<iostream>
#include<conio.h>
#include<ctime>
#include<Windows.h>
using namespace std;
class Monster;
class Controller;
class Item;
static class UI
{
public:
	void static PrintLogin(Controller*& Ctrl);
	void static PrintMainUI(Controller* Ctrl);
	void static PrintAbout();
	void static Load();
	void static PrintOperate();
	void static PrintPack(Controller* Ctrl);
	void static PrintItem(Item* item, Controller* controller);   //返回卖出金币
	void static PrintState(Controller* Ctrl);// 玩家状态栏
	void static PrintMonsterState(Monster* ai);
	void static PrintItemName(Item* item);
	void static PrintMonsterName(Monster* ai);
	int static PrintFightOper(Controller* Ctrl);// 战斗时操作界面 返回伤害值

	void static PrintMessage(const string& Msg);//发送消息；
	void static PrintMessage(Item* item);
	void static SetTexColor(PrintColor color);
	void static PrintMap(class Character* character,class Map* map);
};
