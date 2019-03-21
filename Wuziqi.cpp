#include "Wuziqi.h"
PLAYER player1;
PLAYER player2;
int points[16][16];		//棋盘
node chessrecord[250] = { 0 };  //按顺序记录棋子的位置
int chesstop = 0;		//上面栈的指针
bool turn;
bool Invalid(const node &t)
{
	bool re =!( t.x >= 1 && t.x <= 15 && t.y >= 1 && t.y <= 15);
	return re;
}

void PLAYER::Action()
{
	if (AI) AI_Action();
	else Person_Action();
}
void PLAYER::Person_Action()
{
	if (Mouse.LButton)
	{
		extern int ScrollX, ScrollY, TileWidth, TileHeight;
		node place;
		place.x = (Mouse.X ) / TileWidth+ 1;
		place.y = (Mouse.Y ) / TileHeight + 1;
		if (Invalid(place) || points[place.x][place.y]) return;
		if (First) points[place.x][place.y] = 1;
		else points[place.x][place.y] = 2;
		turn = !turn;
		chessrecord[chesstop].x = place.x;	//压栈
		chessrecord[chesstop].y = place.y;
		chesstop++;
		if (Winner()) gameover = true;
	}
}

void PLAYER::AI_Action()
{
	node place = AI_Compute();
	if (First) points[place.x][place.y] = 1;
	else points[place.x][place.y] = 2;
	turn = !turn;
	chessrecord[chesstop].x = place.x;	//压栈
	chessrecord[chesstop].y = place.y;
	chesstop++;
	if (Winner()) gameover = true;
}

