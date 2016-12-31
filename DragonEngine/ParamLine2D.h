#pragma once
#include "datatype.h"
#include "Vector2.h"

//////////////////////////////////////////////////////////////////////////
//2D参数化直线
//////////////////////////////////////////////////////////////////////////

class CParamLine2D
{
public:
	CParamLine2D(void);
	~CParamLine2D(void);

	void InitParamLine2D(const CPoint2D& pa,const CPoint2D& pb);
	inline CPoint2D ComputeParamLine2D(float t)
	{
		return CPoint2D(p0.x+p1.x*t,p0.y+p1.y*t);
	}
	int IntersectParamLine2D(const CParamLine2D& L,float& t1,float& t2);
	int IntersectParamLine2D(const CParamLine2D& L,CPoint2D& pt);
public:
	CPoint2D p0;       //参数化直线的起点
	CPoint2D p1;       //参数化直线的终点
	CVector2 v;        //线段的方向向量
};

