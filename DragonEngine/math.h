#pragma once


#include "Matrix1x2.h"
#include "Matrix1x3.h"
#include "Matrix3x2.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Matrix1x4.h"
#include "Matrix2x2.h"
#include "Matrix4x3.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "ParamLine2D.h"
#include "ParamLine3D.h"
#include "Plane3D.h"
#include "Quaternion.h"
#include "Spherical3D.h"
#include "Cylindrical3D.h"
#include "polygon2D.H"
#include "Triangle2D.h"
#include "Line2D.h"
#include "BoundBox.h"
#include "BoundSphere.h"



inline CMatrix1x2 Solve2X2System(CMatrix2x2& A,CMatrix1x2& B)
{
	CMatrix1x2 X;
	CMatrix2x2 temp;
	float Adet=A.MatrixDet();
	if (fabs(Adet)<EPSILON_E5)
	{
		return CMatrix1x2();
	}

	temp=A;
	temp.M00=B.M00;temp.M10=B.M01;
	float tempdet=temp.MatrixDet();
	X.M00=tempdet/Adet;

	temp=A;
	temp.M10=B.M00;temp.M11=B.M01;
	float tempdet1=temp.MatrixDet();
	X.M01=tempdet1/Adet;

	return X;
}

inline CMatrix1x3 Solve2X2System(CMatrix3x3& A,CMatrix1x3& B)
{
	CMatrix1x3 X;
	CMatrix3x3 temp;
	float Adet=A.MatrixDet();
	if (fabs(Adet)<EPSILON_E5)
	{
		return CMatrix1x3();
	}

	temp=A;
	temp.M00=B.M00;temp.M10=B.M01;temp.M20=B.M02;
	float tempdet=temp.MatrixDet();
	X.M00=tempdet/Adet;

	temp=A;
	temp.M10=B.M00;temp.M11=B.M01;temp.M12=B.M02;
	float tempdet1=temp.MatrixDet();
	X.M01=tempdet1/Adet;

	temp=A;
	temp.M20=B.M00;temp.M21=B.M01;temp.M22=B.M02;
	float tempdet2=temp.MatrixDet();
	X.M02=tempdet2/Adet;

	return X;
}

