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
RENDERCONTEXT rc;

CCamera camera;
CLight light;

int id;
CZBuffer zb;

CBitmapFile bmpfile;
CBitmapImage bmpImage;

#define CAM_DECEL            (.25)  // deceleration/friction
#define MAX_SPEED             20
#define NUM_OBJECTS           5     // number of objects system loads
#define NUM_SCENE_OBJECTS     100    // number of scenery objects 
#define UNIVERSE_RADIUS       2000  // size of universe
#define MAX_VEL               20   // maxim velocity of objects

int constantSphereId;
int flatSphereId;
int gauroudSphereId;


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
	
	CPoint4D camPos(0,0,0);
	CVector4 camDir(0,0,0);
	
	CVector4 vscale(50.0,50.0,50.0);
	CVector4 vpos(0,0,150);
	CVector4 vrot(0,0,0);

	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,50,12000,120,SCREEN_WIDTH,SCREEN_HEIGHT);
	
	render.LoadCOBModel("..//res//sphere_blue_alpha01.cob",id,vscale,vpos,vrot,VERTEX_FLAGS_SWAP_YZ | 
		VERTEX_FLAGS_TRANSFORM_LOCAL/* | 
		VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD*/ );
	/*render.LoadCOBModel("..//res//water_constant_01.cob",constantSphereId,vscale,vpos,vrot,
		VERTEX_FLAGS_TRANSFORM_LOCAL|VERTEX_FLAGS_SWAP_YZ|VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);
	render.LoadCOBModel("..//res//water_flat_01.cob",flatSphereId,vscale,vpos,vrot,
		VERTEX_FLAGS_TRANSFORM_LOCAL|VERTEX_FLAGS_SWAP_YZ|VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);
	render.LoadCOBModel("..//res//water_gouraud_01.cob",gauroudSphereId,vscale,vpos,vrot,
		VERTEX_FLAGS_TRANSFORM_LOCAL|VERTEX_FLAGS_SWAP_YZ|VERTEX_FLAGS_TRANSFORM_LOCAL_WORLD);*/

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
	
	/*bmpfile.LoadBitmapFromFile("..//res//checkerboard800.bmp");
	bmpImage.CreateBitmapImage(0,0,bmpfile.bitmapinfoheader.biWidth,bmpfile.bitmapinfoheader.biHeight,
		bmpfile.bitmapinfoheader.biBitCount);
	bmpImage.LoadImageBitmap16(bmpfile,0,0,BITMAP_EXTRACT_MODE_ABS);
	bmpfile.UnLoadBitmapFile();*/

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

	//render.DDrawFillSurface(render.lpddsback,_RGB16BIT565(255,255,255));
	/*render.DDrawLockBackSurface();
	bmpImage.RenderBitmapImage16(render.backBuffer,render.backlpitch,false);
	render.DDrawUnLockBackSurface();*/

	render.ResetRenderList4DV1();
	render.ResetOBJ4DV1(id);

	render.CreateRotateMatrix4x4(0,angleY,0,mrot);

	if (KEYDOWN(VK_RIGHT))
	{
		camera.dir.y+=4;
	}
	if (KEYDOWN(VK_LEFT))
	{
		camera.dir.y-=4;
	}
	
	render.TransOBJModel4DV1(id,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);

	render.ptrModels[id]->worldPos.x=0;
	render.ptrModels[id]->worldPos.y=0;
	render.ptrModels[id]->worldPos.z=150;

	render.ModelToWorldOBJ4DV1(id,TRANSFORM_LOCAL_TO_TRANS);

	render.LoadRenderListFromModel(id);
//////////////////////////////////////////////////////////////////////////
	/*render.ResetOBJ4DV1(constantSphereId);
	render.ptrModels[constantSphereId]->worldPos.x=-80;
	render.ptrModels[constantSphereId]->worldPos.y=0;
	render.ptrModels[constantSphereId]->worldPos.z=120;

	render.TransOBJModel4DV1(constantSphereId,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);

	render.ModelToWorldOBJ4DV1(constantSphereId,TRANSFORM_LOCAL_TO_TRANS);

	render.LoadRenderListFromModel(constantSphereId,FALSE);

	render.ResetOBJ4DV1(flatSphereId);

	render.ptrModels[flatSphereId]->worldPos.x=0;
	render.ptrModels[flatSphereId]->worldPos.y=0;
	render.ptrModels[flatSphereId]->worldPos.z=120;

	render.TransOBJModel4DV1(flatSphereId,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);

	render.ModelToWorldOBJ4DV1(flatSphereId,TRANSFORM_LOCAL_TO_TRANS);

	render.LoadRenderListFromModel(flatSphereId,FALSE);

	render.ResetOBJ4DV1(gauroudSphereId);

	render.ptrModels[gauroudSphereId]->worldPos.x=80;
	render.ptrModels[gauroudSphereId]->worldPos.y=0;
	render.ptrModels[gauroudSphereId]->worldPos.z=120;

	render.TransOBJModel4DV1(gauroudSphereId,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);

	render.ModelToWorldOBJ4DV1(gauroudSphereId,TRANSFORM_LOCAL_TO_TRANS);

	render.LoadRenderListFromModel(gauroudSphereId,FALSE);*/
//////////////////////////////////////////////////////////////////////////
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
	rc.attr=RENDER_ATTR_INVZBUFFER  |RENDER_ATTR_ALPHA|RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT;
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
