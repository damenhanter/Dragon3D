#pragma once

#include "datatype.h"

class CZBuffer
{
public:
	CZBuffer(void);
	~CZBuffer(void);

	int CreateZBuffer(int width,int height,int attr);
	void ClearZBuffer(UINT data);
public:
	int attr;    //属性
	UCHAR* zbuffer;  //指向缓存空间的指针
	int width;       //宽度，单位像素
	int height;      //高度
	int sizeq;       //大小，单位为四元组
};
