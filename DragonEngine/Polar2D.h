#pragma once
#include "datatype.h"
#include "Vector2.h"

//////////////////////////////////////////////////////////////////////////
//2D¼«×ø±ê
//////////////////////////////////////////////////////////////////////////

class CPolar2D
{
public:
	CPolar2D(void);
	CPolar2D(float r0,float theta0)
	{
		r=r0;theta=theta0;
	}
	CPolar2D(CPolar2D& src)
	{
		r=src.r;theta=src.theta;
	}
	~CPolar2D(void);

	CPolar2D& operator=(CPolar2D& src)
	{
		r=src.r;theta=src.theta;
		return *this;
	}

	inline CPoint2D Polar2DToPoint2D()const
	{
		return CPoint2D(r*cosf(theta),r*sinf(theta));
	}
	inline void Polar2DToPoint2D(float& x,float& y)const
	{
		x=r*cosf(theta);
		y=r*sinf(theta);
	}
	inline void Point2DToPolar2D(const CPoint2D& src)
	{
		r=sqrtf(src.x*src.x+src.y*src.y);
		theta=atanf(src.y/src.x);
	}
	
public:
	float r; //°ë¾¶
	float theta;   //½Ç¶È
};
