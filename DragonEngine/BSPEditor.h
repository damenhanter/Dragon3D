#pragma once
#include "datatype.h"
#include "BSPTree.h"
#include "RenderSystem.h"
#include "Animation.h"
#include "Line2D.h"
#include "COBModel.h"
#include "BSPEditor.h"
//控件类型
enum Gadget_TYP
{
	GADGET_TYPE_TOGGLE,      //控件开关
	GADGET_TYPE_MOMENTARY,        //点亮按钮，当你按下时
	GADGET_TYPE_STATIC            //什么也不做
};

enum ActionState
{
	ACTION_STARTING_LINE,
	ACTION_ENDING_LINE,
	ACTION_STARTING_POLYLINE,
	ACTION_ADDING_POLYLINE,
	ACTION_ENDING_POLYLINE,
	ACTION_DELETE_LINE,
	ACTION_FLOOR_MODE
};
//控件ID
#define GADGET_ID_SEGMENTMODE       0
#define GADGET_ID_POLYLINEMODE      2
#define GADGET_ID_DELETEMODE        4
#define GADGET_ID_CLEARALL          6
#define GADGET_ID_FLOORMODE         8

#define GADGET_ID_WALL_TEXTURE_DEC       20
#define GADGET_ID_WALL_TEXTURE_INC       21

#define GADGET_ID_WALL_HEIGHT_DEC       22
#define GADGET_ID_WALL_HEIGHT_INC        23

#define GADGET_ID_FLOOR_TEXTURE_DEC      24
#define GADGET_ID_FLOOR_TEXTURE_INC      25

#define GADGET_ON                        1
#define GADGET_OFF                       0
 
#define NUM_GADGET                       11

#define NUM_TEXTURES                     23
#define TEXTURE_SIZE                    128

#define MAX_LINES                        256

#define LINE_INVALID                     0
#define LINE_VALID                       1

#define BSP_MIN_X                        0
#define BSP_MAX_X                        648
#define BSP_MIN_Y                        0
#define BSP_MAX_Y                        576

#define BSP_GRID_SIZE                    24 //每个单元格的宽度和高度
#define BSP_CELLS_X                      28 //水平核对符号数
#define BSP_CELLS_Y                      25

#define SCREEN_TO_WORLD_X                (-BSP_MAX_X/2)
#define SCREEN_TO_WORLD_Z                (-BSP_MAX_Y/2)

#define WORLD_SCALE_X                    2
#define WORLD_SCALE_Y                    2
#define WORLD_SCALE_Z                    -2

//GUI界面
#define INTERFACE_MIN_X                  656
#define INTERFACE_MAX_X                  800
#define INTERFACE_MIN_Y                  0
#define INTERFACE_MAX_Y                  600

#define EDITOR_STATE_INIT                0
#define EDITOR_STATE_EDIT                1
#define EDITOR_STATE_VIEW                2

typedef struct  Gadget_TYPE
{
	int type;
	int state;
	int id;
	int x0; //位置 
	int y0;
	int width;  //宽度和高度
	int height;
	CAnimation* bmpImage;
	int count;
}GADGET,*LPGADGET;

typedef struct BSPLine_TYP
{
	int id;
	int attr;
	int color;
	int textureid;
	CPoint2D p0;
	CPoint2D p1;
	int elev;
	int height;
}BSPLINE,*LPBSPLINE;

class CEditor
{
public:
	CEditor(CRenderSystem* render);
	~CEditor();

	void ResetEditor();
	void DrawFloors();
	void DrawLines(DIMMOUSE& mouseState);
	int DeleteLines(int x,int y,BOOL deleteline);
	void LoadTextureData();
	void DrawGadgets();
	int ProcessGadgets(int mx,int my,DIMMOUSE& mouseState);
	void DrawGrid(int rgbcolor, int x0, int y0, 
		int xpitch, int ypitch,int xcells, int ycells);
	void ConvertLinesToWalls();
	int GenerateFloors(int color,CPoint4D& vpos,CPoint4D& vrot,int TriAttr);
	int LoadBSPFileLEV(char* filename);
	int SaveBSPFileLEV(char* filename);
public:
	int action;
	int editorState;
	int totalLines;
	int startingLine;
	int snapGrid;          // flag to snap to grid        
	int viewGrid;          // flag to show grid
	int viewWalls;         // flag to show walls
	int viewFloors;        // flag to show floors
	int nearestLine;      // index of nearest line to mouse pointer
	int textureIndexWall;      // index of wall texture
	int textureIndexFloor;      // index of floor texture
	int wallHeight;      // initial wall height
	int numFloorTiles;       // current number of floor tiles
	int polyAttr;       // general polygon attribute
	int polyColor;       // general polygon color
	int ceilingHeight;       // height of ceiling 
	CAnimation buttonImages;
	CAnimation textures;  // holds the textures for display in editor
	CBitmapImage textureMaps[NUM_TEXTURES];// holds the textures for the 3D display

	int floors[BSP_CELLS_Y-1][BSP_CELLS_X-1]; // hold the texture id's for the floor
	BSPLINE lines[MAX_LINES];

	CRenderSystem* render;
	
	CCOBModel* ptrModel;

	LPBSPNODE wallList;
	LPBSPNODE root;
};
