#pragma once
#include <vector>
using namespace std;

class Controller;
class Monster;
class Item;
static class Manager
{
public:
	static Monster* SpawnMonster();
	static Item* SpawnItem(int Type, int Index);

	static bool Fight(Controller *Ctrl, Monster* ai);
	static int GetEquipListSize();
	static int GetMonsterListSize();
	static int GetUseingListSize();
	static Item *MonsterDrop(int MonsterLevel);  //生成怪物掉落列表
	static class NPC *ReturnNPC(int MapIndex, int NpcCount);
/*
	static void InitGame();
	static void InitItemData();

	static vector<Item*> ItemDataList;*/
};

