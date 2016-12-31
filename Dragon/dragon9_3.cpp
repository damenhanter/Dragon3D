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

CCamera camera;
CLight light;

int id;
CZBuffer zb;

CBitmapFile bmpfile;
CBitmapImage bmpImage;

float camSpeed=0.0f;
#define CAM_DECEL            (.25)  // deceleration/friction
#define MAX_SPEED             20
#define NUM_OBJECTS           4     // number of objects system loads
#define NUM_SCENE_OBJECTS     500    // number of scenery objects 
#define UNIVERSE_RADIUS       1000  // size of universe

CCOBModel* ptrWork;
CCOBModel objArray[NUM_OBJECTS];
CCOBModel objScene;

BHVNODE bhvTree;
char *objFileNames[NUM_OBJECTS] = { 
	"..//res//cube_flat_textured_01.cob",
	"..//res//cube_flat_textured_02.cob", 
	"..//res//fire_cube01.cob",
	"..//res//earth01.cob",
};
int currObj=0;

OBJCONTAINER sceneObj[NUM_SCENE_OBJECTS];


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

	input8=new CInputSystem(FALSE);

	errorlog.CreateErrorFile();

	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);

	CPoint4D camPos(0,0,0);
	CVector4 camDir(0,0,0);

	CVector4 vscale(20.0,20.0,20.0);
	CVector4 vpos(0,0,150);
	CVector4 vrot(0,0,0);

	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,10,12000,120,SCREEN_WIDTH,SCREEN_HEIGHT);
	
	for (int i=0;i<NUM_OBJECTS;i++)
	{
		objArray[i].LoadModelFromFile(objFileNames[i],vscale,vpos,vrot,VERTEX_FLAGS_SWAP_YZ 
			| VERTEX_FLAGS_TRANSFORM_LOCAL 
			| VERTEX_FLAGS_INVERT_TEXTURE_V);
	}
	currObj=0;
	ptrWork=&objArray[currObj];

	for (int i=0;i<NUM_SCENE_OBJECTS;i++)
	{
		sceneObj[i].ptrModel=ptrWork;
		sceneObj[i].pos.x=RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		sceneObj[i].pos.y=RAND_RANGE(-200, 200);
		sceneObj[i].pos.z=RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);

		sceneObj[i].state=sceneObj[i].ptrModel->state;
		sceneObj[i].attr=sceneObj[i].ptrModel->attr;
	}

	memset(&bhvTree,0,sizeof(BHVNODE));
	BHVTreeSetUp(&bhvTree,sceneObj,NUM_SCENE_OBJECTS,0,3,UNIVERSE_RADIUS);

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

	if (input8->KeyDown(DIK_N))
	{
		if (++currObj>NUM_OBJECTS-1)
		{
			currObj=0;
		}
		ptrWork=&objArray[currObj];
		render.WaitClock(100);
	}

	ResetBHVTree(&bhvTree);
	camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);
	render.BHVFrustrumCull(&bhvTree,camera,CULLED_XYZ_PLANES);

	/*if ((angleY+=1)>=360)
	{
		angleY=0;
	}*/
	render.CreateRotateMatrix4x4(0,angleY,0,mrot);

	for (int i=0;i<NUM_SCENE_OBJECTS;i++)
	{
		if (sceneObj[i].state & OBJMODEL4DV1_STATE_CULLED)
		{
			continue;
		}
		render.ResetOBJ4DV1(ptrWork);
		ptrWork->worldPos=sceneObj[i].pos;
		if (!render.CullOBJ4DV1(ptrWork,camera,CULLED_XYZ_PLANES))
		{
			render.TransOBJModel4DV1(ptrWork,mrot,TRANSFORM_LOCAL_ONLY,TRUE,FALSE);
			render.ModelToWorldOBJ4DV1(ptrWork,TRANSFORM_LOCAL_TO_TRANS,FALSE);
			render.LoadRenderListFromModel(ptrWork,FALSE);
		}
	}

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
	rc.attr=RENDER_ATTR_INVZBUFFER  |RENDER_ATTR_ALPHA|RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE;
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
