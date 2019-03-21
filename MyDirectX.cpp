// Beginning Game Programming
// MyDirectX.cpp

#include "MyDirectX.h"
using namespace std;

//Direct3D variables
LPDIRECT3D9 d3d = NULL; 
LPDIRECT3DDEVICE9 d3ddev = NULL; 
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPD3DXSPRITE spriteobj = NULL;

//DirectInput variables
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
MouseManager Mouse;
char keys[256];
XINPUT_GAMEPAD controllers[4];


bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
    //initialize Direct3D
    d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!d3d) return false;

    //set Direct3D presentation parameters
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.hDeviceWindow = window;
    d3dpp.Windowed = (!fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.EnableAutoDepthStencil = 1;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
    d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.BackBufferWidth = width;
    d3dpp.BackBufferHeight = height;

    //create Direct3D device
    d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
    if (!d3ddev) return false;


    //get a pointer to the back buffer surface
    d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

    //create sprite object
    D3DXCreateSprite(d3ddev, &spriteobj);

    return 1;
}

void Direct3D_Shutdown()
{
    if (spriteobj) spriteobj->Release();

    if (d3ddev) d3ddev->Release();
    if (d3d) d3d->Release();
}

void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
    //get width/height from source surface
    D3DSURFACE_DESC desc;
    source->GetDesc(&desc);

    //create rects for drawing
    RECT source_rect = {0, 0, (long)desc.Width, (long)desc.Height };
    RECT dest_rect = { (long)x, (long)y, (long)x+desc.Width, (long)y+desc.Height};
    
    //draw the source surface onto the dest
    d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);

}

LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
    LPDIRECT3DSURFACE9 image = NULL;
    
    //get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK)
        return NULL;

    //create surface
    result = d3ddev->CreateOffscreenPlainSurface(
        info.Width,         //width of the surface
        info.Height,        //height of the surface
        D3DFMT_X8R8G8B8,    //surface format
        D3DPOOL_DEFAULT,    //memory pool to use
        &image,             //pointer to the surface
        NULL);              //reserved (always NULL)

    if (result != D3D_OK) return NULL;

    //load surface from file into newly created surface
    result = D3DXLoadSurfaceFromFile(
        image,                  //destination surface
        NULL,                   //destination palette
        NULL,                   //destination rectangle
        filename.c_str(),       //source filename
        NULL,                   //source rectangle
        D3DX_DEFAULT,           //controls how image is filtered
        D3DCOLOR_XRGB(0,0,0),   //for transparency (0 for none)
        NULL);                  //source image info (usually NULL)

    //make sure file was loaded okay
    if (result != D3D_OK) return NULL;

    return image;
}

LPDIRECT3DTEXTURE9 LoadTexture(std::string filename, D3DCOLOR transcolor)
{  
    LPDIRECT3DTEXTURE9 texture = NULL;

    //get width and height from bitmap file
    D3DXIMAGE_INFO info;
    HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
    if (result != D3D_OK) return NULL;

    //create the new texture by loading a bitmap image file
    D3DXCreateTextureFromFileEx( 
        d3ddev,                //Direct3D device object
        filename.c_str(),      //bitmap filename
        info.Width,            //bitmap image width
        info.Height,           //bitmap image height
        1,                     //mip-map levels (1 for no chain)
        D3DPOOL_DEFAULT,       //the type of surface (standard)
        D3DFMT_UNKNOWN,        //surface format (default)
        D3DPOOL_DEFAULT,       //memory class for the texture
        D3DX_DEFAULT,          //image filter
        D3DX_DEFAULT,          //mip filter
        transcolor,            //color key for transparency
        &info,                 //bitmap file info (from loaded file)
        NULL,                  //color palette
        &texture );            //destination texture

    //make sure the bitmap textre was loaded correctly
    if (result != D3D_OK) return NULL;

    return texture;
}


void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty, int framenum, int framew, int frameh, int columns)
{
    D3DXVECTOR3 position( (float)destx, (float)desty, 0 );
    D3DCOLOR white = D3DCOLOR_XRGB(255,255,255);

    RECT rect;
     rect.left = (framenum % columns) * framew;
    rect.top = (framenum / columns) * frameh;
    rect.right = rect.left + framew;
    rect.bottom = rect.top + frameh;

    spriteobj->Draw( texture, &rect, NULL, &position, white);
}
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
	int frame, int columns, float rotation, float scaleW, float scaleH, D3DCOLOR color)
{
	//create a scale vector
	D3DXVECTOR2 scale(scaleW, scaleH);

	//create a translate vector
	D3DXVECTOR2 trans((float)x, (float)y);

	//set center by dividing width and height by two
	D3DXVECTOR2 center((float)(width * scaleW) / 2, (float)(height * scaleH) / 2);

	//create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotation, &trans);

	//tell sprite object to use the transform
	spriteobj->SetTransform(&mat);

	//calculate frame location in source image
	int fx = (frame % columns) * width;
	int fy = (frame / columns) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height };

	//draw the sprite frame
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);


	//added in chapter 14
	D3DXMatrixIdentity(&mat);
	spriteobj->SetTransform(&mat);
}

