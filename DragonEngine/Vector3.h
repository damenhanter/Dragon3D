#ifndef _UGP_VECTOR_H_
#define _UGP_VECTOR_H_
#include "datatype.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Matrix4x3.h"

//////////////////////////////////////////////////////////////////////////
//不包含w分量的3D向量和点
//私有变量中使用联合来构造变量，意在支持更多的数据访问方法，
//使编写的算法清晰
//////////////////////////////////////////////////////////////////////////
class CMatrix3x3;
class CMatrix4x4;
class CMatrix4x3;

class CVector3
{
   public:
      CVector3();
      CVector3(float X, float Y, float Z)
	  {
		  x=X;
		  y=Y;
		  z=Z;
	  }
	  CVector3(const CVector3& vsrc)
	  {
		 x=vsrc.x;
		 y=vsrc.y;
		 z=vsrc.z;
	  }
      CVector3& operator=(const CVector3 &vsrc)
	  {
		 x=vsrc.x;
		 y=vsrc.y;
		 z=vsrc.z;
		 return *this;
	  }
	
	  CVector3 operator*(const CMatrix3x3& m)const;
	  CVector3 operator*(const CMatrix4x4& m)const;
	  CVector3 operator*(const CMatrix4x3& m)const;	

      CVector3 operator-(const CVector3 &v)const;
      CVector3 operator+(const CVector3 &v)const;
      CVector3 operator*(const CVector3 &v)const;
      CVector3 operator/(const CVector3 &v)const;

      CVector3 operator+(const float f)const;
      CVector3 operator-(const float f)const;
      CVector3 operator*(const float f)const;
      CVector3 operator/(const float f)const;

      void operator +=(const CVector3 &v);
      void operator -=(const CVector3 &v);
      void operator *=(const CVector3 &v);
      void operator /=(const CVector3 &v);
      void operator +=(const float f);
      void operator -=(const float f);
      void operator *=(const float f);
      void operator /=(const float f);

	  inline void CreateVector(const CVector3& p0,const CVector3& p1)
	  {
		  x=p1.x-p0.x;
		  y=p1.y-p0.y;
		  z=p1.z-p0.z;
	  }
	  inline float GetCosth(const CVector3& v)const
	  {
		  float a=DotProduct(v);
		  float b=GetLength();
		  float c=v.GetLength();
		  return a/(b*c);
	  }
      CVector3 CrossProduct(const CVector3 &v1)const;
      float DotProduct(const CVector3 &v1)const;
      float GetLength()const;
      void Normalize();
	  
public:
	union
	{
		float M[3];
		struct  
		{
			float x, y, z;
		};
	};
};

typedef CVector3 CPoint3D;

#endif