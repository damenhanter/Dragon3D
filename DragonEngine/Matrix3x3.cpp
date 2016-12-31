#include "StdAfx.h"
#include "Matrix3x3.h"


CMatrix3x3::~CMatrix3x3(void)
{
}

CMatrix3x3 CMatrix3x3::operator*(const CMatrix3x3& m)
{
	//TODO: insert return statement here
	int i,j,k;
	CMatrix3x3 mat;
	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)
		{
			for(k=0; k<3; k++)
			{
				mat.M[i][j]+=M[i][k]*m.M[k][j];
			}
		}
	}
	return mat;
}

