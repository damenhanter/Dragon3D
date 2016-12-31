#include "StdAfx.h"
#include "Animation.h"

extern HWND hWnd;
extern RECT g_rRectClip;
CAnimation::CAnimation(void)
{

}

CAnimation::~CAnimation(void)
{
	Release();
}
/*x,y Animation位置，width,height,Animation宽度高度
numFrames动画帧的数量,attr 动画属性,memFlags内存标志
colorKeyValue 颜色键
*/
int CAnimation::CreateAnimation(CRenderSystem* render,float x, float y, int width, int height, int numFrames, int attr,
								int memFlags, USHORT colorKeyValue, int bpp)
{
	state=ANIM_STATE_ALIVE;
	this->attr=attr;
	this->animState=0;
	this->counter1=0;
	this->counter2=0;
	this->maxCounter1=0;
	this->maxCounter2=0;
	this->currFrame=0;
	this->numFrames=numFrames;
	this->bpp=bpp;
	this->currAnimation=0;
	this->animCounter=0;
	this->animIndex=0;
	this->animCountMax=0;
	this->x=x;
	this->y=y;
	this->xv=0;
	this->yv=0;
	this->width=width;
	this->height=height;
	for(int index=0; index<ANIM_MAX_FRAMES; index++)
	{
		images[index]=NULL;
	}
	for(int index=0; index<ANIM_MAX_ANIMATIONS; index++)
	{
		animations[index]=NULL;
	}
	//创建各离屏表面
	for(int index=0; index<numFrames; index++)
	{
		images[index]=render->CreateSurface(width,height,memFlags,colorKeyValue);
	}
	return 0;
}

int CAnimation::CloneAnim(CAnimation* lpSourceAnim)
{
	if((lpSourceAnim)||(lpSourceAnim!=this))
	{
		memcpy(this,lpSourceAnim,sizeof(CAnimation));
		this->attr|=ANIM_ATTR_CLONE;
	}
	else
		return FALSE;	
	return TRUE;
}

