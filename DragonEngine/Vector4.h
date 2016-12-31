#pragma once

#include "datatype.h"
#include "Matrix4x4.h"
#include "Matrix4x3.h"

//////////////////////////////////////////////////////////////////////////
//包含w分量的4D齐次向量和点
//私有变量中使用联合来构造变量，意在支持更多的数据访问方法，
//使编写的算法清晰
//////////////////////////////////////////////////////////////////////////

class CMatrix4x4;
class CMatrix4x3;

class CVector4
{
public:
	CVector4(void);
	CVector4(float x,float y,float z)
	{
		this->x=x;
		this->y=y;
		this->z=z;
		w=1.0f;
	}
	CVector4(const CVector4& vscr)
	{
		x=vscr.x;
		y=vscr.y;
		z=vscr.z;
		w=vscr.w;
	}
	CVector4& operator=(const CVector4& vsrc)
	{
		x=vsrc.x;
		y=vsrc.y;
		z=vsrc.z;
		w=vsrc.w;
		return *this;
	}
	~CVector4(void);
	
	inline void ConvertToNotHomogeneous()
	{
		x/=w;
		y/=w;
		z/=w;
		w=1.0;
	}
	inline void CreateVector(const CVector4& p0,const CVector4& p1)
	{
		x=p1.x-p0.x;
		y=p1.y-p0.y;
		z=p1.z-p0.z;
	}
	inline CVector4 operator+(const CVector4& vsrc)const
	{
		return CVector4(x+vsrc.x,y+vsrc.y,z+vsrc.z);
	}
	
	inline CVector4 operator-(const CVector4& vsrc)const
	{
		return CVector4(x-vsrc.x,y-vsrc.y,z-vsrc.z);
	}
	inline CVector4 operator*(const float k)
	{
		return CVector4(x*k,y*k,z*k);
	}
	inline BOOL operator==(const CVector4& vsrc)const
	{
		if ((x==vsrc.x)&&(y==vsrc.y)&&(z==vsrc.z))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	inline CVector4 operator*(const float k)const
	{
		return CVector4(x*k,y*k,z*k);
	}
	CVector4 operator*(const CMatrix4x4& m);
	CVector4 operator*(const CMatrix4x4& m)const;
	CVector4 operator*(const CMatrix4x3& m);
	CVector4 operator*(const CMatrix4x3& m)const;


	inline float DotProduct(const CVector4& v)const
	{
		return (x*v.x+y*v.y+z*v.z);
	}
	inline CVector4 CrossProduct(const CVector4& v)const
	{
		return CVector4((y*v.z-z*v.y),(z*v.x-x*v.z),(x*v.y-y*v.x));
	}
	inline float GetLength()const
	{
		//return Distance3D(x,y,z);
		return sqrtf(x*x+y*y+z*z);
	}
	inline void Normalize()
	{
		//float length=Distance3D(x,y,z);
		float length=sqrtf(x*x+y*y+z*z);
		if (length<EPSILON_E6)
		{
			return ;
		}
		length=1.0/length;
		x*=length;
		y*=length;
		z*=length;
	}
	inline float GetCosth(const CVector4& v)const
	{
		float a=DotProduct(v);
		return a/(GetLength()*v.GetLength());
	}
public:
	union
	{
		float M[4];
		struct  
		{
			float x,y,z,w;
		};
	};
};

typedef CVector4 CPoint4D;