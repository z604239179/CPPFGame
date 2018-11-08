#include "stdafx.h"
#include "ENPC.h"
#include "Controller.h"
#include <iostream>
#include "UI.h"
using namespace std;

ENPC::ENPC(int index, const string& name, int level, NPCType mtype):NPC(index,name,level,NPCType::EquipNpc)
{

}

ENPC::ENPC(const NPC & copynpc):NPC(copynpc.Index, copynpc.Name,copynpc.Level,NPCType::EquipNpc)
{
}

void ENPC::Talk(Controller * c)
{
	while (1)
	{


		char input;
		system("cls");
		UI::PrintMainUI(c);
		cout << endl;
		cout << "年轻人，买些新装备吧。" << endl;
		cout << "                1.任务" << endl;
		cout << "                2.交易" << endl;
		cout << "                3.强化" << endl;
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

void ENPC::Deal(Controller * c)
{
	while (1)
	{
		char in;
		system("cls");
		UI::PrintMainUI(c);
		cout << endl;


		for (int i = 0; i < 5; i++)
		{
			cout << i + 1 << ". " << DealEquipmentList[i].Name << endl;
			cout << "价格： " << (int)DealEquipmentList[i].Gold*1.5f << "     品质:";
			switch (DealEquipmentList[i].Itemquality)
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
		if (in >= '1'&&in <= '5')
		{
			int getno = in - 48;
			if (c->Gold >= (int)(DealEquipmentList[getno - 1].Gold*1.5f))  //判断金币是否足够
			{
				c->Gold -= (int)(DealEquipmentList[getno - 1].Gold*1.5f);
				c->GetItem(new Equipment(DealEquipmentList[getno - 1]));

			}
		}
		else if (in == 'l')
		{
			return;
		}
	}
}