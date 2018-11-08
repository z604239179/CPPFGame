#pragma once
#include"Item.h"

class Controller;

class Using :public Item
{
public:
	Using(int index, const string& name, int gold, int nums,ItemQuality quality,int Hp, int Mp);
	Using(const Using& copy);
	Item* Use();
	int AddHp;
	int AddMp;
};

const Using UsingList[] = {
	//���  ����  �ۼ�   ���� Ʒ�� �ָ�hp  �ָ�mp 
	Using(0,"��",0,0,ItemQuality::Norml,0,0),
	Using(1,"΢������ҩ��",5,1,ItemQuality::Norml,10,0),
	Using(2,"΢��ħ��ҩ��",5,1,ItemQuality::Norml,0,10),
	Using(3,"С������ҩ��",10,1,ItemQuality::Unusual,50,0),
	Using(4,"С��ħ��ҩ��",10,1,ItemQuality::Unusual,0,50),
	Using(5,"��������ҩ��",30,1,ItemQuality::Epic,100,0),
	Using(6,"����ħ��ҩ��",30,1,ItemQuality::Epic,0,100),
	Using(7,"��������ҩ��",50,1,ItemQuality::Legend,200,0),
	Using(8,"����ħ��ҩ��",50,1,ItemQuality::Epic,0,200)
};
