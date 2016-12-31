#include "StdAfx.h"
#include "BitmapImage.h"
extern HWND hWnd;

CBitmapImage::CBitmapImage(void)
{
	attr=0;
	bpp=0;
	width=0;
	height=0;
	numBytes=0;
	state=BITMAP_STATE_ALIVE;
	x=0;
	y=0;
	buffer=NULL;
}

CBitmapImage::~CBitmapImage(void)
{
	Release();
}

CBitmapImage::CBitmapImage(CBitmapImage& image)
{
	attr=image.attr;
	bpp=image.bpp;
	width=image.width;
	height=image.height;
	numBytes=image.numBytes;
	state=image.state;
	x=image.x;
	y=image.y;
	buffer=(UCHAR*)malloc(sizeof(image.buffer));
	memcpy(buffer,image.buffer,sizeof(image.buffer));
}

CBitmapImage& CBitmapImage::operator=(CBitmapImage& image)
{
	attr=image.attr;
	bpp=image.bpp;
	width=image.width;
	height=image.height;
	numBytes=image.numBytes;
	state=image.state;
	x=image.x;
	y=image.y;
	buffer=(UCHAR*)malloc(sizeof(image.buffer));
	memcpy(buffer,image.buffer,sizeof(image.buffer));
	return *this;
}

int CBitmapImage::CreateBitmapImage(int x, int y, int width, int height, int bpp)
{
	if(!(buffer=(UCHAR*)malloc(width*height*(bpp>>3))))
	{
		MessageBox(hWnd,"BitmapImage内存分配失败！","error",MB_OK);
		return FALSE;
	}
	state=BITMAP_STATE_ALIVE;
	attr=0;
	this->height=height;
	this->width=width;
	this->x=x;
	this->y=y;
	this->bpp=bpp;
	numBytes=width*height*(bpp>>3);
	memset(buffer,0,width*height*(bpp>>3));
	return true;
}

int CBitmapImage::RenderBitmapImage(UCHAR* destBuffer, int lpitch, int transparent)
{
	if(!(attr && BITMAP_ATTR_LOADED))
		return FALSE;
	UCHAR* destAddr,*sourceAddr;
	UCHAR pixel;
	destAddr=destBuffer+y*lpitch+x;
	sourceAddr=buffer;
	if(transparent)
	{
		for(int index=0; index<height; index++)
		{
			for(int i=0; i<width; i++)
			{
				if((pixel=sourceAddr[i])!=0)
					destAddr[i]=pixel;
			}
			destAddr+=lpitch;
			sourceAddr+=width;
		}
	}
	else
	{
		for(int index=0; index<height; index++)
		{
			memcpy(destAddr,sourceAddr,width);
			destAddr+=lpitch;
			sourceAddr+=width;
		}
	}
	return TRUE;
}

int CBitmapImage::RenderBitmapImage16(UCHAR* destBuffer, int& lpitch, int transparent)
{
	if(!(attr && BITMAP_ATTR_LOADED))
		return FALSE;
	USHORT* destAddr,*sourceAddr;
	USHORT pixel;
	lpitch>>=1;
	destAddr=(USHORT*)destBuffer+y*lpitch+x;
	sourceAddr=(USHORT*)buffer;

	if(transparent)
	{
		for(int index=0; index<height; index++)
		{
			for(int i=0; i<width; i++)
			{
				if((pixel=sourceAddr[i])!=0)
					destAddr[i]=pixel;
			}
			destAddr+=lpitch;
			sourceAddr+=width;
		}
	}
	else
	{
		for(int index=0; index<height; index++)
		{
			memcpy(destAddr,sourceAddr,width*2);
			destAddr+=lpitch;
			sourceAddr+=width;
		}
	}
	return TRUE;
}

