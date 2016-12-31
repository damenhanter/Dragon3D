#pragma once
#include "datatype.h"
#include "Vector2.h"

class CMatrix2x2
{
public:
	CMatrix2x2(void)
	{
		MemSetDWORD(M,0,4);
	}
	CMatrix2x2(const CMatrix2x2 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];
	}
	CMatrix2x2(float r11, float r12,
		float r21, float r22 )
	{
		M[0][0]  = r11; M[0][1]  = r12; 
		M[1][0]  = r21; M[1][1]  = r22; 
	}
	~CMatrix2x2(void);
	CMatrix2x2& operator=(const CMatrix2x2 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];
		return* this;
	}
	CMatrix2x2 operator+(const CMatrix2x2& m)const
	{
		return CMatrix2x2(M[0][0]+m.M[0][0],M[0][1]+m.M[0][1],
			M[1][0]+m.M[1][0],M[1][1]+m.M[1][1]);
	}
	CMatrix2x2 operator*(const CMatrix2x2& m)const
	{
		CMatrix2x2 tem;
		tem.M[0][0]=M[0][0]*m.M[0][0]+M[0][1]*m.M[1][0];
		tem.M[0][1]=M[0][0]*m.M[0][1]+M[0][1]*m.M[1][1];
		tem.M[1][0]=M[1][0]*m.M[0][0]+M[1][1]*m.M[1][0];
		tem.M[1][1]=M[1][0]*m.M[0][1]+M[1][1]*m.M[1][1];
		return tem;
	}
	inline void Identity()
	{
		M[0][0]  = 1.0f; M[0][1]  = 0.0f;
		M[1][0]  = 0.0f; M[1][1]  = 1.0f;
	}

	inline void Transpose()
	{
		CMatrix2x2 mt;
		mt.M[0][0]=M[0][0];mt.M[0][1]=M[1][0];
		mt.M[1][0]=M[0][1];mt.M[1][1]=M[1][1];
		*this=mt;
	}
	//”√2Œ¨æÿ’Û
	inline void SetMatCol(int c,const CVector2& v)
	{
		M[0][c]=v.M[0];M[1][c]=v.M[1];
	}
	inline float MatrixDet()const
	{
		return M[0][0]*M[1][1]-M[0][1]*M[1][0];
	}
	inline CMatrix2x2 MatrixInverse()const
	{
		CMatrix2x2 tem;
		float det = MatrixDet();
		if (fabs(det) < EPSILON_E5)
			return CMatrix2x2();

		float det_inv = 1.0/det;
		tem.M[0][0]=  M11*det_inv;
		tem.M[0][1]= -M01*det_inv;
		tem.M[1][0]= -M10*det_inv;
		tem.M[1][1]=  M00*det_inv;
		return tem;
	}
public:
	union
	{
		float M[2][2];
		struct  
		{
			float M00,M01;
			float M10,M11;
		};
	};
};
