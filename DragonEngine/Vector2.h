#pragma once
#include "datatype.h"
#include "Matrix3x2.h"
//////////////////////////////////////////////////////////////////////////
//不包含w分量的2D向量和点
//私有变量中使用联合来构造变量，意在支持更多的数据访问方法
//使编写的算法清晰
//////////////////////////////////////////////////////////////////////////

class CVector2
{
public:
	CVector2(void);
	CVector2(float x,float y)
	{
		this->x=x;
		this->y=y;
	}
	CVector2(const CVector2& vsrc)
	{
		x=vsrc.x;
		y=vsrc.y;
	}
	CVector2& operator=(const CVector2& vsrc)
	{
		x=vsrc.x;
		y=vsrc.y;
		return *this;
	}
	~CVector2(void);
	
	inline void CreateVector(const CVector2& p0,const CVector2& p1)
	{
		x=p1.x-p0.x;
		y=p1.y-p0.y;
	}
	inline CVector2 operator+(const CVector2& vsrc)const
	{
		return CVector2(x+vsrc.x,y+vsrc.y);
	}
	inline CVector2 operator-(const CVector2& vsrc)const
	{
		return CVector2(x-vsrc.x,y-vsrc.y);
	}
	inline CVector2 operator*(const float k)const
	{
		return CVector2(x*k,y*k);
	}
	inline float DotProduct(const CVector2& vsrc)const
	{
		return x*vsrc.x+y*vsrc.y;
	}
	inline float GetLength()const
	{
		return Distance2D(x,y);
	}
	inline void Normalize()
	{
		float length=GetLength();
		if (length<EPSILON_E6)
		{
			return ;
		}
		length=1.0/length;
		x*=length;
		y*=length;
	}
	inline float GetCosth(const CVector2& v)const
	{
		float a=DotProduct(v);
		float costh=a/(GetLength()*v.GetLength());
		return costh;
	}

	CVector2 operator*(const CMatrix3x2& m)const;
public:
	union
	{
		float M[2];
		struct  
		{
			float x,y;
		};
	};
};

typedef CVector2 CPoint2D;