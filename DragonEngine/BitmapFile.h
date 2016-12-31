#pragma once
#include "datatype.h"

class CBitmapFile
{
public:
	CBitmapFile(void);
	~CBitmapFile(void);

	int LoadBitmapFromFile(char* filename);
	int FlipBitmap(UCHAR* image, int BytesPerLine, int height);
public:
	BITMAPFILEHEADER bitmapfileheader;      //包含了位图文件头部信息
	BITMAPINFOHEADER bitmapinfoheader;      //包含了所有的位图信息
	PALETTEENTRY palette[256];              //如果位图像素是八位，存储调色板
	UCHAR* buffer;                          //数据指针
	int UnLoadBitmapFile(void);
};
