#pragma once
#include "MyDirectX.h"
struct node
{
	int x, y;
};

class PLAYER
{
private:
	void Person_Action();
	void AI_Action();
	node AI_Compute();
	int Winner();
public:
	bool AI;
	bool First;
	void Action();
};

extern PLAYER player1;
extern PLAYER player2;
extern int points[16][16];		//����
extern node chessrecord[250];  //��˳���¼���ӵ�λ��
extern int chesstop;		//����ջ��ָ��
extern bool turn;
extern bool gameover;
void Play_Chess();			//������
bool Back();				//����
