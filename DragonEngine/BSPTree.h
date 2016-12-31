#pragma once
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "Vector4.h"
#include "COBModel.h"

typedef struct PolyF4DV1_TYP
{
	int state;
	int attr;
	int color;   //多边形颜色
	int litColor[4];   //用于存储光照后的颜色
	CBitmapImage* ptrTexture;
	int mati;

	float nlength;
	CVector4 n;
	float avgZ;
	VERTEX vlist[4];  //四边形顶点
	VERTEX tvlist[4];
	
	PolyF4DV1_TYP* next;
	PolyF4DV1_TYP* prev;
}PolyF4DV1,*LPPolyF4DV1;

typedef struct BSPNode_TYP
{
	BSPNode_TYP()
	{
		memset(this, 0, sizeof(BSPNode_TYP));
	}
	int id;  //用于调试
	PolyF4DV1 wall;

	BSPNode_TYP* link;  //指向下一个墙面指针 
	BSPNode_TYP* front;
	BSPNode_TYP* back;
}BSPNODE,*LPBSPNODE;

class CBSPTree
{
public:
	CBSPTree(void);
	~CBSPTree(void);

	void IntersectLines(float x0,float y0,float x1,float y1,
		float x2,float y2,float x3,float y3,float& xi,float& yi);
	void SetUp(LPBSPNODE* tempRoot);
	void TransformBSP(LPBSPNODE root,CMatrix4x4& mt,int coord_select=TRANSFORM_LOCAL_TO_TRANS);
	void DeleteBSP(LPBSPNODE* root);
public:
	
};



//////////////////////////////////////////////////////////////////////////
//BHV树
//////////////////////////////////////////////////////////////////////////

#define MAX_OBJECTS_PER_BHV_CELL      64    //每个BHV单元可容纳的最大物体数,这与节点不同
#define MIN_OBJECTS_PER_BHV_CELL      2     //每个BHV单元必须的最小物体数，小于时递归停止
#define MAX_BHV_CELL_DIVISIONS        8     //在每个轴上允许划分的最大单元
#define MAX_OBJECTS_PER_BHV_NODE      512   //每个节点可容纳的最大物体
#define MAX_BHV_PER_NODE              32    //每个节点最大孩子数


typedef struct OBJCONTAINER_TYP
{
	int state;
	int attr;
	CPoint4D pos;   //位置
	CCOBModel* ptrModel;
}OBJCONTAINER,*LPOBJCONTAINER;

typedef struct BHVNODE_TYP
{
	int state;
	int attr;
	CPoint4D pos;   //中心
	CVector4 radius;  //沿x,y,z半径
	int numObjects;   //包含物体数
	LPOBJCONTAINER objects[MAX_OBJECTS_PER_BHV_NODE];  //物体数组
	int numChildren;  //子节点数

	BHVNODE_TYP* links[MAX_BHV_PER_NODE];  //指向子节点的指针数组
}BHVNODE,*LPBHVNODE;

typedef struct BHVCELL_TYP
{
	int numObjects;  //在这个单元格里有多少物体
	LPOBJCONTAINER objList[MAX_OBJECTS_PER_BHV_CELL];  //单元格内存储的物体
}BHVCELL,*LPBHVCELL;


void BHVTreeSetUp(LPBHVNODE bhvTree,   //创建的bhv树
				  LPOBJCONTAINER bhvObjects,  //指向场景中的物体数组的指针
				  int numObjects,    //场景中物体的数目
				  int level,    //bhv树层次
				  int numDivisions, //沿每个轴将节点分割成多少分
				  int universeRadius  //游戏世界的大小
				  );

void ResetBHVTree(LPBHVNODE bhvTree);