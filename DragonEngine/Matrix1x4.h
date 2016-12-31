#pragma once
#include "datatype.h"
#include "Matrix4x4.h"
class CMatrix4x4;

class CMatrix1x4
{
public:
	CMatrix1x4(void)
	{
		MemSetDWORD(M,0,4);
	}
	CMatrix1x4(const CMatrix1x4 &m)
	{
		M[0]=m.M[0];M[1]=m.M[1];M[2]=m.M[2];M[3]=m.M[3];
	}
	CMatrix1x4(float r11, float r12,float r13, float r14 )
	{
		M[0]=r11;M[1]=r12;M[2]=r13;M[3]=r14; 
	}
	CMatrix1x4& operator=(const CMatrix1x4& m)
	{
		M[0]=m.M[0];M[1]=m.M[1];M[2]=m.M[2];M[3]=m.M[3];
		return *this;
	}
	~CMatrix1x4(void);

	CMatrix1x4 operator*(const CMatrix4x4& m)const;
	
public:
	union
	{
		float M[4];
		struct  
		{
			float M00,M01,M02,M03;
		};
	};
};
