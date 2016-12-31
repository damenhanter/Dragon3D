#pragma once
#include "datatype.h"

class CVectorI4
{
public:
	CVectorI4(void);
	CVectorI4(const CVectorI4& src);
	CVectorI4& operator=(const CVectorI4& src);
	~CVectorI4(void);
public:
	union
	{
		int M[4];
		struct  
		{
			int x,y,z,w;
		};
	};
};
