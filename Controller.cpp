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
	//��ʼ������
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 10; j++)
		{
			Pack[i][j] = new Item(0, "��", 0, 0, ItemQuality::Norml);
			Pack[i][j]->PackPage = i;
			Pack[i][j]->PackElem = j;
		}

	//��ʼ��װ����
	for (int i = 0; i < 5; i++)
	{
		Equip[i] = new Equipment(0, "��", 0, 0,  ItemQuality::Norml, 0, 0, 0, 0, 0);
	}
}

Controller::~Controller()
{
	delete Pawn;
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 9; j++)
			delete Pack[i][j];  //�ͷű����ռ�

	for (int i = 0; i < 5; i++)
	{
		delete Equip[i]; //�ͷ�װ�����ռ�
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
		msg = "��������";
		
		msg.append(to_string(Level));
		msg.append("��!");
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
				if (Tempi == -1 && Tempj == -1 && Pack[i][j]->Index == 0)		//�ж��Ƿ��д洢�ռ�
				{
					Tempi = i;			
					Tempj = j;
				}
				if (Pack[i][j]->Index == ItemPoint->Index&&typeid(*Pack[i][j])==typeid(Using))  //����Ʒ�ѵ�
				{
					Pack[i][j]->Nums+=ItemPoint->Nums;
					return 1;
				}
			}
		}
		if (Tempi != -1 && Tempj != -1)  //�ҵ����б����ռ�
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
		return 0;  //�������˷���0

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
	return 0;				//������������0
}


void Controller::DropItem(int PackPage, int PackElem)
{
	delete Pack[PackPage][PackElem];
	Pack[PackPage][PackElem] = nullptr;
	Item* TempItem =  new Item(0, "��", 0, 0, ItemQuality::Norml);
	TempItem->PackPage = PackPage;
	TempItem->PackElem = PackElem;
	TempItem->Owner = this;
	Pack[PackPage][PackElem] = TempItem;
	return;

}

void Controller::UseItem(int PackPage, int PackElem)
{
	if (typeid(*Pack[PackPage][PackElem]) == typeid(Using))  //�ж��ǲ�������Ʒ
	{

		Using* useitem = (Using*)Pack[PackPage][PackElem];
		useitem->Nums--;
		if (useitem->Nums <= 0)  //������
		{
			delete useitem;
			Pack[PackPage][PackElem] = nullptr;
			Pack[PackPage][PackElem] = new Item(0,"��",0,0,ItemQuality::Norml);
		}

		 return;

	}

	else if (typeid(*Pack[PackPage][PackElem]) == typeid(Equipment))
	{
		Equipment* useitem = (Equipment*)Pack[PackPage][PackElem];
		//��������
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
	int Page = Index % 10;  //ȡ����Ʒ����ҳ��
	int Loc = Index - Page * 10; //ȡ����Ʒ����ҳ������λ��
	Gold += Pack[Page][Loc]->Gold;
	delete Pack[Page][Loc];  //�ͷ���������Ʒ�Ķ���
	Pack[Page][Loc] = new Item(0, "��", 0, 0, ItemQuality::Norml);

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
		if (map->BaseMap[Pawn->X - 1][Pawn->Y].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X - 1][Pawn->Y].nType != Map::NodeType::Tree)  //�ж���һ���ǲ���Χǽ
		{
			Pawn->X--;
		}
		break;
	case 's':
		if (map->BaseMap[Pawn->X + 1][Pawn->Y].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X + 1][Pawn->Y].nType != Map::NodeType::Tree)	//�ж���һ���ǲ���Χǽ
			Pawn->X++;
		break;
	case 'a':
		if (map->BaseMap[Pawn->X][Pawn->Y - 1].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X][Pawn->Y - 1].nType != Map::NodeType::Tree) //�ж���һ���ǲ���Χǽ
			Pawn->Y--;
		break;
	case 'd':
		if (map->BaseMap[Pawn->X][Pawn->Y + 1].nType != Map::NodeType::Wall&& map->BaseMap[Pawn->X][Pawn->Y + 1].nType != Map::NodeType::Tree) //�ж���һ���ǲ���Χǽ
			Pawn->Y++;
		break;

	case'1':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 1)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc ==0 )
		{ //�ж��ǲ��ǹ���
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]))
			{
				LevelUp();
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0] = Manager::SpawnMonster(); //ˢ���¹�
			}
		}
		else 
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :ս��ʧ�ܵĴ���
		break;
	case'2':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 2)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc == 0)
		{ //�ж��ǲ��ǹ���
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1]))
			{
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[1] = Manager::SpawnMonster();//ˢ���¹�
				LevelUp();
			}
		}
		else
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :ս��ʧ�ܵĴ���
		break;
	case'3':
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster.size() < 3)
			return;
		if (map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0]->isNpc == 0)
		{ //�ж��ǲ��ǹ���
			if (Manager::Fight(this, map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2]))
			{
				delete map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2];
				map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[2] = Manager::SpawnMonster();//ˢ���¹�
				LevelUp();
			}
		}
		else
		{
			NPC* TempNpc = (NPC*)map->BaseMap[this->Pawn->X][this->Pawn->Y].LocalMonster[0];
			TempNpc->Talk(this);
		}
		//TODO :ս��ʧ�ܵĴ���
		break;
	}

}

