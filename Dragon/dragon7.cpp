#include "dragon.h"
#include "resource.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)

extern HWND hWnd;
extern HINSTANCE hInstance;

#define WINDOW_APP   0

#define UNIVERSE_RADIUS   4000
#define POINT_SIZE        100
#define NUM_POINTS_X       (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z       (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)

#define NUM_TOWERS        64
#define NUM_TANKS         32
#define TANK_SPEED        15

CPoint4D towersPos[NUM_TOWERS];
CPoint4D tanksPos[NUM_TANKS];

CLight light;

CInputSystem* input8;
CRenderSystem render;

CVector4 vscale(0.75,0.75,0.75);
CVector4 vpos(0,0,0);
CVector4 vrot(0,0,0);

CCamera camera;
CPoint4D camPos(0,40,0);
CVector4 camDir(0,0,0);
CPoint4D camTarget(0,0,0);

CErrorLog errorlog;


int towerId,playId,tankId,markerId;

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
	
	input8=new CInputSystem(FALSE);

	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);

	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,50,12000,90,SCREEN_WIDTH,SCREEN_HEIGHT);

	render.LoadPLGModel("tank3.plg",tankId,vscale,vpos,vrot);

	render.LoadPLGModel("tank2.plg",playId,vscale,vpos,vrot);
	
	vscale=CVector4(1,2,1);
	render.LoadPLGModel("tower1.plg",towerId,vscale,vpos,vrot);

	vscale=CVector4(3,3,3);
	render.LoadPLGModel("marker1.plg",markerId,vscale,vpos,vrot);

	//坦克的位置
	for (int i=0;i<NUM_TANKS;i++)
	{
		tanksPos[i].x=RAND_RANGE(-UNIVERSE_RADIUS,UNIVERSE_RADIUS);
		tanksPos[i].y=0;
		tanksPos[i].z=RAND_RANGE(-UNIVERSE_RADIUS,UNIVERSE_RADIUS);
		tanksPos[i].w=RAND_RANGE(0,360);
	}

	//塔的位置
	for (int i=0;i<NUM_TOWERS;i++)
	{
		towersPos[i].x=RAND_RANGE(-UNIVERSE_RADIUS,UNIVERSE_RADIUS);
		towersPos[i].y=0;
		towersPos[i].z=RAND_RANGE(-UNIVERSE_RADIUS,UNIVERSE_RADIUS);
	}
	
	RGBAV1 white, gray, black, red, green, blue;

	white.rgba = _RGBA32BIT(255,255,255,0);
	gray.rgba  = _RGBA32BIT(100,100,100,0);
	black.rgba = _RGBA32BIT(0,0,0,0);
	red.rgba   = _RGBA32BIT(255,0,0,0);
	green.rgba = _RGBA32BIT(0,255,0,0);
	blue.rgba  = _RGBA32BIT(0,0,255,0);

	//创建方向光
	CVector4 lightDir(0,-1,0);
	light.CreateLightV1(LIGHTV1_STATE_ON,LIGHTV1_ATTR_AMBIENT|LIGHTV1_ATTR_INFINITE|LIGHTV1_ATTR_DIFFUSE|
		LIGHTV1_ATTR_SPECULAR,gray,gray,gray,CVector4(0,0,0),lightDir,0,0,0,0,0,0,5);
	
	render.CreateRenderList();
	
	errorlog.CreateErrorFile();
	
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
	static float angleX=0,angleZ=0,angleY=1;
	static float CamDist=6000;
	static float tankSpeed=0;
	static float turning=0;
	static BOOL wireMode=FALSE;
	static BOOL backfaceMode=TRUE;
	static BOOL lightMode=TRUE;
	static BOOL zsortMode=TRUE;

	render.DDrawFillSurface(render.lpddsback,0);
	input8->UpateDevice();
	
	//画天空
	render.FillRectangle(0,0,SCREEN_WIDTH-1,(SCREEN_HEIGHT)/2,_RGB16BIT565(10,255,255),render.lpddsback);
	//画大地
	render.FillRectangle(0,SCREEN_HEIGHT/2-1,SCREEN_WIDTH-1,SCREEN_HEIGHT-1,_RGB16BIT565(50,50,200),render.lpddsback);
	
	render.ResetRenderList4DV1();
	
	if (input8->KeyDown(DIK_SPACE))
	{
		tankSpeed=5*TANK_SPEED;
	}
	else
	{
		tankSpeed=TANK_SPEED;
	}

	if (input8->KeyDown(DIK_RIGHT))
	{
		camera.dir.y+=3;
		if ((turning+=2)>15)
		{
			turning=15;
		}
	}
	else if (input8->KeyDown(DIK_LEFT))
	{
		camera.dir.y-=3;
		if ((turning-=2)<-15)
		{
			turning=-15;
		}
	}
	else
	{
		if (turning>0)
		{
			turning-=1;
		}
		else
		{
			turning+=1;
		}
	}

	if (input8->KeyDown(DIK_UP))
	{
		camera.pos.x+=tankSpeed*FastSin(camera.dir.y);
		camera.pos.z+=tankSpeed*FastCos(camera.dir.y);
	}
	if (input8->KeyDown(DIK_DOWN))
	{
		camera.pos.x-=tankSpeed*FastSin(camera.dir.y);
		camera.pos.z-=tankSpeed*FastCos(camera.dir.y);
	}

	if (input8->KeyDown(DIK_W))
	{
		wireMode=!wireMode;
		render.WaitClock(100);
	}
	if (input8->KeyDown(DIK_B))
	{
		backfaceMode=!backfaceMode;
		render.WaitClock(100);
	}
	if (input8->KeyDown(DIK_L))
	{
		lightMode=!lightMode;
		render.WaitClock(100);
	}
	if (input8->KeyDown(DIK_Z))
	{
		zsortMode=!zsortMode;
		render.WaitClock(100);
	}

	camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);

	render.ResetOBJ4DV1(playId);

	render.ptrModels[playId]->worldPos.x=camera.pos.x+300*FastSin(camera.dir.y);
	render.ptrModels[playId]->worldPos.y=camera.pos.y-60;
	render.ptrModels[playId]->worldPos.z=camera.pos.z+200*FastCos(camera.dir.y);
	
	render.CreateRotateMatrix4x4(0,camera.dir.y+turning,0,mrot);
	
	render.TransOBJModel4DV1(playId,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,false);

	render.ModelToWorldOBJ4DV1(playId,TRANSFORM_TRANS_ONLY);

	render.LoadRenderListFromModel(playId,FALSE);

	for (int i=0;i<NUM_TANKS;i++)
	{
		render.ResetOBJ4DV1(tankId);
		render.CreateRotateMatrix4x4(0,tanksPos[i].w,0,mrot);
		render.TransOBJModel4DV1(tankId,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,FALSE);

		render.ptrModels[tankId]->worldPos.x=tanksPos[i].x;
		render.ptrModels[tankId]->worldPos.y=tanksPos[i].y;
		render.ptrModels[tankId]->worldPos.z=tanksPos[i].z;

		render.ModelToWorldOBJ4DV1(tankId,TRANSFORM_TRANS_ONLY);
		
		render.CullOBJ4DV1(tankId,camera,CULLED_XYZ_PLANES);
		render.RemoveBackfaceOBJ4DV1(tankId,camera);
		
		render.LoadRenderListFromModel(tankId,FALSE);
	}
	
	for (int i=0;i<NUM_TOWERS;i++)
	{
		render.ResetOBJ4DV1(towerId);
		render.ptrModels[towerId]->worldPos=towersPos[i];
		
		render.ModelToWorldOBJ4DV1(towerId,TRANSFORM_LOCAL_TO_TRANS);

		render.CullOBJ4DV1(towerId,camera,CULLED_XYZ_PLANES);
		render.RemoveBackfaceOBJ4DV1(towerId,camera);
		
		render.LoadRenderListFromModel(towerId,FALSE);
	}

	srand(13);
	
	for (int x=0;x<NUM_POINTS_X;x++)
	{
		for (int z=0;z<NUM_POINTS_Z;z++)
		{
			render.ResetOBJ4DV1(markerId);

			render.ptrModels[markerId]->worldPos.x =RAND_RANGE(-100,100)-UNIVERSE_RADIUS+x*POINT_SIZE;
			render.ptrModels[markerId]->worldPos.y =0;//render.ptrModels[markerId]->maxRadius
			render.ptrModels[markerId]->worldPos.z =RAND_RANGE(-100,100)-UNIVERSE_RADIUS+z*POINT_SIZE;

			render.CullOBJ4DV1(markerId,camera,CULLED_XYZ_PLANES);
			render.ModelToWorldOBJ4DV1(markerId,TRANSFORM_LOCAL_TO_TRANS);
			render.LoadRenderListFromModel(markerId,FALSE);
		}
	}

	if (backfaceMode)
	{
		render.RemoveBackfaceRenderList4DV1(camera);
		//render.WaitClock(100);
	}
	
	render.WorldToCamRenderList4DV1(camera);
	
	render.ClipTriangleRenderList4DV1(camera,CLIP_XYZ_PLANE);
	
	if (lightMode)
	{
		render.TransLight4DV1(light,camera.mcam,TRANSFORM_LOCAL_TO_TRANS);
		render.LightRenderList4DV1World16(camera,light);
		//render.WaitClock(100);
	}

	if (zsortMode)
	{
		render.SortRenderList4DV1(SORT_RENDERLIST_AVGZ);
		//render.WaitClock(100);
	}
	render.CameraToPersRenderList4DV1(camera);
	render.PersToScreenRenderList4DV1(camera);

	render.DDrawLockBackSurface();
	if (wireMode)
	{
		render.DrawRenderList4DV1Wire16();
	}
	else
	{
		render.DrawRenderList4DV1Solid16();
	}
	
	render.DDrawUnLockBackSurface();

	render.FlipSurface();

	render.WaitClock(30);
	
	if (KEYDOWN(VK_ESCAPE)||input8->KeyDown(DIK_ESCAPE))
	{
		PostMessage(hWnd,WM_DESTROY,0,0);
	}
	return TRUE;

}
