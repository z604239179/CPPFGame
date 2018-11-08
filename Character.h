#pragma once


class Character
{
public:
	//基础属性
	float Hp;
	float MaxHp;
	float Mp;
	float MaxMp;
	//战斗属性
	int ATK;		//攻击力
	int DEF;		//防御力

	Character();//构造角色

	int Mapindex; //所处地图编号
	int X;		//地图中的X坐标
	int Y;		//地图中的Y坐标
};