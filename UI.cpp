#include "stdafx.h"
#include "UI.h"
#include "Map.h"
#include "Monster.h"
#include "SaveGame.h"
#include "Controller.h"
#include "Item.h"
#include "Character.h"
void UI::PrintLogin(Controller*& Ctrl)
{
	while (true)
	{
		system("cls");
		SetTexColor(Yellow);
		cout << "\n\n\n\n\n\n\n\n\n";
		cout << "====================热寂====================\n";
		cout << "+               1.开始游戏                 +\n";
		cout << "+               2.读取游戏                 +\n";
		cout << "+               3.关于制作                 +\n";
		cout << "+               4.退出游戏                 +\n";
		cout << "============================================\n";
		cout << "\n\n\n\n\n\n\n\n\n";

		cout << "                             Version:Alpha";
		SetTexColor(Noraml);
		char input = _getch();
		switch (input)
		{
		case '1':
		{
			system("cls");
			cout << "输入角色名称：" << endl;
			cin >> Ctrl->Name;
			return;
		}
			break;
		case '2':
			SaveGame::Read(Ctrl);
			return;
			break;
		case '3':
			system("cls");
			PrintAbout();
			break;
		case '4':
			exit(0);
			break;
		default:
			break;
		}
	}
}

void UI::PrintMainUI(Controller* Ctrl)
{
	cout << (char)3<<endl;
	cout << Ctrl->Name << "  " << "Lv." << Ctrl->Level << "  " << "G：" << Ctrl->Gold << endl;
	///////////输出血条蓝条图形
	
	printf("HP:");
	SetTexColor(Red);
	for (int i = 0; i < (int)(Ctrl->Pawn->Hp * 20 / Ctrl->Pawn->MaxHp) ; i++)
		printf("■");
	for (int i = (int)(Ctrl->Pawn->Hp * 20 / Ctrl->Pawn->MaxHp); i < 20; i++)
		printf("□"); 

	SetTexColor(Noraml);
	
	printf("\n");

	printf("MP:");
	SetTexColor(Blue);
	for (int i = 0; i < (int)(Ctrl->Pawn->Mp * 20/ Ctrl->Pawn->MaxMp) ; i++)
		printf("■");
	for (int i = (int)(Ctrl->Pawn->Mp * 20/ Ctrl->Pawn->MaxMp) ; i < 20; i++)
		printf("□");

	cout << "\n";
	////////////

	SetTexColor(Noraml);
	cout << "攻击力:" << Ctrl->Pawn->ATK << "  " << "防御力:" << Ctrl->Pawn->DEF << "\n";


	cout << "EXP:" << (int)Ctrl->Exp << "/" << (int)Ctrl->MaxExp;
	SetTexColor(Perpul);
	for (int i = 0; i < (int)((Ctrl->Exp  /Ctrl->MaxExp)*20) ; i++)
		printf(".");

	SetTexColor(Noraml);

	//输出坐标
	cout << "\t\t\t";
	switch (Ctrl->Pawn->Mapindex)
	{
	case 0:
		cout << "村庄 " << Ctrl->Pawn->X << "," << Ctrl->Pawn->Y;
		break;
	default:
		cout << "野外 " << Ctrl->Pawn->X << "," << Ctrl->Pawn->Y;
		break;
	}

	printf("\n");
}
void UI::PrintAbout()
{
	SetTexColor(Green);
	cout<<"▲\t\t\t"<<"树木\n";
	SetTexColor(Noraml);
	cout<<"■\t\t\t"<<"围墙\n";
	SetTexColor(Yellow);
	cout<<"●\t\t\t"<<"玩家\n";
	SetTexColor(Perpul);
	cout<<"○\t\t\t"<<"NPC\n";
	SetTexColor(Noraml);
	cout<<"□\t\t\t"<<"安全区\n\n\n\n";

	SetTexColor(Yellow);
	cout << "\t\t制作人：Ssaturday\n";
	cout << "\t\tQQ:604239179\n";

	system("pause");
	SetTexColor(Noraml);
	
}

void UI::Load()
{
	cout << "加载中...";
}

void UI::PrintOperate()
{
	printf("W--上  S--下 A--左 D--右\n");
	printf("C.状态  I.背包  K.技能  P.保存  L.任务");
}

