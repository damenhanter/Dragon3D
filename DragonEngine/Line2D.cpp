#include "StdAfx.h"
#include "Line2D.h"
extern RECT g_rRectClip;

CLine2D::CLine2D(void)
{
}

CLine2D::~CLine2D(void)
{
}

// »­Ö±Ïß

int CLine2D::DrawLine2D(int x1, int y1, int x2, int y2, UCHAR color, UCHAR* vb_start, int lpitch)
{
	int dx,dy,dk,xadd,yadd;
	dx=x2-x1;
	dy=y2-y1;

	if(dx>=0)
	{
		xadd=1;
	}
	else
	{
		xadd=-1;
		dx=-dx;
	}
	if(dy>=0)
	{
		yadd=lpitch;
	}
	else
	{
		yadd=-lpitch;
		dy=-dy;
	}
	vb_start=vb_start+x1+y1*lpitch;
	if(dx>=dy)
	{
		dk=(dy<<1)-dx;
		for(int i=0;i<dx;i++)
		{
			*vb_start=color;
			if(dk>=0)
			{
				vb_start+=yadd;
				dk=dk+(dy<<1)-(dx<<1);
			}
			else
				dk=dk+(dy<<1);
			vb_start+=xadd;
		}
	}
	else
	{
		dk=(dx<<1)-dy;
		for(int i=0;i<dy;i++)
		{
			*vb_start=color;
			if(dk>=0)
			{
				vb_start+=xadd;
				dk=dk+(dx<<1)-(dy<<1);
			}
			else
				dk=dk+(dx<<1);
			vb_start+=yadd;
		}
	}
	return true;
}

int CLine2D::DrawLine162D(int x1, int y1, int x2, int y2, int color, UCHAR* vb_start, int lpitch)
{
	int dx,dy,dk,xadd,yadd;
	lpitch>>=1;
	USHORT* vb_start16=(USHORT*)vb_start;

	dx=x2-x1;
	dy=y2-y1;
	if(dx>=0)
	{
		xadd=1;
	}
	else
	{
		xadd=-1;
		dx=-dx;
	}
	if(dy>=0)
	{
		yadd=lpitch;
	}
	else
	{
		yadd=-lpitch;
		dy=-dy;
	}
	vb_start16=vb_start16+x1+y1*lpitch;
	if(dx>dy)
	{
		dk=(dy<<1)-dx;
		for(int i=0;i<=dx;i++)
		{
			*vb_start16=color;
			if(dk>=0)
			{
				vb_start16+=yadd;
				dk=dk+(dy<<1)-(dx<<1);
			}
			else
				dk=dk+(dy<<1);
			vb_start16+=xadd;
		}
	}
	else
	{
		dk=(dx<<1)-dy;
		for(int i=0;i<=dy;i++)
		{
			*vb_start16=color;
			if(dk>=0)
			{
				vb_start16+=xadd;
				dk=dk+(dx<<1)-(dy<<1);
			}
			else
				dk=dk+(dx<<1);
			vb_start16+=yadd;
		}
	}
	return true;
}

int CLine2D::IsInArea2D(int x,int y)
{
	if(x >= g_rRectClip.left && x <=  g_rRectClip.right-2  &&  
		y <= g_rRectClip.bottom-2 && y >= g_rRectClip.top)
		return true;
	else
		return false;
}

int CLine2D::NotIntersect2D(int x1,int y1,int x2,int y2)
{
	int maxx,maxy,minx,miny;
	maxx = (x1>x2) ? x1 : x2 ;
	minx = (x1<x2) ? x1 : x2 ;
	maxy = (y1>y2) ? y1 : y2 ;
	miny = (y1<y2) ? y1 : y2 ;

	if(maxx<=g_rRectClip.left || minx >= g_rRectClip.right-1 || 
		miny >= g_rRectClip.bottom-1 || maxy<= g_rRectClip.top )
		return TRUE;
	else 
		return FALSE;
}

POINT CLine2D::ClipMid2D(int x1,int y1,int x2,int y2)
{
	POINT mid,temp;
	if(IsInArea2D(x1,y1))
	{
		temp.x=x1;
		temp.y=y1;
	}
	else if(NotIntersect2D(x1,y1,x2,y2))
		return POINT();
	else
	{
		mid.x=(x1+x2)/2;
		mid.y=(y1+y2)/2;
		if(abs(mid.x-x2)<=1 && abs(mid.y-y2)<=1)
		{
			temp=mid;
		}
		else
		{
			if(NotIntersect2D(x1,y1,mid.x,mid.y))
				temp=ClipMid2D(mid.x,mid.y,x2,y2);
			else
				temp=ClipMid2D(x1,y1,mid.x,mid.y);
		}
	}
	return temp;
}



int CLine2D::ClipLine2D(int& x1, int& y1, int& x2, int& y2)
{
	POINT p1,p2;
	p1=ClipMid2D(x1,y1,x2,y2);
	p2=ClipMid2D(x2,y2,x1,y1);
	x1=p1.x;y1=p1.y;
	x2=p2.x;y2=p2.y;
	return true;
}

int CLine2D::DrawClipLine2D(int x1,int y1,int x2,int y2,
							int color, UCHAR* vb_start, int lpitch)
{
	ClipLine2D(x1,y1,x2,y2);
	DrawLine2D(x1,y1,x2,y2,color,vb_start,lpitch);
	return true;
}

int CLine2D::DrawClipLine162D(int x1,int y1,int x2,int y2,
							int color, UCHAR* vb_start, int lpitch)
{ 
	ClipLine2D(x1,y1,x2,y2);
	DrawLine162D(x1,y1,x2,y2,color,vb_start,lpitch);
	return true;
}