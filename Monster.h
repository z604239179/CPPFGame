#pragma once
#include<string>
#include<vector>
#include<conio.h>
#include<iostream>
using namespace std;
enum ItemQuality;
class Item;
class Controller;


class Monster;


class Monster
{
public:
	Monster(int index, const string& name, int level, ItemQuality type);
	Monster(const Monster& copymonster);
	string Name;
	//������Ʒ
	int Index;
	int Level;
	int Exp;
	int Gold;
	//��������
	int ATK;
	int DEF;
	int Hp;
	int MaxHp;
	ItemQuality Monstertype;
	int isNpc; //0:���� 1��NPC

};

//��ʼ�������б�
const  Monster MonsterList[] = {
	Monster(0,"ʷ��ķ",1,ItemQuality::Norml),
	Monster(1,"�粼��",5,ItemQuality::Norml),
	Monster(2,"Ұ��",10,ItemQuality::Norml),
	Monster(3,"Ұ��",30,ItemQuality::Unusual),
	Monster(4,"����",50,ItemQuality::Unusual),
	Monster(5,"��ʬ",70,ItemQuality::Epic),
	Monster(6,"����",100,ItemQuality::Epic),
	Monster(7,"������",150,ItemQuality::Legend)

};