void SPRITE::SetValues(float tx, float ty, int twidth, int theight, int tframe, int tcolumns,
	int tstartframe, int tendframe, int tdelay, int tstarttime, int tdirection, D3DCOLOR tcolor)
{
	x = tx; y = ty; width = twidth; height = theight; frame = tframe; columns = tcolumns;
	startframe = tstartframe; endframe = tendframe; delay = tdelay; starttime = tstarttime;
	direction = tdirection; color = tcolor;
}

void SPRITE::Animate()
{
    if ((int)GetTickCount() > starttime + delay)
    {
        starttime = GetTickCount();

        frame += direction;
        if (frame > endframe) frame = startframe;
        if (frame < startframe) frame = endframe;
    }    
}

//define Sprite

void SPRITE::Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y)
{
    //create a scale vector
    D3DXVECTOR2 scale( scaleW, scaleH );

    //create a translate vector
    D3DXVECTOR2 trans( (float)x, (float)y );

    //set center by dividing width and height by two
    D3DXVECTOR2 center( (float)( width * scaleW )/2, (float)( height * scaleH )/2);

    //create 2D transformation matrix
    D3DXMATRIX mat;
    D3DXMatrixTransformation2D( &mat, NULL, 0, &scale, &center, rotation, &trans );

    //tell sprite object to use the transform
    spriteobj->SetTransform( &mat );

    //calculate frame location in source image
    int fx = (frame % columns) * width;
    int fy = (frame / columns) * height;
    RECT srcRect = {fx, fy, fx + width, fy + height};

    //draw the sprite frame
    spriteobj->Draw( image, &srcRect, NULL, NULL, color );


    //added in chapter 14
    D3DXMatrixIdentity( &mat );
    spriteobj->SetTransform( &mat );
}

void DrawTile(LPDIRECT3DSURFACE9 source, int tilenum, int width, int height, int columns, LPDIRECT3DSURFACE9 dest, int destx, int desty)
{
	//create a RECT to describe the source image
	RECT r1;
	r1.left = (tilenum % columns) * width;
	r1.top = (tilenum / columns) * height;
	r1.right = r1.left + width;
	r1.bottom = r1.top + height;

	//set destination rect
	RECT r2 = { destx, desty, destx + width, desty + height };

	//draw the tile
	d3ddev->StretchRect(source, &r1, dest, &r2, D3DTEXF_NONE);

}

//clear the screen
void ClearScene(D3DCOLOR color)
{
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);
}

//bounding  box collision detection
int SPRITE::Collision(SPRITE sprite2)
{
	SPRITE& sprite1 = *this;
    RECT rect1;
    rect1.left = (long)sprite1.x;
    rect1.top = (long)sprite1.y;
    rect1.right = (long)(sprite1.x + sprite1.width * sprite1.scaling);
    rect1.bottom = (long)(sprite1.y + sprite1.height * sprite1.scaling);

    RECT rect2;
    rect2.left = (long)sprite2.x;
    rect2.top = (long)sprite2.y;
    rect2.right = (long)(sprite2.x + sprite2.width * sprite2.scaling);
    rect2.bottom = (long)(sprite2.y + sprite2.height * sprite2.scaling);

    RECT dest; //ignored
    return IntersectRect(&dest, &rect1, &rect2);
}


bool SPRITE::CollisionD(SPRITE sprite2, int distance)
{
	SPRITE& sprite1 = *this;
    double radius1, radius2;

    //calculate radius 1
    if (sprite1.width > sprite1.height)
        radius1 = (sprite1.width * sprite1.scaling) / 2.0;
    else
        radius1 = (sprite1.height * sprite1.scaling) / 2.0;

    //center point 1
    double x1 = sprite1.x + radius1;
    double y1 = sprite1.y + radius1;
    D3DXVECTOR2 vector1( (float)x1, (float)y1);

    //calculate radius 2
    if (sprite2.width > sprite2.height)
        radius2 = (sprite2.width * sprite2.scaling) / 2.0;
    else
        radius2 = (sprite2.height * sprite2.scaling) / 2.0;

    //center point 2
    double x2 = sprite2.x + radius2;
    double y2 = sprite2.y + radius2;
    D3DXVECTOR2 vector2( (float)x2, (float)y2);

    //calculate distance
    double deltax = vector1.x - vector2.x;
    double deltay = vector2.y - vector1.y;
    double dist = sqrt((deltax * deltax) + (deltay * deltay));

    //return distance comparison
	//add functions to judge whether the object within a distance
	return (dist < radius1 + radius2 + distance);
	
}

