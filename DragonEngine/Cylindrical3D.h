#pragma once
#include "datatype.h"
#include "Vector3.h"

//////////////////////////////////////////////////////////////////////////
//3DÖùÃæ×ø±ê
//////////////////////////////////////////////////////////////////////////
class CCylindrical3D
{
public:
	CCylindrical3D(void);
	CCylindrical3D(float r0,float theta0,float z0)
	{
		r=r0;theta=theta0;z=z0;
	}
	CCylindrical3D(CCylindrical3D& src)
	{
		r=src.r;
		theta=src.theta;
		z=src.z;
	}
	~CCylindrical3D(void);

	CCylindrical3D& operator=(CCylindrical3D& src)
	{
		r=src.r;
		theta=src.theta;
		z=src.z;
		return *this;
	}

	inline CPoint3D Cylindrical3DToPoint3D()const
	{
		return CPoint3D(r*cosf(theta),r*sinf(theta),z);
	}
	inline void Cylindrical3DToPoint3D(float& x,float& y,float& z)const
	{
		x=r*cosf(theta);
		y=r*sinf(theta);
		z=this->z;
	}
	inline void Point3DToCylindrical3D(const CPoint3D& src)
	{
		r=sqrtf(src.x*src.x+src.y*src.y);
		theta=atanf(src.y/src.x);
		z=src.z;
	}
public:
	float r;      //°ë¾¶
	float theta;  //ÓëzÖá¼Ğ½Ç
	float z;       //z×ø±ê
};
