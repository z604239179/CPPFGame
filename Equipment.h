#pragma once
#include"Item.h"
class Equipment:public Item
{
public:
	Equipment(int index, string name, int gold, int nums,ItemQuality quality,int type,int atk,int def, int hp ,int mp);
	Equipment(const Equipment& copy);
	int Type;// 0.ͷ�� 1.���� 2.���� 3.��װ 4.Ь��
	int ATK;
	int DEF;
	int Hp;
	int Mp;
	Item* Use();
};
ostream& operator<<(ostream& out, const Equipment euiqp); //�������

//��ʼ��װ���б�
const  Equipment EquipList[] = {  //���  ����  �ۼ�  ����  Ʒ��  ����  ������  ������ ����ֵ  ħ��ֵ
									   //0.ͷ�� 1.���� 2.���� 3.���� 4.Ь��
	Equipment(0,"��",0,0,ItemQuality::Norml,0,0,0,0,0),
	Equipment(1,"ͷ��",1,1,ItemQuality::Norml,0,0,1,0,0),
	Equipment(2,"����",1,1,ItemQuality::Norml,1,0,1,0,0),
	Equipment(3,"��֦",1,1,ItemQuality::Norml,2,1,1,0,0),
	Equipment(4,"���߲�",1,1,ItemQuality::Norml,3,0,1,0,0),
	Equipment(5,"��Ь",1,1,ItemQuality::Norml,4,0,1,0,0),
	Equipment(6,"ľ��",5,1,ItemQuality::Norml,0,0,2,0,0),
	Equipment(7,"ľ��",5,1,ItemQuality::Norml,1,0,2,0,0),
	Equipment(8,"ľ��",5,1,ItemQuality::Norml,2,3,0,0,0),
	Equipment(9,"ľ��",5,1,ItemQuality::Norml,3,0,2,0,0),
	Equipment(10,"ľ��",5,1,ItemQuality::Norml,4,0,2,0,0),
	Equipment(11,"ͭ��",20,1,ItemQuality::Unusual,0,0,4,5,0),
	Equipment(12,"ͭ��",20,1,ItemQuality::Unusual,1,0,4,5,0),
	Equipment(13,"ͭ��",20,1,ItemQuality::Unusual,2,5,0,0,0),
	Equipment(14,"ͭ��",20,1,ItemQuality::Unusual,3,0,4,5,0),
	Equipment(15,"ͭѥ",20,1,ItemQuality::Unusual,4,0,4,5,0),
	Equipment(16,"����",50,1,ItemQuality::Unusual,0,0,10,10,0),
	Equipment(17,"����",50,1,ItemQuality::Unusual,1,0,10,10,0),
	Equipment(18,"����",50,1,ItemQuality::Unusual,2,0,12,10,0),
	Equipment(19,"����",50,1,ItemQuality::Unusual,3,0,10,10,0),
	Equipment(20,"����",50,1,ItemQuality::Unusual,4,0,10,10,0),
	Equipment(21,"�ֿ�",100,1,ItemQuality::Unusual,0,3,15,10,0),
	Equipment(22,"�ּ�",100,1,ItemQuality::Unusual,1,3,15,10,0),
	Equipment(23,"�ֽ�",100,1,ItemQuality::Unusual,2,23,0,0,0),
	Equipment(24,"����",100,1,ItemQuality::Unusual,3,3,15,10,0),
	Equipment(25,"��ѥ",100,1,ItemQuality::Unusual,4,3,15,10,0)

};



