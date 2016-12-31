#pragma once
#include "datatype.h"
#include "RenderSystem.h"
#include "BitmapFile.h"
#include "BitmapImage.h"

class CAnimation
{
public:
	CAnimation(void);
	~CAnimation(void);
public:
	int CreateAnimation(CRenderSystem* render,float x, float y, int width, int height, int numFrames, int attr, 
		int memFlags, USHORT colorKeyValue, int bpp);
	int CloneAnim(CAnimation* lpSourceAnim);
	int RenderAnimation(CRenderSystem* render,LPDIRECTDRAWSURFACE7 destSurface);
	int RenderScaleAnim(CRenderSystem* render,int swidth, int sheight, LPDIRECTDRAWSURFACE7 destSurface);
	int LoadAnimFrame(CBitmapFile* bitmap, int frame, int cx, int cy, int mode);
	int LoadAnimFrame16(CBitmapFile* bitmap, int frame, int cx, int cy, int mode);
	int DoAnimation(void);
	int Move(void);
	int LoadAnimation(int anim, int numFrames, int* sequence);
	int SetPos(float x, float y);
	int SetAnimSpeed(int speed);
	int SetAnimation(int anim);
	int SetVelocity(float xv, float yv);
	int Hide(void);
	int Show(void);
	int CollisionCheck(CAnimation* anim);
	int Release(void);
public:
	int state;         //对象状态
	int animState;    //动画状态
	int attr;          //对象属性
	float x,y;         //位图显示的位置
	int bpp;
	float xv,yv;       //对象速度
	int width,height;  //对象宽高
	int counter1;
	int counter2;
	int maxCounter1;
	int maxCounter2;
	int varsI[16];      //16位整数栈
	float varsF[16];    //16位浮点数栈
	int currFrame;      //当前帧
	int numFrames;      //动画总帧数
	int currAnimation;   //当前动画索引
	int animCounter;    //用来计算动画转变
	int animIndex;      //动画元素索引
	int animCountMax;   
	int* animations[ANIM_MAX_ANIMATIONS];  //动画次序
public:
	LPDIRECTDRAWSURFACE7 images[ANIM_MAX_FRAMES];   //承载位图的表面

};
