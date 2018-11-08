#pragma once
#include <string>
#include "Monster.h"
using namespace std;
class Controller;
enum NPCType{NULLNPC,UsingNPC,EquipNpc};
class NPC:public Monster
{
public:
	NPCType type;

	NPC(int index,const string& name, int level, NPCType mtype);
	NPC(const NPC& CopyNpc);
	virtual void Talk(Controller *c);
	virtual void Deal(Controller *c) {};
	void CheckMisson();
	bool operator==(const NPC& npc);
};





const NPC NPCList[2][4] =
{
	{
		NPC(0,"商人",10,NPCType::UsingNPC),NPC(1,"村长",5,NPCType::NULLNPC),NPC(2,"铁匠",10,NPCType::EquipNpc), NPC(3,"牧童",1,NPCType::NULLNPC)
	},
	{
		 NPC(4,"士兵",25,EquipNpc),NPC(5,"商人",10,NPCType::UsingNPC),NPC(6,"铁匠",10,NPCType::EquipNpc), NPC(7,"镇长",5,NPCType::NULLNPC)
	}
};

