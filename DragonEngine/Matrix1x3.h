#pragma once
#include "datatype.h"
#include "Matrix3x3.h"

class CMatrix3x3;

class CMatrix1x3
{
public:
	CMatrix1x3(void)
	{
		MemSetDWORD(M,0,3);
	};
	CMatrix1x3(const CMatrix1x3 &m)
	{
		M[0]=m.M[0];M[1]=m.M[1];M[2]=m.M[2];
	}
	CMatrix1x3(float r11, float r12,float r13)
	{
		M[0]=r11;M[1]=r12;M[2]=r13;
	}
	CMatrix1x3& operator=(const CMatrix1x3& m)
	{
		M[0]=m.M[0];M[1]=m.M[1];M[2]=m.M[2];
		return *this;
	}
	~CMatrix1x3(void);

	CMatrix1x3 operator*(CMatrix3x3& m)const;
public:
	union
	{
		float M[3];
		struct  
		{
			float M00,M01,M02;
		};
	};
};
