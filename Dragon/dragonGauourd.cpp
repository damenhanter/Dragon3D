#include "dragon.h"
#include "resource.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4305)
extern HWND hWnd;
extern HINSTANCE hInstance;

#define WINDOW_APP   0

CInputSystem input(false);
CRenderSystem render;

CCamera camera;
CLight light;
int cubeId;


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
	
	CreateTable();
	
	errorlog.CreateErrorFile();

	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);
	
	//CPoint4D camPos(0,0,-100);
	//CVector4 camDir(0,0,0);
	//
	//CVector4 vscale(5.0,5.0,5.0);
	//CVector4 vpos(0,0,100);
	//CVector4 vrot(0,0,0);

	//camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,50,500,90,SCREEN_WIDTH,SCREEN_HEIGHT);
	//
	//render.LoadPLGModel("cube2.plg",cubeId,vscale,vpos,vrot);
	//
	//RGBAV1 white, gray, black, red, green, blue;

	//white.rgba = _RGBA32BIT(255,255,255,0);
	//gray.rgba  = _RGBA32BIT(100,100,100,0);
	//black.rgba = _RGBA32BIT(0,0,0,0);
	//red.rgba   = _RGBA32BIT(255,0,0,0);
	//green.rgba = _RGBA32BIT(0,255,0,0);
	//blue.rgba  = _RGBA32BIT(0,0,255,0);

	////创建方向光
	//CVector4 lightDir(-1,0,1);
	//CPoint4D lightPos(0,200,100);
	//light.CreateLightV1(LIGHTV1_STATE_ON,LIGHTV1_ATTR_POINT|LIGHTV1_ATTR_AMBIENT|LIGHTV1_ATTR_DIFFUSE
	//	,gray,gray,gray,lightPos,lightDir,0,0.001,0,0,0,0,5);
	
	

	render.CreateRenderList();

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
	
	TriF4DV1 face;
	face.tvlist[0].x=RAND_RANGE(0,SCREEN_WIDTH-2);
	face.tvlist[0].y=RAND_RANGE(0,SCREEN_HEIGHT-2);
	face.litColor[0]=_RGB16BIT565(RAND_RANGE(0,255),RAND_RANGE(0,255),RAND_RANGE(0,255));
	face.tvlist[1].x=RAND_RANGE(0,SCREEN_WIDTH-2);
	face.tvlist[1].y=RAND_RANGE(0,SCREEN_HEIGHT-2);
	face.litColor[1]=_RGB16BIT565(RAND_RANGE(0,255),RAND_RANGE(0,255),RAND_RANGE(0,255));
	face.tvlist[2].x=RAND_RANGE(0,SCREEN_WIDTH-2);
	face.tvlist[2].y=RAND_RANGE(0,SCREEN_HEIGHT-2);
	face.litColor[2]=_RGB16BIT565(RAND_RANGE(0,255),RAND_RANGE(0,255),RAND_RANGE(0,255));
	
	CTriangle2D triangle;
	render.DDrawLockBackSurface();
	triangle.DrawGouraudTriangle16(&face,render.backBuffer,render.backlpitch);
	render.DDrawUnLockBackSurface();
		
	//render.ResetRenderList4DV1();

	//	
	//render.ModelToWorldOBJ4DV1(cubeId,TRANSFORM_LOCAL_TO_TRANS);
	//
	//camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);
	//
	//render.CullOBJ4DV1(cubeId,camera,CULLED_XYZ_PLANES);

	//render.RemoveBackfaceOBJ4DV1(cubeId,camera);
	//
	////render.LightOBJ4DV1World16(cubeId,camera,light);

	//render.LoadRenderListFromModel(cubeId,FALSE,FALSE);

	//render.LightRenderList4DV1World16(camera,light);
	//
	//render.SortRenderList4DV1(SORT_RENDERLIST_AVGZ);

	//render.WorldToCamRenderList4DV1(camera);
	//
	//render.CameraToPersRenderList4DV1(camera);

	//render.PersToScreenRenderList4DV1(camera);

	//render.DDrawLockBackSurface();

	//render.DrawRenderList4DV1Solid16();

	//render.DDrawUnLockBackSurface();

	render.FlipSurface();

	render.WaitClock(30);
	
	if (KEYDOWN(VK_ESCAPE)||input.KeyDown(DIK_ESCAPE))
	{
		PostMessage(hWnd,WM_DESTROY,0,0);
	}
	return TRUE;

}
