#pragma once
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "Vector4.h"
#include "COBModel.h"

typedef struct PolyF4DV1_TYP
{
	int state;
	int attr;
	int color;   //�������ɫ
	int litColor[4];   //���ڴ洢���պ����ɫ
	CBitmapImage* ptrTexture;
	int mati;

	float nlength;
	CVector4 n;
	float avgZ;
	VERTEX vlist[4];  //�ı��ζ���
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
	int id;  //���ڵ���
	PolyF4DV1 wall;

	BSPNode_TYP* link;  //ָ����һ��ǽ��ָ�� 
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
//BHV��
//////////////////////////////////////////////////////////////////////////

#define MAX_OBJECTS_PER_BHV_CELL      64    //ÿ��BHV��Ԫ�����ɵ����������,����ڵ㲻ͬ
#define MIN_OBJECTS_PER_BHV_CELL      2     //ÿ��BHV��Ԫ�������С��������С��ʱ�ݹ�ֹͣ
#define MAX_BHV_CELL_DIVISIONS        8     //��ÿ�����������ֵ����Ԫ
#define MAX_OBJECTS_PER_BHV_NODE      512   //ÿ���ڵ�����ɵ��������
#define MAX_BHV_PER_NODE              32    //ÿ���ڵ��������


typedef struct OBJCONTAINER_TYP
{
	int state;
	int attr;
	CPoint4D pos;   //λ��
	CCOBModel* ptrModel;
}OBJCONTAINER,*LPOBJCONTAINER;

typedef struct BHVNODE_TYP
{
	int state;
	int attr;
	CPoint4D pos;   //����
	CVector4 radius;  //��x,y,z�뾶
	int numObjects;   //����������
	LPOBJCONTAINER objects[MAX_OBJECTS_PER_BHV_NODE];  //��������
	int numChildren;  //�ӽڵ���

	BHVNODE_TYP* links[MAX_BHV_PER_NODE];  //ָ���ӽڵ��ָ������
}BHVNODE,*LPBHVNODE;

typedef struct BHVCELL_TYP
{
	int numObjects;  //�������Ԫ�����ж�������
	LPOBJCONTAINER objList[MAX_OBJECTS_PER_BHV_CELL];  //��Ԫ���ڴ洢������
}BHVCELL,*LPBHVCELL;


void BHVTreeSetUp(LPBHVNODE bhvTree,   //������bhv��
				  LPOBJCONTAINER bhvObjects,  //ָ�򳡾��е����������ָ��
				  int numObjects,    //�������������Ŀ
				  int level,    //bhv�����
				  int numDivisions, //��ÿ���Ὣ�ڵ�ָ�ɶ��ٷ�
				  int universeRadius  //��Ϸ����Ĵ�С
				  );

void ResetBHVTree(LPBHVNODE bhvTree);