#pragma once
#include<iostream>
#include<conio.h>
#include<ctime>
#include<Windows.h>
using namespace std;
class Monster;
class Controller;
class Item;
static class UI
{
public:
	void static PrintLogin(Controller*& Ctrl);
	void static PrintMainUI(Controller* Ctrl);
	void static PrintAbout();
	void static Load();
	void static PrintOperate();
	void static PrintPack(Controller* Ctrl);
	void static PrintItem(Item* item, Controller* controller);   //�����������
	void static PrintState(Controller* Ctrl);// ���״̬��
	void static PrintMonsterState(Monster* ai);
	void static PrintItemName(Item* item);
	void static PrintMonsterName(Monster* ai);
	int static PrintFightOper(Controller* Ctrl);// ս��ʱ�������� �����˺�ֵ

	void static PrintMessage(const string& Msg);//������Ϣ��
	void static PrintMessage(Item* item);
	void static SetTexColor(PrintColor color);
	void static PrintMap(class Character* character,class Map* map);
};
