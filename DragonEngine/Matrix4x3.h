#pragma once
#include "datatype.h"
#include "Vector4.h"

class CMatrix4x3
{
public:
	CMatrix4x3(void)
	{
		MemSetDWORD(M,0,12);
	}
	CMatrix4x3(const CMatrix4x3 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];
		M[3][0]  = m.M[3][0];M[3][1]  = m.M[3][1];M[3][2]  = m.M[3][2];
	}
	CMatrix4x3(float r11, float r12, float r13,
		float r21, float r22, float r23, 
		float r31, float r32, float r33, 
		float r41, float r42, float r43 )
	{
		M[0][0]  = r11; M[0][1]  = r12; M[0][2]  = r13;
		M[1][0]  = r21; M[1][1]  = r22; M[1][2]  = r23;
		M[2][0]  = r31; M[2][1]  = r32; M[2][2]  = r33;
		M[3][0]  = r41; M[3][1]  = r42; M[3][2]  = r43;
	}
	~CMatrix4x3(void);
	CMatrix4x3& operator=(const CMatrix4x3 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];
		M[3][0]  = m.M[3][0];M[3][1]  = m.M[3][1];M[3][2]  = m.M[3][2];
		return* this;
	}
	inline void Identity()
	{
		M[0][0]  = 1.0f; M[0][1]  = 0.0f; M[0][2]  = 0.0f;
		M[1][0]  = 0.0f; M[1][1]  = 1.0f; M[1][2]  = 0.0f;
		M[2][0]  = 0.0f; M[2][1]  = 0.0f; M[2][2]  = 1.0f;
		M[3][0]  = 0.0f; M[3][1]  = 0.0f; M[3][2]  = 0.0f;
	}
	
	/*inline void SetMatCol(int c,const CVector4& m)
	{
		M[0][c]=m.M[0];M[1][c]=m.M[1];M[2][c]=m.M[2];M[3][c]=m.M[3];
	}*/
public:
	union
	{
		float M[4][3];
		struct  
		{
			float M00,M01,M02;
			float M10,M11,M12;
			float M20,M21,M22;
			float M30,M31,M32;
		};
	};
};
