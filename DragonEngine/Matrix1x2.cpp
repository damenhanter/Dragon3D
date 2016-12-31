#include "StdAfx.h"
#include "Matrix1x2.h"
#include "Matrix3x2.h"

CMatrix1x2::~CMatrix1x2(void)
{
}

CMatrix1x2 CMatrix1x2::operator*(CMatrix3x2& m)const
{
	CMatrix1x2 mat;
	int j,k;
	for(j=0; j<2; j++)
	{
		for(k=0; k<2; k++)
		{
			mat.M[j]+=M[k]*m.M[k][j];
		}
	}
	return mat;
}
