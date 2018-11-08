#pragma once
#include "NPC.h"
#include "Using.h"
class Controller;
class UNPC:public NPC
{
public:
	UNPC(int index, const string& name, int level, NPCType mtype);
	UNPC(const NPC& copynpc);
	void Talk(Controller *c) override;
	void Deal(Controller* c) override;

};

const Using DealUsingList[] =
{
	//编号  名称  售价   数量 品质 恢复hp  恢复mp 
	Using(1,"微型生命药剂",5,1,ItemQuality::Norml,10,0),
	Using(2,"微型魔法药剂",5,1,ItemQuality::Norml,0,10)
};
