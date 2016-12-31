#pragma once
#include "datatype.h"

class CFIXP16
{
public:
	CFIXP16(void);
	CFIXP16(int x)
	{
		this->x=x;
	}

	CFIXP16(CFIXP16& src)
	{
		x=src.x;
	}
	~CFIXP16(void);

	CFIXP16 operator=(const CFIXP16& src)
	{
		x=src.x;
	}
	inline CFIXP16 operator+(const CFIXP16& fp)
	{
		return CFIXP16(x+fp.x);
	}
	inline CFIXP16 operator-(const CFIXP16& fp)
	{
		return CFIXP16(x-fp.x);
	}
	inline CFIXP16 operator*(const CFIXP16& fp)
	{
		CFIXP16 result;
		int temp=fp.x;
		_asm
		{
			mov eax,x
			imul temp
			shrd eax,edx,16
			mov  result.x,eax
		}
		//格式：   shrd   目的操作数，源操作数，移位位数 
		//指令不改变源操作数，源操作数必须为寄存器。
		//将目的操作数向右移动相应位数，空出来的位由源操作数的低位填充。
		return result;
	}
	inline CFIXP16 operator/(const CFIXP16& fp)
	{
		CFIXP16 result;
		int temp=fp.x;
		_asm
		{
			mov eax,x
			cdq        //带符号扩展到edx:eax中
			shld edx,eax,16   //对edx进行移位
			sal eax,16
			idiv temp
			mov result.x,eax
		}
		return result;
	}
	inline void IntegerToFixp16(int i)
	{
		x=(i)<<FIXP16_SHIFT;
	}
	inline void FloatToFixp16(float f)
	{
		x=int(f*FIXP16_MAG+0.5);
	}
	//将定点数转化为浮点数
	inline float Fixp16ToFloat()
	{
		return ((float)x)/FIXP16_MAG;
	}
	//从16.16格式的定点数中提取整数部分和小数部分
	inline int GetWP()
	{
		return (x)>>FIXP16_SHIFT;
	}
	inline int GetDP()
	{
		return (x)&&FIXP16_DP_MASK;
	}
public:
	int x;
};
