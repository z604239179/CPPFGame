#pragma once

#include<vector>
#include<string>
class Map;
class Misson;
class Item;
class Character;
class Equipment;
class Using;
using namespace std;
class Controller
{
public:
	Character* Pawn;

	//玩家信息
	string Name;
	int Level;
	int Gold;
	float Exp;
	float MaxExp;
	Misson* CurrenMisson;

	Controller(string InName);
	~Controller();


	void LevelUp(); //升级

	//物品相关
	Item* Pack[3][10];
	Equipment* Equip[5];
	bool GetItem(Item* ItemPoint);
	void DropItem(int PackPage, int PackElem);
	void UseItem(int PackPage,int PackElem);
	void SellItem(int Index);
	//装备相关
	//输入相关
	void input(char input);  //处理正常输入
	void input(char input,Map*& map);  //处理移动

};