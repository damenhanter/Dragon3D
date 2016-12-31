#include "StdAfx.h"
#include "Vector2.h"

CVector2::CVector2(void)
:x(0.0f)
,y(0.0f)
{
}

CVector2::~CVector2(void)
{
}

CVector2 CVector2::operator *(const CMatrix3x2 &m)const
{
	CVector2 v;
	int j,k;
	for(j=0; j<2; j++)
	{
		for(k=0; k<2; k++)
		{
			v.M[j]+=M[k]*m.M[k][j];
		}
	}
	return v;
}