int CAnimation::RenderAnimation(CRenderSystem* render,LPDIRECTDRAWSURFACE7 destSurface)
{
	if(!(attr & ANIM_ATTR_VISIBLE))
		return FALSE;
	int ret=render->RenderRectangle(0,0,width,height,(int)x,(int)y,(int)x+width,(int)y+height,images[currFrame],destSurface);
	if(!ret)
	{
		MessageBox(hWnd,"RenderAnimation失败！","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}

int CAnimation::RenderScaleAnim(CRenderSystem* render,int swidth, int sheight, LPDIRECTDRAWSURFACE7 destSurface)
{
	if(!(attr & ANIM_ATTR_VISIBLE))
		return FALSE;
	int ret=render->RenderRectangle(0,0,width,height,(int)x,(int)y,(int)x+swidth,(int)y+sheight,images[currFrame],destSurface);
	if(!ret)
	{
		MessageBox(hWnd,"RenderAnimation失败！","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}

int CAnimation::LoadAnimFrame(CBitmapFile* bitmap, int frame, int cx, int cy, int mode)
{
	CBitmapImage image;
	image.CreateBitmapImage(0,0,width,height,bpp);
	image.LoadImageBitmap(*bitmap,cx,cy,mode);
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if(FAILED(images[frame]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL)))
	{
		MessageBox(hWnd,"LoadAnimFrame--->所表面失败！","error",MB_OK);
		return FALSE;
	}
	int lpitch=ddsd.lPitch;
	image.RenderBitmapImage((UCHAR*)ddsd.lpSurface,lpitch,0);
	images[frame]->Unlock(NULL);
	attr|=ANIM_ATTR_LOADED;
	return TRUE;
}
int CAnimation::LoadAnimFrame16(CBitmapFile* bitmap, int frame, int cx, int cy, int mode)
{
	CBitmapImage image;
	image.CreateBitmapImage(0,0,width,height,bpp);
	image.LoadImageBitmap16(*bitmap,cx,cy,mode);
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	
	if(FAILED(images[frame]->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL)))
	{
		MessageBox(hWnd,"LoadAnimFrame--->所表面失败！","error",MB_OK);
		return FALSE;
	}
	int lpitch=ddsd.lPitch;
	image.RenderBitmapImage16((UCHAR*)ddsd.lpSurface,lpitch,0);
	images[frame]->Unlock(NULL);
	attr|=ANIM_ATTR_LOADED;
	return TRUE;
}

int CAnimation::DoAnimation(void)
{
	if(attr & ANIM_ATTR_SINGLE_FRAME)
	{
		currFrame=0;
		return TRUE;
	}
	else if(attr & ANIM_ATTR_MULTI_FRAME)
	{
		if(++animCounter>=animCountMax)
		{
			animCounter=0;
			if(++currFrame>=numFrames)
				currFrame=0;
		}
	}
	else if(attr & ANIM_ATTR_MULTI_ANIM)
	{
		if(++animCounter>=animCountMax)
		{
			animCounter=0;
			animIndex++;
			currFrame=animations[currAnimation][animIndex];
			if(currFrame==-1)
			{
				if(attr & ANIM_ATTR_ANIM_ONE_SHOT)
				{
					animState=ANIM_STATE_ANIM_DONE;
					animIndex--;
					currFrame=animations[currAnimation][animIndex];
				}
				else
				{
					animIndex=0;
					currFrame=animations[currAnimation][animIndex];
				}
			}
		}
	}
	return TRUE;
}

int CAnimation::Move(void)
{
	x+=xv;y+=yv;
	int x0,y0;
	x0=(int)x;y0=(int)y;
	if(attr & ANIM_ATTR_WRAPAROUND)
	{
		if(x0>g_rRectClip.right)
			x0=g_rRectClip.left-width;
		else if(x0<g_rRectClip.left-width)
			x0=g_rRectClip.right;
		if(y0>g_rRectClip.bottom)
			y0=(g_rRectClip.top-height);
		else if(y0<g_rRectClip.top-height)
			y0=g_rRectClip.bottom;
	}
	else if(attr & ANIM_ATTR_BOUNCE)
	{
		if(x0>=g_rRectClip.right-width||x0<g_rRectClip.left)
			xv=-xv;
		if(y0>=g_rRectClip.bottom-height||y0<g_rRectClip.top)
			yv=-yv;
	}
	return TRUE;
}

int CAnimation::LoadAnimation(int anim, int numFrames, int* sequence)
{
	animations[anim]=(int*)malloc((numFrames+1)*sizeof(int));
	int i=0;
	for(i=0; i<numFrames; i++)
		animations[anim][i]=sequence[i];
	animations[anim][i]=-1;
	return TRUE;
}

int CAnimation::SetPos(float x, float y)
{
	this->x=x;
	this->y=y;
	return TRUE;
}

int CAnimation::SetAnimSpeed(int speed)
{
	animCountMax=speed;
	return 0;	
}

int CAnimation::SetAnimation(int anim)
{
	currAnimation=anim;
	this->animIndex=0;
	return TRUE;
}

int CAnimation::SetVelocity(float xv, float yv)
{
	this->xv=xv;
	this->yv=yv;
	return TRUE;
}

int CAnimation::Hide(void)
{
	RESET_BIT(attr,ANIM_ATTR_VISIBLE);
	return true;
}

int CAnimation::Show(void)
{
	SET_BIT(attr,ANIM_ATTR_VISIBLE);
	return TRUE;
}

int CAnimation::CollisionCheck(CAnimation* anim)
{
	int width1=(width>>1)-(width>>3);
	int height1=(height>>1)-(height>>3);
	int width2=(anim->width>>1)-(anim->width>>3);
	int height2=(anim->height>>1)-(anim->height>>3);
	int dx=(int)x+width1;
	int dy=(int)y+height1;
	int dx1=(int)anim->x+width2;
	int dy1=(int)anim->y+height2;
	int dz=dx1-dx;
	int dz1=dy1-dy;
	if((width1+width2)<dz && (height1+height2)<dz1)
		return FALSE;
	return TRUE;
}

int CAnimation::Release(void)
{
	if(attr & ANIM_ATTR_CLONE)
	{
		for(int index=0; index<ANIM_MAX_FRAMES; index++)
		{
			if(images[index])
			{
				images[index]=NULL;
			}
		}
		for(int index=0; index<ANIM_MAX_ANIMATIONS; index++)
		{
			if(animations[index])
			{
				animations[index]=NULL;
			}
		}
	}
	else
	{
		for(int index=0; index<ANIM_MAX_FRAMES; index++)
		{
			if(images[index])
			{
				images[index]->Release();
				images[index]=NULL;
			}
		}
		for(int index=0; index<ANIM_MAX_ANIMATIONS; index++)
		{
			if(animations[index])
			{
				free(animations[index]);			
				animations[index]=NULL;
			}
		}
	}
	return TRUE;
}
