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
	//���  ����  �ۼ�   ���� Ʒ�� �ָ�hp  �ָ�mp 
	Using(1,"΢������ҩ��",5,1,ItemQuality::Norml,10,0),
	Using(2,"΢��ħ��ҩ��",5,1,ItemQuality::Norml,0,10)
};
