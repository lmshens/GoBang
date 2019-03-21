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
extern int points[16][16];		//棋盘
extern node chessrecord[250];  //按顺序记录棋子的位置
extern int chesstop;		//上面栈的指针
extern bool turn;
extern bool gameover;
void Play_Chess();			//主函数
bool Back();				//悔棋
