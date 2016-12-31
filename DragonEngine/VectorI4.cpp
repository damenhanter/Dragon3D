#include "StdAfx.h"
#include "VectorI4.h"

CVectorI4::CVectorI4(void)
{
	x=y=z=w=1;
}

CVectorI4::CVectorI4(const CVectorI4& src)
{
	x=src.x;
	y=src.y;
	z=src.z;
	w=src.w;
}

CVectorI4& CVectorI4::operator =(const CVectorI4& src)
{
	x=src.x;
	y=src.y;
	z=src.z;
	w=src.w;
	return *this;
}

CVectorI4::~CVectorI4(void)
{
}
