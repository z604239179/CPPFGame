#pragma once
#include <string>
using namespace std;
enum MissonState {UnAccept,Accept,Compeleted};
enum MissonTpye{FindPeople,KillMonster,FindItem};
class Controller;
class NPC;
class Monster;
class Misson
{
public:
	int Index;
	string Name;
	string Produce;
	int RewardGold;
	float RewardExp;
	int RewardItemIndex;

	int FromNpc;
	int TargetNpc;
	MissonState State;
	MissonTpye Type;

	int TargetIndex;
	int TargetNums;
	int CurrentNums;

	Misson(int index,string name, string produce,int rewardgold,float rewardexp,int rewarditemindex,int fromnpcindex,int targetnpcindex,int targetIndex,int targetnums,MissonTpye type);
	virtual bool CheakMisson(NPC* npc,Controller* Ctrl);
};

const Misson MissonList[] =
{
	Misson(0,"诞生","去找村长谈谈话。",1,5.0f,0,0,0,0,0,FindPeople)
};