#pragma once
#include"stdafx.h"
#include"Misson.h"
#include "Controller.h"
#include "NPC.h"
Misson::Misson(int index, string name, string produce, int rewardgold, float rewardexp, int rewarditemindex,int fromnpcindex, int targetnpcindex,  int targetIndex, int targetnums,MissonTpye type)
	:Index(index), Name(name), Produce(produce),RewardGold(rewardgold),RewardExp(rewardexp),RewardItemIndex(rewarditemindex),FromNpc(fromnpcindex),
	TargetNpc(targetnpcindex),  TargetIndex(targetnums), TargetNums(targetnums),Type(type)
{
	CurrentNums = 0;
}

bool Misson::CheakMisson(NPC* npc, Controller* Ctrl)
{
	if (Ctrl->CurrenMisson->TargetNpc == npc->Index)
	{
		if (Ctrl->CurrenMisson->TargetNums <= Ctrl->CurrenMisson->CurrentNums)
			return true;
	}
	return false;
}
