#include "stdafx.h"
#include "Using.h"
#include "Character.h"
#include"Controller.h"

Using::Using(int index, const string& name, int gold, int nums,ItemQuality quality,int hp,int mp):Item(index,name,gold,nums, quality)
{
	AddHp = hp;
	AddMp = mp;
}

Using::Using(const Using & copy):Item(copy.Index,copy.Name,copy.Gold,copy.Nums, copy.Itemquality)
{
	AddHp = copy.AddHp;
	AddMp = copy.AddMp;

}

Item * Using::Use()
{
	if (Nums >= 1)
	{
		Owner->Pawn->Hp += this->AddHp;
		if (Owner->Pawn->Hp > Owner->Pawn->MaxHp)
			Owner->Pawn->Hp = Owner->Pawn->MaxHp;
		Owner->Pawn->Mp += this->AddMp;
		if (Owner->Pawn->Mp > Owner->Pawn->MaxMp)
			Owner->Pawn->Mp = Owner->Pawn->MaxMp;
	}
	//////////////使用物品

	this->Nums--;
	if (Nums <= 0)
	{
		Owner->Pack[PackPage][PackElem] = new Item(0, "空", 0, 0, ItemQuality::Norml);
	}

	return this;
}
