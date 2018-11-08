#include "stdafx.h"
#include "Item.h"
#include "UI.h"

Item::Item(int index, string name, int gold, int nums, ItemQuality quality)
{
	this->Name = name;
	this->Index = index;
	this->Gold = gold;
	this->Nums = nums;
	this->Itemquality = quality;
}

Item* Item::Use()
{
	return this;
}

