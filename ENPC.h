#pragma once
#include "NPC.h"
#include "Equipment.h"
class Controller;
class ENPC:public NPC
{
public:
	ENPC(int index, const string& name, int level, NPCType mtype);
	ENPC(const NPC& copynpc);
	void Talk(Controller *c) override;
	void Deal(Controller* c) override;
};
const Equipment DealEquipmentList[] =
{
	//编号  名称  售价  数量  品质  类型  攻击力  防御力 生命值  魔法值
									   //0.头部 1.上衣 2.武器 3.裤子 4.鞋子
	Equipment(1,"头巾",1,1,ItemQuality::Norml,0,0,1,0,0),
	Equipment(2,"布衣",1,1,ItemQuality::Norml,1,0,1,0,0),
	Equipment(3,"树枝",1,1,ItemQuality::Norml,2,1,1,0,0),
	Equipment(4,"遮羞布",1,1,ItemQuality::Norml,3,0,1,0,0),
	Equipment(5,"布鞋",1,1,ItemQuality::Norml,4,0,1,0,0)
};
