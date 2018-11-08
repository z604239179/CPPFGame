#pragma once
#include<string>
using namespace std;
class Controller; //Ç°ÖÃÉùÃ÷ ControllerÀà

class Item
{
public:
	Controller *Owner;
	int Index;
	int PackPage;
	int PackElem;
	string Name;
	int Gold;
	int Nums;
	ItemQuality Itemquality;
	Item(int index, string name, int gold, int nums, ItemQuality quality);
	virtual ~Item() {}
	virtual Item* Use();

};
