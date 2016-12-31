#include "StdAfx.h"
#include "Matrix1x3.h"
#include "Matrix3x3.h"


CMatrix1x3::~CMatrix1x3(void)
{
}

 CMatrix1x3 CMatrix1x3::operator*(CMatrix3x3& m)const
 {
 	//TODO: insert return statement here
 	int j,k;
 	CMatrix1x3 mat;
 	for(j=0; j<3; j++)
 	{
 		for(k=0; k<3; k++)
 		{
 			mat.M[j]+=M[k]*m.M[k][j];
 		}
 	}
 	return mat;
 }
