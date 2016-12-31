#pragma once
#include "datatype.h"
#include "Vector3.h"

//////////////////////////////////////////////////////////////////////////
//3D参数化直线
//////////////////////////////////////////////////////////////////////////

class CParamLine3D
{
public:
	CParamLine3D(void);
	~CParamLine3D(void);

	void InitParamLine3D(const CPoint3D& p0,const CPoint3D& p1);
	CPoint3D ComputeParamLine3D(float t);
public:
	CPoint3D p0;   //参数化直线的起点
	CPoint3D p1;   //参数化直线的终点
	CVector3 v;    //线段的方向向量
};
