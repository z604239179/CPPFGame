#include "stdafx.h"
#include "Controller.h"
#include "UI.h"
#include"Manager.h"
#include "SaveGame.h"
#include "Character.h"
#include"Equipment.h"
#include"Using.h"
#include "Item.h"
#include "Map.h"
#include "Monster.h"
#include "NPC.h"
#include <string>

Controller::Controller(string InName)
{
	Name = InName;
	Gold = 0;
	Pawn = new Character();
	MaxExp = 100.0f;
	Exp = 0;
	Level = 1;
	//初始化背包
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 10; j++)
		{
			Pack[i][j] = new Item(0, "空", 0, 0, ItemQuality::Norml);
			Pack[i][j]->PackPage = i;
			Pack[i][j]->PackElem = j;
		}

	//初始化装备栏
	for (int i = 0; i < 5; i++)
	{
		Equip[i] = new Equipment(0, "空", 0, 0,  ItemQuality::Norml, 0, 0, 0, 0, 0);
	}
}

Controller::~Controller()
{
	delete Pawn;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 9; j++)
			delete Pack[i][j];  //释放背包空间

	for (int i = 0; i < 5; i++)
	{
		delete Equip[i]; //释放装备栏空间
	}
}

void Controller::LevelUp()
{
	while (Exp >= MaxExp)
	{
		string msg;
		Level++;
		Exp -= MaxExp;
		MaxExp *= 1.5f;
		Pawn->ATK += 1;
		Pawn->DEF += 1;
		Pawn->MaxHp += 5;
		Pawn->Hp = Pawn->MaxHp;
		Pawn->MaxMp += 5;
		Pawn->Mp = Pawn->MaxMp;
		msg = "你升到了";
		
		msg.append(to_string(Level));
		msg.append("级!");
		UI::PrintMessage(msg);

	}
}

bool Controller::GetItem(Item * ItemPoint)
{

	if (typeid(*ItemPoint) == typeid(Using))
	{
		int Tempi = -1;
		int Tempj = -1;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (Tempi == -1 && Tempj == -1 && Pack[i][j]->Index == 0)		//判断是否有存储空间
				{
					Tempi = i;			
					Tempj = j;
				}
				if (Pack[i][j]->Index == ItemPoint->Index&&typeid(*Pack[i][j])==typeid(Using))  //消耗品堆叠
				{
					Pack[i][j]->Nums+=ItemPoint->Nums;
					return 1;
				}
			}
		}
		if (Tempi != -1 && Tempj != -1)  //找到空闲背包空间
		{
			delete this->Pack[Tempi][Tempj];
			this->Pack[Tempi][Tempj] = nullptr;
			this->Pack[Tempi][Tempj] = ItemPoint;
			this->Pack[Tempi][Tempj]->PackElem = Tempj;
			this->Pack[Tempi][Tempj]->PackPage = Tempi;
			this->Pack[Tempi][Tempj]->Owner = this;
			return 1;
		}
		delete ItemPoint;
		return 0;  //背包满了返回0

	}
	else if (typeid(*ItemPoint) == typeid(Equipment))
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				if (Pack[i][j]->Index == 0)
				{
					delete Pack[i][j];
					Pack[i][j] = nullptr;
					Pack[i][j] = ItemPoint;
					Pack[i][j]->PackElem = j;
					Pack[i][j]->PackPage = i;
					Pack[i][j]->Owner = this;
					return 1;
				}
			}
		}
		return 0;

	}
	return 0;				//背包已满返回0
}


void Controller::DropItem(int PackPage, int PackElem)
{
	delete Pack[PackPage][PackElem];
	Pack[PackPage][PackElem] = nullptr;
	Item* TempItem =  new Item(0, "空", 0, 0, ItemQuality::Norml);
	TempItem->PackPage = PackPage;
	TempItem->PackElem = PackElem;
	TempItem->Owner = this;
	Pack[PackPage][PackElem] = TempItem;
	return;

}

void Controller::UseItem(int PackPage, int PackElem)
{
	if (typeid(*Pack[PackPage][PackElem]) == typeid(Using))  //判断是不是消耗品
	{

		Using* useitem = (Using*)Pack[PackPage][PackElem];
		useitem->Nums--;
		if (useitem->Nums <= 0)  //消耗完
		{
			delete useitem;
			Pack[PackPage][PackElem] = nullptr;
			Pack[PackPage][PackElem] = new Item(0,"空",0,0,ItemQuality::Norml);
		}

		 return;

	}

	else if (typeid(*Pack[PackPage][PackElem]) == typeid(Equipment))
	{
		Equipment* useitem = (Equipment*)Pack[PackPage][PackElem];
		//更改属性
		this->Pawn->ATK += ((useitem->ATK) - (this->Equip[useitem->Type]->ATK));
		this->Pawn->DEF += ((useitem->DEF) - (this->Equip[useitem->Type]->DEF));
		this->Pawn->MaxMp += ((useitem->Hp) - (this->Equip[useitem->Type]->Hp));
		this->Pawn->MaxMp += ((useitem->Mp) - (this->Equip[useitem->Type]->Mp));

		//
		Item* TempItemptr = Equip[useitem->Type];	
		Equip[useitem->Type] = useitem;
		Pack[PackPage][PackElem] = TempItemptr;
		return;
	}

}

void Controller::SellItem(int Index)
{
	int Page = Index % 10;  //取得物品所在页；
	int Loc = Index - Page * 10; //取得物品所在页的所在位置
	Gold += Pack[Page][Loc]->Gold;
	delete Pack[Page][Loc];  //释放所出售物品的对象
	Pack[Page][Loc] = new Item(0, "空", 0, 0, ItemQuality::Norml);

}

void Controller::input(char input)
{
	switch (input)
	{
	case 'p':
		SaveGame::Save(this);
		break;
	case 'i':
		UI::PrintPack(this);
		break;
	case 'c':
		UI::PrintState(this);
		break;
	}
}

void Controller::input(char input, Map*& map)
{


	switch (input)
	{
	case 'w':
		if (map->BaseMap[Pawn->X - 1][Pawn->Y].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X - 1][Pawn->Y].nType != Map::NodeType::Tree)  //判断下一步是不是围墙
		{
			Pawn->X--;
		}
		break;
	case 's':
		if (map->BaseMap[Pawn->X + 1][Pawn->Y].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X + 1][Pawn->Y].nType != Map::NodeType::Tree)	//判断下一步是不是围墙
			Pawn->X++;
		break;
	case 'a':
		if (map->BaseMap[Pawn->X][Pawn->Y - 1].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X][Pawn->Y - 1].nType != Map::NodeType::Tree) //判断下一步是不是围墙
			Pawn->Y--;
		break;
	case 'd':
		if (map->BaseMap[Pawn->X][Pawn->Y + 1].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X][Pawn->Y + 1].nType != Map::NodeType::Tree) //判断下一步是不是围墙
			Pawn->Y++;
		break;

	case'1':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 1)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc ==0 )
		{ //判断是不是怪物
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]))
			{
				LevelUp();
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0] = Manager::SpawnMonster(); //刷新新怪
			}
		}
		else 
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :战斗失败的处罚
		break;
	case'2':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 2)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc == 0)
		{ //判断是不是怪物
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1]))
			{
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1] = Manager::SpawnMonster();//刷新新怪
				LevelUp();
			}
		}
		else
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :战斗失败的处罚
		break;
	case'3':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 3)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc == 0)
		{ //判断是不是怪物
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2]))
			{
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2] = Manager::SpawnMonster();//刷新新怪
				LevelUp();
			}
		}
		else
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :战斗失败的处罚
		break;
	}

}

