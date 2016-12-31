#pragma once
#include "datatype.h"

class CMatrix3x2
{
public:
	CMatrix3x2(void)
	{
		MemSetDWORD(M,0,6);
	}
	CMatrix3x2(const CMatrix3x2 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];
	}
	CMatrix3x2(float r11, float r12,
		float r21, float r22,
		float r31, float r32)
	{
		M[0][0]  = r11; M[0][1]  = r12;
		M[1][0]  = r21; M[1][1]  = r22; 
		M[2][0]  = r31; M[2][1]  = r32; 
	}
	~CMatrix3x2(void);
	CMatrix3x2& operator=(const CMatrix3x2 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];
		return* this;
	}


	inline void Identity()
	{
		M[0][0]  = 1.0f; M[0][1]  = 0.0f;
		M[1][0]  = 0.0f; M[1][1]  = 1.0f; 
		M[2][0]  = 0.0f; M[2][1]  = 0.0f; 
	}
public:
	union
	{
		float M[3][2];
		struct
		{
			float M00,M01,M10,M11,M20,M21;
		};
	};
	
};
