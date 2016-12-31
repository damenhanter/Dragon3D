#include "StdAfx.h"
#include "ZBuffer.h"

CZBuffer::CZBuffer(void)
{
	zbuffer=NULL;
}

CZBuffer::~CZBuffer(void)
{
	if (zbuffer!=NULL)
	{
		delete zbuffer;
		zbuffer=NULL;
	}
}

int CZBuffer::CreateZBuffer(int width, int height, int attr)
{
	if (zbuffer)
	{
		delete zbuffer;
	}

	this->width=width;
	this->height=height;
	this->attr=attr;
	//16位还是32位
	if (attr & ZBUFFER_ATTR_16BIT)
	{
		sizeq=width*height/2;
		if ((zbuffer=new UCHAR[sizeq<<2])==NULL)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else if (attr & ZBUFFER_ATTR_32BIT)
	{
		sizeq=width*height;
		if ((zbuffer=new UCHAR[sizeq<<3])==NULL)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
		return FALSE;
}

void CZBuffer::ClearZBuffer(UINT data)
{
	//如果z缓存是16位的，必须使用用来填充的16位值的两个拷贝创建一个四元组
	//如果z缓存是32位的，只需将用于填充的值转化为UINT即可
	MemSetDWORD(zbuffer,data,sizeq);
}