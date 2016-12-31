#pragma once
#include "datatype.h"
#include "Vector3.h"
#include "Vector4.h"

class CQuaternion
{
public:
	CQuaternion(void);
	CQuaternion(float w,float x,float y,float z)
	{
		this->w=w;
		this->x=x;
		this->y=y;
		this->z=z;
	}
	CQuaternion(CQuaternion& q)
	{
		w=q.w;x=q.x;y=q.y;z=q.z;
	}
	~CQuaternion(void);
	CQuaternion& operator=(CQuaternion& q)
	{
		w=q.w;x=q.x;y=q.y;z=q.z;
		return *this;
	}
	inline CQuaternion operator+(const CQuaternion& q)
	{
		return CQuaternion(w+q.w,x+q.x,y+q.y,z+q.z);
	}
	inline CQuaternion operator-(const CQuaternion& q)
	{
		return CQuaternion(w-q.w,x-q.x,y-q.y,z-q.z);
	}
	void CreateQuatFromVector3DTheta(const CVector3& v,float theta);
	void CreateQuatFromVector4DTheta(const CVector4& v,float theta);
	void CreateQuatFromEulerZYX(float thetaZ,float thetaY,float thetaX);
	void QuatToVector3DTheta(CVector3& v,float& theta);
	inline CQuaternion GetConjugate()const
	{
		return CQuaternion(w,-x,-y,-z);
	}
	inline void SetScale(float scale)
	{
		w*=scale;x*=scale;y*=scale;z*=scale;
	}

	inline float GetLength()const
	{
		return sqrtf(w*w+x*x+y*y+z*z);
	}
	inline float GetLenght2()const
	{
		return w*w+x*x+y*y+z*z;
	}
	inline void Normalize()
	{
		float length=GetLength();
		float length_inv=1.0f/length;
		
		w*=length_inv;
		x*=length_inv;
		y*=length_inv;
		z*=length_inv;
	}
	inline CQuaternion UnitQuatInverse()const
	{
		return CQuaternion(w,-x,-y,-z);
	}
	inline CQuaternion QuatInverse()const
	{
		float length2_inv=1.0f/GetLenght2();
		return CQuaternion(w*length2_inv,-x*length2_inv,-y*length2_inv,-z*length2_inv);
	}
	CQuaternion operator*(const CQuaternion& q);
	CQuaternion QuatTripleProduct(CQuaternion& q0,CQuaternion& q1,CQuaternion& q2); 
public:
	union
	{
		float M[4];
		struct  
		{
			float q0;  //Êµ²¿
			CVector3 qv;   //Ðé²¿
		};
		struct
		{
			float w,x,y,z;
		};
	};

};
