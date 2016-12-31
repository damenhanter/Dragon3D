#pragma once
#include "datatype.h"
#include "BitmapFile.h"

class CBitmapImage
{
public:
	CBitmapImage(void);
	~CBitmapImage(void);
	CBitmapImage(CBitmapImage& image);
	CBitmapImage& operator=(CBitmapImage& image);

	int CreateBitmapImage(int x, int y, int width, int height, int bpp);
	int RenderBitmapImage(UCHAR* destBuffer, int lpitch, int transparent);
	int RenderBitmapImage16(UCHAR* destBuffer, int& lpitch, int transparent);
	int LoadImageBitmap(CBitmapFile& bitmap, int cx, int cy, int mode);
	int LoadImageBitmap16(CBitmapFile& bitmap, int cx, int cy, int mode);
	int CopyBitmapImage(int destX , int destY , CBitmapImage& sourceBitmap, int sourceX, int sourceY, int width, int height);
	int ScrollBitmap(int dx, int dy);
	UCHAR* buffer;      //位图像素
	int width,height;   //位图大小
	int numBytes;       //位图字节
public:
	int state;          //位图状态
	int attr;           //位图属性
	int x,y;            //位图位置
	int bpp;
	


public:
	int Release(void);
};
