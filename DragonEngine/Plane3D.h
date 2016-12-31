#pragma once
#include "datatype.h"
#include "Vector3.h"
#include "ParamLine3D.h"

//////////////////////////////////////////////////////////////////////////
//3D平面
//采用点法式
//////////////////////////////////////////////////////////////////////////

class CPlane3D
{
public:
	CPlane3D(void);
	~CPlane3D(void);
	
	void InitPlane3D(const CPoint3D& pt,CVector3 v,BOOL normalize=FALSE); 
	float IsPointInPlane3D(const CPoint3D& pt);
	int IntersectParamLine3DPlane3D(const CParamLine3D& paramline3D,float& t,CPoint3D& pt);
public:
	CPoint3D p0;     //平面上任意一点
	CVector3 n;      //平面法线（不必是单位向量）
};
