#include "StdAfx.h"
#include "BitmapFile.h"
extern HWND hWnd;

CBitmapFile::CBitmapFile(void)
{
	buffer=NULL;
	memset(palette,0,sizeof(PALETTEENTRY)*256);
}

CBitmapFile::~CBitmapFile(void)
{
	
}
int CBitmapFile::LoadBitmapFromFile(char* filename)
{
	HFILE filehandle;    //文件句柄
	OFSTRUCT filedata;
	if ((filehandle = OpenFile(filename,&filedata,OF_READ))==-1)
		return(0);
	//装载位图文件头
	_lread(filehandle,&bitmapfileheader,sizeof(BITMAPFILEHEADER));
	if(bitmapfileheader.bfType!=BITMAP_ID)
	{
		_lclose(filehandle);
		MessageBoxA(hWnd,"文件不是bitmap类型!","Tips",MB_OK);
		return 0;
	}
	_lread(filehandle,&bitmapinfoheader,sizeof(BITMAPINFOHEADER));
	if(bitmapinfoheader.biBitCount==8)
	{
		_lread(filehandle, &palette,256*sizeof(PALETTEENTRY));
		for(int index=0;index<256;index++)
		{
			int tempcolor=palette[index].peRed;
			palette[index].peRed=palette[index].peBlue;
			palette[index].peBlue=tempcolor;

			palette[index].peFlags=PC_NOCOLLAPSE;
		}
	}

	if(bitmapinfoheader.biBitCount==8||
		bitmapinfoheader.biBitCount==16||
		bitmapinfoheader.biBitCount==24)
	{
		if(buffer)
		{
			free(buffer);
			buffer=NULL;
		}
		DWORD nSizeImage=bitmapinfoheader.biWidth*
			(bitmapinfoheader.biBitCount>>3)*bitmapinfoheader.biHeight;
		if(!(buffer=(UCHAR*)malloc(nSizeImage)))
		{
			_lclose(filehandle);
			return 0;
		}
		//如果有颜色掩码去除颜色掩码
		if (bitmapinfoheader.biCompression==BI_BITFIELDS)
		{
			_lread(filehandle,buffer,12);
		}
		_lread(filehandle,buffer,nSizeImage);
	}
	else
		return 0;
	_lclose(filehandle);

	FlipBitmap(buffer,bitmapinfoheader.biWidth*
		(bitmapinfoheader.biBitCount>>3),bitmapinfoheader.biHeight);
	return true;
}

int CBitmapFile::FlipBitmap(UCHAR* image, int BytesPerLine, int height)
{
	UCHAR* buffer;
	if(!(buffer=(UCHAR*)malloc(BytesPerLine*height)))
		return 0;
	memcpy(buffer,image,BytesPerLine*height);

	for(int index=0; index<height ; index++ )
	{
		memcpy(&image[((height-1)-index)*BytesPerLine],&buffer[index*BytesPerLine],
			BytesPerLine);
	}
	free(buffer);
	return true;
}
int CBitmapFile::UnLoadBitmapFile(void)
{
	if(buffer)
	{
		free(buffer);
		buffer=NULL;
	}
	return TRUE;
}

