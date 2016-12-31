#include "StdAfx.h"
#include "VectorI2.h"

CVectorI2::CVectorI2(void)
:x(0),y(0)
{
}


CVectorI2::CVectorI2(const CVectorI2& src)
{
	x=src.x;
	y=src.y;
}

CVectorI2& CVectorI2::operator =(const CVectorI2& src)
{
	x=src.x;
	y=src.y;
	return *this;
}

CVectorI2::~CVectorI2(void)
{
}
