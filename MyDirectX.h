// Beginning Game Programming
// MyDirectX.h

#pragma once

//header files
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <xinput.h>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <io.h>
#include <algorithm>
using namespace std;

//libraries
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")

//program values
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern bool gameover;

//macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

//Direct3D objects
extern LPDIRECT3D9 d3d; 
extern LPDIRECT3DDEVICE9 d3ddev; 
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteobj;

//sprite class
class SPRITE
{
public:
	bool alive;
	float x,y;
	int frame, columns;
	int width, height;
	float scaling,scaleW,scaleH, rotation;
	int startframe, endframe;
	int starttime, delay;
	int direction;
	float velx, vely;
	bool move;
	D3DCOLOR color;

	SPRITE() 
	{
		frame = 0;
		columns = 1;
		width = height = 0;
		scaling = scaleW = scaleH = 1.0f;
		rotation = 0.0f;
		startframe = endframe = 0;
		direction = 1;
		starttime = 0;
		delay = 30;
		velx = vely = 0.0f;
		color = D3DCOLOR_XRGB(255,255,255);
		move = 0;
	}
	void SetValues(float x, float y, int width, int height, int frame = 0, int columns = 1,
		int startframe = 0, int endframe = 0, int delay = 30, int starttime = 0, int direction = 1, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
	void Animate();
	void Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y);
	//bounding box collision detection
	int Collision(SPRITE sprite2);
	//distance based collision detection
	bool CollisionD(SPRITE sprite2, int distance = 0);
};
class MouseManager;

//Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0,0,0));
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, 
    int framenum, int framew, int frameh, int columns);
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
	int frame = 0, int columns = 1, float rotation = 0.0f,
	float scaleW = 1.0f, float scaleH = 1.0f, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
void ClearScene(D3DCOLOR color = D3DCOLOR_XRGB(0, 0, 100));

//DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
extern MouseManager Mouse;
extern XINPUT_GAMEPAD controllers[4];

//DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
bool Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
void XInput_Vibrate(int contNum = 0, int amount = 65535);
bool XInput_Controller_Found();

//game functions
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();

//font functions
LPD3DXFONT MakeFont(string name, int size);
void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color = D3DCOLOR_XRGB(255,255,255));

//mouse class
class MouseManager
{
private:
	HWND window;		//本程序窗口
public:
	MouseManager();
	void Init(HWND hWnd);
	int X; // 鼠标在屏幕上的X坐标  
	int Y; // 鼠标在屏幕上的Y坐标  
	bool LButton; // 鼠标当前左键是否按下  
	bool RButton; // 鼠标当前右键是否按下  
//	int Status; // 鼠标状态 1:普通，2:攻击，3:施法，4:其他 
	void Update();
};


