#include "stdafx.h"
#include "Monster.h"
#include"Controller.h"
#include"UI.h"
#include"Manager.h"
#include<ctime>
#include<cstdlib>
#include <iostream>
using namespace std;
Monster::Monster(int index ,const string& name, int level, ItemQuality type)
{
	this->Index = index;
	this->Name = name;
	this->Level = level;
	Exp = level * 100;
	Gold = level * 10;
	ATK = level * 1;
	DEF = level * 1;
	MaxHp = Level * 5;
	Hp = MaxHp;
	Monstertype = type;
	isNpc = 0;
}

Monster::Monster(const Monster& copymonster)
{
	this->Index = copymonster.Index;
	this->Name = copymonster.Name;
	this->Level = copymonster.Level;
	Exp = copymonster.Level * 100;
	Gold = copymonster.Level * 10;
	ATK = copymonster.Level * 1;
	DEF = copymonster.Level * 1;
	MaxHp = copymonster.Level * 5;
	Hp = MaxHp;
	Monstertype = copymonster.Monstertype;
	isNpc = 0;
	
}

