#pragma once
#include "Vector4.h"
#include "Plane3D.h"
#include "Matrix4x4.h"

//////////////////////////////////////////////////////////////////////////
//相机支持欧拉模型或UVN模型
//////////////////////////////////////////////////////////////////////////
class CCamera
{
public:
	CCamera(void);
	
	~CCamera(void);
	void InitCamera(int camAttr,CPoint4D& camPos,CVector4& camDir,
		CPoint4D* camTarget,float nz,float fz,float fov,float vpWidth,float vpHeight);
	void CreateCAM4DV1MatrixEuler(int rotateSeq);
	void CreateCAM4DV1MatrixUVN(int mode);
public:
	int state;  //相机状态
	int attr;   //相机属性
	
	CPoint4D pos;      //相机在世界坐标系中的位置
	CVector4 dir;      //欧拉角度/UVN相机模型的注视向量
	
	CVector4 u;        //UVN模型的朝向向量
	CVector4 v;
	CVector4 n;
	CPoint4D target;   //UVN模型的目标点
	 
	float fov;         //水平或垂直方向的视野

	float viewDist;  //水平或垂直视距

	//3D裁截面
	float nearZ;
	float farZ;
	CPlane3D rtPlane;
	CPlane3D ltPlane;
	CPlane3D tpPlane;
	CPlane3D btPlane;

	float viewPlaneWidth;       //视平面的宽度和高度
	float viewPlaneHeight;

	float viewPortWidth;        //屏幕/视口的宽度和高度
	float viewPortHeight;
	float viewPortCenterX;      //视口中心
	float viewPortCenterY;

	float aspectRatio;           //宽高比

	// 变换矩阵
	CMatrix4x4 mcam;
	CMatrix4x4 mper;
	CMatrix4x4 mscr;
};
