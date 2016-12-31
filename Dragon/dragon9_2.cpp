#include "dragon.h"
#include "resource.h"
#include "resource1.h"

#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4305)

extern HWND hWnd;
extern HINSTANCE hInstance;

#define WINDOW_APP		  1

#define CAM_DECEL         0.25
#define MAX_SPEED         20
float camSpeed=0.0f;

CGUISystem gui;
CInputSystem input(false);
CRenderSystem render;
CEditor* MapEditor;
char ascFileName[256];

CCamera camera;
CLight light;
RENDERCONTEXT rc;

CBitmapImage image;
CBitmapImage bgImage;
int id;
CZBuffer zb;
int floorId;
CBSPTree bspTree;
LPBSPNODE root=NULL;
LPBSPNODE wallList=NULL;

CCOBModel objMove;

CErrorLog errorlog;

DWORD startClockCount;
int windowMode;
int windowClose;
char buffer[256];

DIMMOUSE MouseState;

LRESULT APIENTRY WndProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	wndclass.lpszMenuName  = MAKEINTRESOURCEA(IDR_MENU) ;
	wndclass.lpszClassName = ClassName ;
	wndclass.hIconSm       = LoadIcon (NULL, IDI_APPLICATION) ;

	RegisterClassEx (&wndclass) ;

	hwnd = CreateWindow(ClassName, "SCOTT",
		(WINDOW_APP?(WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION):(WS_POPUP|WS_VISIBLE)),
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
	HMENU hMenu;

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
	case WM_MOUSEMOVE:
		{
			MouseState.x=LOWORD(lParam);
			MouseState.y=HIWORD(lParam);
		}break;
	case WM_LBUTTONDOWN:
		{
			MouseState.x=LOWORD(lParam);
			MouseState.y=HIWORD(lParam);
			MouseState.rcButtons[0]=1;
		}break;
	case WM_MBUTTONDOWN:
		{
			MouseState.x=LOWORD(lParam);
			MouseState.y=HIWORD(lParam);
			MouseState.rcButtons[1]=1;
		}break;
	case WM_RBUTTONDOWN:
		{
			MouseState.x=LOWORD(lParam);
			MouseState.y=HIWORD(lParam);
			MouseState.rcButtons[2]=1;
		}break;
	case WM_INITMENU:
		{
			hMenu=(HMENU)wParam;
			if (MapEditor->viewGrid==1)
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWGRID,MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWGRID,MF_UNCHECKED);
			}
			if (MapEditor->viewWalls==1)
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWWALL,MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWWALL,MF_UNCHECKED);
			}
			if (MapEditor->viewFloors==1)
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWFLOOR,MF_CHECKED);
			}
			else
			{
				CheckMenuItem(hMenu,ID_VIEW_VIEWFLOOR,MF_UNCHECKED);
			}
		}break;
	case WM_COMMAND:
		{
			hMenu=GetMenu(hWnd);
			//命令id
			switch (LOWORD(wParam))
			{
			case ID_FILE_EXIT:
				{
					PostQuitMessage(0);
				}break;
			case ID_FILE_LOAD_LEV:
				{
					if (DialogBox(hInstance,MAKEINTRESOURCEA(IDD_DIALOG),hWnd,DialogProc)==IDOK)
					{
						MapEditor->LoadBSPFileLEV(ascFileName);
					}
				}break;
			case ID_FILE_SAVE_LEV:
				{
					if (DialogBox(hInstance,MAKEINTRESOURCEA(IDD_DIALOG1),hWnd,DialogProc)==IDOK)
					{
						MapEditor->SaveBSPFileLEV(ascFileName);
					}
				}break;
			case ID_VIEW_VIEWGRID:
				{
					MapEditor->viewGrid=-MapEditor->viewGrid;
					if (MapEditor->viewGrid==1)
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWGRID,MF_CHECKED);
					}
					else
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWGRID,MF_UNCHECKED);
					}
					
				}break;
			case ID_VIEW_VIEWWALL:
				{
					
					if (MapEditor->viewWalls==1)
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWWALL,MF_CHECKED);
					}
					else
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWWALL,MF_UNCHECKED);
					}
					MapEditor->viewWalls=-MapEditor->viewWalls;
				}break;
			case ID_VIEW_VIEWFLOOR:
				{
					
					if (MapEditor->viewFloors==1)
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWFLOOR,MF_CHECKED);
					}
					else
					{
						CheckMenuItem(hMenu,ID_VIEW_VIEWFLOOR,MF_UNCHECKED);
					}
					MapEditor->viewFloors=-MapEditor->viewFloors;
				}break;
			case ID_COMPILE_VIEW:
				{
					bspTree.DeleteBSP(&MapEditor->root);
					MapEditor->ceilingHeight=0;
					MapEditor->ConvertLinesToWalls();
					bspTree.SetUp(&MapEditor->wallList);
					MapEditor->root=MapEditor->wallList;

					camera.pos.x=0;
					camera.pos.y=MapEditor->ceilingHeight/2;
					camera.pos.z=0;

					camera.dir.y=0;
					camSpeed=0;

					MapEditor->GenerateFloors(_RGB16BIT565(255,255,255),CPoint4D(0,0,0),
						CPoint4D(0,0,0), TRI4DV1_ATTR_DISABLE_MATERIAL 
						| TRI4DV1_ATTR_2SIDED 
						| TRI4DV1_ATTR_RGB16 
						| TRI4DV1_ATTR_SHADE_MODE_FLAT
 						| TRI4DV1_ATTR_SHADE_MODE_TEXTURE);

					MapEditor->editorState=EDITOR_STATE_VIEW;
				}break;
			case ID_HELP_ABOUT:
				{
					MessageBox(hWnd,"Version 1.0 Dragon Tools Programmer.","Dragon",MB_OK);
				}break;
			default:
				break;
			}
		}break;
	case WM_KEYUP:
		/*if(wParam==VK_ESCAPE)
		{
			::PostQuitMessage(0);
		}*/
		break;
	case WM_DESTROY:
		{
			::PostQuitMessage(0);
			windowClose=1;
		}
		break;
	default:
		break;
	}
	return DefWindowProc (hWnd, message, wParam, lParam) ;
}

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hLEVFileName=GetDlgItem(hwndDlg,IDC_LEV_FILENAME);
	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			SetFocus(hLEVFileName);
		}break;
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					int fileLength=SendMessage(hLEVFileName,EM_LINELENGTH,0,0);
					ascFileName[0]=(unsigned char)255;
					SendMessage(hLEVFileName,EM_GETLINE,0,(LPARAM)(LPCTSTR)ascFileName);
					ascFileName[fileLength]=0;

					EndDialog(hwndDlg,IDOK);
					return TRUE;
				}break;
			case IDCANCEL:
				{
					EndDialog(hwndDlg,IDCANCEL);
					return TRUE;
				}break;
			default:
				break;
			}
		}break;
	default:
		break;
	}
	return 0;
}