int CBitmapImage::LoadImageBitmap16(CBitmapFile& bitmap, int cx, int cy, int mode)
{
	//此为24rgb模式
	/*if(mode==BITMAP_EXTRACT_MODE_CELL)
	{
		cx=cx*(width+1)+1;
		cy=cy*(height+1)+1;
	}
	UCHAR* sourceAddr=(UCHAR*)bitmap.buffer+bitmap.bitmapinfoheader.biWidth*cy+cx;
	USHORT* destAddr=(USHORT*)buffer;
	UCHAR blue,green,red;
	for(int index_y=0; index_y<height; index_y++)
	{
		for(int index_x=0; index_x<width; index_x++)
		{
			blue=sourceAddr[index_y*width*3+index_x*3+0]>>3;
			green=sourceAddr[index_y*width*3+index_x*3+1]>>3;
			red=sourceAddr[index_y*width*3+index_x*3+2]>>3;
			USHORT pixel=_RGB16BIT565(red,green,blue);
			destAddr[index_y*width+index_x]=pixel;
		}
	}
	attr|=BITMAP_ATTR_LOADED;
	return TRUE;*/
	USHORT* destAddr,*sourceAddr;
	if(mode==BITMAP_EXTRACT_MODE_CELL)
	{
		cx=cx*(width+1)+1;
		cy=cy*(height+1)+1;
	}
	sourceAddr=(USHORT*)bitmap.buffer+bitmap.bitmapinfoheader.biWidth*cy+cx;
	destAddr=(USHORT*)buffer;
	for(int index=0; index<height; index++)
	{
		memcpy(destAddr,sourceAddr,width*2);
		destAddr+=width;
		sourceAddr+=bitmap.bitmapinfoheader.biWidth;
	}
	/*memset(destAddr-width-8,0,8*2);
	memset(destAddr-8,0,8*2);*/

	attr|=BITMAP_ATTR_LOADED;
	return TRUE;
}
int CBitmapImage::LoadImageBitmap(CBitmapFile& bitmap, int cx, int cy, int mode)
{
	UCHAR* destAddr,*sourceAddr;
	if(mode==BITMAP_EXTRACT_MODE_CELL)
	{
		cx=cx*(width+1)+1;
		cy=cy*(height+1)+1;
	}
	sourceAddr=bitmap.buffer+bitmap.bitmapinfoheader.biWidth*cy+cx;
	destAddr=buffer;
	for(int index=0; index<height; index++)
	{
		memcpy(destAddr,sourceAddr,bitmap.bitmapinfoheader.biWidth);
		destAddr+=width;
		sourceAddr+=bitmap.bitmapinfoheader.biWidth;
	}
	attr|=BITMAP_ATTR_LOADED;
	return TRUE;
}
int CBitmapImage::CopyBitmapImage(int destX , int destY , CBitmapImage& sourceBitmap, int sourceX, int sourceY, int width, int height)
{
	UCHAR* destAddr=buffer+(destY*width+destX)*(bpp>>3);
	UCHAR* sourceAddr=sourceBitmap.buffer+(sourceY*sourceBitmap.width+sourceX)*(sourceBitmap.bpp>>3);
	for(int i=0; i<height; i++)
	{
		memcpy(destAddr,sourceAddr,width*(bpp>>3));
		destAddr+=this->width*(bpp>>3);
		sourceAddr+=sourceBitmap.width*(sourceBitmap.bpp>>3);
	}
	return TRUE;
}

int CBitmapImage::ScrollBitmap(int dx, int dy)
{
	CBitmapImage temImage;
	if(dx==0 && dy==0)
		return FALSE;
	if(dx!=0)
	{
		dx%=width;
		if(dx>0)
		{//向右翻转
			temImage.CreateBitmapImage(0,0,dx,height,SCREEN_BPP);
			temImage.CopyBitmapImage(0,0,*this,width-dx,0,dx,height);
			UCHAR* sourAddr=buffer;
			int shift=(SCREEN_BPP>>3)*dx;
			for(int i=0; i<height; i++)
			{
				memmove(sourAddr+shift,sourAddr,(width-dx)*(SCREEN_BPP>>3));
				sourAddr+=width*(SCREEN_BPP>>3);
			}
			this->CopyBitmapImage(0,0,temImage,0,0,dx,height);
		}
		else
		{//向左翻转
			dx=-dx;
			temImage.CreateBitmapImage(0,0,dx,height,SCREEN_BPP);
			temImage.CopyBitmapImage(0,0,*this,0,0,dx,height);
			UCHAR* sourAddr=buffer;
			int shift=(SCREEN_BPP>>3)*dx;
			for(int i=0; i<height; i++)
			{
				memmove(sourAddr,sourAddr+shift,(width-dx)*(SCREEN_BPP>>3));
				sourAddr+=width*(SCREEN_BPP>>3);
			}
			this->CopyBitmapImage(width-dx,0,temImage,0,0,dx,height);
		}
	}
	return TRUE;
}


int CBitmapImage::Release(void)
{
	if(buffer!=NULL)
	{
		free(buffer);
		buffer=NULL;
	}
	return TRUE;
}
