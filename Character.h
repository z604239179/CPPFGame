#pragma once


class Character
{
public:
	//��������
	float Hp;
	float MaxHp;
	float Mp;
	float MaxMp;
	//ս������
	int ATK;		//������
	int DEF;		//������

	Character();//�����ɫ

	int Mapindex; //������ͼ���
	int X;		//��ͼ�е�X����
	int Y;		//��ͼ�е�Y����
};