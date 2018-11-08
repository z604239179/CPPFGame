#include "stdafx.h"
#include "Equipment.h"
#include"Controller.h"


Equipment::Equipment(int index, string name, int gold, int nums,ItemQuality quality,int type, int atk, int def, int hp, int mp) :Item(index,name,gold,nums, quality)

{
	Type = type;
	ATK = atk;
	DEF = def;
	Hp = hp;
	Mp = mp;
	Nums = 1;
}

Equipment::Equipment(const Equipment & copy):Item(copy.Index,copy.Name,copy.Gold,copy.Nums,copy.Itemquality)
{
	Type = copy.Type;
	ATK = copy.ATK;
	DEF = copy.DEF;
	Hp = copy.Hp;
	Mp = copy.Mp;
	Nums = 1;
}


Item* Equipment::Use()
{

	Equipment* TempEquip;
	TempEquip=Owner->Equip[this->Type];			//ȡ������װ��
	Owner->Equip[this->Type] = this;			//װ���˼�װ��
	TempEquip->PackElem = this->PackElem;		//����ȡ��װ���ı���λ��
	TempEquip->PackPage = this->PackPage;		//����ȥ��װ���ı���ҳ��
	Owner->Pack[TempEquip->PackPage][TempEquip->PackElem] = TempEquip;  //������ұ���

	return this;

}

