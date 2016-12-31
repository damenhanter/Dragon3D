#include "stdafx.h"
#include "Matrix4x4.h"


CMatrix4x4 CMatrix4x4::operator -(const CMatrix4x4 &m)const
{
	return CMatrix4x4(M[0][0] - m.M[0][0], M[0][1] - m.M[0][1], M[0][2] - m.M[0][2],
		M[0][3] - m.M[0][3], M[1][0] - m.M[1][0], M[1][1] - m.M[1][1],
		M[1][2] - m.M[1][2], M[1][3] - m.M[1][3], M[2][0] - m.M[2][0],
		M[2][1] - m.M[2][1], M[2][2] - m.M[2][2], M[2][3] - m.M[2][3],
		M[3][0] - m.M[3][0], M[3][1] - m.M[3][1],
		M[3][2] - m.M[3][2], M[3][3] - m.M[3][3]);
}


CMatrix4x4 CMatrix4x4::operator +(const CMatrix4x4 &m)const
{
	return CMatrix4x4(M[0][0] + m.M[0][0], M[0][1] + m.M[0][1], M[0][2] + m.M[0][2],
		M[0][3] + m.M[0][3], M[1][0] + m.M[1][0], M[1][1] + m.M[1][1],
		M[1][2] + m.M[1][2], M[1][3] + m.M[1][3], M[2][0] + m.M[2][0],
		M[2][1] + m.M[2][1], M[2][2] + m.M[2][2], M[2][3] + m.M[2][3],
		M[3][0] + m.M[3][0], M[3][1] + m.M[3][1],
		M[3][2] + m.M[3][2], M[3][3] + m.M[3][3]);
}


CMatrix4x4 CMatrix4x4::operator *(const CMatrix4x4 &m)const
{
	int i,j,k;
	CMatrix4x4 mat; 
	for(i=0; i<4; i++)
	{
		for(j=0; j<4; j++)
		{
			mat.M[i][j]=0;
			for(k=0; k<4; k++)
			{
				mat.M[i][j]=mat.M[i][j]+M[i][k]*m.M[k][j];
			}
		}
	}
	return mat;
}

CMatrix4x4 CMatrix4x4::operator *(const float f)const
{
	return CMatrix4x4(M[0][0]  * f, M[0][1]  * f, M[0][2]  * f, M[0][3]  * f,
		M[1][0]  * f, M[1][1]  * f, M[1][2]  * f, M[1][3]  * f,
		M[2][0]  * f, M[2][1]  * f, M[2][2]  * f, M[2][3]  * f,
		M[3][0]  * f, M[3][1]  * f, M[3][2]  * f, M[3][3]  * f);
}


CMatrix4x4 CMatrix4x4::operator /(float f)const
{
	if(f == 0) f = 1;
	f = 1/f;

	return CMatrix4x4(M[0][0]  * f, M[0][1]  * f, M[0][2]  * f, M[0][3]  * f,
		M[1][0]  * f, M[1][1]  * f, M[1][2]  * f, M[1][3]  * f,
		M[2][0]  * f, M[2][1]  * f, M[2][2]  * f, M[2][3]  * f,
		M[3][0]  * f, M[3][1]  * f, M[3][2]  * f, M[3][3]  * f);
}


void CMatrix4x4::operator +=(const CMatrix4x4 &m)
{
	(*this) = (*this) + m;
}


void CMatrix4x4::operator -=(const CMatrix4x4 &m)
{
	(*this) = (*this) - m;
}


void CMatrix4x4::operator *=(const CMatrix4x4 &m)
{
	(*this) = (*this) * m;
}


void CMatrix4x4::operator *=(const float f)
{
	(*this) = (*this) * f;
}


void CMatrix4x4::operator /=(const float f)
{
	(*this) = (*this) / f;
}


void CMatrix4x4::Translate(float x, float y, float z)
{
	M[3][0] = x;
	M[3][1] = y;
	M[3][2] = z;
	M[3][3] = 1.0f;
}


void CMatrix4x4::inverseTranslate()
{
	M[3][0] = -M[3][0];
	M[3][1] = -M[3][1];
	M[3][2] = -M[3][2];
}

CMatrix4x4 CMatrix4x4::MatrixInverse()const
{
	CMatrix4x4 mat;
	float det = MatrixDet();
	if (fabs(det) < EPSILON_E5)
		return CMatrix4x4();

	float det_inv  = 1.0f / det;

	mat.M00 =  det_inv * ( M11 * M22 - M12 * M21 );
	mat.M01 = -det_inv * ( M01 * M22 - M02 * M21 );
	mat.M02 =  det_inv * ( M01 * M12 - M02 * M11 );
	mat.M03 = 0.0f; 

	mat.M10 = -det_inv * ( M10 * M22 - M12 * M20 );
	mat.M11 =  det_inv * ( M00 * M22 - M02 * M20 );
	mat.M12 = -det_inv * ( M00 * M12 - M02 * M10 );
	mat.M13 = 0.0f;

	mat.M20 =  det_inv * ( M10 * M21 - M11 * M20 );
	mat.M21 = -det_inv * ( M00 * M21 - M01 * M20 );
	mat.M22 =  det_inv * ( M00 * M11 - M01 * M10 );
	mat.M23 = 0.0f;

	mat.M30 = -(M30 *mat.M00 + M31 * mat.M10 +M32 * mat.M20 );
	mat.M31 = -(M30 *mat.M01 + M31 * mat.M11 +M32 * mat.M21 );
	mat.M32 = -(M30 *mat.M02 + M31 * mat.M12 +M32 * mat.M22 );
	mat.M33 = 1.0f;
	return mat;
}

float CMatrix4x4::MatrixDet()const
{
	float det=( M00 * ( M11 * M22 - M12 * M21 ) -
		M01 * ( M10 * M22 - M12 * M20 ) +
		M02 * ( M10 * M21 - M11 * M20 ) );
	return det;

}