#include "stdafx.h"
#include "Manager.h"
#include"Controller.h"
#include"Monster.h"
#include"UI.h"
#include "Map.h"
#include "Controller.h"
#include "Character.h"
#include "Using.h"
#include "Equipment.h"
#include "NPC.h"
#include "ENPC.h"
#include "UNPC.h"
#include "Misson.h"



#include <fstream>
#include"string"



Monster* Manager::SpawnMonster()
{
	int GetNo=rand() % Manager::GetMonsterListSize();
	return new Monster(MonsterList[GetNo]);

	// TODO: �ڴ˴����� return ���
}


Item * Manager::SpawnItem(int Type, int Index)
{
	if(Index==0)
		return nullptr;
	else
	{
		switch (Type)
		{
		case 0:
			return new Using(UsingList[Index]); //��������Ʒ
			break;
		case 1:
			return new Equipment(EquipList[Index]); //����װ��
			break;
		default:
			return nullptr;
			break;
		}
	}
}


bool Manager::Fight(Controller * Ctrl, Monster * ai)
{
	system("cls");
	int GetHurt;
	while (true)
	{
		UI::PrintMainUI(Ctrl);  //��ʾ������
		UI::PrintMonsterState(ai); //��ʾAI��״̬
		GetHurt=UI::PrintFightOper(Ctrl);
		if (GetHurt == -1)
			return false;  //ս��ʧ��


		///////////////////////////////////////////////////////
		//��������


		/*cout << "1.��ͨ����  2.���ܹ���  3.ʹ����Ʒ  4.����\n";
		//input = _getch();
		//switch (input)
		//{
		//case '1':
		//{
		//	GetHurt = Ctrl->Pawn->ATK;
		//	break;
		//}
		//case '2':
		//{
		//	GetHurt = Ctrl->Pawn->ATK*1.5;
		//	break;
		//	//ʹ�ü���
		//}
		//case '3':
		//{
		//	UI::PrintPack(*Ctrl);
		//	break;
		//}
		//case '4':
		//{
		//	return false;
		//	break;
		//}
		//default:
		//	GetHurt = 0;
		//	break;
		}*/

		///////////////////////////////////////////////////////
		//���� ս��ʧ�ܣ�
		 //ս�� �����Ѫ
		if (GetHurt != 0)
		{
			if (GetHurt - ai->DEF > 0)
			{
				ai->Hp -= GetHurt - ai->DEF;
				cout << "���" << ai->Name << "�����" << GetHurt - ai->DEF << "���˺�!" << endl;
			}
			else cout << "���" << ai->Name << "�����" << "0���˺�" << endl;
			if (ai->ATK - Ctrl->Pawn->DEF > 0)
			{
				Ctrl->Pawn->Hp -= ai->ATK - Ctrl->Pawn->DEF;
				cout << ai->Name << "����" << "�����" << ai->ATK - Ctrl->Pawn->DEF << "���˺�!" << endl;
			}
			else cout << ai->Name << "����" << "�����" << "0���˺�" << endl;

			Sleep(1000);
			if (ai->Hp <= 0)
			{
				string msg = "ս��ʤ����\n";

				Item* TempItemptr = MonsterDrop(ai->Level);
				Ctrl->GetItem(TempItemptr);
				UI::PrintMessage(TempItemptr);
				//


				Ctrl->Gold += ai->Gold;
				Ctrl->Exp += ai->Exp;
				return true;   //ս��ʤ��

			}
			if (Ctrl->Pawn->Hp <= 0)
			{
				UI::PrintMessage("ս��ʧ�ܣ�");
				Ctrl->Pawn->Hp = Ctrl->Pawn->MaxHp*0.5f;
				Ctrl->Pawn->Mp = Ctrl->Pawn->MaxMp*0.5f;
				return false;  //ս��ʧ��
			}
		}
		system("cls");

	}
}

