#include "MyDirectX.h"
#include "Wuziqi.h"

using std::string;
const string APPTITLE = "五子棋";
const int SCREENW = 1024;				//应用窗口大小
const int SCREENH = 1024;
int ScrollX = 30;
int ScrollY = 30;
int TileWidth = 69;
int TileHeight = 68;
int Win = 0;
LPD3DXFONT font;

LPDIRECT3DSURFACE9 ChessBoard = NULL;
LPDIRECT3DTEXTURE9 WhiteChess = NULL;
LPDIRECT3DTEXTURE9 BlackChess = NULL;
LPDIRECT3DTEXTURE9 MouseImg = NULL;
D3DXIMAGE_INFO Boardimginfo;


void GameWorld_Init()
{
	HRESULT result;
	LPDIRECT3DSURFACE9 BackPicture;
	D3DXGetImageInfoFromFile("棋盘.bmp", &Boardimginfo);
	BackPicture = LoadSurface("棋盘.bmp");
	result = d3ddev->CreateOffscreenPlainSurface(Boardimginfo.Width, Boardimginfo.Height, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &ChessBoard, NULL);
	if (result != D3D_OK)
	{
		MessageBox(NULL, "没找到棋盘", "error", 0);
		return;
	}
	RECT r1 = { 0, 0, Boardimginfo.Width, Boardimginfo.Height };
	d3ddev->StretchRect(BackPicture, &r1, ChessBoard, NULL, D3DTEXF_NONE);
	BackPicture->Release();
	WhiteChess = LoadTexture("白棋.png");
	BlackChess = LoadTexture("黑棋.png");
	MouseImg = LoadTexture("鼠标.png");
	font = MakeFont("Arial", 50);
	if (MessageBox(NULL, "确定玩家vs电脑，取消玩家vs玩家", "选择战斗方式", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		if (MessageBox(NULL, "确定先手，取消后手", "选择先后手", MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			player1.First = true;
			player2.AI = true;
			turn = 1;
		}
		else
		{
			player2.First = true;
			player2.AI = true;
			turn = 0;
		}
	}
	else
	{
		player1.First = true;
		turn = 1;
	}
	if (MessageBox(NULL, "是否打开声音", "声音设置", MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		mciSendString("open \"背景音乐.mp3\" alias mymusic", NULL, 0, NULL);
		mciSendString("play mymusic repeat", NULL, 0, NULL);
	}
	ShowCursor(false);
}
bool Game_Init(HWND window)
{
	if (!Direct3D_Init(window, SCREENW, SCREENH, false)) return false;		//初始化游戏窗口
	if (!DirectInput_Init(window)) return true;						//初始化游戏输入设备
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	RECT window_rect;
	GetWindowRect(window, &window_rect);
	MoveWindow(window, window_rect.left, window_rect.top - 50, window_rect.right - window_rect.left, window_rect.bottom - window_rect.top, TRUE);

	GameWorld_Init();
	return true;
}

void Game_Draw()
{
	ClearScene(); //clear the scene
	//start rendering
	if (d3ddev->BeginScene())
	{
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		//画背景
		RECT r1 = { 0, 0, Boardimginfo.Width, Boardimginfo.Height };
		d3ddev->StretchRect(ChessBoard, &r1, backbuffer, NULL, D3DTEXF_NONE);
		//画棋盘
		for (int i = 1; i <= 15;i++)
		for (int j = 1; j <= 15;j++)
		if (points[i][j] ==2)
		{
			Sprite_Transform_Draw(WhiteChess, ScrollX + (i - 1) * TileWidth - 30, ScrollY + (j - 1) *TileHeight - 30, 200, 200, 0, 1, 0, 0.3, 0.3);
		}
		else if (points[i][j] == 1)
		{
			Sprite_Transform_Draw(BlackChess, ScrollX + (i - 1) * TileWidth - 25, ScrollY + (j - 1) *TileHeight - 25, 143, 136, 0, 1, 0,0.4 , 0.4);
		}
		//画鼠标
		Sprite_Transform_Draw(MouseImg, Mouse.X, Mouse.Y, 30, 66);
		//画字
		if (Win > 0)
		{
			char result[50];
			if (Win == 1) strcpy(result,"黑子获胜");
			if (Win == 2) strcpy(result,"白子获胜");
			FontPrint(font, 200, 200, result, D3DCOLOR_XRGB(255, 0, 0));
		}

		spriteobj->End();
		//stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

}

void Game_Run(HWND window)
{
	if (!d3ddev) return;
	DirectInput_Update();

	Play_Chess();
	static DWORD starttime = 0;
	if (GetTickCount() - starttime >= 30)
	{
		starttime = GetTickCount();
		Game_Draw();
	}
	if (KEY_DOWN(VK_ESCAPE)) gameover = true;
}

void Game_End()
{
	Game_Draw();
	Game_Draw();
	Game_Draw();
	Sleep(2000);
	DirectInput_Shutdown();
	if(ChessBoard) ChessBoard->Release();
	if(WhiteChess) WhiteChess->Release();
	if(BlackChess) BlackChess->Release();
	
}