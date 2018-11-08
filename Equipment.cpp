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
	TempEquip=Owner->Equip[this->Type];			//取下自身装备
	Owner->Equip[this->Type] = this;			//装备此件装备
	TempEquip->PackElem = this->PackElem;		//设置取下装备的背包位置
	TempEquip->PackPage = this->PackPage;		//设置去下装备的背包页数
	Owner->Pack[TempEquip->PackPage][TempEquip->PackElem] = TempEquip;  //设置玩家背包

	return this;

}

