#include "StdAfx.h"
#include "Triangle2D.h"
#include "FIXP16.h"
#include "ErrorLog.h"

#pragma warning (disable:4018) 

extern RECT g_rRectClip;
extern CErrorLog errorlog;
extern USHORT AlphaTable[NUM_ALPHA_LEVELS][65535];
extern UCHAR gLog2[513];

CTriangle2D::CTriangle2D(void)
: state(0)
, x0(0)
, y0(0)
, Vx(0)
, Vy(0)
{
	
}

CTriangle2D::CTriangle2D(int state, int x0, int y0, int Vx, int Vy)
{
	this->state=state;
	this->x0=x0;
	this->y0=y0;
	this->Vx=Vx;
	this->Vy=Vy;
}
CTriangle2D::~CTriangle2D(void)
{
}


CTriangle2D::CTriangle2D(CTriangle2D& t)
{
	state=t.state;
	x0=t.x0;y0=t.y0;
	Vx=t.Vx;Vy=t.Vy;
}


CTriangle2D& CTriangle2D::operator=(CTriangle2D& t)
{
	//TODO: insert return statement here
	state=t.state;
	x0=t.x0;y0=t.y0;
	Vx=t.Vx;Vy=t.Vy;
	return *this;
}


void CTriangle2D::DrawTopTri(int x1, int y1, int x2, int y2, int x3, int y3,
						   int color,UCHAR* destbuffer,int lpitch)
{	
	float dxright,dxleft,xs,xe,height;
	int right,left;
	UCHAR* destaddr=NULL;
	if(x2<x1)
	{
		swap(x1,x2);
	}
	height=(float)(y3-y1);
	dxleft=(x3-x1)/height;
	dxright=(x3-x2)/height;
	xs=(float)x1;
	xe=(float)x2+(float)0.5;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(float)(g_rRectClip.top-y1);
		xe=xe+dxright*(float)(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			memset((UCHAR*)destaddr+(unsigned int)xs,color,(unsigned int)(xe-xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(int)xs;
			right=(int)xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			memset((UCHAR*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawBottomTri(int x1, int y1, int x2, int y2, int x3, int y3, 
							  int color, UCHAR* destbuffer, int lpitch)
{
	float dxright,dxleft,xs,xe,height;
	int right,left;
	UCHAR* destaddr=NULL;
	if(x3<x2)
	{
		swap(x3,x2);
	}
	height=(float)(y3-y1);
	dxleft=(x2-x1)/height;
	dxright=(x3-x1)/height;
	xs=(float)x1;
	xe=(float)x1;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(float)(g_rRectClip.top-y1);
		xe=xe+dxright*(float)(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			memset((UCHAR*)destaddr+(unsigned int)xs,color,(unsigned int)(xe-xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(int)xs;
			right=(int)xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right)
			{
				right=g_rRectClip.right;
				if(left>g_rRectClip.right)
					continue;
			}
			memset((UCHAR*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawTriangle2D(int x1, int y1, int x2, int y2, int x3, int y3,
							 int color , UCHAR* destbuffer, int lpitch)
{
	int newx;
	if((x1==x2 && x2==x3)||(y1==y2 && y2==y3))
		return ;
	if(y2<y1)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(y3<y1)
	{
		swap(y1,y3);
		swap(x1,x3);
	}
	if(y3<y2)
	{
		swap(x3,x2);
		swap(y3,y2);
	}
	if((y3<g_rRectClip.top) || (y1>g_rRectClip.bottom-1)
		|| (x1<g_rRectClip.left && x2<g_rRectClip.left && x3<g_rRectClip.left)
		|| (x1>g_rRectClip.right-1 && x2>g_rRectClip.right-1 && x3>g_rRectClip.right-1))
		return ;
	if(y1==y2)
	{
		DrawTopTri(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else if(y2==y3)
	{
		DrawBottomTri(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else
	{
		newx=x1+(int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));
		DrawBottomTri(x1,y1,newx,y2,x2,y2,color,destbuffer,lpitch);
		DrawTopTri(x2,y2,newx,y2,x3,y3,color,destbuffer,lpitch);
	}
}
void CTriangle2D::DrawTopTri16(int x1, int y1, int x2, int y2, int x3, int y3,int color,UCHAR* destbuffer,int lpitch)
{
	float dxright,dxleft,xs,xe,height;
	int right,left;
	USHORT* destaddr=NULL;
	USHORT* destbuffer16=(USHORT*)destbuffer;
	lpitch>>=1;
	if(x2<x1)
	{
		swap(x1,x2);
	}
	height=(float)(y3-y1);
	dxleft=(x3-x1)/height;
	dxright=(x3-x2)/height;
	xs=(float)x1;
	xe=(float)x2+(float)0.5;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(float)(g_rRectClip.top-y1);
		xe=xe+dxright*(float)(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer16+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			MemSetWORD((USHORT*)destaddr+(unsigned int)xs,color,(unsigned int)(xe-xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(int)xs;
			right=(int)xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}



void CTriangle2D::DrawBottomTri16(int x1, int y1, int x2, int y2, int x3, int y3, int color, UCHAR* destbuffer, int lpitch)
{
	float dxright,dxleft,xs,xe,height;
	int right,left;
	USHORT* destaddr=NULL;
	USHORT* destbuffer16=(USHORT*)destbuffer;
	lpitch>>=1;
	if(x3<x2)
	{
		swap(x3,x2);
	}
	height=(float)(y3-y1);
	dxleft=(x2-x1)/height;
	dxright=(x3-x1)/height;
	xs=(float)x1;
	xe=(float)x1;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(float)(g_rRectClip.top-y1);
		xe=xe+dxright*(float)(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer16+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			MemSetWORD((USHORT*)destaddr+(unsigned int)xs,color,(unsigned int)(xe-xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(int)xs;
			right=(int)xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawTriangle162D(int x1, int y1, int x2, int y2, int x3, int y3, int color , UCHAR* destbuffer, int lpitch)
{
	int newx;
	if((x1==x2 && x2==x3)||(y1==y2 && y2==y3))
		return ;
	if(y2<y1)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(y3<y1)
	{
		swap(y1,y3);
		swap(x1,x3);
	}
	if(y3<y2)
	{
		swap(x3,x2);
		swap(y3,y2);
	}
	if((y3<g_rRectClip.top) || (y1>g_rRectClip.bottom-1)
		|| (x1<g_rRectClip.left && x2<g_rRectClip.left && x3<g_rRectClip.left)
		|| (x1>g_rRectClip.right-1 && x2>g_rRectClip.right-1 && x3>g_rRectClip.right-1))
		return ;
	if(y1==y2)
	{
		DrawTopTri16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else if(y2==y3)
	{
		DrawBottomTri16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else
	{
		newx=x1+(int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));
		DrawBottomTri16(x1,y1,newx,y2,x2,y2,color,destbuffer,lpitch);
		DrawTopTri16(x2,y2,newx,y2,x3,y3,color,destbuffer,lpitch);
	}
}

void CTriangle2D::DrawTopTri16(float x1, float y1, float x2, float y2, float x3, float y3,int color,UCHAR* destbuffer,int lpitch)
{
	float dxright,dxleft,xs,xe,height,left,right;
	int iy1,iy3;
	USHORT* destaddr=NULL;
	USHORT* destbuffer16=(USHORT*)destbuffer;
	lpitch>>=1;
	if(x2<x1)
	{
		swap(x1,x2);
	}
	height=(y3-y1);
	dxleft=(x3-x1)/height;
	dxright=(x3-x2)/height;
	xs=x1;
	xe=x2;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
		iy1=y1;
	}
	else
	{
		iy1=ceil(y1);
		xs=xs+dxleft*(iy1-y1);
		xe=xe+dxright*(iy1-y1);
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
		iy3=y3-1;
	}
	else
	{
		iy3=ceil(y3)-1;
	}

	destaddr=destbuffer16+iy1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=iy1; i<=iy3; i++,destaddr+=lpitch)
		{
			//errorlog.WriteError("平顶(%f,%f,%f,%f,%f)\n",xs,xe,(xe-xs),dxright,height);
			MemSetWORD((USHORT*)destaddr+(unsigned int)xs,color,(unsigned int)((int)xe-(int)xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=iy1; i<=iy3; i++,destaddr+=lpitch)
		{
			left=xs;
			right=xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right-1)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)((int)right-(int)left+1));
		}
	}
}

void CTriangle2D::DrawBottomTri16(float x1, float y1, float x2, float y2, float x3, float y3, int color, UCHAR* destbuffer, int lpitch)
{
	float dxright,dxleft,xs,xe,height,left,right;
	int iy1,iy3;
	USHORT* destaddr=NULL;
	USHORT* destbuffer16=(USHORT*)destbuffer;
	lpitch>>=1;
	if(x3<x2)
	{
		swap(x3,x2);
	}
	height=(y3-y1);
	dxleft=(x2-x1)/height;
	dxright=(x3-x1)/height;
	xs=x1;
	xe=x1;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
		iy1=y1;
	}
	else
	{
		iy1=ceil(y1);
		xs=xs+dxleft*(iy1-y1);
		xe=xe+dxright*(iy1-y1);
	}
	

	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
		iy3=y3-1;
	}
	else
	{
		iy3=ceil(y3)-1;
	}

	destaddr=destbuffer16+iy1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right-1 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=iy1; i<=iy3; i++,destaddr+=lpitch)
		{
			//errorlog.WriteError("平底(%f,%f,%f,%f)\n",xs,xe,(xe-xs),dxright);
			MemSetWORD((USHORT*)destaddr+(unsigned int)xs,color,(unsigned int)((int)xe-(int)xs+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=iy1; i<=iy3; i++,destaddr+=lpitch)
		{
			left=xs;
			right=xe;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right-1)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)((int)right-(int)left+1));
		}
	}
}

void CTriangle2D::DrawTriangle162D(float x1, float y1, float x2, float y2, float x3, float y3, int color , UCHAR* destbuffer, int lpitch)
{
	float newx;
	if(((FCMP(x1,x2)) && (FCMP(x2,x3)))||(FCMP(y1,y3) && FCMP(y2,y3)))
		return ;
	if(y2<y1)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(y3<y1)
	{
		swap(y1,y3);
		swap(x1,x3);
	}
	if(y3<y2)
	{
		swap(x3,x2);
		swap(y3,y2);
	}
	if((y3<g_rRectClip.top) || (y1>g_rRectClip.bottom-1)
		|| (x1<g_rRectClip.left && x2<g_rRectClip.left && x3<g_rRectClip.left)
		|| (x1>g_rRectClip.right-1 && x2>g_rRectClip.right-1 && x3>g_rRectClip.right-1))
		return ;
	if(FCMP(y1,y2))
	{
		DrawTopTri16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else if(FCMP(y2,y3))
	{
		DrawBottomTri16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else
	{
		newx=x1+(y2-y1)*(x3-x1)/(y3-y1);
		//errorlog.WriteError("%f\n",newx);
		DrawBottomTri16(x1,y1,newx,y2,x2,y2,color,destbuffer,lpitch);
		DrawTopTri16(x2,y2,newx,y2,x3,y3,color,destbuffer,lpitch);
	}
}

void CTriangle2D::DrawTopTriFP(int x1, int y1, int x2, int y2, int x3, int y3,
						   int color,UCHAR* destbuffer,int lpitch)
{	
	int dxright,dxleft,xs,xe,height;
	int right,left;
	UCHAR* destaddr=NULL;
	if(x2<x1)
	{
		swap(x1,x2);
	}
	height=(y3-y1);
	dxleft=((x3-x1)<<FIXP16_SHIFT)/height;
	dxright=((x3-x2)<<FIXP16_SHIFT)/height;
	xs=x1<<FIXP16_SHIFT;
	xe=x2<<FIXP16_SHIFT;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			memset((UCHAR*)destaddr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
				color,(((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			right=(xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			memset((UCHAR*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawBottomTriFP(int x1, int y1, int x2, int y2, int x3, int y3, 
							  int color, UCHAR* destbuffer, int lpitch)
{
	int dxright,dxleft,xs,xe,height;
	int right,left;
	UCHAR* destaddr=NULL;
	if(x3<x2)
	{
		swap(x3,x2);
	}
	height=(y3-y1);
	dxleft=((x2-x1)<<FIXP16_SHIFT)/height;
	dxright=((x3-x1)<<FIXP16_SHIFT)/height;
	xs=x1<<FIXP16_SHIFT;
	xe=x1<<FIXP16_SHIFT;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			memset((UCHAR*)destaddr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
				color,(((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			right=(xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			memset((UCHAR*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawTriangleFP2D(int x1, int y1, int x2, int y2, int x3, int y3,
							 int color , UCHAR* destbuffer, int lpitch)
{
	int newx;
	if((x1==x2 && x2==x3)||(y1==y2 && y2==y3))
		return ;
	if(y2<y1)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(y3<y1)
	{
		swap(y1,y3);
		swap(x1,x3);
	}
	if(y3<y2)
	{
		swap(x3,x2);
		swap(y3,y2);
	}
	if(y3<g_rRectClip.top || y1>g_rRectClip.bottom-1
		|| (x1<g_rRectClip.left && x2<g_rRectClip.left && x3<g_rRectClip.left)
		|| (x1>g_rRectClip.right-1 && x2>g_rRectClip.right-1 && x3>g_rRectClip.right-1))
		return ;
	if(y1==y2)
	{
		DrawTopTriFP(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else if(y2==y3)
	{
		DrawBottomTriFP(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else
	{
		newx=x1+(int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));
		DrawBottomTriFP(x1,y1,newx,y2,x2,y2,color,destbuffer,lpitch);
		DrawTopTriFP(x2,y2,newx,y2,x3,y3,color,destbuffer,lpitch);
	}
}
void CTriangle2D::DrawTopTriFP16(int x1, int y1, int x2, int y2, int x3, int y3,
						   int color,UCHAR* destbuffer,int lpitch)
{	
	int dxright,dxleft,xs,xe,height;
	int right,left;
	USHORT* destaddr=NULL;
	USHORT* destbuffer16=(USHORT*)destbuffer;
	lpitch>>=1;
	if(x2<x1)
	{
		swap(x1,x2);
	}
	height=(y3-y1);
	dxleft=((x3-x1)<<FIXP16_SHIFT)/height;
	dxright=((x3-x2)<<FIXP16_SHIFT)/height;
	xs=x1<<FIXP16_SHIFT;
	xe=x2<<FIXP16_SHIFT;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=destbuffer16+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right-1
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right-1)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			MemSetWORD((USHORT*)destaddr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
				color,(((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			right=(xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right-1)
			{
				right=g_rRectClip.right-1;
				if(left>g_rRectClip.right)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawBottomTriFP16(int x1, int y1, int x2, int y2, int x3, int y3, 
							  int color, UCHAR* destbuffer, int lpitch)
{
	int dxright,dxleft,xs,xe,height;
	int right,left;
	USHORT* destaddr=NULL;
	lpitch>>=1;
	if(x3<x2)
	{
		swap(x3,x2);
	}
	height=(y3-y1);
	dxleft=((x2-x1)<<FIXP16_SHIFT)/height;
	dxright=((x3-x1)<<FIXP16_SHIFT)/height;
	xs=x1<<FIXP16_SHIFT;
	xe=x1<<FIXP16_SHIFT;
	//对y进行裁剪
	if(y1<g_rRectClip.top)
	{
		xs=xs+dxleft*(g_rRectClip.top-y1);
		xe=xe+dxright*(g_rRectClip.top-y1);
		y1=g_rRectClip.top;
	}
	if(y3>g_rRectClip.bottom-1)
	{
		y3=g_rRectClip.bottom-1;
	}
	destaddr=(USHORT*)destbuffer+y1*lpitch;
	//测试x是否需要裁剪
	if(x1>=g_rRectClip.left && x1<=g_rRectClip.right 
		&& x2>=g_rRectClip.left && x2<=g_rRectClip.right
		&& x3>=g_rRectClip.left && x3<=g_rRectClip.right)
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			MemSetWORD((USHORT*)destaddr+((xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT),
				color,(((xe-xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT)+1));
			xs+=dxleft;
			xe+=dxright;
		}
	}
	else
	{
		for(int i=y1; i<=y3; i++,destaddr+=lpitch)
		{
			left=(xs+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			right=(xe+FIXP16_ROUND_UP)>>FIXP16_SHIFT;
			xs+=dxleft;
			xe+=dxright;
			if(left<g_rRectClip.left)
			{
				left=g_rRectClip.left;
				if(right<g_rRectClip.left)
					continue;
			}
			if(right>g_rRectClip.right)
			{
				right=g_rRectClip.right;
				if(left>g_rRectClip.right)
					continue;
			}
			MemSetWORD((USHORT*)destaddr+(unsigned int)left,color,(unsigned int)(right-left+1));
		}
	}
}

void CTriangle2D::DrawTriangleFP162D(int x1, int y1, int x2, int y2, int x3, int y3,
									 int color , UCHAR* destbuffer, int lpitch)
{
	int newx;
	if((x1==x2 && x2==x3)||(y1==y2 && y2==y3))
		return ;
	if(y2<y1)
	{
		swap(x1,x2);
		swap(y1,y2);
	}
	if(y3<y1)
	{
		swap(y1,y3);
		swap(x1,x3);
	}
	if(y3<y2)
	{
		swap(x3,x2);
		swap(y3,y2);
	}
	if(y3<g_rRectClip.top || y1>g_rRectClip.bottom-1
		|| (x1<g_rRectClip.left && x2<g_rRectClip.left && x3<g_rRectClip.left)
		|| (x1>g_rRectClip.right-1 && x2>g_rRectClip.right-1 && x3>g_rRectClip.right-1))
		return ;
	if(y1==y2)
	{
		DrawTopTriFP16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else if(y2==y3)
	{
		DrawBottomTriFP16(x1,y1,x2,y2,x3,y3,color,destbuffer,lpitch);
	}
	else
	{
		newx=x1+(int)(0.5+(float)(y2-y1)*(float)(x3-x1)/(float)(y3-y1));
		DrawBottomTriFP16(x1,y1,newx,y2,x2,y2,color,destbuffer,lpitch);
		DrawTopTriFP16(x2,y2,newx,y2,x3,y3,color,destbuffer,lpitch);
	}
}


//z缓存不支持alpha混合
void CTriangle2D::DrawTriangleZB162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  = (int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  = (int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;
		
			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
		
				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);
				
				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;
			
			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;
		
			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);
			
				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				
				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);
				
				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;
			
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<=xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}
					
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;
				
				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);
			
			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  
			
			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				
				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  
		
			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);
			
			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  
						
						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
					
						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  
						
						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						
						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;
	
	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
						zptr[xi]=zi;
					}
					
					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3)); 
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));    
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTriangleZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
 	int v0=0,v1=1,v2=2;
 	int temp=0;
 	int tri_type=TRI_TYPE_NONE;
 	int irestart=INTERP_LHS;
 
 	int dx,dy,dz;
 	int dyl,dyr;
 	int du,dv;
 	int xi,yi,zi;
 	int ui,vi;
 	int xstart,xend,ystart,yend,yrestart;
 	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
 	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
 	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;
 
 	USHORT* screenPtr=NULL;
 	USHORT* screenLine=NULL;
 	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
 	USHORT* destBuffer=(USHORT*)_destBuffer;
	
	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

 	lpitch>>=1;
	zpitch>>=2;

 	int textShift=gLog2[face->ptrTexture->width];
 	// 三角形不在屏幕内
 	if (((face->tvlist[0].y < g_rRectClip.top)  && 
 		(face->tvlist[1].y < g_rRectClip.top)  &&
 		(face->tvlist[2].y < g_rRectClip.top)) ||
 
 		((face->tvlist[0].y > g_rRectClip.bottom)  && 
 		(face->tvlist[1].y > g_rRectClip.bottom)  &&
 		(face->tvlist[2].y > g_rRectClip.bottom)) ||
 
 		((face->tvlist[0].x < g_rRectClip.left)  && 
 		(face->tvlist[1].x < g_rRectClip.left)  &&
 		(face->tvlist[2].x < g_rRectClip.left)) ||
 
 		((face->tvlist[0].x > g_rRectClip.right)  && 
 		(face->tvlist[1].x > g_rRectClip.right)  &&
 		(face->tvlist[2].x > g_rRectClip.right)))
 		return;
 
 	//对y0,y1,y2排序
 	if (face->tvlist[v0].y>face->tvlist[v1].y)
 	{
 		swap(v0,v1);
 	}
 	if (face->tvlist[v0].y>face->tvlist[v2].y)
 	{
 		swap(v0,v2);
 	}
 	if (face->tvlist[v1].y>face->tvlist[v2].y)
 	{
 		swap(v1,v2);
 	}
 	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
 	{
 		tri_type=TRI_TYPE_FLAT_TOP;
 		if(face->tvlist[v0].x>face->tvlist[v1].x)
 		{
 			swap(v0,v1);
 		}
 	}
 	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
 	{
 		tri_type=TRI_TYPE_FLAT_BOTTOM;
 		if (face->tvlist[v1].x>face->tvlist[v2].x)
 		{
 			swap(v1,v2);
 		}
 	}
 	else
 	{
 		tri_type=TRI_TYPE_GENERAL;
 	}
 
 	x0  = (int)(face->tvlist[v0].x+0.5);
 	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

 	tu0 = (int)(face->tvlist[v0].u0+0.5);
 	tv0 = (int)(face->tvlist[v0].v0+0.5);

 	x1  = (int)(face->tvlist[v1].x+0.5);
 	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

 	tu1 = (int)(face->tvlist[v1].u0+0.5);
 	tv1 = (int)(face->tvlist[v1].v0+0.5); 
 
 	x2  = (int)(face->tvlist[v2].x+0.5);
 	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

 	tu2 = (int)(face->tvlist[v2].u0+0.5); 
 	tv2 = (int)(face->tvlist[v2].v0+0.5); 
 
	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
 	if (tri_type & TRI_TYPE_FLAGMASK)
 	{
 		if (tri_type == TRI_TYPE_FLAT_TOP)
 		{
 			dy=y2-y0;
 			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
 			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
 			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;
 
 			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
 			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
 			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;
 
 			//裁剪
 			if (y0<g_rRectClip.top)
 			{
 				dy=g_rRectClip.top-y0;
 				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
 				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
 				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
 
 				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
 				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
 				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);
 
 				ystart=g_rRectClip.top;
 			}
 			else
 			{
 				//不需要裁剪
 				xl=(x0<<FIXP16_SHIFT);
 				ul=(tu0<<FIXP16_SHIFT);
 				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
 
 				xr=(x1<<FIXP16_SHIFT);
 				ur=(tu1<<FIXP16_SHIFT);
 				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);
 
 				ystart=y0;
 			}
 		} 
 		else
 		{
 			//平底三角形
 			dy=y1-y0;
 			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
 			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
 			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;
 
 			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
 			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
 			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy; 
 
 			//裁剪
 			if (y0<g_rRectClip.top)
 			{
 				dy=g_rRectClip.top-y0;
 				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
 				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
 				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
 
 				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
 				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
 				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);
 
 				ystart=g_rRectClip.top;
 			}
 			else
 			{
 				//不需要裁剪
 				xl=(x0<<FIXP16_SHIFT);
 				ul=(tu0<<FIXP16_SHIFT);
 				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
 
 				xr=(x0<<FIXP16_SHIFT);
 				ur=(tu0<<FIXP16_SHIFT);
 				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);
 
 				ystart=y0;
 			}
 		}// end else bottom triangle
 
 		if ((yend=y2)>g_rRectClip.bottom-1)
 		{
 			yend=g_rRectClip.bottom-1;
 		}
 
 		//水平裁剪
 		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
 			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
 			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
 		{
 			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

 			for (yi=ystart;yi<yend;yi++)
 			{
 				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
 				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
 
 				ui = ul + FIXP16_ROUND_UP;
 				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
 
 				if ((dx = (xend - xstart))>0)
 				{
 					du = (ur - ul)/dx;
 					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
 				}
 				else
 				{
 					du = (ur - ul);
 					dv = (vr - vl);
					dz = (zr - zl);
 				}
 				if (xstart<g_rRectClip.left)
 				{
 					dx=g_rRectClip.left-xstart;
 
 					ui+=dx*du;
 					vi+=dx*dv;
					zi+=dx*dz;
 
 					xstart=g_rRectClip.left;
 				}
 				if (xend>g_rRectClip.right-1)
 				{
 					xend=g_rRectClip.right-1;
 				}
 
 				for (xi=xstart; xi<xend; xi++)
 				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
 						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}
 					
 					ui+=du;
 					vi+=dv;
					zi+=dz;
 				} 
 				xl+=dxdyl;
 				ul+=dudyl;
 				vl+=dvdyl;
				zl+=dzdyl;
 
 				xr+=dxdyr;
 				ur+=dudyr;
 				vr+=dvdyr;
				zr+=dzdyr;
 
 				screenPtr+=lpitch;
				zptr+=zpitch;
 			}
 		}
 		else
 		{
 			// 没有裁剪
 			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
 			for (yi = ystart; yi<=yend; yi++)
 			{
 				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 
 				ui = ul + FIXP16_ROUND_UP;
 				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
 
 				if ((dx = (xend - xstart))>0)
 				{
 					du = (ur - ul)/dx;
 					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
 				} 
 				else
 				{
 					du = (ur - ul);
 					dv = (vr - vl);
					dz = (zr - zl);
 				} 
 
				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
 			}
 		}
 	}
 	else if (tri_type==TRI_TYPE_GENERAL)
 	{
 		if ((yend=y2)>g_rRectClip.bottom-1)
 		{
 			yend=g_rRectClip.bottom-1;
 		}
 		if(y1<g_rRectClip.top)
 		{
 			dyl = (y2 - y1);
 
 			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
 			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
 			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
 
 			dyr = (y2 - y0);	
 
 			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
 			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
 			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
 
 			dyl = (g_rRectClip.top - y1);
 			dyr = (g_rRectClip.top - y0);
 
 			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
 			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
 			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);
 
 			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
 			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
 			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);
 
 			ystart = g_rRectClip.top;
 
 			//为了保证渲染从左到右
 			if (dxdyr > dxdyl)
 			{
 				swap(dxdyl,dxdyr);
 				swap(dudyl,dudyr);
 				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
 				swap(xl,xr);
 				swap(ul,ur);
 				swap(vl,vr);
				swap(zl,zr);
 				swap(x1,x2);
 				swap(y1,y2);
				swap(tz1,tz2);
 				swap(tu1,tu2);
 				swap(tv1,tv2);
 
 				irestart = INTERP_RHS;
 			} 
 		}
 		else if (y0<g_rRectClip.top)
 		{
 			dyl = (y1 - y0);
 
 			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
 			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
 			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

 			dyr = (y2 - y0);	
 
 			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
 			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
 			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
 
 			dy = (g_rRectClip.top - y0);
 
 			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
 			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
 			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);
 
 			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
 			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
 			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);
 
 			ystart = g_rRectClip.top;
 
 			if (dxdyr < dxdyl)
 			{
 				swap(dxdyl,dxdyr);
 				swap(dudyl,dudyr);
 				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
 				swap(xl,xr);
 				swap(ul,ur);
 				swap(vl,vr);
				swap(zl,zr);
 				swap(x1,x2);
 				swap(y1,y2);
				swap(tz1,tz2);
 				swap(tu1,tu2);
 				swap(tv1,tv2);
 
 				irestart = INTERP_RHS;
 			} 
 		}
 		else
 		{
 			dyl = (y1 - y0);
 
 			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
 			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
 			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;
 
 			dyr = (y2 - y0);	
 
 			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
 			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
 			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
 
 			xl = (x0 << FIXP16_SHIFT);
 			xr = (x0 << FIXP16_SHIFT);
 
 			ul = (tu0 << FIXP16_SHIFT);
 			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

 			ur = (tu0 << FIXP16_SHIFT);
 			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);
 
 			ystart = y0;
 
 			//确保渲染从左到右
 			if (dxdyr < dxdyl)
 			{
 				swap(dxdyl,dxdyr);
 				swap(dudyl,dudyr);
 				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
 				swap(xl,xr);
 				swap(ul,ur);
 				swap(vl,vr);
				swap(zl,zr);
 				swap(x1,x2);
 				swap(y1,y2);
				swap(tz1,tz2);
 				swap(tu1,tu2);
 				swap(tv1,tv2);
 
 				irestart = INTERP_RHS;
 			} 
 		}
 		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
 			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
 			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
 		{
 			//需要裁剪
 			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

 			for (yi = ystart; yi<yend; yi++)
 			{
 				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 
 				ui = ul + FIXP16_ROUND_UP;
 				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
 
 				if ((dx = (xend - xstart))>0)
 				{
 					du = (ur - ul)/dx;
 					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
 				}
 				else
 				{
 					du = (ur - ul);
 					dv = (vr - vl);
					dz = (zr - zl);
 				} 
 
 				if (xstart < g_rRectClip.left)
 				{
 					dx = g_rRectClip.left - xstart;
 
 					ui+=dx*du;
 					vi+=dx*dv;
					zi+=dx*dz;
 
 					xstart = g_rRectClip.left;
 				} 
 				if (xend > g_rRectClip.right-1)
 					xend = g_rRectClip.right-1;
 
				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
 
 				if (yi==yrestart)
 				{
 					if (irestart == INTERP_LHS)
 					{
 						// LHS
 						dyl = (y2 - y1);	
 
 						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
 						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
 						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
 
 						xl = (x1  << FIXP16_SHIFT);
 						ul = (tu1 << FIXP16_SHIFT);
 						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
 
 						xl+=dxdyl;
 						ul+=dudyl;
 						vl+=dvdyl;
						zl+=dzdyl;
 					}
 					else
 					{
 						// RHS
 						dyr = (y1 - y2);	
 
 						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
 						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
 						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
 
 						xr = (x2  << FIXP16_SHIFT);
 						ur = (tu2 << FIXP16_SHIFT);
 						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
 
 						xr+=dxdyr;
 						ur+=dudyr;
 						vr+=dvdyr;
						zr+=dzdyr;
 					}
 				} 
 			} 
 		}
 		else
 		{
 			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

 			for (yi = ystart; yi<yend; yi++)
 			{
 				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
 
 				ui = ul + FIXP16_ROUND_UP;
 				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
 
 				if ((dx = (xend - xstart))>0)
 				{
 					du = (ur - ul)/dx;
 					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
 				}
 				else
 				{
 					du = (ur - ul);
 					dv = (vr - vl);
					dz = (zr - zl);
 				} 
 
				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
 
 				// test for yi hitting second region, if so change interpolant
 				if (yi==yrestart)
 				{
 					if (irestart == INTERP_LHS)
 					{
 						// LHS
 						dyl = (y2 - y1);	
 
 						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
 						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
 						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
 
 						xl = (x1  << FIXP16_SHIFT);
 						ul = (tu1 << FIXP16_SHIFT);
 						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
 
 						xl+=dxdyl;
 						ul+=dudyl;
 						vl+=dvdyl;
						zl+=dzdyl;
 					} 
 					else
 					{
 						// RHS
 						dyr = (y1 - y2);	
 
 						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
 						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
 						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
 
 						xr = (x2  << FIXP16_SHIFT);
 						ur = (tu2 << FIXP16_SHIFT);
 						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
 
 						xr+=dxdyr;
 						ur+=dudyr;
 						vr+=dvdyr;
						zr+=dzdyr;
 					} 
 				} 
 			} 
 		}
 	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 
	tz1 = (int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 
	tz2 = (int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleZBGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);
	
	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);
						
						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel	 =  (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}

void CTriangle2D::DrawTextureTriangleBilerpZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	int textureSize=face->ptrTexture->width-1;

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);
						
						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;
						
						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);
						
						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}


//z缓存直接写入
void CTriangle2D::DrawTriangleWTZB162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  = (int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  = (int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<=xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleWTZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTriangleWTZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<=yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleWTZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 
	tz1 = (int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 
	tz2 = (int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleWTZBGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}



//z缓存支持alpha混合
void CTriangle2D::DrawTriangleZBAlpha162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  = (int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  = (int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;
	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];
	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<=yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = alphaSrc1[screenPtr[xi]]+
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3))];   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = alphaSrc1[screenPtr[xi]]+
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3))];   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = alphaSrc1[screenPtr[xi]]+
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3))];   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = alphaSrc1[screenPtr[xi]]+
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3))];   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTriangleZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0);
	tv0 = (int)(face->tvlist[v0].v0);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0);
	tv1 = (int)(face->tvlist[v1].v0); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0); 
	tv2 = (int)(face->tvlist[v2].v0); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<=yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0);
	tv0 = (int)(face->tvlist[v0].v0);
	tz0 = (int)(face->tvlist[v0].z);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0);
	tv1 = (int)(face->tvlist[v1].v0); 
	tz1 = (int)(face->tvlist[v1].z);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0); 
	tv2 = (int)(face->tvlist[v2].v0); 
	tz2 = (int)(face->tvlist[v2].z);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleZBAlphaGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch,int alpha)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];
	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<FIXP16_SHIFT);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<FIXP16_SHIFT)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<FIXP16_SHIFT)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<FIXP16_SHIFT);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<FIXP16_SHIFT);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<FIXP16_SHIFT);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<FIXP16_SHIFT);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel	 =  (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << FIXP16_SHIFT);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << FIXP16_SHIFT);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << FIXP16_SHIFT);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << FIXP16_SHIFT)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << FIXP16_SHIFT)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << FIXP16_SHIFT);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl + FIXP16_ROUND_UP;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi<zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << FIXP16_SHIFT)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << FIXP16_SHIFT);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << FIXP16_SHIFT)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << FIXP16_SHIFT);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}

