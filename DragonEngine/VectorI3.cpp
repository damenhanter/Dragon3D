#include "StdAfx.h"
#include "VectorI3.h"

CVectorI3::CVectorI3(void)
{
	x=y=z=0;
}

CVectorI3::CVectorI3(const CVectorI3& scr)
{
	x=scr.x;
	y=scr.y;
	z=scr.z;
}

CVectorI3& CVectorI3::operator =(const CVectorI3& scr)
{
	x=scr.x;
	y=scr.y;
	z=scr.z;
	return *this;
}
CVectorI3::~CVectorI3(void)
{
}
