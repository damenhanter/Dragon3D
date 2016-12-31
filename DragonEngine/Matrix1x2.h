#pragma once
#include "datatype.h"
#include "Matrix3x2.h"

class CMatrix1x2
{
public:
	CMatrix1x2(void)
	{
		MemSetDWORD(M,0,2);
	}
	CMatrix1x2(const CMatrix1x2 &m)
	{
		M[0]=m.M[0];M[1]=m.M[1];
	}
	CMatrix1x2(float r11, float r12,float r13)
	{
		M[0]=r11;M[1]=r12;
	}
	CMatrix1x2& operator=(const CMatrix1x2& m)
	{
		M[0]=m.M[0];M[1]=m.M[1];
		return *this;
	}
	~CMatrix1x2(void);
	CMatrix1x2 operator*(CMatrix3x2& m)const;
public:
	union
	{
		float M[2];
		struct  
		{
			float M00,M01;
		};
	};
	
};
