#pragma once
#include "datatype.h"
#include "material.h"
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MD2Container.h"

typedef struct VERTEX_TYPE
{
	VERTEX_TYPE() { memset(this, 0, sizeof(VERTEX_TYPE)); }
	~VERTEX_TYPE() {}

	VERTEX_TYPE & operator =(const VERTEX_TYPE & it)
	{
		memcpy(M, it.M, sizeof(VERTEX_TYPE));
		return *this;
	}

	union
	{
		struct
		{
			float x,y,z,w;
			float nx,ny,nz,nw;
			float u0,v0;       //纹理坐标

			float i;   //最终顶点亮度
			int attr;
		};
		float M[12];
		struct
		{
			CPoint4D v;  //顶点
			CVector4 n;  //向量Gouraud着色需要知道顶点法线
			CPoint2D t;  //纹理坐标
			float i;
			int attr;
		};
	};
}VERTEX,*LPVERTEX;

typedef struct Tri4DV1_TYPE
{
	int state;     //状态信息
	int attr;       //物理属性
	int color;      //颜色

	int litColor[3];  //存储光照后的颜色，flat着色存储在数组第一个值，gouraud着色，数组存储顶点值
	CBitmapImage* ptrTexture;
	int mati;       //材质索引

	LPVERTEX vlist;   // 顶点列表
	CPoint2D* textlist; //纹理坐标顶点数组 
	int vert[3];       //顶点索引
	int text[3];       //纹理顶点索引
	float nlength;  //法线长度
}Tri4DV1,*LPTri4DV1;

class CCOBModel
{
public:
	CCOBModel(void);
	~CCOBModel(void);

	int InitModel(int numVertices,int numTriangles,int numFrames);
	void ComputeOBJModel4DV1Radius();
	void SetFrame(int frame);
	void ComputeOBJTriLength();
	void ComputeOBJVertexNormal();
	void LoadModelFromFile(char* filename,CVector4& scale,
		CVector4& pos,CVector4& rotate,int VertexFlags=false);
	int CreateMipmaps(CBitmapImage* source,CBitmapImage** mipmaps,float gamma);
	void DeleteMipMaps(CBitmapImage** mipmaps,int level0=true);

	void PrepareOBJForMD2(CMD2Container* md2Obj);
	void ExtractMD2Frame(CMD2Container* md2Obj);
public:
	int id;			//物体的数字ID
	char name[256]; //物体的字符名称 
	int state;
	int attr;
	int mati;

	float* avgRadius;   //平均半径
	float* maxRadius;   //最大半径
	//物体包含多帧，平均半径，最大半径随不同的帧不同而不同

	CPoint4D worldPos;  //世界坐标

	CVector4 dir;      //物体在局部坐标系中的旋转角度

	CVector4 ux,uy,uz; //在局部坐标系中的超向

	int numVertices;     //物体每帧的顶点数
	int totalVertices;   //物体总共顶点数
	int numFrames;     //总共有多少帧
	int currFrame;     //当前帧

	LPVERTEX vlocalList;  //存储顶点局部坐标的数组,指向活动帧的网格顶点指针
	LPVERTEX vtranslist;   //存储变换后的顶点坐标数组,指向活动帧的网格顶点指针（变换后的版本）

	LPVERTEX vlocalListHead;
	LPVERTEX vtranslistHead;

	CPoint2D* textlist;  //纹理坐标列表
	CBitmapImage* ptrTexture;

	int nTriangles;       //物体网格的多边形数
	LPTri4DV1 plist;    //指向多边形指针

	int numMaterials;
	CMaterial Materials[MAX_MATERIALS];
};
