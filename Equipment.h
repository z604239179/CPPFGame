#pragma once
#include"Item.h"
class Equipment:public Item
{
public:
	Equipment(int index, string name, int gold, int nums,ItemQuality quality,int type,int atk,int def, int hp ,int mp);
	Equipment(const Equipment& copy);
	int Type;// 0.Í·¿ø 1.ÉÏÒÂ 2.ÎäÆ÷ 3.ÏÂ×° 4.Ğ¬×Ó
	int ATK;
	int DEF;
	int Hp;
	int Mp;
	Item* Use();
};
ostream& operator<<(ostream& out, const Equipment euiqp); //Êä³öÃû×Ö

//³õÊ¼»¯×°±¸ÁĞ±í
const  Equipment EquipList[] = {  //±àºÅ  Ãû³Æ  ÊÛ¼Û  ÊıÁ¿  Æ·ÖÊ  ÀàĞÍ  ¹¥»÷Á¦  ·ÀÓùÁ¦ ÉúÃüÖµ  Ä§·¨Öµ
									   //0.Í·²¿ 1.ÉÏÒÂ 2.ÎäÆ÷ 3.¿ã×Ó 4.Ğ¬×Ó
	Equipment(0,"¿Õ",0,0,ItemQuality::Norml,0,0,0,0,0),
	Equipment(1,"Í·½í",1,1,ItemQuality::Norml,0,0,1,0,0),
	Equipment(2,"²¼ÒÂ",1,1,ItemQuality::Norml,1,0,1,0,0),
	Equipment(3,"Ê÷Ö¦",1,1,ItemQuality::Norml,2,1,1,0,0),
	Equipment(4,"ÕÚĞß²¼",1,1,ItemQuality::Norml,3,0,1,0,0),
	Equipment(5,"²¼Ğ¬",1,1,ItemQuality::Norml,4,0,1,0,0),
	Equipment(6,"Ä¾¿ø",5,1,ItemQuality::Norml,0,0,2,0,0),
	Equipment(7,"Ä¾¼×",5,1,ItemQuality::Norml,1,0,2,0,0),
	Equipment(8,"Ä¾½£",5,1,ItemQuality::Norml,2,3,0,0,0),
	Equipment(9,"Ä¾ÍÈ",5,1,ItemQuality::Norml,3,0,2,0,0),
	Equipment(10,"Ä¾åì",5,1,ItemQuality::Norml,4,0,2,0,0),
	Equipment(11,"Í­¿ø",20,1,ItemQuality::Unusual,0,0,4,5,0),
	Equipment(12,"Í­¼×",20,1,ItemQuality::Unusual,1,0,4,5,0),
	Equipment(13,"Í­½£",20,1,ItemQuality::Unusual,2,5,0,0,0),
	Equipment(14,"Í­ÍÈ",20,1,ItemQuality::Unusual,3,0,4,5,0),
	Equipment(15,"Í­Ñ¥",20,1,ItemQuality::Unusual,4,0,4,5,0),
	Equipment(16,"Ìú¿ø",50,1,ItemQuality::Unusual,0,0,10,10,0),
	Equipment(17,"Ìú¼×",50,1,ItemQuality::Unusual,1,0,10,10,0),
	Equipment(18,"Ìú¿ø",50,1,ItemQuality::Unusual,2,0,12,10,0),
	Equipment(19,"Ìú¿ø",50,1,ItemQuality::Unusual,3,0,10,10,0),
	Equipment(20,"Ìú¿ø",50,1,ItemQuality::Unusual,4,0,10,10,0),
	Equipment(21,"¸Ö¿ø",100,1,ItemQuality::Unusual,0,3,15,10,0),
	Equipment(22,"¸Ö¼×",100,1,ItemQuality::Unusual,1,3,15,10,0),
	Equipment(23,"¸Ö½£",100,1,ItemQuality::Unusual,2,23,0,0,0),
	Equipment(24,"¸ÖÍÈ",100,1,ItemQuality::Unusual,3,3,15,10,0),
	Equipment(25,"¸ÖÑ¥",100,1,ItemQuality::Unusual,4,3,15,10,0)

};