int GameInit(void* parms)
{
	srand(render.StartClock());
	
	CreateMathTable();
	
	errorlog.CreateErrorFile();
	
	render.CreateDDraw(hWnd,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,WINDOW_APP);

	MapEditor=new CEditor(&render);
	MapEditor->LoadTextureData();

	CPoint4D camPos(0,0,0);
	CVector4 camDir(0,0,0);
	
	camera.InitCamera(CAM_MODEL_EULER,camPos,camDir,NULL,5,12000,120,SCREEN_WIDTH,SCREEN_HEIGHT);

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

	CBitmapFile bitmapfile;
	bitmapfile.LoadBitmapFromFile("..//res//bspgui02.bmp");
	image.CreateBitmapImage(0,0,bitmapfile.bitmapinfoheader.biWidth,
		bitmapfile.bitmapinfoheader.biHeight,bitmapfile.bitmapinfoheader.biBitCount);
	image.LoadImageBitmap16(bitmapfile,0,0,BITMAP_EXTRACT_MODE_ABS);
	bitmapfile.UnLoadBitmapFile();

	bitmapfile.LoadBitmapFromFile("..//res//red01a.bmp");
	bgImage.CreateBitmapImage(0,0,bitmapfile.bitmapinfoheader.biWidth,
		bitmapfile.bitmapinfoheader.biHeight,bitmapfile.bitmapinfoheader.biBitCount);
	bgImage.LoadImageBitmap16(bitmapfile,0,0,BITMAP_EXTRACT_MODE_ABS);
	bitmapfile.UnLoadBitmapFile();
	
	objMove.LoadModelFromFile("..//res//rec_flat_textured_01.cob",CVector4(.15,.15,.15),
		CVector4(0,0,150),CVector4(0,0,0),VERTEX_FLAGS_SWAP_YZ  | VERTEX_FLAGS_INVERT_Z |
		VERTEX_FLAGS_TRANSFORM_LOCAL );

	render.CreateRenderList();
	
	MapEditor->ResetEditor();

	zb.CreateZBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,ZBUFFER_ATTR_32BIT);

	return true;
}
int GameShutdown(void* parms)
{
	errorlog.CloseFile();
	if (MapEditor!=NULL)
	{
		delete MapEditor;
		MapEditor=NULL;
	}
	
	return true;
}
int GameMain(void* parms)
{
	static CMatrix4x4 mrot;
	static float angleY=2;
	POINT posMouse;
	static int id=-1;

	switch (MapEditor->editorState)
	{
	case EDITOR_STATE_INIT:
		{
			MapEditor->editorState=EDITOR_STATE_EDIT;
		}break;
	case EDITOR_STATE_EDIT:
		{
			render.StartClock();

			render.DDrawFillSurface(render.lpddsback,0);
			
			render.DDrawLockBackSurface();
			image.RenderBitmapImage16(render.backBuffer,render.backlpitch,FALSE);
			render.DDrawUnLockBackSurface();
			
			input.UpateDevice();
			posMouse.x=MouseState.x;
			posMouse.y=MouseState.y;
			MapEditor->nearestLine=MapEditor->DeleteLines(posMouse.x,posMouse.y,FALSE);

			if (MouseState.rcButtons[DIM_LB]|MouseState.rcButtons[DIM_RB]|MouseState.rcButtons[DIM_MB])
			{
				if (posMouse.x>INTERFACE_MIN_X && posMouse.x<INTERFACE_MAX_X &&
					posMouse.y>INTERFACE_MIN_Y && posMouse.y<INTERFACE_MAX_Y)
				{
					if ((id=MapEditor->ProcessGadgets(posMouse.x,posMouse.y,MouseState))>=0)
					{
						switch (id)
						{
						case GADGET_ID_SEGMENTMODE:
							{
								MapEditor->action=ACTION_STARTING_LINE;
							}break;
						case GADGET_ID_POLYLINEMODE:
							{
								MapEditor->action=ACTION_STARTING_POLYLINE;
							}break;
						case GADGET_ID_DELETEMODE:
							{
								MapEditor->action=ACTION_DELETE_LINE;
							}break;
						case GADGET_ID_CLEARALL:
							{
								MapEditor->ResetEditor();
							}break;
						case GADGET_ID_FLOORMODE:
							{
								MapEditor->action=ACTION_FLOOR_MODE;
							}break;
						case GADGET_ID_FLOOR_TEXTURE_INC:
							{
								if((++MapEditor->textureIndexFloor)>=NUM_TEXTURES)
								{
									MapEditor->textureIndexFloor=NUM_TEXTURES-1;
								}
							}break;
						case GADGET_ID_FLOOR_TEXTURE_DEC:
							{
								if((--MapEditor->textureIndexFloor)<0)
								{
									MapEditor->textureIndexFloor=0;
								}
							}break;
						case GADGET_ID_WALL_TEXTURE_INC:
							{
								if (++MapEditor->textureIndexWall>=NUM_TEXTURES)
								{
									MapEditor->textureIndexWall=NUM_TEXTURES-1;
								}
							}break;
						case GADGET_ID_WALL_TEXTURE_DEC:
							{
								if (--MapEditor->textureIndexWall<0)
								{
									MapEditor->textureIndexWall=0;
								}
							}break;
						case GADGET_ID_WALL_HEIGHT_DEC:
							{
								if ((MapEditor->wallHeight-=8)<0)
								{
									MapEditor->wallHeight=0;
								}
							}break;
						case GADGET_ID_WALL_HEIGHT_INC:
							{
								MapEditor->wallHeight+=8;
							}break;
						default:
							break;
						}
					}
				}
				if (posMouse.x>BSP_MIN_X && posMouse.x<BSP_MAX_X &&
					posMouse.y>BSP_MIN_Y && posMouse.y<BSP_MAX_Y)
				{
					if (MapEditor->action==ACTION_STARTING_LINE)
					{
						if (!MapEditor->snapGrid)
						{
							MapEditor->lines[MapEditor->totalLines].p0.x=posMouse.x;
							MapEditor->lines[MapEditor->totalLines].p0.y=posMouse.y;
						}
						else
						{
							MapEditor->lines[MapEditor->totalLines].p0.x=BSP_GRID_SIZE*(int)(0.5+posMouse.x/(float)BSP_GRID_SIZE);
							MapEditor->lines[MapEditor->totalLines].p0.y=BSP_GRID_SIZE*(int)(0.5+posMouse.y/(float)BSP_GRID_SIZE);
						}
						MapEditor->startingLine=MapEditor->totalLines;
						MapEditor->action=ACTION_ENDING_LINE;
					}
					else if (MapEditor->action==ACTION_ENDING_LINE)
					{
						if (MouseState.rcButtons[DIM_LB])
						{
							if (!MapEditor->snapGrid)
							{
								MapEditor->lines[MapEditor->totalLines].p1.x=posMouse.x;
								MapEditor->lines[MapEditor->totalLines].p1.y=posMouse.y;
							}
							else
							{
								MapEditor->lines[MapEditor->totalLines].p1.x=BSP_GRID_SIZE*(int)(0.5+posMouse.x/(float)BSP_GRID_SIZE);
								MapEditor->lines[MapEditor->totalLines].p1.y=BSP_GRID_SIZE*(int)(0.5+posMouse.y/(float)BSP_GRID_SIZE);
							}
							MapEditor->lines[MapEditor->totalLines].textureid=MapEditor->textureIndexWall;
							MapEditor->lines[MapEditor->totalLines].height=MapEditor->wallHeight;
							MapEditor->lines[MapEditor->totalLines].attr=MapEditor->polyAttr;
							MapEditor->lines[MapEditor->totalLines].color=MapEditor->polyColor;

							MapEditor->action=ACTION_STARTING_LINE;
							
							if (++MapEditor->totalLines>=MAX_LINES)
							{
								MapEditor->totalLines=MAX_LINES-1;
							}
						}
						else if (MouseState.rcButtons[DIM_RB])
						{
							MapEditor->action=ACTION_STARTING_LINE;
						}
					}
					else if (MapEditor->action==ACTION_STARTING_POLYLINE)
					{
						if (!MapEditor->snapGrid)
						{
							MapEditor->lines[MapEditor->totalLines].p0.x=posMouse.x;
							MapEditor->lines[MapEditor->totalLines].p0.y=posMouse.y;
						}
						else
						{
							MapEditor->lines[MapEditor->totalLines].p0.x=BSP_GRID_SIZE*(int)(0.5+(float)posMouse.x/(float)BSP_GRID_SIZE);
							MapEditor->lines[MapEditor->totalLines].p0.y=BSP_GRID_SIZE*(int)(0.5+(float)posMouse.y/(float)BSP_GRID_SIZE);
						}
						MapEditor->startingLine=MapEditor->totalLines;
						MapEditor->action=ACTION_ADDING_POLYLINE;
					}	
					else if (MapEditor->action==ACTION_ADDING_POLYLINE)
					{
						if (MouseState.rcButtons[DIM_LB])
						{
							if (!MapEditor->snapGrid)
							{
								MapEditor->lines[MapEditor->totalLines].p1.x=posMouse.x;
								MapEditor->lines[MapEditor->totalLines].p1.y=posMouse.y;
							}
							else
							{
								MapEditor->lines[MapEditor->totalLines].p1.x=BSP_GRID_SIZE*(int)(0.5+posMouse.x/(float)BSP_GRID_SIZE);
								MapEditor->lines[MapEditor->totalLines].p1.y=BSP_GRID_SIZE*(int)(0.5+posMouse.y/(float)BSP_GRID_SIZE);
							}
							MapEditor->lines[MapEditor->totalLines].textureid=MapEditor->textureIndexWall;
							MapEditor->lines[MapEditor->totalLines].height=MapEditor->wallHeight;
							MapEditor->lines[MapEditor->totalLines].attr=MapEditor->polyAttr;
							MapEditor->lines[MapEditor->totalLines].color=MapEditor->polyColor;

							MapEditor->action=ACTION_ADDING_POLYLINE;
							
							if (++MapEditor->totalLines>=MAX_LINES)
							{
								MapEditor->totalLines=MAX_LINES-1;
							}
							MapEditor->lines[MapEditor->totalLines].p0=MapEditor->lines[MapEditor->totalLines-1].p1;
						}
						else if (MouseState.rcButtons[DIM_RB])
						{
							MapEditor->action=ACTION_STARTING_POLYLINE;
						}
					}
					else if (MapEditor->action==ACTION_DELETE_LINE)
					{
						MapEditor->DeleteLines(posMouse.x,posMouse.y,TRUE);
					}
					else if (MapEditor->action==ACTION_FLOOR_MODE)
					{
						int x=(posMouse.x-BSP_MIN_X)/BSP_GRID_SIZE;
						int y=(posMouse.y-BSP_MIN_Y)/BSP_GRID_SIZE;
						if (MouseState.rcButtons[DIM_LB])
						{
							MapEditor->floors[y][x]=MapEditor->textureIndexFloor;
						}
						else if (MouseState.rcButtons[DIM_RB])
						{
							MapEditor->floors[y][x]=-1;
						}
					}
				}
				if (MouseState.rcButtons[0])
				{
					MouseState.rcButtons[0]=0;
				}
				if (MouseState.rcButtons[1])
				{
					MouseState.rcButtons[1]=0;
				}
				if (MouseState.rcButtons[2])
				{
					MouseState.rcButtons[2]=0;
				}
			}
			
			if (MapEditor->viewGrid==1)
			{
				MapEditor->DrawGrid(_RGB16BIT565(255,255,255),BSP_MIN_X, BSP_MIN_Y,
					BSP_GRID_SIZE, BSP_GRID_SIZE,BSP_CELLS_X,BSP_CELLS_Y);
			}
			if (MapEditor->viewFloors==1)
			{
				MapEditor->DrawFloors();
			}

			render.DDrawLockBackSurface();
			if (MapEditor->viewWalls==1)
			{
				MapEditor->DrawLines(MouseState);
			}
			render.DDrawUnLockBackSurface();

			MapEditor->DrawGadgets();

			//画 墙面纹理预览
			if (MapEditor->textureIndexWall>=0)
			{
				MapEditor->textures.x=667;
				MapEditor->textures.y=302-55;
				MapEditor->textures.currFrame=MapEditor->textureIndexWall;
				MapEditor->textures.RenderScaleAnim(&render,64,64,render.lpddsback);

				MapEditor->nearestLine=MapEditor->DeleteLines(posMouse.x,posMouse.y,FALSE);
				if (MapEditor->nearestLine>=0)
				{
					MapEditor->textures.x=738;
					MapEditor->textures.y=247;
					MapEditor->textures.currFrame=MapEditor->lines[MapEditor->nearestLine].textureid;
					MapEditor->textures.RenderScaleAnim(&render,32,32,render.lpddsback);

					sprintf_s(buffer,"%d",MapEditor->lines[MapEditor->nearestLine].height);
					gui.DrawTextGDI(buffer, 750,188,RGB(0,255,0),render.lpddsback);
				}
			}
			//画 地板纹理预览
			if (MapEditor->textureIndexFloor>=0)
			{
				MapEditor->textures.x=668;
				MapEditor->textures.y=453;
				MapEditor->textures.currFrame=MapEditor->textureIndexFloor;
				MapEditor->textures.RenderScaleAnim(&render,64,64,render.lpddsback);
			}
			
			if (MapEditor->textureIndexWall>=0)
			{
				sprintf_s(buffer,"%d",MapEditor->textureIndexWall);
				gui.DrawTextGDI(buffer,738,336-56,_RGB16BIT565(255,0,0),render.lpddsback);
			}
			else
			{
				gui.DrawTextGDI("None", 738,336,_RGB16BIT565(255,0,00),render.lpddsback);
			}
			sprintf_s(buffer,"%d",MapEditor->wallHeight);
			gui.DrawTextGDI(buffer, 688, 188,_RGB16BIT565(255,0,0),render.lpddsback);

			if (MapEditor->textureIndexFloor>=0)
			{
				sprintf_s(buffer,"%d",MapEditor->textureIndexFloor);
				gui.DrawTextGDI(buffer, 738,488,_RGB16BIT565(255,0,0),render.lpddsback);
			} 
			else
			{
				gui.DrawTextGDI("None", 738,488,_RGB16BIT565(255,0,0),render.lpddsback);
			} 

			render.FlipSurface();
			render.WaitClock(30);

		}break;
	case EDITOR_STATE_VIEW:
		{
			static CVector4 vz(0,0,1);

			render.StartClock();
			render.DDrawFillSurface(render.lpddsback,0);
			input.UpateDevice();

			if (KEYDOWN(VK_RIGHT))
			{
				camera.dir.y+=10;
				bgImage.ScrollBitmap(-10,0);
			}
			if (KEYDOWN(VK_LEFT))
			{
				camera.dir.y-=10;
				bgImage.ScrollBitmap(10,0);
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

			static int scrollCounter=0;
			if (++scrollCounter>5)
			{
				scrollCounter=0;
				bgImage.ScrollBitmap(1,0);
			}

			camera.CreateCAM4DV1MatrixEuler(CAM_ROTATE_ZYX);
			
			render.CreateRotateMatrix4x4(camera.dir.x,camera.dir.y,camera.dir.z,mrot);
			camera.n=vz*mrot;
			camera.n.Normalize();

			render.ResetRenderList4DV1();
			render.ResetOBJ4DV1(MapEditor->ptrModel);

			MapEditor->ptrModel->worldPos.x=0;
			MapEditor->ptrModel->worldPos.y=MapEditor->ceilingHeight;
			MapEditor->ptrModel->worldPos.z=0;

			mrot.Identity();
			render.TransOBJModel4DV1(MapEditor->ptrModel,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,FALSE);
			render.ModelToWorldOBJ4DV1(MapEditor->ptrModel,TRANSFORM_TRANS_ONLY,FALSE);
			render.LoadRenderListFromModel(MapEditor->ptrModel);

			render.ResetOBJ4DV1(MapEditor->ptrModel);

			MapEditor->ptrModel->worldPos.x=0;
			MapEditor->ptrModel->worldPos.y=0;
			MapEditor->ptrModel->worldPos.z=0;
			
			mrot.Identity();
			render.TransOBJModel4DV1(MapEditor->ptrModel,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,FALSE);
			render.ModelToWorldOBJ4DV1(MapEditor->ptrModel,TRANSFORM_TRANS_ONLY,FALSE);
			render.LoadRenderListFromModel(MapEditor->ptrModel);
			
			int numTriangles=render.ptrRenderList->nTriangles;

			render.TraversalBSPInsertCullFrustrumRENDERLIST(MapEditor->root,camera,TRUE);

			numTriangles=render.ptrRenderList->nTriangles-numTriangles;

			render.WorldToCamRenderList4DV1(camera);

			render.ClipTriangleRenderList4DV1(camera,CLIP_XYZ_PLANE);

			render.TransLight4DV1(light,camera.mcam,TRANSFORM_LOCAL_TO_TRANS);

			render.LightRenderList4DV1World16(camera,light);

			//render.SortRenderList4DV1(SORT_RENDERLIST_AVGZ);

			render.CameraToPersRenderList4DV1(camera);

			render.PersToScreenRenderList4DV1(camera);

			render.DDrawLockBackSurface();
			bgImage.RenderBitmapImage16(render.backBuffer,render.backlpitch,FALSE);
			render.DDrawUnLockBackSurface();

			render.DDrawLockBackSurface();
			zb.ClearZBuffer(0);
			// | RENDER_ATTR_ALPHA  
			// | RENDER_ATTR_BILERP
			//| RENDER_ATTR_MIPMAP  
			rc.attr=RENDER_ATTR_WRITETHRUINVZBUFFER  
				|RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT;

			rc.videoBuffer=render.backBuffer;
			rc.lpitch=render.backlpitch;
			rc.mipDist=4500;
			rc.zbuffer=zb.zbuffer;
			rc.zpitch=SCREEN_WIDTH*4;
			rc.ptrRenderList=render.ptrRenderList;
			rc.alphaOverride=-1;
			render.DrawRenderListRENDERCONTEXT(&rc);

			render.DDrawUnLockBackSurface();

//////////////////////////////////////////////////////////////////////////
			render.ResetRenderList4DV1();
			render.ResetOBJ4DV1(&objMove);
			static float rec_count=0;
			objMove.worldPos.x=500*FastCos(rec_count);
			objMove.worldPos.y = 40 + 20*FastSin(rec_count*2);
			objMove.worldPos.z = 500*FastSin(rec_count);

			if ((rec_count+=.5) > 360)
			{
				rec_count =- 360;
			}

			if (!render.CullOBJ4DV1(&objMove,camera,CULLED_XYZ_PLANES))
			{
				mrot.Identity();
				render.CreateRotateMatrix4x4(0,-rec_count,0,mrot); 
				render.TransOBJModel4DV1(&objMove,mrot,TRANSFORM_LOCAL_TO_TRANS,TRUE,FALSE);
				render.ModelToWorldOBJ4DV1(&objMove,TRANSFORM_TRANS_ONLY,FALSE);
				render.LoadRenderListFromModel(&objMove,FALSE);
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
			//zb.ClearZBuffer(0);
			// | RENDER_ATTR_ALPHA  
			// | RENDER_ATTR_BILERP
			//| RENDER_ATTR_MIPMAP  
			rc.attr=RENDER_ATTR_INVZBUFFER  
				|RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT;

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
				MapEditor->editorState=EDITOR_STATE_EDIT;
			}
		}break;
		default:
			break;
	}
	return TRUE;
}