bool DirectInput_Init(HWND hwnd)
{
    //initialize DirectInput object
    DirectInput8Create(
        GetModuleHandle(NULL), 
        DIRECTINPUT_VERSION, 
        IID_IDirectInput8,
        (void**)&dinput,
        NULL);

    //initialize the keyboard
    dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
    dikeyboard->SetDataFormat(&c_dfDIKeyboard);
    dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    dikeyboard->Acquire();

    //initialize the mouse
	Mouse.Init(hwnd);

    return true;
}

void DirectInput_Update()
{
	//update Mouse
	Mouse.Update();
    
    //update keyboard
    dikeyboard->Poll();
    if (!SUCCEEDED(dikeyboard->GetDeviceState(256,(LPVOID)&keys)))
    {
        //keyboard device lost, try to re-acquire
        dikeyboard->Acquire();
    }

    //update controllers
    for (int i=0; i< 4; i++ )
    {
        ZeroMemory( &controllers[i], sizeof(XINPUT_STATE) );

        //get the state of the controller
        XINPUT_STATE state;
        DWORD result = XInputGetState( i, &state );

        //store state in global controllers array
        if (result == 0) controllers[i] = state.Gamepad;
    }
}


int Mouse_X()
{
    return mouse_state.lX;
}

int Mouse_Y()
{
    return mouse_state.lY;
}

int Mouse_Button(int button)
{
    return mouse_state.rgbButtons[button] & 0x80;
}

bool Key_Down(int key) 
{
    return (bool)(keys[key] & 0x80);
}


void DirectInput_Shutdown()
{
    if (dikeyboard) 
    {
        dikeyboard->Unacquire();
        dikeyboard->Release();
        dikeyboard = NULL;
    }
    if (dimouse) 
    {
        dimouse->Unacquire();
        dimouse->Release();
        dimouse = NULL;
    }
}

LPD3DXFONT MakeFont(string name, int size)
{
    LPD3DXFONT font = NULL;

    D3DXFONT_DESC desc = {
        size,                   //height
        0,                      //width
        0,                      //weight
        0,                      //miplevels
        false,                  //italic
        DEFAULT_CHARSET,        //charset
        OUT_TT_PRECIS,          //output precision
        CLIP_DEFAULT_PRECIS,    //quality
        DEFAULT_PITCH,          //pitch and family
        ""                      //font name
    };

    strcpy(desc.FaceName, name.c_str());

    D3DXCreateFontIndirect(d3ddev, &desc, &font);

    return font;
}

void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color)
{
    //figure out the text boundary
    RECT rect = { x, y, 0, 0 };
    font->DrawText( NULL, text.c_str(), text.length(), &rect, DT_CALCRECT, color); 

    //print the text
    font->DrawText(spriteobj, text.c_str(), text.length(), &rect, DT_LEFT, color); 
}

MouseManager::MouseManager()
{
	LButton = false;
	RButton = false;
	X = SCREENW / 2;
	Y = SCREENH / 2;
}
void MouseManager::Init(HWND hWnd)
{
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(false);
//	ShowCursor(false);
	window = hWnd;
	LButton = false;
	RButton = false;
	X = SCREENW / 2;
	Y = SCREENH / 2;
}
/*
void MouseManager::Update()
{
	//update mouse
	dimouse->Poll();
	if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state)))
	{
		//mouse device lose, try to re-acquire
		dimouse->Acquire();
	}

	// 设置X和Y  
	X += (mouse_state.lX);
	Y += (mouse_state.lY);

	if (X >= SCREENW)
		X = SCREENW - 1;
	else
	if (X < 0)
		X = 0;

	if (Y >= SCREENH)
		Y = SCREENH - 1;
	else
	if (Y < 0)
		Y = 0;

	// 设置按钮  
	if (mouse_state.rgbButtons[0])
		LButton = true;
	else
		LButton = false;

	if (mouse_state.rgbButtons[1])
		RButton = true;
	else
		RButton = false;
	RECT window_rect;
	GetWindowRect(window, &window_rect);
	window_rect.left += 5;
	window_rect.right -= 5;
	window_rect.top += 23;
	ClipCursor(&window_rect);
}*/

void MouseManager::Update()
{
	POINT point;
	GetCursorPos(&point);				//获取鼠标指针位置（屏幕坐标）
	ScreenToClient(window, &point);		//将鼠标指针位置转换为窗口坐标
	X = point.x;
	Y = point.y;
	//update mouse
	dimouse->Poll();
	if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse_state)))
	{
	//mouse device lose, try to re-acquire
	dimouse->Acquire();
	}
	// 设置按钮  
	if (mouse_state.rgbButtons[0])
		LButton = true;
	else
		LButton = false;

	if (mouse_state.rgbButtons[1])
		RButton = true;
	else
		RButton = false;
}