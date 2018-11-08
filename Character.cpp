#include "stdafx.h"
#include "Character.h"

Character::Character()
{
	//初始化角色属性
	MaxHp = 100.0f;
	Hp = MaxHp;
	MaxMp = 100.0f;
	Mp = MaxMp;
	ATK = 10;
	DEF = 0;
	Mapindex = 0;
	X = 5;
	Y = 5;
}
