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
	//奖励物品
	int Index;
	int Level;
	int Exp;
	int Gold;
	//基础属性
	int ATK;
	int DEF;
	int Hp;
	int MaxHp;
	ItemQuality Monstertype;
	int isNpc; //0:怪物 1：NPC

};

//初始化怪物列表
const  Monster MonsterList[] = {
	Monster(0,"史莱姆",1,ItemQuality::Norml),
	Monster(1,"哥布林",5,ItemQuality::Norml),
	Monster(2,"野猪",10,ItemQuality::Norml),
	Monster(3,"野狼",30,ItemQuality::Unusual),
	Monster(4,"骷髅",50,ItemQuality::Unusual),
	Monster(5,"僵尸",70,ItemQuality::Epic),
	Monster(6,"屠夫",100,ItemQuality::Epic),
	Monster(7,"精灵龙",150,ItemQuality::Legend)

};

