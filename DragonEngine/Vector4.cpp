#include "StdAfx.h"
#include "Vector4.h"

CVector4::CVector4(void)
:x(0.0f)
,y(0.0f)
,z(0.0f)
,w(1.0f)
{
}

CVector4::~CVector4(void)
{
}

CVector4 CVector4::operator*(const CMatrix4x4 &m)
{
	int j,k;
	CVector4 v;
	v.M[3]=0;
	for(j=0; j<4; j++)
	{
		for(k=0; k<4; k++)
		{
			v.M[j]+=M[k]*m.M[k][j];
		}
	}
	return v;
}

 CVector4 CVector4::operator*(const CMatrix4x4 &m)const
 {
 	int j,k;
 	CVector4 v;
 	v.M[3]=0;
 	for(j=0; j<4; j++)
 	{
 		for(k=0; k<4; k++)
 		{
 			v.M[j]+=M[k]*m.M[k][j];
 		}
 	}
 	return v;
 }

 CVector4 CVector4::operator *(const CMatrix4x3 &m)
 {
 	int j,k;
 	CVector4 v;
 	for(j=0; j<3; j++)
 	{
 		for(k=0; k<4; k++)
 		{
 			v.M[j]+=M[k]*m.M[k][j];
 		}
 	}
 	v.M[3]=M[3];
 	return v;
 }

 CVector4 CVector4::operator *(const CMatrix4x3 &m)const
 {
 	int j,k;
 	CVector4 v;
 	for(j=0; j<3; j++)
 	{
 		for(k=0; k<4; k++)
 		{
 			v.M[j]+=M[k]*m.M[k][j];
 		}
 	}
 	v.M[3]=M[3];
 	return v;
 }

