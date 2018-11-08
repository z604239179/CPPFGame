#include "stdafx.h"
#include "Map.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include<stdlib.h>
#include"Manager.h"
#include"Character.h"
using namespace std;
Map::Map(int MapId)
{
	int NpcConut=0;
	//��ʼ����ͼ��Ϣ
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			BaseMap[i][j].X = i;
			BaseMap[i][j].Y = j;
			BaseMap[i][j].nType = NodeType(MapList[MapId][i][j]);
			switch (BaseMap[i][j].nType)
			{
			case Space:
				//ˢ�¹���
				BaseMap[i][j].LocalMonster.assign(3, nullptr);
				for (int k = 0; k <= 2; k++)
				{
					BaseMap[i][j].LocalMonster[k] = Manager::SpawnMonster(); //����Monsterָ��
				}
				break;

			case People:

				BaseMap[i][j].LocalMonster.assign(1, nullptr);
				BaseMap[i][j].LocalMonster[0] = (Monster*)Manager::ReturnNPC(MapId, NpcConut);
				NpcConut++;
				break;

			default:
				break;
			}
			/* 
		 ��������

			////����ÿ�������Ϣ
			//BaseMap[i][j].X = i;
			//BaseMap[i][j].Y = j;
			//if (i == 0 || j == 0 || i == 19 || j == 19)
			//	BaseMap[i][j].nType = Wall;
			//else
			//{
			//	//��������м䲿�ֵ�ò
			//	int r = (rand() % 15);
			//	if (r == 1)
			//	{
			//		BaseMap[i][j].nType = Tree;
			//	}
			//	else if(BaseMap[i][j].nType==NodeType::People)  //ˢ��NPC
			//	{
			//		BaseMap[i][j].LocalMonster[0] = (Monster*)Manager::ReturnNPC(MapId,NpcConut);
			//		
			//	}
			//	else
			//	{
			//		BaseMap[i][j].nType = Space;

			//		//ˢ�¹���
			//		BaseMap[i][j].LocalMonster.assign(3, nullptr);
			//		for (int k=0;k<=2;k++)
			//		{
			//			BaseMap[i][j].LocalMonster[k] = Manager::SpawnMonster(); //����Monsterָ��
			//		}
			//	}

			//}
			*/
		}
	}
	OnNode = BaseMap[5][5];  //��ʼ��������ڵ�λ��
}

