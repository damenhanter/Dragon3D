#include "StdAfx.h"
#include "Quaternion.h"

CQuaternion::CQuaternion(void)
:x(0.0f)
,y(0.0f)
,z(0.0f)
,w(0.0f)
{
	
}

CQuaternion::~CQuaternion(void)
{
}

//////////////////////////////////////////////////////////////////////////
//使用一个3D方向向量和角度初始化四元数，方向向量必须是单位向量
//////////////////////////////////////////////////////////////////////////

void CQuaternion::CreateQuatFromVector3DTheta(const CVector3 &v, float theta)
{
	float theta_div_2=(0.5)*theta;

	float sinf_theta=sinf(theta_div_2);

	w=cosf(theta_div_2);
	x=sinf_theta*v.x;
	y=sinf_theta*v.y;
	z=sinf_theta*v.z;
}

//////////////////////////////////////////////////////////////////////////
//使用一个4D方向向量和角度初始化四元数，方向向量必须是单位向量
//////////////////////////////////////////////////////////////////////////

void CQuaternion::CreateQuatFromVector4DTheta(const CVector4 &v, float theta)
{
	float theta_div_2=(0.5)*theta;

	float sinf_theta=sinf(theta_div_2);

	w=cosf(theta_div_2);
	x=sinf_theta*v.x;
	y=sinf_theta*v.y;
	z=sinf_theta*v.z;
}

//////////////////////////////////////////////////////////////////////////
//根据绕X\Y\z旋转的角度，创建一个zyx顺序进行旋转的四元数
//////////////////////////////////////////////////////////////////////////

void CQuaternion::CreateQuatFromEulerZYX(float thetaZ, float thetaY, float thetaX)
{
	float cos_z_2=0.5*cosf(thetaZ);
	float cos_y_2=0.5*cosf(thetaY);
	float cos_x_2=0.5*cosf(thetaX);

	float sin_z_2=0.5*sinf(thetaZ);
	float sin_y_2=0.5*sinf(thetaY);
	float sin_x_2=0.5*sinf(thetaX);

	w = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
	x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
	y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
	z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;
}

void CQuaternion::QuatToVector3DTheta(CVector3 &v,float& theta)
{
	theta=acosf(w);

	float sinf_theta_inv=1.0/sinf(theta);

	v.x=x*sinf_theta_inv;
	v.y=y*sinf_theta_inv;
	v.z=z*sinf_theta_inv;

	theta*=2;
}
//////////////////////////////////////////////////////////////////////////
//(a+bi+cj+dk)*(w+xi+yj+zk) =
//(aw-bx-cy-dz)+(ax+bw+cz-dy)i+(ay-bz+cw+dx)j+(az+by-cx+dw)k
//////////////////////////////////////////////////////////////////////////
CQuaternion CQuaternion::operator *(const CQuaternion &q)
{
	CQuaternion quat;
	float prd_0 = (z - y) * (q.y - q.z);
	float prd_1 = (w + x) * (q.w + q.x);
	float prd_2 = (w - x) * (q.y + q.z);
	float prd_3 = (y + z) * (q.w - q.x);
	float prd_4 = (z - x) * (q.x - q.y);
	float prd_5 = (z + x) * (q.x + q.y);
	float prd_6 = (w + y) * (q.w - q.z);
	float prd_7 = (w - y) * (q.w + q.z);

	float prd_8 = prd_5 + prd_6 + prd_7;
	float prd_9 = 0.5 * (prd_4 + prd_8);

	quat.w = prd_0 + prd_9 - prd_5;
	quat.x = prd_1 + prd_9 - prd_8;
	quat.y = prd_2 + prd_9 - prd_7;
	quat.z = prd_3 + prd_9 - prd_6;
	return quat;
}

CQuaternion CQuaternion::QuatTripleProduct(CQuaternion& q0, CQuaternion& q1, CQuaternion& q2)
{
	CQuaternion tem;
	tem=q0*q1;
	tem=tem*q2;
	return tem;
}