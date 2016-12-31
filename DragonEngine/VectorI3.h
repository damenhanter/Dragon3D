#pragma once
#include "datatype.h"

class CVectorI3
{
public:
	CVectorI3(void);
	CVectorI3(const CVectorI3& scr);
	CVectorI3& operator=(const CVectorI3& scr);
	~CVectorI3(void);
public:
	union
	{
		int M[3];
		struct  
		{
			int x,y,z;
		};
	};
};