void UI::PrintPack(Controller* Ctrl)
{
	system("cls");
	int i = 0, j = 0;
	char input;
	//////////////输出物品栏
	for (i = 0; i < 3; i++)
	{
		system("cls");
		for (j = 0; j < 10; j++)
		{
			if (Ctrl->Pack[i][j]->Index != 0)
			{
				cout << j << ".";
				UI::PrintItemName(Ctrl->Pack[i][j]);
				cout<< "     *" << Ctrl->Pack[i][j]->Nums << "\n";
			}
			else cout << j << "." << Ctrl->Pack[i][j]->Name << "\n";
		}
			cout << "J--下一页  K--上一页  L--返回\n";
			cout << "第：" << i << "页\n";
			/////////////////



			//////////////操作物品
			input = _getch();
			if(input>='0'&&input<='9')//使用物品
			{ 
				if (Ctrl->Pack[i][(int)input - 48]->Index != 0)  //使用输入数值的物品
				{
					PrintItem(Ctrl->Pack[i][((int)input) - 48],Ctrl);
				}
				i--;
			}
			else if (input == 'j')//显示下一页物品
			{
				if (i == 2)
					i--;
				continue;   
			}
			else if (input == 'k')//显示上一页
			{
				if(i==0)
					i--; 
				else i = i - 2;
				continue;
			}
			else if (input == 'l')//返回主界面
			{
				return;
			}
			else i--;
	}

	///////////////

	
}

void UI::PrintItem(Item* item,Controller* controller)
{
	char input;
	loop:
	{
		system("cls");
		UI::PrintItemName(item);
		cout << endl;
		cout << "数量：" << item->Nums << endl;
		cout << "价格：" << item->Gold << endl;
		cout << "J--使用 K--丢弃  L--返回";
		input = _getch();
		if (input == 'j')
		{
			controller->UseItem(item->PackPage, item->PackElem);
			return;
			
		}
		else if (input == 'k')
		{
			if (item->Index != 0)
			{
				controller->DropItem(item->PackPage, item->PackElem);
				return;
			}
		}
		else if (input == 'l')
			return ;
		goto loop;
	}
	return ;
}

void UI::PrintState(Controller * Ctrl)
{
	while (1)
	{
		char input;
	system("cls");
	cout << "名称：" << Ctrl->Name << "  " << "等级:" << Ctrl->Level<<"  "<<"金币："<<Ctrl->Gold<<endl;
	cout << "生命值：" << Ctrl->Pawn->Hp << "/" << Ctrl->Pawn->MaxHp << "   " << "魔法值：" << Ctrl->Pawn->Mp << "/" << Ctrl->Pawn->MaxMp << endl;
	cout << "攻击力：" << Ctrl->Pawn->ATK << "        " << "防御力：" << Ctrl->Pawn->DEF << endl;

	cout << "头部：";
	UI::PrintItemName((Item*)Ctrl->Equip[0]);
	cout<< endl;

	cout << "上衣：";
	UI::PrintItemName((Item*)Ctrl->Equip[1]);
	cout<< endl;

	cout << "武器：";
	UI::PrintItemName((Item*)Ctrl->Equip[2]);
	cout<< endl;

	cout << "下装：";
	UI::PrintItemName((Item*)Ctrl->Equip[3]);
	cout<< endl;

	cout << "鞋子：";
	UI::PrintItemName((Item*)Ctrl->Equip[4]);
	cout<< endl;
	cout << endl;
	cout << "                                     L--返回";
	input = _getch();
	if (input == 'l')
		return;
	}
}

void UI::PrintMonsterState(Monster* ai)
{
	cout << endl;
	UI::PrintMonsterName(ai);
	cout << endl;
	cout << "HP:"<<ai->Hp << "/" << ai->MaxHp ;
	cout << "   Level:" << ai->Level << endl;
	//输出怪物的血量

	UI::SetTexColor(Red);
	for (int i = 0; i < (int)(ai->Hp * 20 / ai->MaxHp); i++)
		cout << "■";
	for (int i = (int)(ai->Hp * 20 / ai->MaxHp); i < 20; i++)
		cout << "□";

	UI::SetTexColor(Noraml);
	cout << endl;
}

