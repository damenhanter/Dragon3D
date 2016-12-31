#pragma once

#include "datatype.h"
#include "Vector4.h"
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "COBModel.h"

class CTerrain
{
public:
	CTerrain();
	~CTerrain();

	int GenerateTerrain(float twidth,float theight,float vscale,
		CPoint4D& pos,char* heightFieldFile,char* textMapFile,int rgbcolor,int Triattr);
public:
	void InitTerrain(int nVertices,int nTriangles);
	void ComputeOBJTriLength();
	void ComputeOBJVertexNormal();
public:
	int id;       //物体的数字ID
	char name[256]; //物体的字符名称 
	int state;
	int attr;
	int mati;
	
	int ivar1;
	int ivar2;
	float fvar1;
	float fvar2;

	CPoint4D worldPos;  //世界坐标

	CVector4 dir;      //物体在局部坐标系中的旋转角度

	CVector4 ux,uy,uz; //在局部坐标系中的超向

	LPVERTEX vlocalList;  //存储顶点局部坐标的数组,指向活动帧的网格顶点指针
	LPVERTEX vtranslist;   //存储变换后的顶点坐标数组,指向活动帧的网格顶点指针（变换后的版本）

	CPoint2D* textlist;  //纹理坐标列表
	CBitmapImage* ptrTexture;

	int nVertices;
	int nTriangles;       //物体网格的多边形数
	LPTri4DV1 plist;    //指向多边形指针
};
