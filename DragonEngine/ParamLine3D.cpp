#include "StdAfx.h"
#include "ParamLine3D.h"

CParamLine3D::CParamLine3D(void)
{
}

CParamLine3D::~CParamLine3D(void)
{
}

void CParamLine3D::InitParamLine3D(const CPoint3D& p0, const CPoint3D& p1)
{
	v.CreateVector(p0,p1);
}

CPoint3D CParamLine3D::ComputeParamLine3D(float t)
{
	return CPoint3D(p0.x+p1.x*t,p0.y+p1.y*t,p0.z+p1.z*t);
}