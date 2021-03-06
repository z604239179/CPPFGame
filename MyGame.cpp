// MyGame.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include"Map.h"
#include"UI.h"
#include"Controller.h"
#include<iostream>
#include<conio.h>
using namespace std;


int main()
{
	system("mode con cols=45 lines=30  "); //初始化控制台大小
	srand((unsigned)time(NULL));//随机种子
	//初始化游戏
	UI::Load();// 显示加载界面
	Map* m=new Map(0);
	Controller* myctrl=new Controller("a");
	char input;  //操作值
	/////////////////
	UI::PrintLogin(myctrl);  //输出登陆界面
	

	UI::Load();// 显示加载界面
	////////////////运行游戏
	while (true)
	{
		UI::PrintMainUI(myctrl);  //输出主UI
		//m.PrintMap(myctrl.Pawn);  // 输出地图
		UI::PrintMap(myctrl->Pawn, m);
		UI::PrintOperate();  //输出操作界面


		input = _getch();			//输入操作
		if(input=='w'|| input == 'a'|| input == 's'|| input == 'd' || input=='1' || input=='2' || input=='3')   //判读移动、战斗还是其他操作
			myctrl->input(input,m);
		else myctrl->input(input);



		system("cls");
	}
	return 0;
}

