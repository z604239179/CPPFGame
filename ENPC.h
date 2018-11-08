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
	//���  ����  �ۼ�  ����  Ʒ��  ����  ������  ������ ����ֵ  ħ��ֵ
									   //0.ͷ�� 1.���� 2.���� 3.���� 4.Ь��
	Equipment(1,"ͷ��",1,1,ItemQuality::Norml,0,0,1,0,0),
	Equipment(2,"����",1,1,ItemQuality::Norml,1,0,1,0,0),
	Equipment(3,"��֦",1,1,ItemQuality::Norml,2,1,1,0,0),
	Equipment(4,"���߲�",1,1,ItemQuality::Norml,3,0,1,0,0),
	Equipment(5,"��Ь",1,1,ItemQuality::Norml,4,0,1,0,0)
};