void UI::PrintItemName(Item* item)
{
	switch (item->Itemquality)
	{
	case ItemQuality::Norml:
		SetTexColor(Noraml);
		cout << item->Name;
		break;
	case ItemQuality::Unusual:
		SetTexColor(Blue);
		cout << item->Name;
		break;
	case ItemQuality::Epic:
		SetTexColor(Perpul);
		cout << item->Name;
		break;
	case ItemQuality::Legend:
		SetTexColor(Yellow);
		cout << item->Name;
		break;
	default:
		SetTexColor(Noraml);
		cout << item->Name;
		break;
	}
	SetTexColor(Noraml);
}

void UI::PrintMonsterName(Monster* ai)
{
	switch (ai->Monstertype)
	{
	case ItemQuality::Norml:
		SetTexColor(Noraml);
		cout << ai->Name;
		break;
	case ItemQuality::Unusual:
		SetTexColor(Blue);
		cout << ai->Name;
		break;
	case ItemQuality::Epic:
		SetTexColor(Perpul);
		cout << ai->Name;
		break;
	case ItemQuality::Legend:
		SetTexColor(Yellow);
		cout << ai->Name;
		break;
	default:
		SetTexColor(Noraml);
		cout << ai->Name;
		break;
	}
	SetTexColor(Noraml);
}

int UI::PrintFightOper(Controller * Ctrl)
{	
		char input;
		printf("1.普通攻击  2.技能攻击  3.使用物品  4.逃跑\n");
		input = _getch();
		switch (input)
		{
		case '1':
		{
			return Ctrl->Pawn->ATK;
			break;
		}
		case '2':
		{
			return (int)(Ctrl->Pawn->ATK*1.5);

			return 0;
			break;
			//使用技能
		}
		case '3':
		{
			PrintPack(Ctrl);
			break;
		}
		case '4':
		{
			return -1;
			break;
		}
		default:
			return 0;
			break;
		}
		return 0;
}



void UI::PrintMessage(const string& Msg)
{
	system("cls");
	cout << Msg;
	Sleep(2000);
	return;
}

void UI::PrintMessage(Item* item)
{
	system("cls");
	cout << "你获得了:";
	UI::PrintItemName(item);
	cout << "!\n";
	Sleep(2000);
	return;
}

void UI::SetTexColor(PrintColor color)
{
	switch (color)
	{
	case Noraml:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		break;
	case Red:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		break;
	case Blue:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE| FOREGROUND_INTENSITY);
		break;
	case Green:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN| FOREGROUND_INTENSITY);
		break;
	case Yellow:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN| FOREGROUND_RED|FOREGROUND_INTENSITY);
		break;
	case Perpul:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED| FOREGROUND_INTENSITY);
		break;
	default:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
		break;
	}
}

void UI::PrintMap( Character* character,  Map* map)
{
	map->BaseMap[map->OnNode.X][map->OnNode.Y].nType = map->OnNode.nType;    //复原玩家所在的位置
	map->OnNode = map->BaseMap[character->X][character->Y];  //重新记录玩家所在的位置
	map->BaseMap[character->X][character->Y].nType =Map::NodeType::Player;//设置玩家的位置
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			//输出地图信息
			switch (map->BaseMap[i][j].nType)
			{
			case Map::NodeType::Space:
				printf("  "); 
				break;
			case Map::NodeType::Tree:
				SetTexColor(Green);
				printf("▲");
				break;
			case Map::NodeType::Wall:
				SetTexColor(Noraml);
				printf("■");
				break;
			case Map::NodeType::Player:
				SetTexColor(Yellow);
				printf("●");
				break;
			case Map::NodeType::People:
				SetTexColor(Perpul);
				printf("○"); 
				break;

			case Map::NodeType::SafeSpace:
				SetTexColor(Noraml);
				printf("□");
				break;
			default:
				printf("  ");
				break;

			}
		}
		printf("\n");
		///////////

	}
	//输出怪物
	for (int i = 0; i < (map->BaseMap[character->X][character->Y].LocalMonster.size()); i++)
	{
		if (map->BaseMap[character->X][character->Y].LocalMonster[i] != nullptr)
		{
			cout << i + 1 << ".";
			UI::PrintMonsterName(map->BaseMap[character->X][character->Y].LocalMonster[i]);
			cout << "  LV:" << map->BaseMap[character->X][character->Y].LocalMonster[i]->Level << endl;
		}
	}

}
