#pragma once
#include "datatype.h"
#include "Vector3.h"

//////////////////////////////////////////////////////////////////////////
//3D球面坐标
//////////////////////////////////////////////////////////////////////////

class CSpherical3D
{
public:
	CSpherical3D(void);
	CSpherical3D(float p0,float theta0,float phi0)
	{
		p=p0;theta=theta0;phi=phi0;
	}
	CSpherical3D(CSpherical3D& src)
	{
		p=src.p;theta=src.theta;phi=src.phi;
	}
	~CSpherical3D(void);

	CSpherical3D& operator=(CSpherical3D& src)
	{
		p=src.p;theta=src.theta;phi=src.phi;
		return *this;
	}

	inline CPoint3D Spherical3DToPoint3D()const
	{
		float z=p*cosf(phi);
		float r=p*sinf(phi);
		float x=r*cosf(theta);
		float y=r*sinf(theta);
		return CPoint3D(x,y,z);
	}
	inline void Spherical3DToPoint3D(float& x,float& y,float& z)const
	{
		float r=p*sinf(phi);
		z=p*cosf(phi);
		x=r*cosf(theta);
		y=r*sinf(theta);
	}
	inline void Point3DToSpherical3D(const CPoint3D& src)
	{
		float r=sqrtf(src.x*src.x+src.y*src.y);
		theta=atanf(src.y/src.x);
		phi=atanf(r/src.z);
		p=sqrtf(src.z*src.z+r*r);
	}
public:
	float p;        //到原点的距离
	float theta;     //线段op在xy面上的投影与正x轴之间的夹角
	float phi;       //线段op到z轴的夹角
};
