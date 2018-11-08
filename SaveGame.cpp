#include "stdafx.h"
#include "SaveGame.h"
#include <fstream>
#include <string>
#include "Controller.h"
#include "Character.h"
#include "Manager.h"
#include "Item.h"
#include "Using.h"
#include"Equipment.h"
using namespace std;
#define SAVEPATH "C:/Users/Administrator/Desktop/SaveFile.txt"

void SaveGame::Save(Controller * Ctrl)
{
	ofstream out;
	out.open(SAVEPATH);
	/*
	string Name;
	int Level;
	int Gold;
	float Exp;
	float MaxExp;
	float Hp;
	float MaxHp;
	float Mp;
	float MaxMp;
	int ATK;
	int DEF;

	//物品相关
	Item* Pack[3][10];
	Equipment* Equip[5];*/
	out << Ctrl->Name << endl;
	out << Ctrl->Level << endl;
	out << Ctrl->Gold << endl;
	out << Ctrl->Exp << endl;
	out << Ctrl->MaxExp << endl;
	out << Ctrl->Pawn->Hp << endl;
	out << Ctrl->Pawn->MaxHp << endl;
	out << Ctrl->Pawn->Mp << endl;
	out << Ctrl->Pawn->MaxMp << endl;
	out << Ctrl->Pawn->ATK << endl;
	out << Ctrl->Pawn->DEF << endl;

	for(int i=0;i<3;i++) //保存背包
	{
		for (int j = 0; j < 10; j++)
		{
			if (Ctrl->Pack[i][j]->Index == 0)
				break;

			if (typeid(*Ctrl->Pack[i][j])==typeid(Using))
			{
				out << "0" << Ctrl->Pack[i][j]->Index << "*" << Ctrl->Pack[i][j]->Nums<<endl;
			}
			else if (typeid(*Ctrl->Pack[i][j]) == typeid(Equipment))
			{
				out << "1" << Ctrl->Pack[i][j]->Index<<endl;
			}
		}
	} 
	out << "PACKEND" << endl;
	for (int i = 0; i < 5; i++)//保存装备
	{
		out << Ctrl->Equip[i]->Index<<endl;
	}
	out << "EQUIPEND" << endl;

	out.close();
}

void SaveGame::Read(Controller *&Ctrl)
{
	ifstream in;
	in.open(SAVEPATH);	
	in >> Ctrl->Name;
	in >> Ctrl->Level;
	in >> Ctrl->Gold;
	in >> Ctrl->Exp;
	in >> Ctrl->MaxExp;
	in >> Ctrl->Pawn->Hp;
	in >> Ctrl->Pawn->MaxHp;
	in >> Ctrl->Pawn->Mp;
	in >> Ctrl->Pawn->MaxMp;
	in >> Ctrl->Pawn->ATK;
	in >> Ctrl->Pawn->DEF;

	while (1) //读取背包
	{
		string Tempstring;
		char Itemtype;
		string Index;
		string nums;
		size_t i = 1;
		Item * ReadItem;
		in >> Tempstring;
		if (Tempstring == "PACKEND")
			break;
		Itemtype = Tempstring[0];

		for (i; i < Tempstring.size(); i++)
		{
			if (Tempstring[i] == '*')
				break;

			Index.push_back(Tempstring[i]);
		}

		switch (Itemtype)  
		{
		case '0':
			for (i; i < Tempstring.size(); i++)
			{

				if (Tempstring[i] == '*')
					continue;
				nums.push_back(Tempstring[i]);
			}
			ReadItem = Manager::SpawnItem(0, atoi(Index.c_str()));
			ReadItem->Nums = atoi(nums.c_str());
			Ctrl->GetItem(ReadItem);
			break;
		case '1':

			ReadItem = Manager::SpawnItem(1, atoi(Index.c_str()));
			Ctrl->GetItem(ReadItem);
			break;
		default:
			break;
		}
	

	}

	for (size_t i = 0; i < 5; i++)
	{
		int EquipIndex;
		in >> EquipIndex;
		if(EquipIndex==0)
			break;/*
		Ctrl->Equip[i] =(Equipment*) Manager::SpawnItem(1, EquipIndex);*/
		//新添
		Equipment* useitem = (Equipment*)Manager::SpawnItem(1, EquipIndex);
		//更改属性
		Ctrl->Pawn->ATK += (useitem->ATK);
		Ctrl->Pawn->DEF += (useitem->DEF) ;
		Ctrl->Pawn->MaxMp += (useitem->Hp) ;
		Ctrl->Pawn->MaxMp += (useitem->Mp) ;

		//
	}
	in.close();
	return; 

}
