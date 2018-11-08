#include"stdafx.h"
#include"NPC.h"
#include<iostream>
#include<conio.h>
#include"Controller.h"
#include"UI.h"
#include "Manager.h"
#include <conio.h>
using namespace std;

NPC::NPC(int index,const string& name, int level, NPCType mtype) :Monster(index,name, level, ItemQuality::Legend)
{

	this->Name = name;
	this->Level = level;
	Exp = level * 100;
	Gold = level * 10;
	ATK = level * 1;
	DEF = level * 1;
	MaxHp = Level * 5;
	Hp = MaxHp;

	isNpc = 1;
	type = mtype;
}


NPC::NPC(const NPC& CopyNpc) :Monster(CopyNpc.Index,CopyNpc.Name, CopyNpc.Level, ItemQuality::Legend)
{
	Exp = CopyNpc.Level * 100;
	Gold = CopyNpc.Level * 10;
	ATK = CopyNpc.Level * 1;
	DEF = CopyNpc.Level * 1;
	MaxHp = CopyNpc.Level * 5;
	Hp = MaxHp;
	Monstertype = CopyNpc.Monstertype;

	isNpc = 1;
	type = CopyNpc.type;
}

void NPC::Talk(Controller *c)
{
	while (1)
	{


		char input;
		system("cls");
		UI::PrintMainUI(c);
		cout << endl;
		switch (this->Level)
		{
			/*
			1.牧童
			5.村长 
			*/
		case 1:
			cout << "你是新来的吗?" << endl;
			break;
		case 5:
			cout << "年轻人，有何贵干啊。" << endl;
			break;
		default:
			cout << "你好~" << endl;
			break;
		}
		cout << "                1.任务" << endl;
		cout << "                2.交易" << endl;
		cout << "                3.闲谈" << endl;
		cout << "                4.离开" << endl;
		input = _getch();
		switch (input)
		{
		case '1':
			CheckMisson();
			break;
		case '2':
			Deal(c);
			break;
		case '3':
			break;
		case '4':
			return;
			break;
		default:
			break;
		}
	}
}
//void NPC::Deal(Controller *c)
//{
//	while (1)
//	{
//		char in;
//		system("cls");
//		UI::PrintMainUI(c);
//		cout << endl;
//
//		//输出商店
//		switch (this->type)
//		{
//		case NPCType::EquipNpc:
//			for (int i = 0; i < 5; i++)
//			{
//				cout << i + 1 << ". " << DealEquipmentList[i].Name << endl;
//				cout << "价格： " << (int)DealEquipmentList[i].Gold*1.5f << "     品质:";
//				switch (DealEquipmentList[i].Itemquality)
//				{
//				case ItemQuality::Norml:
//					UI::SetTexColor(Noraml);
//					cout << "普通" << endl;
//					break;
//				case ItemQuality::Unusual:
//					UI::SetTexColor(Blue);
//					cout << "稀有" << endl;
//					break;
//
//				case ItemQuality::Epic:
//					UI::SetTexColor(Perpul);
//					cout << "史诗" << endl;
//					break;
//
//				case ItemQuality::Legend:
//					UI::SetTexColor(Yellow);
//					cout << "传说" << endl;
//					break;
//				default:
//
//					UI::SetTexColor(Noraml);
//					cout << "普通" << endl;
//					break;
//				}
//
//				UI::SetTexColor(Noraml);
//
//			}
//			cout << endl;
//			cout <<"                                     L--返回\n";
//
//			in = _getch();
//			if (in >= '1'&&in <= '5')
//			{
//				int getno = in - 48;
//				if (c->Gold >= (int)(DealEquipmentList[getno-1].Gold*1.5f))  //判断金币是否足够
//				{
//					c->Gold -= (int)(DealEquipmentList[getno - 1].Gold*1.5f);
//					c->GetItem(new Equipment(DealEquipmentList[getno - 1]));
//					
//				}
//			}
//			else if (in == 'l')
//			{
//				return;
//			}
//
//			break;
//		case NPCType::UsingNPC:
//			for (int i = 0; i < 2; i++)
//			{
//				cout << i + 1 << ". " << DealUsingList[i].Name << endl;
//				cout << "价格： " << (int)(DealUsingList[i].Gold*1.5f) << "     品质:";
//				switch (DealUsingList[i].Itemquality)
//				{
//				case ItemQuality::Norml:
//					UI::SetTexColor(Noraml);
//					cout << "普通" << endl;
//					break;
//				case ItemQuality::Unusual:
//					UI::SetTexColor(Blue);
//					cout << "稀有" << endl;
//					break;
//
//				case ItemQuality::Epic:
//					UI::SetTexColor(Perpul);
//					cout << "史诗" << endl;
//					break;
//
//				case ItemQuality::Legend:
//					UI::SetTexColor(Yellow);
//					cout << "传说" << endl;
//					break;
//				default:
//
//					UI::SetTexColor(Noraml);
//					cout << "普通" << endl;
//					break;
//				}
//
//				UI::SetTexColor(Noraml);
//			}
//			cout << endl;
//			cout << "                                     L--返回\n";
//
//			in = _getch();
//			if (in >= '1'&&in <= '2')
//			{
//				int getno = in - 48;
//				if (c->Gold >= (int)((DealUsingList[getno - 1].Gold*1.5f)))  //判断金币是否足够
//				{
//					c->Gold -= (int)(DealUsingList[getno - 1].Gold*1.5f);
//					c->GetItem(new Using(DealUsingList[getno - 1]));
//
//				}
//			}
//			else if (in == 'l')
//			{
//				return;
//			}
//
//			break;
//		default:
//			break;
//		}
//
//
//	}
//	return;
//}

void NPC::CheckMisson()
{

	return;
}

bool NPC::operator==(const NPC& npc)
{
	return this->Index == npc.Index ? true : false;
}


