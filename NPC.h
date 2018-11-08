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
		NPC(0,"����",10,NPCType::UsingNPC),NPC(1,"�峤",5,NPCType::NULLNPC),NPC(2,"����",10,NPCType::EquipNpc), NPC(3,"��ͯ",1,NPCType::NULLNPC)
	},
	{
		 NPC(4,"ʿ��",25,EquipNpc),NPC(5,"����",10,NPCType::UsingNPC),NPC(6,"����",10,NPCType::EquipNpc), NPC(7,"��",5,NPCType::NULLNPC)
	}
};

