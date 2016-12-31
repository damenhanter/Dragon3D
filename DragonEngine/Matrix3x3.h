#pragma once
#include "datatype.h"
#include "matrix1x3.h"


class CMatrix3x3
{
public:
	CMatrix3x3(void)
	{
		MemSetDWORD(M,0,9);
	}
	CMatrix3x3(const CMatrix3x3 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];
	}
	CMatrix3x3(float r11, float r12, float r13,
		float r21, float r22, float r23, 
		float r31, float r32, float r33 )
	{
		M[0][0]  = r11; M[0][1]  = r12; M[0][2]  = r13;
		M[1][0]  = r21; M[1][1]  = r22; M[1][2]  = r23;
		M[2][0]  = r31; M[2][1]  = r32; M[2][2]  = r33;
	}
	~CMatrix3x3(void);
	CMatrix3x3& operator=(const CMatrix3x3 &m)
	{
		M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];
		M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];
		M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];
		return* this;
	}
	
	CMatrix3x3 operator*(const CMatrix3x3& m);

	inline void Identity()
	{
		M[0][0]  = 1.0f; M[0][1]  = 0.0f; M[0][2]  = 0.0f;
		M[1][0]  = 0.0f; M[1][1]  = 1.0f; M[1][2]  = 0.0f;
		M[2][0]  = 0.0f; M[2][1]  = 0.0f; M[2][2]  = 1.0f;
	}

	inline void Transpose()
	{
		CMatrix3x3 mt;
		mt.M[0][0]=M[0][0];mt.M[0][1]=M[1][0];mt.M[0][2]=M[2][0];
		mt.M[1][0]=M[0][1];mt.M[1][1]=M[1][1];mt.M[1][2]=M[2][1];
		mt.M[2][0]=M[0][2];mt.M[2][1]=M[1][2];mt.M[2][2]=M[2][2];
		*this=mt;
	}

	/*inline void SetMatCol(int c,const CMatrix1x3& v)
	{
		M[0][c]=v.M[0];
		M[1][c]=v.M[1];
		M[2][c]=v.M[2];
	}*/

	inline CMatrix3x3 operator+(const CMatrix3x3& m)const
	{
		CMatrix3x3 tem;
		tem.M[0][0]=M[0][0]+m.M[0][0];tem.M[0][1]=M[0][1]+m.M[0][1];tem.M[0][2]=M[0][2]+m.M[0][2];
		tem.M[1][0]=M[1][0]+m.M[1][0];tem.M[1][1]=M[1][1]+m.M[1][1];tem.M[1][2]=M[1][2]+m.M[1][2];
		tem.M[2][0]=M[2][0]+m.M[2][0];tem.M[2][1]=M[2][1]+m.M[2][1];tem.M[2][2]=M[2][2]+m.M[2][2];
		return tem;
	}
	
	inline float MatrixDet()const
	{
		return(M00*(M11*M22 -M21*M12) - 
			M01*(M10*M22 -M20*M12) + 
			M02*(M10*M21 -M20*M11) );
	}
	inline CMatrix3x3 MatrixInverse()const
	{
		CMatrix3x3 mt;
		float det = MatrixDet();
		if (fabs(det) < EPSILON_E5)
			return CMatrix3x3();

		float det_inv = 1.0/det;

		mt.M00 =  det_inv*(M11*M22 -M21*M12);
		mt.M10 = -det_inv*(M10*M22 -M20*M12);
		mt.M20 =  det_inv*(M10*M21 -M20*M11);

		mt.M01 = -det_inv*(M01*M22 -M21*M02);
		mt.M11 =  det_inv*(M00*M22 -M20*M02);
		mt.M21 = -det_inv*(M00*M21 -M20*M01);

		mt.M02 =  det_inv*(M01*M12 -M11*M02);
		mt.M12 = -det_inv*(M00*M12 -M10*M02);
		mt.M22 =  det_inv*(M00*M11 -M10*M01);
		return mt;
	}
public:
	union
	{
		float M[3][3];
		struct  
		{
			float M00,M01,M02,M10,M11,M12,M20,M21,M22;
		};
	};
	
};