//1/z缓存不支持alpha混合
void CTriangle2D::DrawTriangleINVZB162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  =(1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  =(1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<=xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<=xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=color;
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3)); 
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));    
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTriangleINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleINVZBGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<0);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<0);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel	 =  (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << 0)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}

//透视修正固定着色

void CTriangle2D::DrawTextureTrianglePerINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = ((int)(face->tvlist[v0].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = ((int)(face->tvlist[v1].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = ((int)(face->tvlist[v2].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//透视修正平面着色纹理

void CTriangle2D::DrawTextureTrianglePerINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
							(((vi<< (FIXP28_SHIFT-FIXP22_SHIFT))/zi)<< textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
							(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}

}


//1/z直接写入
void CTriangle2D::DrawTriangleWTINVZB162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  =(1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  =(1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<=xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<=xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			zl = (tz0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					screenPtr[xi]=color;
					zPtr[xi]=zi;
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleWTINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi] = ((ui >> (FIXP16_SHIFT+3)) << 11) + ((vi >> (FIXP16_SHIFT+2)) << 5) + (wi >> (FIXP16_SHIFT+3));   
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTriangleWTINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleWTINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleWTINVZBGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<0);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<0);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << 0)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

					Rtextel  = ((textel >> 11)); 
					Gtextel  = ((textel >> 5)  & 0x3f); 
					Btextel =   (textel        & 0x1f);

					Rtextel*=ui; 
					Gtextel*=vi;
					Btextel*=wi;

					// RGB 5.6.5
					screenPtr[xi]=(Btextel >> (FIXP16_SHIFT+8)) + 
						((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
						((Rtextel >> (FIXP16_SHIFT+8)) << 11);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}

//透视修正固定着色

void CTriangle2D::DrawTextureTriangleWTPerINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = ((int)(face->tvlist[v0].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = ((int)(face->tvlist[v1].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = ((int)(face->tvlist[v2].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					screenPtr[xi]=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) + 
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT)) /zi)<< textShift)];
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//透视修正平面着色纹理

void CTriangle2D::DrawTextureTriangleWTPerINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					// RGB 5.6.5
					textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) +
						(((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

					rTextel=textel>>11;
					gTextel=((textel>>5) & 0x3f);
					bTextel=(textel & 0x1f);

					rTextel*=rBase;
					gTextel*=gBase;
					bTextel*=bBase;

					rTextel>>=5;
					gTextel>>=6;
					bTextel>>=5;

					screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
					zptr[xi]=zi;

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}



//透视修正固定着色

void CTriangle2D::DrawTextureTrianglePerLPINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;
	int ul2,ur2,vl2,vr2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = ((int)(face->tvlist[v0].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = ((int)(face->tvlist[v1].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = ((int)(face->tvlist[v2].u0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				
				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP22_SHIFT) + 
							((vi >> FIXP22_SHIFT)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP22_SHIFT) + 
							((vi >> FIXP22_SHIFT)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP22_SHIFT) + 
							((vi >> FIXP22_SHIFT)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=textMap[(ui >> FIXP22_SHIFT) + 
							((vi >> FIXP22_SHIFT)<< textShift)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//透视修正平面着色纹理

void CTriangle2D::DrawTextureTrianglePerLPINVZBFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;
	int ul2,ur2,vl2,vr2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) +
							((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;


				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) +
							((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;


				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) +
							((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;


				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) +
							((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=_RGB16BIT565(rTextel,gTextel,bTextel);
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

void CTriangle2D::DrawTextureTriangleBilerpINVZB16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	int textureSize=face->ptrTexture->width-1;
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						int uint=ui>>FIXP16_SHIFT;
						int vint=vi>>FIXP16_SHIFT;

						int uintadd1=uint+1;
						if (uintadd1>textureSize)
						{
							uintadd1=textureSize;
						}
						int vintadd1=vint+1;
						if (vintadd1>textureSize)
						{
							vintadd1=textureSize;
						}

						int textel00 = textMap[(uint+0)     + ((vint+0) << textShift)];
						int textel10 = textMap[(uintadd1) + ((vint+0) << textShift)];
						int textel01 = textMap[(uint+0)     + ((vintadd1) << textShift)];
						int textel11 = textMap[(uintadd1) + ((vintadd1) << textShift)];
						//提取4个RGB位图值
						int r_textel00  = ((textel00 >> 11)       ); 
						int g_textel00  = ((textel00 >> 5)  & 0x3f); 
						int b_textel00  =  (textel00        & 0x1f);

						int r_textel10  = ((textel10 >> 11)       ); 
						int g_textel10  = ((textel10 >> 5)  & 0x3f); 
						int b_textel10  =  (textel10        & 0x1f);

						int r_textel01  = ((textel01 >> 11)       ); 
						int g_textel01  = ((textel01 >> 5)  & 0x3f); 
						int b_textel01  =  (textel01        & 0x1f);

						int r_textel11  = ((textel11 >> 11)       ); 
						int g_textel11  = ((textel11 >> 5)  & 0x3f); 
						int b_textel11  =  (textel11        & 0x1f);

						//dtu dtv 是 ui vi的小数部分
						//采用24.8格式存储
						int dtu = (ui & (0xffff)) >> 8;
						int dtv = (vi & (0xffff)) >> 8;

						int one_minus_dtu = (1 << 8) - dtu;
						int one_minus_dtv = (1 << 8) - dtv;

						int one_minus_dtu_x_one_minus_dtv = (one_minus_dtu) * (one_minus_dtv);
						int dtu_x_one_minus_dtv           = (dtu)           * (one_minus_dtv);
						int dtu_x_dtv                     = (dtu)           * (dtv);
						int one_minus_dtu_x_dtv           = (one_minus_dtu) * (dtv);

						//计算采样的公式
						int r_textel = one_minus_dtu_x_one_minus_dtv * r_textel00 + 
							dtu_x_one_minus_dtv           * r_textel10 +
							dtu_x_dtv                     * r_textel11 +
							one_minus_dtu_x_dtv           * r_textel01;

						int g_textel = one_minus_dtu_x_one_minus_dtv * g_textel00 + 
							dtu_x_one_minus_dtv           * g_textel10 +
							dtu_x_dtv                     * g_textel11 +
							one_minus_dtu_x_dtv           * g_textel01;

						int b_textel = one_minus_dtu_x_one_minus_dtv * b_textel00 + 
							dtu_x_one_minus_dtv           * b_textel10 +
							dtu_x_dtv                     * b_textel11 +
							one_minus_dtu_x_dtv           * b_textel01;

						screenPtr[xi]=_RGB16BIT565(r_textel>>FIXP16_SHIFT,g_textel>>FIXP16_SHIFT,b_textel>>FIXP16_SHIFT);
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}

}

//1/z缓存支持alpha混合
void CTriangle2D::DrawTriangleINVZBAlpha162D(LPTriF4DV1 face, UCHAR *destbuffer, int lpitch, UCHAR *zBuffer, int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int dz,zi;
	int xi,yi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int x0,y0,tz0,x1,y1,tz1,x2,y2,tz2;

	USHORT color;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)destbuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zPtr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0  =(1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1  =(1<<FIXP28_SHIFT)/ (int)(face->tvlist[v0].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2  =(1<<FIXP28_SHIFT)/ (int)(face->tvlist[v0].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;
	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];
	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	color=face->litColor[0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zPtr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<=yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				} 
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}

					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz2 - tz1) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dzdyl = ((tz1 - tz0) << 0)/dyl;  

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << 0);

			zl = (tz0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);
						xl+=dxdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zPtr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				zi = zl;
				if ((dx = (xend - xstart))>0)
				{
					dz = (zr - zl)/dx;
				}
				else
				{
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zPtr[xi])
					{
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[color];
						zPtr[xi]=zi;
					}
					zi+=dz;
				} 
				xl+=dxdyl;
				zl+=dzdyl; 

				xr+=dxdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zPtr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dzdyr = ((tz1 - tz2) << 0)/dyr;  

						xr = (x2  << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawGouraudTriangleINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	//画Gouraud着色三角形
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz;
	int xi,yi;
	int ui,vi,wi,zi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr;
	int x0,y0,tz0,tu0,tv0,tw0,x1,y1,tz1,tu1,tv1,tw1,x2,y2,tz2,tu2,tv2,tw2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy = (y2 - y0);

			dxdyl = ((x2 - x0)   << FIXP16_SHIFT)/dy;
			dudyl = ((tu2 - tu0) << FIXP16_SHIFT)/dy;  
			dvdyl = ((tv2 - tv0) << FIXP16_SHIFT)/dy;    
			dwdyl = ((tw2 - tw0) << FIXP16_SHIFT)/dy;  
			dzdyl = ((tz2 - tz0) << 0)/dy; 

			dxdyr = ((x2 - x1)   << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu1) << FIXP16_SHIFT)/dy;  
			dvdyr = ((tv2 - tv1) << FIXP16_SHIFT)/dy;   
			dwdyr = ((tw2 - tw1) << FIXP16_SHIFT)/dy;   
			dzdyr = ((tz2 - tz1) << 0)/dy;   

			if (y0 < g_rRectClip.top)
			{
				dy = (g_rRectClip.top - y0);

				xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x1  << FIXP16_SHIFT);
				ur = dudyr*dy + (tu1 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv1 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw1 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz1 << 0);

				ystart = g_rRectClip.top;
			}
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x1 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu1 << FIXP16_SHIFT);
				vr = (tv1 << FIXP16_SHIFT);
				wr = (tw1 << FIXP16_SHIFT);
				zr = (tz1 << 0);

				ystart = y0;
			} 
		} 
		else
		{
			dy = (y1 - y0);

			dxdyl = ((x1 - x0)   << FIXP16_SHIFT)/dy;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dy;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dy;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dy; 
			dzdyl = ((tz1 - tz0) << 0)/dy; 

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dy;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dy;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dy;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dy;   
			dzdyr = ((tz2 - tz0) << 0)/dy;   

			if (y0 < g_rRectClip.top)
			{
				dy = (g_rRectClip.top - y0);

				xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
				ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
				vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
				wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
				zl = dzdyl*dy + (tz0 << 0);

				xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
				ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
				vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
				wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
				zr = dzdyr*dy + (tz0 << 0);

				ystart = g_rRectClip.top;
			} 
			else
			{
				xl = (x0 << FIXP16_SHIFT);
				xr = (x0 << FIXP16_SHIFT);

				ul = (tu0 << FIXP16_SHIFT);
				vl = (tv0 << FIXP16_SHIFT);
				wl = (tw0 << FIXP16_SHIFT);
				zl = (tz0 << 0);

				ur = (tu0 << FIXP16_SHIFT);
				vr = (tv0 << FIXP16_SHIFT);
				wr = (tw0 << FIXP16_SHIFT);
				zr = (tz0 << 0);

				ystart = y0;
			} 
		}

		if ((yend = y2) > g_rRectClip.bottom-1)
			yend = g_rRectClip.bottom-1;

		if ((x0 < g_rRectClip.left) || (x0 > g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 > g_rRectClip.right-1))
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 
				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				}

				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;
				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > zptr[xi])
					{
						screenPtr[xi] = alphaSrc1[screenPtr[xi]] + 
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + 
							((vi >> (FIXP16_SHIFT+2)) << 5) + 
							(wi >> (FIXP16_SHIFT+3))];
						zptr[xi] = zi;           
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			} 
		} 
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > zptr[xi])
					{
						screenPtr[xi] = alphaSrc1[screenPtr[xi]] + 
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + 
							((vi >> (FIXP16_SHIFT+2)) << 5) + 
							(wi >> (FIXP16_SHIFT+3))];

						zptr[xi] = zi;           
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				}

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			} 
		} 
	} 
	else if (tri_type==TRI_TYPE_GENERAL)
	{

		if ((yend = y2) > g_rRectClip.bottom-1)
			yend = g_rRectClip.bottom-1;

		if (y1 < g_rRectClip.top)
		{
			// LHS
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			// RHS
			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dyr = (g_rRectClip.top - y0);
			dyl = (g_rRectClip.top - y1);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);

			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);

			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			} 
		} 
		else if (y0 < g_rRectClip.top)
		{
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			// RHS
			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			}
		}
		else
		{
			// LHS
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			// RHS
			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);

				irestart = INTERP_RHS;
			}
		}

		if ((x0 < g_rRectClip.left) || (x0 > g_rRectClip.right) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right) ||
			(x2 < g_rRectClip.left) || (x2 > g_rRectClip.right))
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 
				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;

					xstart = g_rRectClip.left;

				}
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > zptr[xi])
					{
						screenPtr[xi] = alphaSrc1[screenPtr[xi]] + 
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + 
							((vi >> (FIXP16_SHIFT+2)) << 5) + 
							(wi >> (FIXP16_SHIFT+3))];
						zptr[xi] = zi;           
					} 

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;  
						dzdyl = ((tz2 - tz1) << 0)/dyl;  

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;   		
						dzdyr = ((tz1 - tz2) << 0)/dyr;   

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		} 
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer + (ystart * zpitch);

			for (yi = ystart; yi < yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi < xend; xi++)
				{
					if (zi > zptr[xi])
					{
						screenPtr[xi] = alphaSrc1[screenPtr[xi]] + 
							alphaSrc2[((ui >> (FIXP16_SHIFT+3)) << 11) + 
							((vi >> (FIXP16_SHIFT+2)) << 5) + 
							(wi >> (FIXP16_SHIFT+3))]; 

						zptr[xi] = zi;           
					} 

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
				} 

				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl;   
						dzdyl = ((tz2 - tz1) << 0)/dyl;   

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;   

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		} 
	} 
}
void CTriangle2D::DrawTextureTriangleINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl ;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTriangleINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = (int)(face->tvlist[v0].u0+0.5);
	tv0 = (int)(face->tvlist[v0].v0+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = (int)(face->tvlist[v1].u0+0.5);
	tv1 = (int)(face->tvlist[v1].v0+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = (int)(face->tvlist[v2].u0+0.5); 
	tv2 = (int)(face->tvlist[v2].v0+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			zl = (tz0 << 0);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP16_SHIFT) + ((vi >> FIXP16_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//在颜色的调制之后要相应的除以颜色的最大值，避免溢出

void CTriangle2D::DrawTextureTriangleINVZBAlphaGS16(LPTriF4DV1 face, UCHAR *_destBuffer, int lpitch, UCHAR *zBuffer, int zpitch,int alpha)
{
	//画Gouraud着色三角形
	//u,v,w 颜色三原色  s,t纹理坐标
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dw,dz,ds,dt;
	int xi,yi;
	int ui,vi,wi,zi,si,ti;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dwdyl,wl,dudyr,ur,dvdyr,vr,dwdyr,wr,sl,dsdyl,sr,dsdyr,tl,dtdyl,tr,dtdyr;
	int x0,y0,tz0,tu0,tv0,tw0,ts0,tt0,x1,y1,tz1,tu1,tv1,tw1,ts1,tt1,x2,y2,tz2,tu2,tv2,tw2,ts2,tt2;
	int rbase0,gbase0,bbase0,rbase1,gbase1,bbase1,rbase2,gbase2,bbase2;
	UINT Rtextel,Gtextel,Btextel;
	USHORT textel;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	int textShif2=gLog2[face->ptrTexture->width];

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	zpitch>>=2;
	lpitch>>=1;

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	ts0 = (int)(face->tvlist[v0].u0+0.5);
	tt0 = (int)(face->tvlist[v0].v0+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	ts1 = (int)(face->tvlist[v1].u0+0.5);
	tt1 = (int)(face->tvlist[v1].v0+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);
	ts2 = (int)(face->tvlist[v2].u0+0.5);
	tt2 = (int)(face->tvlist[v2].v0+0.5);

	// 三角形不在屏幕内
	if (((y0 < g_rRectClip.top)  && 
		(y1 < g_rRectClip.top)  &&
		(y2 < g_rRectClip.top)) ||

		((y0 > g_rRectClip.bottom-1)  && 
		(y1 > g_rRectClip.bottom-1)  &&
		(y2 > g_rRectClip.bottom-1)) ||

		((x0 < g_rRectClip.left)  && 
		(x1 < g_rRectClip.left)  &&
		(x2 < g_rRectClip.left)) ||

		((x0 > g_rRectClip.right-1)  && 
		(x1 > g_rRectClip.right-1)  &&
		(x2 > g_rRectClip.right-1)))
		return;

	//三角形为直线
	if ( ((x0==x1) && (x1==x2)) ||
		((y0==y1) && (y1==y2)))
		return;

	//对y0,y1,y2排序
	if (y0>y1)
	{
		swap(x0,x1);
		swap(y0,y1);
		swap(tz0,tz1);
		swap(ts0,ts1);
		swap(tt0,tt1);
	}
	if (y0>y2)
	{
		swap(x0,x2);
		swap(y0,y2);
		swap(tz0,tz2);
		swap(ts0,ts2);
		swap(tt0,tt2);
	}
	if (y1>y2)
	{
		swap(x1,x2);
		swap(y1,y2);
		swap(tz1,tz2);
		swap(ts1,ts2);
		swap(tt1,tt2);
	}
	if (y0==y1)
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(x0>x1)
		{
			swap(x0,x1);
			swap(y0,y1);
			swap(tz0,tz1);
			swap(ts0,ts1);
			swap(tt0,tt1);
		}
	}
	else if (y1==y2)
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (x1>x2)
		{
			swap(x1,x2);
			swap(y1,y2);
			swap(tz1,tz2);
			swap(ts1,ts2);
			swap(tt1,tt2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	_RGB565FROM16BIT(face->litColor[0],&rbase0,&gbase0,&bbase0);
	_RGB565FROM16BIT(face->litColor[1],&rbase1,&gbase1,&bbase1);
	_RGB565FROM16BIT(face->litColor[2],&rbase2,&gbase2,&bbase2);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];
	//转为8.8.8格式
	rbase0<<=3;
	gbase0<<=2;
	bbase0<<=3;

	rbase1<<=3;
	gbase1<<=2;
	bbase1<<=3;

	rbase2<<=3;
	gbase2<<=2;
	bbase2<<=3;

	tu0 = rbase0;
	tv0 = gbase0; 
	tw0 = bbase0; 

	tu1 = rbase1;
	tv1 = gbase1; 
	tw1 = bbase1; 

	tu2 = rbase2; 
	tv2 = gbase2; 
	tw2 = bbase2; 

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type & TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;
			dsdyl=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv1)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw1)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;
			dsdyr=((ts2-ts1)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt1)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv1<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw1<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz1<<0);
				sr=dsdyr*dy+(ts1<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt1<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<FIXP16_SHIFT);
				vr=(tv1<<FIXP16_SHIFT);
				wr=(tw1<<FIXP16_SHIFT);
				zr=(tz1<<0);
				sr=(ts1<<FIXP16_SHIFT);
				tr=(tt1<<FIXP16_SHIFT);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<FIXP16_SHIFT)/dy;
			dvdyl=((tv1-tv0)<<FIXP16_SHIFT)/dy;
			dwdyl=((tw1-tw0)<<FIXP16_SHIFT)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;
			dsdyl=((ts1-ts0)<<FIXP16_SHIFT)/dy;
			dtdyl=((tt1-tt0)<<FIXP16_SHIFT)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<FIXP16_SHIFT)/dy;
			dvdyr=((tv2-tv0)<<FIXP16_SHIFT)/dy;
			dwdyr=((tw2-tw0)<<FIXP16_SHIFT)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;
			dsdyr=((ts2-ts0)<<FIXP16_SHIFT)/dy;
			dtdyr=((tt2-tt0)<<FIXP16_SHIFT)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<FIXP16_SHIFT);
				vl=dvdyl*dy+(tv0<<FIXP16_SHIFT);
				wl=dwdyl*dy+(tw0<<FIXP16_SHIFT);
				zl=dzdyl*dy+(tz0<<0);
				sl=dsdyl*dy+(ts0<<FIXP16_SHIFT);
				tl=dtdyl*dy+(tt0<<FIXP16_SHIFT);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<FIXP16_SHIFT);
				vr=dvdyr*dy+(tv0<<FIXP16_SHIFT);
				wr=dwdyr*dy+(tw0<<FIXP16_SHIFT);
				zr=dzdyr*dy+(tz0<<0);
				sr=dsdyr*dy+(ts0<<FIXP16_SHIFT);
				tr=dtdyr*dy+(tt0<<FIXP16_SHIFT);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<FIXP16_SHIFT);
				vl=(tv0<<FIXP16_SHIFT);
				wl=(tw0<<FIXP16_SHIFT);
				zl=(tz0<<0);
				sl=(ts0<<FIXP16_SHIFT);
				tl=(tt0<<FIXP16_SHIFT);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<FIXP16_SHIFT);
				vr=(tv0<<FIXP16_SHIFT);
				wr=(tw0<<FIXP16_SHIFT);
				zr=(tz0<<0);
				sr=(ts0<<FIXP16_SHIFT);
				tr=(tt0<<FIXP16_SHIFT);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel	 =  (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz2 - tz1) << 0)/dyl;
			dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << FIXP16_SHIFT);
			vl = dvdyl*dyl + (tv1 << FIXP16_SHIFT);
			wl = dwdyl*dyl + (tw1 << FIXP16_SHIFT);
			zl = dzdyl*dyl + (tz1 << 0);
			sl = dsdyl*dyl + (ts1 << FIXP16_SHIFT);
			tl = dtdyl*dyl + (tt1 << FIXP16_SHIFT);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dyr + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dyr + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dyr + (tz0 << 0);
			sr = dsdyr*dyr + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dyr + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl; 
			dzdyl = ((tz1 - tz0) << 0)/dyl;
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl;
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << FIXP16_SHIFT);
			vl = dvdyl*dy + (tv0 << FIXP16_SHIFT);
			wl = dwdyl*dy + (tw0 << FIXP16_SHIFT);
			zl = dzdyl*dy + (tz0 << 0);
			sl = dsdyl*dy + (ts0 << FIXP16_SHIFT);
			tl = dtdyl*dy + (tt0 << FIXP16_SHIFT);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << FIXP16_SHIFT);
			vr = dvdyr*dy + (tv0 << FIXP16_SHIFT);
			wr = dwdyr*dy + (tw0 << FIXP16_SHIFT);
			zr = dzdyr*dy + (tz0 << 0);
			sr = dsdyr*dy + (ts0 << FIXP16_SHIFT);
			tr = dtdyr*dy + (tt0 << FIXP16_SHIFT);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << FIXP16_SHIFT)/dyl;  
			dvdyl = ((tv1 - tv0) << FIXP16_SHIFT)/dyl;    
			dwdyl = ((tw1 - tw0) << FIXP16_SHIFT)/dyl;   
			dzdyl = ((tz1 - tz0) << 0)/dyl; 
			dsdyl = ((ts1 - ts0) << FIXP16_SHIFT)/dyl; 
			dtdyl = ((tt1 - tt0) << FIXP16_SHIFT)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << FIXP16_SHIFT)/dyr;  
			dvdyr = ((tv2 - tv0) << FIXP16_SHIFT)/dyr;   		
			dwdyr = ((tw2 - tw0) << FIXP16_SHIFT)/dyr;
			dzdyr = ((tz2 - tz0) << 0)/dyr;
			dsdyr = ((ts2 - ts0) << FIXP16_SHIFT)/dyr;
			dtdyr = ((tt2 - tt0) << FIXP16_SHIFT)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << FIXP16_SHIFT);
			vl = (tv0 << FIXP16_SHIFT);
			wl = (tw0 << FIXP16_SHIFT);
			zl = (tz0 << 0);
			sl = (ts0 << FIXP16_SHIFT);
			tl = (tt0 << FIXP16_SHIFT);

			ur = (tu0 << FIXP16_SHIFT);
			vr = (tv0 << FIXP16_SHIFT);
			wr = (tw0 << FIXP16_SHIFT);
			zr = (tz0 << 0);
			sr = (ts0 << FIXP16_SHIFT);
			tr = (tt0 << FIXP16_SHIFT);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dwdyl,dwdyr);
				swap(dzdyl,dzdyr);
				swap(dsdyl,dsdyr);
				swap(dtdyl,dtdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(wl,wr);
				swap(zl,zr);
				swap(sl,sr);
				swap(tl,tr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tu1,tu2);
				swap(tv1,tv2);
				swap(tw1,tw2);
				swap(tz1,tz2);
				swap(ts1,ts2);
				swap(tt1,tt2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl ;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					wi+=dx*dw;
					zi+=dx*dz;
					si+=dx*ds;
					ti+=dx*dt;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl; 
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr; 
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr = zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul + FIXP16_ROUND_UP;
				vi = vl + FIXP16_ROUND_UP;
				wi = wl + FIXP16_ROUND_UP;
				zi = zl;
				si = sl + FIXP16_ROUND_UP;
				ti = tl + FIXP16_ROUND_UP;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dw = (wr - wl)/dx;
					dz = (zr - zl)/dx;
					ds = (sr - sl)/dx;
					dt = (tr - tl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dw = (wr - wl);
					dz = (zr - zl);
					ds = (sr - sl);
					dt = (tr - tl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						textel = textMap[(si >> FIXP16_SHIFT) + ((ti >> FIXP16_SHIFT) << textShif2)];

						Rtextel  = ((textel >> 11)); 
						Gtextel  = ((textel >> 5)  & 0x3f); 
						Btextel =   (textel        & 0x1f);

						Rtextel*=ui; 
						Gtextel*=vi;
						Btextel*=wi;

						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+alphaSrc2[(Btextel >> (FIXP16_SHIFT+8)) + 
							((Gtextel >> (FIXP16_SHIFT+8)) << 5) + 
							((Rtextel >> (FIXP16_SHIFT+8)) << 11)];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					wi+=dw;
					zi+=dz;
					si+=ds;
					ti+=dt;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				wl+=dwdyl;
				zl+=dzdyl;
				sl+=dsdyl;
				tl+=dtdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				wr+=dwdyr;
				zr+=dzdyr;
				sr+=dsdyr;
				tr+=dtdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << FIXP16_SHIFT)/dyl;  
						dvdyl = ((tv2 - tv1) << FIXP16_SHIFT)/dyl;   		
						dwdyl = ((tw2 - tw1) << FIXP16_SHIFT)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;
						dsdyl = ((ts2 - ts1) << FIXP16_SHIFT)/dyl;
						dtdyl = ((tt2 - tt1) << FIXP16_SHIFT)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << FIXP16_SHIFT);
						vl = (tv1 << FIXP16_SHIFT);
						wl = (tw1 << FIXP16_SHIFT);
						zl = (tz1 << 0);
						sl = (ts1 << FIXP16_SHIFT);
						tl = (tt1 << FIXP16_SHIFT);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						wl+=dwdyl;
						zl+=dzdyl;
						sl+=dsdyl;
						tl+=dtdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << FIXP16_SHIFT)/dyr;  
						dvdyr = ((tv1 - tv2) << FIXP16_SHIFT)/dyr;   		
						dwdyr = ((tw1 - tw2) << FIXP16_SHIFT)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;
						dsdyr = ((ts1 - ts2) << FIXP16_SHIFT)/dyr;
						dtdyr = ((tt1 - tt2) << FIXP16_SHIFT)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << FIXP16_SHIFT);
						vr = (tv2 << FIXP16_SHIFT);
						wr = (tw2 << FIXP16_SHIFT);
						zr = (tz2 << 0);
						sr = (ts2 << FIXP16_SHIFT);
						tr = (tt2 << FIXP16_SHIFT);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						wr+=dwdyr;
						zr+=dzdyr;
						sr+=dsdyr;
						tr+=dtdyr;
					} 
				} 
			} 
		}
	}
}
void CTriangle2D::DrawTextureTrianglePerINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui << (FIXP28_SHIFT-FIXP22_SHIFT)/zi) 
							+ ((vi <<(FIXP28_SHIFT-FIXP22_SHIFT)/zi) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui << (FIXP28_SHIFT-FIXP22_SHIFT)/zi) 
							+ ((vi <<(FIXP28_SHIFT-FIXP22_SHIFT)/zi) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl ;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui << (FIXP28_SHIFT-FIXP22_SHIFT)/zi) 
							+ ((vi <<(FIXP28_SHIFT-FIXP22_SHIFT)/zi) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui << (FIXP28_SHIFT-FIXP22_SHIFT)/zi) 
							+ ((vi <<(FIXP28_SHIFT-FIXP22_SHIFT)/zi) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTrianglePerINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) 
							+ (((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) 
							+ (((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) 
							+ (((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ui = ul;
				vi = vl;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[((ui << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) 
							+ (((vi << (FIXP28_SHIFT-FIXP22_SHIFT))/zi) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

void CTriangle2D::DrawTextureTrianglePerLPINVZBAlpha16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy,dz;
	int dyl,dyr;
	int du,dv;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;
	int ul2,ur2,vl2,vr2;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];
	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		} 
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy; 

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}// end else bottom triangle

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >>FIXP22_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch;
			for (yi = ystart; yi<=yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;   
			dzdyl = ((tz2 - tz1) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;    
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zl>>6))<<FIXP16_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/(zr>>6))<<FIXP16_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur2 - ul2)/dx;
					dv = (vr2 - vl2)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur2 - ul2);
					dv = (vr2 - vl2);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						screenPtr[xi]=alphaSrc1[screenPtr[xi]]+
							alphaSrc2[textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)]];
						zptr[xi]=zi;
					}

					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				// test for yi hitting second region, if so change interpolant
				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

//平面着色纹理

void CTriangle2D::DrawTextureTrianglePerLPINVZBAlphaFS16(LPTriF4DV1 face,UCHAR* _destBuffer,int lpitch,UCHAR* zBuffer,int zpitch,int alpha)
{
	int v0=0,v1=1,v2=2;
	int temp=0;
	int tri_type=TRI_TYPE_NONE;
	int irestart=INTERP_LHS;

	int dx,dy;
	int dyl,dyr;
	int du,dv,dz;
	int xi,yi,zi;
	int ui,vi;
	int xstart,xend,ystart,yend,yrestart;
	int xl,dxdyl,xr,dxdyr,zl,dzdyl,zr,dzdyr;
	int dudyl,ul,dvdyl,vl,dudyr,ur,dvdyr,vr;
	int x0,y0,tz0,tu0,tv0,x1,y1,tz1,tu1,tv1,x2,y2,tz2,tu2,tv2;
	int ul2,ur2,vl2,vr2;

	USHORT rBase=0, gBase=0, bBase=0,rTextel=0, gTextel=0, bTextel=0, textel=0;

	USHORT* screenPtr=NULL;
	USHORT* screenLine=NULL;
	USHORT* textMap=(USHORT*)face->ptrTexture->buffer;
	USHORT* destBuffer=(USHORT*)_destBuffer;

	UINT* zbuffer=(UINT*)zBuffer;
	UINT* zptr=NULL;

	lpitch>>=1;
	zpitch>>=2;

	int textShift=gLog2[face->ptrTexture->width];

	// 三角形不在屏幕内
	if (((face->tvlist[0].y < g_rRectClip.top)  && 
		(face->tvlist[1].y < g_rRectClip.top)  &&
		(face->tvlist[2].y < g_rRectClip.top)) ||

		((face->tvlist[0].y > g_rRectClip.bottom)  && 
		(face->tvlist[1].y > g_rRectClip.bottom)  &&
		(face->tvlist[2].y > g_rRectClip.bottom)) ||

		((face->tvlist[0].x < g_rRectClip.left)  && 
		(face->tvlist[1].x < g_rRectClip.left)  &&
		(face->tvlist[2].x < g_rRectClip.left)) ||

		((face->tvlist[0].x > g_rRectClip.right)  && 
		(face->tvlist[1].x > g_rRectClip.right)  &&
		(face->tvlist[2].x > g_rRectClip.right)))
		return;

	//对y0,y1,y2排序
	if (face->tvlist[v0].y>face->tvlist[v1].y)
	{
		swap(v0,v1);
	}
	if (face->tvlist[v0].y>face->tvlist[v2].y)
	{
		swap(v0,v2);
	}
	if (face->tvlist[v1].y>face->tvlist[v2].y)
	{
		swap(v1,v2);
	}
	if ((int)(face->tvlist[v0].y+0.5)==(int)(face->tvlist[v1].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_TOP;
		if(face->tvlist[v0].x>face->tvlist[v1].x)
		{
			swap(v0,v1);
		}
	}
	else if ((int)(face->tvlist[v1].y+0.5)==(int)(face->tvlist[v2].y+0.5))
	{
		tri_type=TRI_TYPE_FLAT_BOTTOM;
		if (face->tvlist[v1].x>face->tvlist[v2].x)
		{
			swap(v1,v2);
		}
	}
	else
	{
		tri_type=TRI_TYPE_GENERAL;
	}

	x0  = (int)(face->tvlist[v0].x+0.5);
	y0  = (int)(face->tvlist[v0].y+0.5);

	tu0 = ((int)(face->tvlist[v0].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tv0 = ((int)(face->tvlist[v0].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v0].z+0.5);
	tz0 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v0].z+0.5);

	x1  = (int)(face->tvlist[v1].x+0.5);
	y1  = (int)(face->tvlist[v1].y+0.5);

	tu1 = ((int)(face->tvlist[v1].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5);
	tv1 = ((int)(face->tvlist[v1].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v1].z+0.5); 
	tz1 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v1].z+0.5);

	x2  = (int)(face->tvlist[v2].x+0.5);
	y2  = (int)(face->tvlist[v2].y+0.5);

	tu2 = ((int)(face->tvlist[v2].u0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tv2 = ((int)(face->tvlist[v2].v0+0.5)<<FIXP22_SHIFT)/(int)(face->tvlist[v2].z+0.5); 
	tz2 = (1<<FIXP28_SHIFT)/(int)(face->tvlist[v2].z+0.5);

	if (((x0==x1)&&(x1==x2))||((y0==y1)&&(y1==y2)))
	{
		return;
	}
	_RGB565FROM16BIT(face->litColor[0],&rBase,&gBase,&bBase);

	USHORT* alphaSrc1=(USHORT*)&AlphaTable[(NUM_ALPHA_LEVELS-1)-alpha][0];
	USHORT* alphaSrc2=(USHORT*)&AlphaTable[alpha][0];

	yrestart=y1;  //斜率改变点
	if (tri_type & TRI_TYPE_FLAGMASK)
	{
		if (tri_type == TRI_TYPE_FLAT_TOP)
		{
			dy=y2-y0;
			dxdyl=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu2-tu0)<<0)/dy;
			dvdyl=((tv2-tv0)<<0)/dy;
			dzdyl=((tz2-tz0)<<0)/dy;

			dxdyr=((x2-x1)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu1)<<0)/dy;
			dvdyr=((tv2-tv1)<<0)/dy;
			dzdyr=((tz2-tz1)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x1<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu1<<0);
				vr=dvdyr*dy+(tv1<<0);
				zr=dzdyr*dy+(tz1<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x1<<FIXP16_SHIFT);
				ur=(tu1<<0);
				vr=(tv1<<0);
				zr=(tz1<<0);

				ystart=y0;
			}
		}
		else
		{
			//平底三角形
			dy=y1-y0;
			dxdyl=((x1-x0)<<FIXP16_SHIFT)/dy;
			dudyl=((tu1-tu0)<<0)/dy;
			dvdyl=((tv1-tv0)<<0)/dy;
			dzdyl=((tz1-tz0)<<0)/dy;

			dxdyr=((x2-x0)<<FIXP16_SHIFT)/dy;
			dudyr=((tu2-tu0)<<0)/dy;
			dvdyr=((tv2-tv0)<<0)/dy;
			dzdyr=((tz2-tz0)<<0)/dy;

			//裁剪
			if (y0<g_rRectClip.top)
			{
				dy=g_rRectClip.top-y0;
				xl=dxdyl*dy+(x0<<FIXP16_SHIFT);
				ul=dudyl*dy+(tu0<<0);
				vl=dvdyl*dy+(tv0<<0);
				zl=dzdyl*dy+(tz0<<0);

				xr=dxdyr*dy+(x0<<FIXP16_SHIFT);
				ur=dudyr*dy+(tu0<<0);
				vr=dvdyr*dy+(tv0<<0);
				zr=dzdyr*dy+(tz0<<0);

				ystart=g_rRectClip.top;
			}
			else
			{
				//不需要裁剪
				xl=(x0<<FIXP16_SHIFT);
				ul=(tu0<<0);
				vl=(tv0<<0);
				zl=(tz0<<0);

				xr=(x0<<FIXP16_SHIFT);
				ur=(tu0<<0);
				vr=(tv0<<0);
				zr=(tz0<<0);

				ystart=y0;
			}
		}

		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}

		//水平裁剪
		if ((x0<g_rRectClip.left)||(x0>g_rRectClip.right-1)||
			(x1<g_rRectClip.left)||(x1>g_rRectClip.right-1)||
			(x2<g_rRectClip.left)||(x2>g_rRectClip.right-1))
		{
			screenPtr=destBuffer+ystart*lpitch;
			zptr=zbuffer+ystart*zpitch;

			for (yi=ystart;yi<yend;yi++)
			{
				xstart=((xl+FIXP16_ROUND_UP)>>FIXP16_SHIFT);
				xend=((xr+FIXP16_ROUND_UP)>>FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				}
				if (xstart<g_rRectClip.left)
				{
					dx=g_rRectClip.left-xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart=g_rRectClip.left;
				}
				if (xend>g_rRectClip.right-1)
				{
					xend=g_rRectClip.right-1;
				}

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
		else
		{
			// 没有裁剪
			screenPtr=destBuffer+(ystart*lpitch);
			zptr=zbuffer+ystart*zpitch; 

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				} 
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >>FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;
			}
		}
	}
	else if (tri_type==TRI_TYPE_GENERAL)
	{
		if ((yend=y2)>g_rRectClip.bottom-1)
		{
			yend=g_rRectClip.bottom-1;
		}
		if(y1<g_rRectClip.top)
		{
			dyl = (y2 - y1);

			dxdyl = ((x2  - x1)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu2 - tu1) << 0)/dyl;  
			dvdyl = ((tv2 - tv1) << 0)/dyl;    
			dzdyl = ((tz2 - tz1) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;  
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			dyl = (g_rRectClip.top - y1);
			dyr = (g_rRectClip.top - y0);

			xl = dxdyl*dyl + (x1  << FIXP16_SHIFT);
			ul = dudyl*dyl + (tu1 << 0);
			vl = dvdyl*dyl + (tv1 << 0);
			zl = dzdyl*dyl + (tz1 << 0);

			xr = dxdyr*dyr + (x0  << FIXP16_SHIFT);
			ur = dudyr*dyr + (tu0 << 0);
			vr = dvdyr*dyr + (tv0 << 0);
			zr = dzdyr*dyr + (tz0 << 0);

			ystart = g_rRectClip.top;

			//为了保证渲染从左到右
			if (dxdyr > dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else if (y0<g_rRectClip.top)
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl;

			dyr = (y2 - y0);	

			dxdyr = ((x2  - x0)  << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr; 
			dzdyr = ((tz2 - tz0) << 0)/dyr;  

			dy = (g_rRectClip.top - y0);

			xl = dxdyl*dy + (x0  << FIXP16_SHIFT);
			ul = dudyl*dy + (tu0 << 0);
			vl = dvdyl*dy + (tv0 << 0);
			zl = dzdyl*dy + (tz0 << 0);

			xr = dxdyr*dy + (x0  << FIXP16_SHIFT);
			ur = dudyr*dy + (tu0 << 0);
			vr = dvdyr*dy + (tv0 << 0);
			zr = dzdyr*dy + (tz0 << 0);

			ystart = g_rRectClip.top;

			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		else
		{
			dyl = (y1 - y0);

			dxdyl = ((x1  - x0)  << FIXP16_SHIFT)/dyl;
			dudyl = ((tu1 - tu0) << 0)/dyl;  
			dvdyl = ((tv1 - tv0) << 0)/dyl;  
			dzdyl = ((tz1 - tz0) << 0)/dyl; 

			dyr = (y2 - y0);	

			dxdyr = ((x2 - x0)   << FIXP16_SHIFT)/dyr;
			dudyr = ((tu2 - tu0) << 0)/dyr;  
			dvdyr = ((tv2 - tv0) << 0)/dyr;   		
			dzdyr = ((tz2 - tz0) << 0)/dyr;

			xl = (x0 << FIXP16_SHIFT);
			xr = (x0 << FIXP16_SHIFT);

			ul = (tu0 << 0);
			vl = (tv0 << 0);
			zl = (tz0 << 0);

			ur = (tu0 << 0);
			vr = (tv0 << 0);
			zr = (tz0 << 0);

			ystart = y0;

			//确保渲染从左到右
			if (dxdyr < dxdyl)
			{
				swap(dxdyl,dxdyr);
				swap(dudyl,dudyr);
				swap(dvdyl,dvdyr);
				swap(dzdyl,dzdyr);
				swap(xl,xr);
				swap(ul,ur);
				swap(vl,vr);
				swap(zl,zr);
				swap(x1,x2);
				swap(y1,y2);
				swap(tz1,tz2);
				swap(tu1,tu2);
				swap(tv1,tv2);

				irestart = INTERP_RHS;
			} 
		}
		if ((x0 < g_rRectClip.left) || (x0 >g_rRectClip.right-1) ||
			(x1 < g_rRectClip.left) || (x1 > g_rRectClip.right-1) ||
			(x2 < g_rRectClip.left) || (x2 >g_rRectClip.right-1))
		{
			//需要裁剪
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				if (xstart < g_rRectClip.left)
				{
					dx = g_rRectClip.left - xstart;

					ui+=dx*du;
					vi+=dx*dv;
					zi+=dx*dz;

					xstart = g_rRectClip.left;
				} 
				if (xend > g_rRectClip.right-1)
					xend = g_rRectClip.right-1;

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl; 
						dzdyl = ((tz2 - tz1) << 0)/dyl; 

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					}
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;  
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					}
				} 
			} 
		}
		else
		{
			screenPtr = destBuffer + (ystart * lpitch);
			zptr=zbuffer+ystart*zpitch;

			for (yi = ystart; yi<yend; yi++)
			{
				xstart = ((xl + FIXP16_ROUND_UP) >> FIXP16_SHIFT);
				xend   = ((xr + FIXP16_ROUND_UP) >> FIXP16_SHIFT);

				ul2=((ul<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				ur2=((ur<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;
				vl2=((vl<<(FIXP28_SHIFT-FIXP22_SHIFT))/zl)<<FIXP22_SHIFT;
				vr2=((vr<<(FIXP28_SHIFT-FIXP22_SHIFT))/zr)<<FIXP22_SHIFT;

				ui = ul2;
				vi = vl2;
				zi = zl;

				if ((dx = (xend - xstart))>0)
				{
					du = (ur - ul)/dx;
					dv = (vr - vl)/dx;
					dz = (zr - zl)/dx;
				}
				else
				{
					du = (ur - ul);
					dv = (vr - vl);
					dz = (zr - zl);
				} 

				for (xi=xstart; xi<xend; xi++)
				{
					if (zi>zptr[xi])
					{
						// RGB 5.6.5
						textel=textMap[(ui >> FIXP22_SHIFT) 
							+ ((vi >> FIXP22_SHIFT) << textShift)];

						rTextel=textel>>11;
						gTextel=((textel>>5) & 0x3f);
						bTextel=(textel & 0x1f);

						rTextel*=rBase;
						gTextel*=gBase;
						bTextel*=bBase;

						rTextel>>=5;
						gTextel>>=6;
						bTextel>>=5;

						screenPtr[xi]=alphaSrc1[screenPtr[xi]]
						+alphaSrc2[_RGB16BIT565(rTextel,gTextel,bTextel)];
						zptr[xi]=zi;
					}
					ui+=du;
					vi+=dv;
					zi+=dz;
				} 
				xl+=dxdyl;
				ul+=dudyl;
				vl+=dvdyl;
				zl+=dzdyl;

				xr+=dxdyr;
				ur+=dudyr;
				vr+=dvdyr;
				zr+=dzdyr;

				screenPtr+=lpitch;
				zptr+=zpitch;

				if (yi==yrestart)
				{
					if (irestart == INTERP_LHS)
					{
						// LHS
						dyl = (y2 - y1);	

						dxdyl = ((x2 - x1)   << FIXP16_SHIFT)/dyl;
						dudyl = ((tu2 - tu1) << 0)/dyl;  
						dvdyl = ((tv2 - tv1) << 0)/dyl;   	
						dzdyl = ((tz2 - tz1) << 0)/dyl;

						xl = (x1  << FIXP16_SHIFT);
						ul = (tu1 << 0);
						vl = (tv1 << 0);
						zl = (tz1 << 0);

						xl+=dxdyl;
						ul+=dudyl;
						vl+=dvdyl;
						zl+=dzdyl;
					} 
					else
					{
						// RHS
						dyr = (y1 - y2);	

						dxdyr = ((x1 - x2)   << FIXP16_SHIFT)/dyr;
						dudyr = ((tu1 - tu2) << 0)/dyr;  
						dvdyr = ((tv1 - tv2) << 0)/dyr;   
						dzdyr = ((tz1 - tz2) << 0)/dyr;

						xr = (x2  << FIXP16_SHIFT);
						ur = (tu2 << 0);
						vr = (tv2 << 0);
						zr = (tz2 << 0);

						xr+=dxdyr;
						ur+=dudyr;
						vr+=dvdyr;
						zr+=dzdyr;
					} 
				} 
			} 
		}
	}
}

int CTriangle2D::CreateMipmaps(CBitmapImage* source,CBitmapImage** mipmaps,float gamma)
{
	CBitmapImage** tmipmaps;  //指向指针数组的指针
	//1.计算mip等级
	int numMipLevels=gLog2[source->width]+1;
	//为指针数组分配内存
	tmipmaps=(CBitmapImage**)malloc(numMipLevels*sizeof(CBitmapImage*));
	//将元素0指向原始纹理
	tmipmaps[0]=source;
	//设置宽度和高度
	int mipWidth=source->width;
	int mipHeight=source->height;

	//使用平均滤波器生成各个mip纹理
	for (int mipLevel=1;mipLevel<numMipLevels;mipLevel++)
	{
		//计算下一个mip纹理的大小
		mipWidth>>=1;
		mipHeight>>=1;
		//为位图对象分配内存

		tmipmaps[mipLevel]=new CBitmapImage;
		//创建用于存储mip纹理的位图
		tmipmaps[mipLevel]->CreateBitmapImage(0,0,mipWidth,mipHeight,source->bpp);
		//让位图可用于渲染
		SET_BIT(tmipmaps[mipLevel]->attr,BITMAP_ATTR_LOADED);
		//遍历前一个mip纹理，使用平均滤波器创建当前mip纹理
		for(int x=0;x<tmipmaps[mipLevel]->width;x++)
		{
			for (int y=0;y<tmipmaps[mipLevel]->height;y++)
			{
				//需要计算4个纹理平均值，这些纹理在前一个mip纹理中的位置：
				//(x*2,y*2) (x*2+1,y*2) (x*2,y*2+1) (x*2+1,y*2+1)
				//然后将计算结果写入mip纹理（x，y）处
				float r0,g0,b0,r1,g1,b1,r2,g2,b2,r3,g3,b3;
				int r_avg,g_avg,b_avg;
				USHORT* scrBuffer=(USHORT*)tmipmaps[mipLevel-1]->buffer;
				USHORT* destBuffer=(USHORT*)tmipmaps[mipLevel]->buffer;
				//提取没个纹理的RGB值
				_RGB565FROM16BIT(scrBuffer[(x*2+0)+(y*2+0)*mipWidth*2],&r0,&g0,&b0);
				_RGB565FROM16BIT(scrBuffer[(x*2+1)+(y*2+0)*mipWidth*2],&r1,&g1,&b1);
				_RGB565FROM16BIT(scrBuffer[(x*2+0)+(y*2+1)*mipWidth*2],&r2,&g2,&b2);
				_RGB565FROM16BIT(scrBuffer[(x*2+1)+(y*2+1)*mipWidth*2],&r3,&g3,&b3);

				//计算平均值，考虑gamma参数
				r_avg=(int)(0.5f+gamma*(r0+r1+r2+r3)/4);
				g_avg=(int)(0.5f+gamma*(g0+g1+g2+g3)/4);
				b_avg=(int)(0.5f+gamma*(b0+b1+b2+b3)/4);

				if (r_avg>31)
				{
					r_avg=31;
				}
				if (g_avg>63)
				{
					g_avg=63;
				}
				if (b_avg>31)
				{
					b_avg=31;
				}
				destBuffer[x+y*mipWidth]=_RGB16BIT565(r_avg,g_avg,b_avg);
			}
		}
	}
	*mipmaps=(CBitmapImage*)tmipmaps;
	return numMipLevels;
}
