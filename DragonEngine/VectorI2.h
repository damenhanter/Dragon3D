#pragma once
#include "datatype.h"

class CVectorI2
{
public:
	CVectorI2(void);
	CVectorI2(const CVectorI2& src);
	CVectorI2& operator=(const CVectorI2& src);
	~CVectorI2(void);
public:
	union
	{
		int M[2];
		struct  
		{
			int x;
			int y;
		};
	};
};
