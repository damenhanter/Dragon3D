#include "StdAfx.h"
#include "BoundSphere.h"
#include "datatype.h"

CBoundSphere::CBoundSphere(void)
:x0(0),y0(0),radius(0)
{
}

CBoundSphere::~CBoundSphere(void)
{
}

int CBoundSphere::CreateColliCheck(int x0, int y0, int radius)
{
	this->x0=x0;this->y0=y0;this->radius=radius;
	return TRUE;
}
int CBoundSphere::CreateColliCheck(POINT point[],int NumVerts)
{
	int d,xmin,xmax,ymin,ymax,maxDistance=0;
	xmin=point[0].x;
	xmax=point[0].x;
	ymin=point[0].y;
	ymax=point[0].y;
	for(int index=1; index<NumVerts; index++)
	{
		if(point[index].x<xmin)
			xmin=point[index].x;
		if(point[index].x>xmax)
			xmax=point[index].x;
		if(point[index].y<ymin)
			ymin=point[index].y;
		if(point[index].y>ymax)
			ymax=point[index].y;
	}
	x0=(xmin+xmax)>>1;
	y0=(ymin+ymax)>>1;
	for(int index=0; index<NumVerts; index++)
	{
		d=Distance2D(point[index].x-x0,point[index].y-y0);
		if(d>maxDistance)
			maxDistance=d;
	}
	radius=maxDistance;
	return true;
}

int CBoundSphere::IsPointInside(int x, int y)
{
	int d=Distance2D(x-x0,y-y0);
	if(d>radius)
		return FALSE;
	else
		return TRUE;
}

BOOL CBoundSphere::IsCollision(CBoundSphere* Bounding)
{
	int d;
	d=Distance2D((x0-Bounding->x0),(y0-Bounding->y0));
	if(d>(radius+Bounding->radius))
		return FALSE;
	return TRUE;
}

int CBoundSphere::Distance2D(int x, int y)
{
	x=fabs(x);
	y=fabs(y);
	int mn=min(x,y);
	return (x+y -(mn>>1)-(mn>>2)+(mn>>4));
}

float CBoundSphere::Distance3D(float x, float y, float z)
{
	int _x = (int)fabs(x) * 1024;
	int _y = (int)fabs(y) * 1024;
	int _z = (int)fabs(z) * 1024;
	if (_y < x) swap(_x,_y);
	if (_z < y) swap(_y,_z);
	if (_y < x) swap(_x,_y);
    int dist = (_z + 11*(_y >> 5) + (_x >> 2) );
	return((float)(dist >> 10));
}
