#include "StdAfx.h"
#include "Matrix1x4.h"


CMatrix1x4::~CMatrix1x4(void)
{
}

CMatrix1x4 CMatrix1x4::operator *(const CMatrix4x4 &m)const
{
	int j,k;
	CMatrix1x4 mat;
	for(j=0; j<4; j++)
	{
		for(k=0; k<4; k++)
		{
			mat.M[j]+=M[k]*m.M[k][j];
		}
	}
	return mat;
}