int Manager::GetEquipListSize()
{
	return sizeof(EquipList) / sizeof(EquipList[0]);
}

int Manager::GetMonsterListSize()
{

	return sizeof(MonsterList) / sizeof(MonsterList[0]);
}

int Manager::GetUseingListSize()
{

	return sizeof(UsingList) / sizeof(UsingList[0]);
}
Item* Manager::MonsterDrop(int MonsterLevel)
{

	int droptype = rand() % 2;
	if (droptype == 0)	//��������Ʒ 
	{

		switch (MonsterLevel)
		{
		case 1:

			return Manager::SpawnItem(0, rand() % 2 + 1); //��������Ʒ
			break;
		case 5:
			return Manager::SpawnItem(0, rand() % 2 + 1); //��������Ʒ
			break;
		case 10:
			return Manager::SpawnItem(0, rand() % 2 + 3); //��������Ʒ
			break;
		case 30:
			return Manager::SpawnItem(0, rand() % 2 + 3); //��������Ʒ
			break;
		case 50:
			return Manager::SpawnItem(0, rand() % 2 + 5); //��������Ʒ
			break;
		case 70:
			return Manager::SpawnItem(0, rand() % 2 + 5); //��������Ʒ
			break;
		case 100:
			return Manager::SpawnItem(0, rand() % 2 + 7); //��������Ʒ
			break;
		case 150:
			return Manager::SpawnItem(0, rand() % 2 + 7); //��������Ʒ
			break;

		default:
			return nullptr;//��������Ʒ
		}

	}
	else if (droptype == 1)//����װ��
	{
		switch (MonsterLevel)
		{
		case 1:
			return Manager::SpawnItem(1, rand() % 5 + 1);//����װ��
			break;
		case 5:
			return Manager::SpawnItem(1, rand() % 5 + 1);//����װ��
			break;
		case 10:
			return Manager::SpawnItem(1, rand() % 5 + 6);//����װ��
			break;
		case 30:
			return Manager::SpawnItem(1, rand() % 5 + 6);//����װ��
			break;
		case 50:
			return Manager::SpawnItem(1, rand() % 5 + 11);//����װ��
			break;
		case 70:
			return Manager::SpawnItem(1, rand() % 5 + 11);//����װ��
			break;
		case 100:
			return Manager::SpawnItem(1, rand() % 5 + 16);//����װ��
			break;
		case 150:
			return Manager::SpawnItem(1, rand() % 5 + 21);//����װ��
			break;
		default:
			return Manager::SpawnItem(1, 0);//����װ��
		}
	}

	return nullptr;
}

class NPC * Manager::ReturnNPC(int MapIndex, int NpcCount)
{
	switch (NPCList[MapIndex][NpcCount].type)
	{
	case NPCType::NULLNPC:
		return new NPC(NPCList[MapIndex][NpcCount]);
	case NPCType::EquipNpc:
		return new ENPC(NPCList[MapIndex][NpcCount]);

	case NPCType::UsingNPC:
		return new UNPC(NPCList[MapIndex][NpcCount]);
	default:
		break;
	}
	return nullptr;

}
//
//void Manager::InitGame()
//{
//
//}

//void Manager::InitItemData()
//{
//	string Tempstring;
//	ifstream in;
//	char TempRead;
//	in.open("ItemData.csv");
//	in >> Tempstring;
//	while (1)
//	{
//		int i = 0;
//		int ReadQuality;
//		Item* ReadItem = new Item(1, "", 0, 0, ItemQuality(0));
//		in >> ReadItem->Index >> TempRead >> ReadItem->Name >> TempRead >> ReadItem->Gold >> TempRead >> ReadItem->Nums >> TempRead >> ReadQuality;
//		ReadItem->Itemquality = ItemQuality(ReadQuality);
//		ItemDataList.push_back(ReadItem);
//		if (in.eof())
//			break;
//	}
//	in.close();
//
//}
//
