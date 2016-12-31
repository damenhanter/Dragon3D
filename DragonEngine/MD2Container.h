#pragma once
#include "datatype.h"
#include "BitmapFile.h"
#include "BitmapImage.h"
#include "Vector4.h"
#include "Vector2.h"

typedef struct MD2_POINT_TYP
{
	unsigned char v[3];       //x,y,z ����
	unsigned char indexNormal;  //��������
}MD2POINT,*LPMD2POINT;

typedef struct MD2_TEXTCOORD_TYP
{
	short u,v;
}MD2TEXTCOORD,*LPMD2TEXTCOORD;   //��������

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
	int identifier;  //ָ���ļ����� IDP2
	int version;     //�汾�� 8
	int skinWidth;   //����Ƥ������Ŀ��
	int skinHeight;  //����Ƥ������ĸ߶�
	int frameSize;   //ÿ������֡�Ĵ�С����λΪ�ֽ�
	int numSkins;    //��Ƥ����
	int numVerts;    //ÿ֡�еĶ�����������֡�еĶ�������ͬ
	int numTextcoords;   //�����ļ��е���������
	int numTris;         //ÿ������֡�еĶ������
	int numOPENGLcmds;   //opengl�������
	int numFrames;       //����֡��

	int offsetSkins;              //Ƥ���������ļ���ͷ�ľ��룬��λΪ�ֽ�
	int offsetTextcoords;         //���������������ļ���ͷ�ľ���
	int offsetTris;               //������б����ļ���ͷ�ľ���
	int offsetFrames;             //�����������ļ���ͷ�ľ���
	int offsetOPENGLcmds;         //opengl����������ļ���ͷ�ľ���
	int offsetend;                //�ļ���ͷ���ļ�ĩβ�ľ���
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
	int color;   //û������ʱ����ɫ
	int numFrames;  //֡��
	int numTriangles;   //�������
	int numVerts;       //������
	int numTextcoords;  //����������
	CBitmapImage* skin;  //����ָ��
	LPMD2TRI plist;      //ָ�������б��ָ��
	CPoint4D* vlist;     //ָ�򶥵������ָ��
	CPoint2D* textlist;     //ָ�����������ָ��
	CPoint4D worldPos;   //�������������
	CPoint4D vel;        //������ٶ�
	
	int ivars[8];        //���α���
	float fvars[8];      //�����ͱ���
	int counters[8];     //ͨ�ü�����
	void* link;          //ͨ��void��ָ��

	int animState;       //����״̬
	int animCounter;     //��������
	int animMode;        //����һ�λ���ѭ������
	int animSpeed;       //�����ٶȣ�ֵԽС����Խ��
	int animComplete;    //�Ƿ񲥷����
	float currFrame;     //��ǰ֡

};
