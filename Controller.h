#pragma once

#include<vector>
#include<string>
class Map;
class Misson;
class Item;
class Character;
class Equipment;
class Using;
using namespace std;
class Controller
{
public:
	Character* Pawn;

	//�����Ϣ
	string Name;
	int Level;
	int Gold;
	float Exp;
	float MaxExp;
	Misson* CurrenMisson;

	Controller(string InName);
	~Controller();


	void LevelUp(); //����

	//��Ʒ���
	Item* Pack[3][10];
	Equipment* Equip[5];
	bool GetItem(Item* ItemPoint);
	void DropItem(int PackPage, int PackElem);
	void UseItem(int PackPage,int PackElem);
	void SellItem(int Index);
	//װ�����
	//�������
	void input(char input);  //������������
	void input(char input,Map*& map);  //�����ƶ�

};