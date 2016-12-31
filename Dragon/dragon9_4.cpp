#include "dragon.h"
#include "resource.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4305)

extern HWND hWnd;
extern HINSTANCE hInstance;

#define WINDOW_APP		  0
#define TERRAIN_WIDTH     4000
#define TERRAIN_HEIGHT    4000
#define TERRAIN_SCALE     700

CInputSystem* input8;
CRenderSystem render;
RENDERCONTEXT rc;

CCOBModel object;
CMD2Container md2Container;

CCamera camera;
CLight light;

int id;
CZBuffer zb;

CBitmapFile bmpfile;
CBitmapImage bmpImage;

float camSpeed=0.0f;
#define CAM_DECEL            (.25)  // deceleration/friction
#define MAX_SPEED             20



CErrorLog errorlog;

DWORD startClockCount;
int windowMode;
int windowClose;
char buffer[256];

int WINAPI WinMain (HINSTANCE hinstance, 
					HINSTANCE hPrevInstance,
					PSTR szCmdLine, 
					int iCmdShow)
{
	HWND        hwnd ;
	MSG         msg ;
	WNDCLASSEX  wndclass ;

	wndclass.cbSize        = sizeof (wndclass) ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hinstance ;
	wndclass.hIcon         = LoadIconA (NULL, MAKEINTRESOURCEA(IDI_ICON)) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (BLACK_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = ClassName ;
	wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

	RegisterClassEx (&wndclass) ;

	hwnd = CreateWindowA(ClassName, "SCOTT",
		WINDOW_APP?(WS_OVERLAPPED):(WS_POPUP|WS_VISIBLE),
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hinstance, NULL) ;
	hWnd=hwnd;
	hInstance=hinstance;

	if(WINDOW_APP)
	{
		RECT windowRect={0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
		AdjustWindowRectEx(&windowRect,GetWindowStyle(hWnd),
			GetMenu(hWnd)!=NULL,GetWindowExStyle(hWnd));

		MoveWindow(hWnd,200,100,
			windowRect.right-windowRect.left,windowRect.bottom-windowRect.top,TRUE);

	}

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	GameInit();

	while (1) 
	{ 
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
				break;
			TranslateMessage (&msg) ;
			DispatchMessage (&msg) ; 
		}
		else
		{
			GameMain();
		}
	}

	GameShutdown();
	UnregisterClassA(ClassName,hinstance);
	return msg.wParam ;
} 

LRESULT APIENTRY WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		{
			if (LOWORD(wParam)==WA_INACTIVE)
			{
				//bRunGame=FALSE;
			}
			else
			{
				//bRunGame=TRUE;
			}
		}
		return 0;
	case WM_PAINT:
		{
			HDC hdc;
			PAINTSTRUCT ps;
			hdc=::BeginPaint(hWnd,&ps);
			::EndPaint(hWnd,&ps);
			return 0;
		}
	case WM_KEYUP:
		if(wParam==VK_ESCAPE)
		{
			::PostQuitMessage(0);
		}
		return 0;
	case WM_DESTROY:
		{
			::PostQuitMessage(0);
			windowClose=1;
		}
		return 0;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

int GameInit(void* parms)
{
	srand(render.StartClock());

	CreateMathTable();
	
	CreateAlphaTable(NUM_ALPHA_LEVELS,AlphaTable);
	
	input8=new CInputSystem(FALSE);

	errorlog.CreateErrorFile();

	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);

	CPoint4D camPos(0,500,-400);
	CVector4 camDir(0,0,0);

	CVector4 vscale(10,10,10);
	CVector4 vpos(0,0,0);
	CVector4 vrot(0,0,0);

	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,10,12000,120,SCREEN_WIDTH,SCREEN_HEIGHT);

	md2Container.LoadMD2Object("..//res//tris.md2",vscale,vpos,vrot,"..//res//blade_black.bmp",
		TRI4DV1_ATTR_RGB16 | TRI4DV1_ATTR_SHADE_MODE_FLAT | TRI4DV1_ATTR_SHADE_MODE_TEXTURE,
		_RGB16BIT565(255,255,255),
		VERTEX_FLAGS_SWAP_YZ);
	object.PrepareOBJForMD2(&md2Container);
	md2Container.SetAnimation(MD2_ANIM_STATE_STANDING_IDLE);

	RGBAV1 white, gray, black, red, green, blue;

	white.rgba = _RGBA32BIT(255,255,255,0);
	gray.rgba  = _RGBA32BIT(100,100,100,0);
	black.rgba = _RGBA32BIT(0,0,0,0);
	red.rgba   = _RGBA32BIT(255,0,0,0);
	green.rgba = _RGBA32BIT(0,255,0,0);
	blue.rgba  = _RGBA32BIT(0,0,255,0);

	//创建方向光
	CVector4 lightDir(0,-1,1);
	CPoint4D lightPos(0,200,0);
	light.CreateLightV1(LIGHTV1_STATE_ON,LIGHTV1_ATTR_INFINITE|LIGHTV1_ATTR_AMBIENT|
		LIGHTV1_ATTR_POINT|LIGHTV1_ATTR_COMPLEX_SPOTLIGHT|LIGHTV1_ATTR_DIFFUSE
		,white,gray,white,lightPos,lightDir,0,0,0.001,0,0,0,1);

	render.CreateRenderList();

	zb.CreateZBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,ZBUFFER_ATTR_32BIT);

	return true;
}
int GameShutdown(void* parms)
{
	errorlog.CloseFile();

	return true;
}
int GameMain(void* parms)
{
	static CMatrix4x4 mrot;
	static float angleY=2;

	render.StartClock();
	input8->UpateDevice();
	render.DDrawFillSurface(render.lpddsback,0);

	render.ResetRenderList4DV1();

	if (KEYDOWN(VK_RIGHT))
	{
		camera.dir.y+=4;
	}
	if (KEYDOWN(VK_LEFT))
	{
		camera.dir.y-=4;
	}
	if (KEYDOWN(VK_UP))
	{
		if((camSpeed+=2)>MAX_SPEED)
		{
			camSpeed=MAX_SPEED;
		}
	}
	if (KEYDOWN(VK_DOWN))
	{
		if ((camSpeed-=2)<-MAX_SPEED)
		{
			camSpeed=-MAX_SPEED;
		}
	}
	
	if (input8->KeyDown(DIK_1))
	{
		if (++md2Container.animState>=MD2_NUM_ANIMATIONS)
		{
			md2Container.animState=0;
		}
		md2Container.SetAnimation(md2Container.animState,MD2_ANIM_SINGLE_SHOT);
		render.WaitClock(100);
	}

	if (camSpeed>CAM_DECEL)
	{
		camSpeed-=CAM_DECEL;
	}
	else if (camSpeed<-CAM_DECEL)
	{
		camSpeed+=CAM_DECEL;
	}
	else
	{
		camSpeed=0;
	}
	camera.pos.x+=camSpeed*FastSin(camera.dir.y);
	camera.pos.z+=camSpeed*FastCos(camera.dir.y);

	md2Container.DoAnimation();
	object.ExtractMD2Frame(&md2Container);

	object.worldPos.x=0;
	object.worldPos.y=300;
	object.worldPos.z=200;
	
	render.ResetRenderList4DV1();
	render.ResetOBJ4DV1(&object);

	mrot.Identity();
	if ((angleY+=2)>=360)
	{
		angleY=0;
	}
	render.CreateRotateMatrix4x4(0,angleY,0,mrot);
	render.TransOBJModel4DV1(&object,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,FALSE);
	render.ModelToWorldOBJ4DV1(&object,TRANSFORM_TRANS_ONLY,FALSE);
	
	render.LoadRenderListFromModel(&object,FALSE);
	camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);
	
	render.RemoveBackfaceRenderList4DV1(camera);

	render.WorldToCamRenderList4DV1(camera);

	render.ClipTriangleRenderList4DV1(camera,CLIP_XYZ_PLANE);

	render.TransLight4DV1(light,camera.mcam,TRANSFORM_LOCAL_TO_TRANS);

	render.LightRenderList4DV1World16(camera,light);

	render.SortRenderList4DV1(SORT_RENDERLIST_AVGZ);

	render.CameraToPersRenderList4DV1(camera);

	render.PersToScreenRenderList4DV1(camera);

	render.DDrawLockBackSurface();

	zb.ClearZBuffer(0);

	// | RENDER_ATTR_ALPHA  
	// | RENDER_ATTR_MIPMAP  
	// | RENDER_ATTR_BILERP
	rc.attr=RENDER_ATTR_INVZBUFFER  |RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE;
	rc.videoBuffer=render.backBuffer;
	rc.lpitch=render.backlpitch;
	rc.mipDist=4500;
	rc.zbuffer=zb.zbuffer;
	rc.zpitch=SCREEN_WIDTH*4;
	rc.ptrRenderList=render.ptrRenderList;
	rc.alphaOverride=-1;
	render.DrawRenderListRENDERCONTEXT(&rc);

	render.DDrawUnLockBackSurface();

	render.FlipSurface();

	render.WaitClock(30);

	if (KEYDOWN(VK_ESCAPE)||input8->KeyDown(DIK_ESCAPE))
	{
		PostMessage(hWnd,WM_DESTROY,0,0);
	}
	return TRUE;

}
