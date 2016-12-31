#pragma once
#include "datatype.h"
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "Vector4.h"
#include "Vector2.h"

typedef struct MD2_POINT_TYP
{
	unsigned char v[3];       //x,y,z 坐标
	unsigned char indexNormal;  //法线索引
}MD2POINT,*LPMD2POINT;

typedef struct MD2_TEXTCOORD_TYP
{
	short u,v;
}MD2TEXTCOORD,*LPMD2TEXTCOORD;   //纹理坐标

typedef struct MD2_FRAME_TYP
{
	float scale[3];
	float translate[3];
	char name[16];
	MD2POINT vlist[1];
}MD2FRAME,*LPMD2FRAME;

typedef struct MD2_TRI_TYP
{
	unsigned short vindex[3];
	unsigned short tindex[3];
}MD2TRI,*LPMD2TRI;

typedef struct MD2_ANIM_TYP
{
	int startFrame;
	int endFrame;
	float irate;
	int animSpeed;
}MD2ANIM,*LPMD2ANIM;


typedef struct MD2_HEADER_TYP
{
	int identifier;  //指出文件类型 IDP2
	int version;     //版本号 8
	int skinWidth;   //用作皮肤纹理的宽度
	int skinHeight;  //用作皮肤纹理的高度
	int frameSize;   //每个动画帧的大小，单位为字节
	int numSkins;    //总皮肤数
	int numVerts;    //每帧中的顶点数，所有帧中的顶点数相同
	int numTextcoords;   //整个文件中的纹理顶点数
	int numTris;         //每个动画帧中的多边形数
	int numOPENGLcmds;   //opengl命令参数
	int numFrames;       //动画帧数

	int offsetSkins;              //皮肤数组离文件开头的距离，单位为字节
	int offsetTextcoords;         //纹理坐标数组离文件开头的距离
	int offsetTris;               //多边形列表离文件开头的距离
	int offsetFrames;             //顶点数组离文件开头的距离
	int offsetOPENGLcmds;         //opengl命令参数离文件开头的距离
	int offsetend;                //文件开头到文件末尾的距离
}MD2HEADER,*LPMD2HEADER;

class CMD2Container
{
public:
	CMD2Container(void);
	~CMD2Container(void);
public:
	BOOL LoadMD2Object(char* modelFile,CVector4& scale,CVector4& pos,CVector4& rot,
		char* textureFile,int attr,int color,int VertexFlag);
	void SetAnimation(int animState,int animMode=MD2_ANIM_LOOP);
	void DoAnimation();
public:
	int state;
	int attr;
	int color;   //没有纹理时的颜色
	int numFrames;  //帧数
	int numTriangles;   //多边形数
	int numVerts;       //顶点数
	int numTextcoords;  //纹理坐标数
	CBitmapImage* skin;  //纹理指针
	LPMD2TRI plist;      //指向多边形列表的指针
	CPoint4D* vlist;     //指向顶点数组的指针
	CPoint2D* textlist;     //指向纹理数组的指针
	CPoint4D worldPos;   //物体的世界坐标
	CPoint4D vel;        //物体的速度
	
	int ivars[8];        //整形变量
	float fvars[8];      //浮点型变量
	int counters[8];     //通用技术器
	void* link;          //通用void型指针

	int animState;       //动作状态
	int animCounter;     //动作计数
	int animMode;        //播放一次还是循环播放
	int animSpeed;       //播放速度，值越小播放越快
	int animComplete;    //是否播放完毕
	float currFrame;     //当前帧

};
