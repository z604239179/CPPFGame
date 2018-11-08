#include "stdafx.h"
#include "UNPC.h"
#include <iostream>
#include "Controller.h"
#include "UI.h"
using namespace std;
UNPC::UNPC(int index, const string& name, int level, NPCType mtype):NPC(index,name,level,NPCType::UsingNPC)
{
}

UNPC::UNPC(const NPC& copynpc):NPC(copynpc.Index,copynpc.Name,copynpc.Level,NPCType::UsingNPC)
{

}

void UNPC::Talk(Controller * c)
{
	while (1)
	{


		char input;
		system("cls");
		UI::PrintMainUI(c);
		cout << endl;
		cout << "嘿！我这里有些宝贝要不要看看？" << endl;
		cout << "                1.任务" << endl;
		cout << "                2.交易" << endl;
		cout << "                3.附魔" << endl;
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

void UNPC::Deal(Controller * c)
{
	while (1)
	{
		char in;
		system("cls");
		UI::PrintMainUI(c);
		cout << endl;

		for (int i = 0; i < 2; i++)
		{
			cout << i + 1 << ". " << DealUsingList[i].Name << endl;
			cout << "价格： " << (int)(DealUsingList[i].Gold*1.5f) << "     品质:";
			switch (DealUsingList[i].Itemquality)
			{
			case ItemQuality::Norml:
				UI::SetTexColor(Noraml);
				cout << "普通" << endl;
				break;
			case ItemQuality::Unusual:
				UI::SetTexColor(Blue);
				cout << "稀有" << endl;
				break;

			case ItemQuality::Epic:
				UI::SetTexColor(Perpul);
				cout << "史诗" << endl;
				break;

			case ItemQuality::Legend:
				UI::SetTexColor(Yellow);
				cout << "传说" << endl;
				break;
			default:

				UI::SetTexColor(Noraml);
				cout << "普通" << endl;
				break;
			}

			UI::SetTexColor(Noraml);
		}
		cout << endl;
		cout << "                                     L--返回\n";

		in = _getch();
		if (in >= '1'&&in <= '2')
		{
			int getno = in - 48;
			if (c->Gold >= (int)((DealUsingList[getno - 1].Gold*1.5f)))  //判断金币是否足够
			{
				c->Gold -= (int)(DealUsingList[getno - 1].Gold*1.5f);
				c->GetItem(new Using(DealUsingList[getno - 1]));

			}
		}
		else if (in == 'l')
		{
			return;
		}
	}
}
