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
	//编号  名称  售价   数量 品质 恢复hp  恢复mp 
	Using(0,"空",0,0,ItemQuality::Norml,0,0),
	Using(1,"微型生命药剂",5,1,ItemQuality::Norml,10,0),
	Using(2,"微型魔法药剂",5,1,ItemQuality::Norml,0,10),
	Using(3,"小型生命药剂",10,1,ItemQuality::Unusual,50,0),
	Using(4,"小型魔法药剂",10,1,ItemQuality::Unusual,0,50),
	Using(5,"中型生命药剂",30,1,ItemQuality::Epic,100,0),
	Using(6,"中型魔法药剂",30,1,ItemQuality::Epic,0,100),
	Using(7,"大型生命药剂",50,1,ItemQuality::Legend,200,0),
	Using(8,"大型魔法药剂",50,1,ItemQuality::Epic,0,200)
};
