#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "datatype.h"
#include "Vector3.h"
#include "Vector4.h"

class CMatrix4x4
{
   public:
      CMatrix4x4() 
	  { 
		  MemSetDWORD(M,0,16);
	  }
      CMatrix4x4(const CMatrix4x4 &m)
	  {
		  M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];M[0][3]  = m.M[0][3];
		  M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];M[1][3]  = m.M[1][3];
		  M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];M[2][3]  = m.M[2][3];
		  M[3][0]  = m.M[3][0];M[3][1]  = m.M[3][1];M[3][2]  = m.M[3][2];M[3][3]  = m.M[3][3];
	  }
      CMatrix4x4(float r11, float r12, float r13, float r14,
                 float r21, float r22, float r23, float r24,
                 float r31, float r32, float r33, float r34,
                 float r41, float r42, float r43, float r44)
	  {
		  M[0][0]  = r11; M[0][1]  = r12; M[0][2]  = r13;M[0][3] = r14;
		  M[1][0]  = r21; M[1][1]  = r22; M[1][2]  = r23;M[1][3] = r24;
		  M[2][0]  = r31; M[2][1]  = r32; M[2][2]  = r33;M[2][3] = r34;
		  M[3][0]  = r41; M[3][1]  = r42; M[3][2]  = r43;M[3][3] = r44;
	  }
      ~CMatrix4x4()
	  {
	  }
	  CMatrix4x4& operator=(const CMatrix4x4 &m)
	  {
		  M[0][0]  = m.M[0][0];M[0][1]  = m.M[0][1];M[0][2]  = m.M[0][2];M[0][3]  = m.M[0][3];
		  M[1][0]  = m.M[1][0];M[1][1]  = m.M[1][1];M[1][2]  = m.M[1][2];M[1][3]  = m.M[1][3];
		  M[2][0]  = m.M[2][0];M[2][1]  = m.M[2][1];M[2][2]  = m.M[2][2];M[2][3]  = m.M[2][3];
		  M[3][0]  = m.M[3][0];M[3][1]  = m.M[3][1];M[3][2]  = m.M[3][2];M[3][3]  = m.M[3][3];
		  return* this;
	  }

      inline void Identity()
	  {
		  M[0][0]  = 1.0f; M[0][1]  = 0.0f; M[0][2]  = 0.0f;M[0][3] = 0.0f;
		  M[1][0]  = 0.0f; M[1][1]  = 1.0f; M[1][2]  = 0.0f;M[1][3] = 0.0f;
		  M[2][0]  = 0.0f; M[2][1]  = 0.0f; M[2][2]  = 1.0f;M[2][3] = 0.0f;
		  M[3][0]  = 0.0f; M[3][1]  = 0.0f; M[3][2]  = 0.0f;M[3][3] = 1.0f;
	  }
	  inline void Transpose()
	  {
		  CMatrix4x4 mt;
		  mt.M[0][0]=M[0][0];mt.M[0][1]=M[1][0];mt.M[0][2]=M[2][0];mt.M[0][3]=M[3][0];
		  mt.M[1][0]=M[0][1];mt.M[1][1]=M[1][1];mt.M[1][2]=M[2][1];mt.M[1][3]=M[3][1];
		  mt.M[2][0]=M[0][2];mt.M[2][1]=M[1][2];mt.M[2][2]=M[2][2];mt.M[2][3]=M[3][2];
		  mt.M[3][0]=M[0][3];mt.M[3][1]=M[1][3];mt.M[3][2]=M[2][3];mt.M[3][3]=M[3][3];
		  *this=mt;
	  }
	  CMatrix4x4 MatrixInverse()const;
	  float MatrixDet()const;

      CMatrix4x4 operator-(const CMatrix4x4 &m)const;
      CMatrix4x4 operator+(const CMatrix4x4 &m)const;
      CMatrix4x4 operator*(const CMatrix4x4 &m)const;

      CMatrix4x4 operator*(const float f)const;
      CMatrix4x4 operator/(float f)const;

      void operator +=(const CMatrix4x4 &m);
      void operator -=(const CMatrix4x4 &m);
      void operator *=(const CMatrix4x4 &m);

      void operator *=(const float f);
      void operator /=(const float f);

      void Translate(float x, float y, float z);
      void inverseTranslate();

public:
	union
	{
		float M[4][4];
		struct  
		{
			float M00,M01,M02,M03;
			float M10,M11,M12,M13;
			float M20,M21,M22,M23;
			float M30,M31,M32,M33;
		};
	};
     
};

#endif