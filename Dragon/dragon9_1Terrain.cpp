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

CInputSystem input(false);
CRenderSystem render;

CCamera camera;
CLight light;
RENDERCONTEXT rc;

CTerrain terrain;
CZBuffer zb;

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
	
	errorlog.CreateErrorFile();

	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);
	
	CPoint4D camPos(0,500,-200);
	CVector4 camDir(0,0,0);
	
	CVector4 vscale(30.0,30.0,30.0);
	CVector4 vpos(0,0,100);
	CVector4 vrot(0,0,0);

	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,10,12000,90,SCREEN_WIDTH,SCREEN_HEIGHT);
	
	CPoint4D terrPos(0,0,0);
	terrain.GenerateTerrain(TERRAIN_WIDTH,TERRAIN_HEIGHT,TERRAIN_SCALE,terrPos,"..//res//checkerheight01.bmp",
		"..//res//checker256256.bmp",_RGB16BIT565(255,255,255),
		TRI4DV1_ATTR_RGB16|TRI4DV1_ATTR_SHADE_MODE_CONSTANT|TRI4DV1_ATTR_SHADE_MODE_TEXTURE);

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
		,white,white,white,lightPos,lightDir,0,0,0.001,0,0,0,1);

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
	
	input.UpateDevice();

	render.DDrawFillSurface(render.lpddsback,0);

	//render.FillRectangle(0,0, SCREEN_WIDTH,SCREEN_HEIGHT*.38, _RGB16BIT565(75,75,255), render.lpddsback);

	//render.FillRectangle(0,SCREEN_HEIGHT*.38, SCREEN_WIDTH, SCREEN_HEIGHT, _RGB16BIT565(39,90,252), render.lpddsback);


	render.ResetRenderList4DV1();
	render.CreateRotateMatrix4x4(0,angleY,0,mrot);

	if (KEYDOWN(VK_RIGHT))
	{
		camera.dir.y+=4;
	}
	if (KEYDOWN(VK_LEFT))
	{
		camera.dir.y-=4;
	}
	
	//render.TransOBJModel4DV1(terrId,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);
	
	render.LoadTerrain(terrain);

	render.ModelToWorldRenderList4DV1(CVector4(0,0,0),TRANSFORM_LOCAL_TO_TRANS);
	
	camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);

	//render.CullOBJ4DV1(terrId,camera,CULLED_XYZ_PLANES);

	render.RemoveBackfaceRenderList4DV1(camera);
	//render.LightOBJ4DV1World16(gauroudSphereId,camera,light);

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
	//
	rc.attr=RENDER_ATTR_INVZBUFFER  
		|RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE
		| RENDER_ATTR_BILERP;
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
	
	if (KEYDOWN(VK_ESCAPE)||input.KeyDown(DIK_ESCAPE))
	{
		PostMessage(hWnd,WM_DESTROY,0,0);
	}
	return TRUE;

}