int PLAYER::Winner()
{
	int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dy[8] = { 0, -1, -1, -1, 0, 1, 1, 1 };
	
	for (int i = 1; i <= 15; i++)
	for (int j = 1; j <= 15; j++)
	{
		int chess = points[i][j];
		if (!chess) continue;
		int link[8] = { 0 ,0,0,0,0,0,0,0};
		for (int k = 0; k < 8; k++)
		{
			int x = i, y = j;
			for (int depth = 0; depth < 4; depth++)
			{
				x += dx[k];
				y += dy[k];
				node t = { x, y };
				if (!Invalid(t) && (chess == points[x][y])) link[k] ++;
				else break;
				
			}
		}
		for (int k = 0; k < 4; k++)
		{
			if (link[k] + link[k + 4] + 1 >= 5)
			{
				extern int Win;
				if (chess == 1)
					Win = 1;
				else if (chess == 2)
					Win = 2;
				return chess;
			}
		}
	}
	return 0;
}
bool Back()
{
	if (chesstop <= 1) return false;
	static DWORD starttime = 0;
	if (GetTickCount() - starttime > 100)
	{
		starttime = GetTickCount();
		if (Mouse.RButton)
		{
			if (!player1.AI && !player2.AI)
			{
				chesstop--;
				points[chessrecord[chesstop].x][chessrecord[chesstop].y] = 0;
				chessrecord[chesstop].x = 0;
				chessrecord[chesstop].y = 0;
				turn = !turn;
				return true;
			}
			else
			{
				chesstop--;
				points[chessrecord[chesstop].x][chessrecord[chesstop].y] = 0;
				chessrecord[chesstop].x = 0;
				chessrecord[chesstop].y = 0;
				chesstop--;
				points[chessrecord[chesstop].x][chessrecord[chesstop].y] = 0;
				chessrecord[chesstop].x = 0;
				chessrecord[chesstop].y = 0;
				return true;
			}
		}
	}
	return false;
}
void Play_Chess()
{
	if (turn)
		player1.Action();
	else player2.Action();
	Back();
}
node PLAYER::AI_Compute()
{
	struct mode
	{
		bool is_blocked;
		int link;
		int block_position;
		mode() :is_blocked(false), link(1), block_position(-1){}
		void reset()
		{
			is_blocked = false;                   //该方向是否被堵上
			block_position = -1;				//若被堵上，堵上它的子距离他的位置
			link = 1;
		}
	};

	int ownColor, oppositeColor;
	if (First)
	{
		ownColor = 1;
		oppositeColor = 2;
	}
	else
	{
		ownColor = 2;
		oppositeColor = 1;
	}

	int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	int dy[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };

	mode ourlink[16][16][8];
	mode oppositelink[16][16][8];

	double attack[16][16];
	double defend[16][16];

	double max_attack = 0, max_defend = 0;
	int x1 = 8, y1 = 8, x2 = 8, y2 = 8;

	//进入i，j大循环
	for (int i = 1; i <= 15; i++)
		for (int j = 1; j <= 15; j++)
		{
			// Figure()
			attack[i][j] = 0;
			defend[i][j] = 0;
			if (points[i][j])continue;
			for (int dk = 0; dk < 8; dk++)
			{
				ourlink[i][j][dk].link = 1;
				ourlink[i][j][dk].is_blocked = false;
				oppositelink[i][j][dk].link = 1;
				oppositelink[i][j][dk].is_blocked = false;
				//把统计link和统计block分开
				//link
				node t = { i, j };
				for (int m = 1; m<5; m++)
				{
					t.x += dx[dk]; t.y += dy[dk];
					if (Invalid(t) || points[t.x][t.y] != ownColor)
						break;
					ourlink[i][j][dk].link++;
				}
				//blcok
				t = { i, j };
				for (int m = 1; m < 5; m++)
				{
					t.x += dx[dk]; t.y += dy[dk];
					if (Invalid(t) || points[t.x][t.y] == oppositeColor)
					{
						ourlink[i][j][dk].is_blocked = true;
						ourlink[i][j][dk].block_position = m;
						break;
					}
				}
					
				//link
				t = { i, j };
				for (int m = 1; m<5; m++)
				{
					t.x += dx[dk]; t.y += dy[dk];
					if (Invalid(t) || points[t.x][t.y] != oppositeColor)
						break;
					oppositelink[i][j][dk].link++;
				}
				//blcok
				t = { i, j };
				for (int m = 1; m < 5; m++)
				{
					t.x += dx[dk]; t.y += dy[dk];
					if (Invalid(t) || points[t.x][t.y] == ownColor)
					{
						oppositelink[i][j][dk].is_blocked = true;
						oppositelink[i][j][dk].block_position = m;
						break;
					}
				}

			}

			//Compute()
			for (int dk = 0; dk < 4; dk++)
			{
				int priority ;
				int t = ourlink[i][j][dk].link + ourlink[i][j][dk + 4].link - 1;
/*				if (ourlink[i][j][dk].is_blocked&&ourlink[i][j][dk + 4].is_blocked)
				{
					if (ourlink[i][j][dk].block_position + ourlink[i][j][dk + 4].block_position - 1 < 5)
					{
						priority = 1; continue;
					}
				}
*/				if (t == 1 && (ourlink[i][j][dk].is_blocked || ourlink[i][j][dk + 4].is_blocked))
					priority = 2;
				if (t == 1 && (!ourlink[i][j][dk].is_blocked) && (!ourlink[i][j][dk + 4].is_blocked))
					priority = 3;

				if (t == 2 && (ourlink[i][j][dk].is_blocked || ourlink[i][j][dk + 4].is_blocked))
					priority = 4;
				if (t == 2 && (!ourlink[i][j][dk].is_blocked) && (!ourlink[i][j][dk + 4].is_blocked))
					priority = 5;

				if (t == 3 && (ourlink[i][j][dk].is_blocked || ourlink[i][j][dk + 4].is_blocked))
					priority = 6;
				if (t == 3 && (!ourlink[i][j][dk].is_blocked) && (!ourlink[i][j][dk + 4].is_blocked))
					priority = 7;

				if (t == 4 && (ourlink[i][j][dk].is_blocked || ourlink[i][j][dk + 4].is_blocked))
					priority = 8;
				if (t == 4 && (!ourlink[i][j][dk].is_blocked) && (!ourlink[i][j][dk + 4].is_blocked))
					priority = 9;

				if (t == 5)priority = 10;
				switch (priority)
				{
				case 1:
					attack[i][j] += 0;
					break;

				case 2:
					attack[i][j] += 5;
					break;

				case 3:
					attack[i][j] += 10;
					break;

				case 4:
					attack[i][j] += 500;
					break;

				case 5:
					attack[i][j] += 1000;
					break;

				case 6:
					attack[i][j] += 50000;
					break;

				case 7:
					attack[i][j] += 100000;
					break;

				case 8:
					attack[i][j] += 5000000;
					break;

				case 9:
					attack[i][j] += 10000000;
					break;

				case 10:
					attack[i][j] += 1000000000;
					break;

				}
			}
			for (int dk = 0; dk < 4;dk++)
			{
				int priority ;
				int t = oppositelink[i][j][dk].link + oppositelink[i][j][dk + 4].link - 1;
				if (oppositelink[i][j][dk].is_blocked&&oppositelink[i][j][dk + 4].is_blocked)
				{
					if (oppositelink[i][j][dk].block_position + oppositelink[i][j][dk + 4].block_position - 1 < 5)
						priority = 1;
				}
				if (t == 1 && (oppositelink[i][j][dk].is_blocked || oppositelink[i][j][dk + 4].is_blocked))
					priority = 2;
				if (t == 1 && (!oppositelink[i][j][dk].is_blocked) && (!oppositelink[i][j][dk + 4].is_blocked))
					priority = 3;

				if (t == 2 && (oppositelink[i][j][dk].is_blocked || oppositelink[i][j][dk + 4].is_blocked))
					priority = 4;
				if (t == 2 && (!oppositelink[i][j][dk].is_blocked) && (!oppositelink[i][j][dk + 4].is_blocked))
					priority = 5;

				if ((t == 3 && (oppositelink[i][j][dk].is_blocked || oppositelink[i][j][dk + 4].is_blocked)))
					priority = 6;
				if (t == 3 && (!oppositelink[i][j][dk].is_blocked) && (!oppositelink[i][j][dk + 4].is_blocked))
					priority = 7;

				if (t == 4 && (oppositelink[i][j][dk].is_blocked || oppositelink[i][j][dk + 4].is_blocked))
					priority = 8;
				if ((t == 4 && (!oppositelink[i][j][dk].is_blocked) && (!oppositelink[i][j][dk + 4].is_blocked)))
					priority = 9;

				if (t == 5)priority = 10;
				switch (priority)
				{
				case 1:
					defend[i][j] += 0;
					break;

				case 2:
					defend[i][j] += 5;
					break;

				case 3:
					defend[i][j] += 10;
					break;

				case 4:
					defend[i][j] += 500;
					break;

				case 5:
					defend[i][j] += 1000;
					break;

				case 6:
					defend[i][j] += 50000;
					break;

				case 7:
					defend[i][j] += 100000;
					break;

				case 8:
					defend[i][j] += 5000000;
					break;

				case 9:
					defend[i][j] += 10000000;
					break;

				case 10:
					defend[i][j] += 1000000000;
					break;
				}
			}
			//Search()
			if (attack[i][j]>max_attack)
			{
				max_attack = attack[i][j];
				x1 = i;
				y1 = j;
			}
			if (defend[i][j]>max_defend)
			{
				max_defend = defend[i][j];
				x2 = i;
				y2 = j;
			}
		}
	//跳出i，j大循环

	//最后的抉择
	if (max_attack >= max_defend)
	{
		node re = { x1, y1 };
		
		return re;
	}
	else
	{
		node re = { x2, y2 };
		return re;
	}
}