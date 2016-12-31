#include "stdafx.h"
#include "Vector3.h"

CVector3::CVector3()
:x(0.0f)
,y(0.0f)
,z(0.0f)
{
}


CVector3 CVector3::operator -(const CVector3 &v)const
{
   return CVector3(x - v.x, y - v.y, z - v.z);
}


CVector3 CVector3::operator +(const CVector3 &v)const
{
   return CVector3(x + v.x, y + v.y, z + v.z);
}


CVector3 CVector3::operator *(const CVector3 &v)const
{
   return CVector3(x * v.x, y * v.y, z * v.z);
}


CVector3 CVector3::operator /(const CVector3 &v)const
{
   return CVector3(x / v.x, y / v.y, z / v.z);
}


CVector3 CVector3::operator +(const float f)const
{
   return CVector3(x + f, y + f, z + f);
}


CVector3 CVector3::operator -(const float f)const
{
   return CVector3(x - f, y - f, z - f);
}


CVector3 CVector3::operator *(const float f)const
{
   return CVector3(x * f, y * f, z * f);
}


CVector3 CVector3::operator /(const float f)const
{
   float f1 = 1/f;

   return CVector3(x * f1, y * f1, z * f1);
}


void CVector3::operator +=(const CVector3 &v)
{
   x += v.x;
   y += v.y;
   z += v.z;
}


void CVector3::operator -=(const CVector3 &v)
{
   x -= v.x;
   y -= v.y;
   z -= v.z;
}


void CVector3::operator *=(const CVector3 &v)
{
   x *= v.x;
   y *= v.y;
   z *= v.z;
}


void CVector3::operator /=(const CVector3 &v)
{
   x /= v.x;
   y /= v.y;
   z /= v.z;
}


void CVector3::operator +=(const float f)
{
   x += f;
   y += f;
   z += f;
}


void CVector3::operator -=(const float f)
{
   x -= f;
   y -= f;
   z -= f;
}


void CVector3::operator *=(const float f)
{
   x *= f;
   y *= f;
   z *= f;
}


void CVector3::operator /=(const float f)
{
   float f1 = 1/f;

   x *= f1;
   y *= f1;
   z *= f1;
}

CVector3 CVector3::operator *(const CMatrix3x3& m)const
{
	int j,k;
	CVector3 v;
	for(j=0; j<3; j++)
	{
		for(k=0; k<3; k++)
		{
			v.M[j]+=M[k]*m.M[k][j];
		}
	}
	return v;
}

CVector3 CVector3::operator *(const CMatrix4x4& m)const
{
	int j,k;
	CVector3 v;
	for(j=0; j<3; j++)
	{
		for(k=0; k<3; k++)
		{
			v.M[j]+=M[k]*m.M[k][j];
		}
		v.M[j]+=m.M[k][j];
	}
	return v;
}

CVector3 CVector3::operator *(const CMatrix4x3& m)const
{
	int j,k;
	CVector3 v;
	for(j=0; j<3; j++)
	{
		for(k=0; k<3; k++)
		{
			v.M[j]+=M[k]*m.M[k][j];
		}
		v.M[j]+=m.M[k][j];
	}
	return v;
}

CVector3 CVector3::CrossProduct(const CVector3 &v1)const
{
   return CVector3((v1.y * z)-(v1.z * y),(v1.z * x)-(v1.x * z),(v1.x * y)-(v1.y * x));
}


float CVector3::DotProduct(const CVector3 &v1)const
{
   return x * v1.x + y * v1.y + z * v1.z;
}

/*浮点数乘法占用1~3个CPU时钟周期
sqrt库函数占用70个CPU时钟周期，所以这里用泰勒公式的麦克劳林级数近似计算
误差为8%
*/
float CVector3::GetLength()const
{
   int X,Y,Z;
   //确保它们是正值
   X=(int)fabs(x)*1024;
   Y=(int)fabs(y)*1024;
   Z=(int)fabs(z)*1024;

   if(Y<X)
	   swap(Y,X);
   if(Z<Y)
	   swap(Z,Y);
   if(Y<X)
	   swap(X,Y);

   int dist=(Z + 11*(Y>>5)+(X>>2));

   return ((float)(dist>>10));
}


void CVector3::Normalize()
{
   float length=GetLength();
   if (length<EPSILON_E6)
   {
	   return ;
   }
   length=1.0f/length;
   x*=length;
   y*=length;
   z*=length;
}