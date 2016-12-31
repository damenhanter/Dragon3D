#include "StdAfx.h"
#include "RenderSystem.h"
#include "Triangle2D.h"
#include "ErrorLog.h"
#include "light.h"
#include "material.h"

extern HWND hWnd;
extern HANDLE hHandle;
extern RECT g_rRectClip;
extern CLight light;
extern CMaterial material;
extern CErrorLog errorlog;
extern UCHAR gLog2[513];

#pragma warning (disable:4996)

CRenderSystem::CRenderSystem(void)
{
	numModels=0;
	ptrRenderList=NULL;
}

CRenderSystem::~CRenderSystem(void)
{
	Release();

}
void CRenderSystem::LoadPaletteFromFile(char* filename)
{
	FILE* pfile;
	if((pfile=fopen(filename,"r"))==NULL)
	{
		MessageBox(hWnd,"调色板文件打开失败!","error",MB_OK);
		PostMessage(hWnd,WM_DESTROY,0,0);
		return ;
	}
	for(int index=0; index<256; index++)
	{
		fscanf(pfile,"%d %d %d %d",&palette[index].peRed,
			&palette[index].peGreen,
			&palette[index].peBlue,                                
			&palette[index].peFlags);
	}
	fclose(pfile);
}
int CRenderSystem::CreateDDraw(HWND hWnd,int screenWidth, int screenHeight, int screenBPP,BOOL windowed)
{
	if(FAILED(DirectDrawCreateEx(NULL,(void**)&lpdd,IID_IDirectDraw7,NULL)))
	{
		MessageBox(hWnd,"创建DirectDraw对象失败！","error",MB_OK);
		return FALSE;
	}
	if(windowed)
	{
		if(FAILED(lpdd->SetCooperativeLevel(hWnd,DDSCL_NORMAL)))
		{
			MessageBox(hWnd,"设置与操作系统的协作失败！","error",MB_OK);
			return FALSE;
		}
	}
	else
	{
		if(FAILED(lpdd->SetCooperativeLevel(hWnd,DDSCL_ALLOWMODEX|
			DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_MULTITHREADED)))
		{
			MessageBox(hWnd,"设置与操作系统的协作失败！","error",MB_OK);
			return false;
		}
		if(FAILED(lpdd->SetDisplayMode(screenWidth,screenHeight,screenBPP,0,0)))
		{
			MessageBox(hWnd,"设置窗口的显示模式失败！","error",MB_OK);
			return false;
		}
	}
	this->screenWidth=screenWidth;
	this->screenHeight=screenHeight;
	this->screenBPP=screenBPP;
	this->windowed=windowed;

	DDRAW_INIT_STRUCT(ddsd);
	if (!windowed)
	{
		ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	} 
	else
	{
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	} 
	if(FAILED(lpdd->CreateSurface(&ddsd,&lpddsprimary,NULL)))
	{
		MessageBox(hWnd,"创建主表面失败！","error",MB_OK);
		return false;
	}
	DDPIXELFORMAT ddpf;
	DDRAW_INIT_STRUCT(ddpf);
	lpddsprimary->GetPixelFormat(&ddpf);
	ddpixelformat=ddpf.dwRGBBitCount;
	if(ddpixelformat==DD_PIXEL_FORMAT555)
	{
		RGB16Bit=RGB16Bit555;
	}
	else if(ddpixelformat==DD_PIXEL_FORMAT565)
	{
		RGB16Bit=RGB16Bit565;
	}
	else if(ddpixelformat==DD_PIXEL_FORMAT888)
	{

	}
	if(!windowed)
	{
		ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
		if(FAILED(lpddsprimary->GetAttachedSurface(&ddscaps,&lpddsback)))
		{
			MessageBox(hWnd,"获得后备表面失败！","error",MB_OK);
			return false;
		}
	}
	else
	{
		lpddsback=CreateSurface(screenWidth,screenHeight,DDSCAPS_SYSTEMMEMORY,0);
	}
	if(screenBPP==DD_PIXEL_FORMAT8)
	{
		memset((void*)palette,0,256*sizeof(PALETTEENTRY));
		LoadPaletteFromFile(PALETTE_DEFAULT);
		if(windowed)
		{
			for (int index=0; index < 10; index++)
			{
				palette[index].peFlags = palette[index+246].peFlags = PC_EXPLICIT;       
			}
			if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE,
				palette,&lpddpal,NULL)))
				return	TRUE;
		} 
		else
		{
			if (FAILED(lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256,
				palette,&lpddpal,NULL)))
				return true;

		}
		if (FAILED(lpddsprimary->SetPalette(lpddpal)))
		{
			MessageBox(hWnd,"设置调色板失败!","",MB_OK);
			return false;
		}
	}
	if(!windowed)
	{
		DDrawFillSurface(lpddsback,0);
		DDrawFillSurface(lpddsprimary,0);
	}
	else
	{
		DDrawFillSurface(lpddsback,0);
	}
	g_rRectClip.top=0;
	g_rRectClip.left=0;
	g_rRectClip.right=screenWidth-1;
	g_rRectClip.bottom=screenHeight-1;
	DDrawAttachClipper(lpddsback,1,&g_rRectClip);
	if (windowed)
	{
		if (FAILED(lpdd->CreateClipper(0,&lpddclipperwin,NULL)))
			return	FALSE;

		if (FAILED(lpddclipperwin->SetHWnd(0, hWnd)))
			return FALSE;

		if (FAILED(lpddsprimary->SetClipper(lpddclipperwin)))
			return FALSE;
	}
	return TRUE;
}

LPDIRECTDRAWSURFACE7 CRenderSystem::CreateSurface(int width, int height, int memflags, USHORT colorKeyValue)
{
	DDSURFACEDESC2 ddsd;      
	LPDIRECTDRAWSURFACE7 lpdds; 

	DDRAW_INIT_STRUCT(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;

	ddsd.dwWidth  =  width;
	ddsd.dwHeight =  height;

	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | memflags;

	if (FAILED(lpdd->CreateSurface(&ddsd,&lpdds,NULL)))
	{
		MessageBox(hWnd,"CreateSurface失败！","error",MB_OK);
		return(NULL);
	}

	DDCOLORKEY color_key; 
	color_key.dwColorSpaceLowValue  = colorKeyValue;
	color_key.dwColorSpaceHighValue = colorKeyValue;
	lpdds->SetColorKey(DDCKEY_SRCBLT, &color_key);
	return lpdds;
}
int CRenderSystem::DDrawFillSurface(LPDIRECTDRAWSURFACE7 lpdds,DWORD color)
{
	DDBLTFX ddbltfx;
	DDRAW_INIT_STRUCT(ddbltfx);
	ddbltfx.dwFillColor=color;
	if(FAILED(lpdds->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddbltfx)))
	{
		MessageBox(NULL,"硬件加速器调用失败！","error",MB_OK);
		return false;
	}
	return true;
}
void CRenderSystem::DDrawAttachClipper(LPDIRECTDRAWSURFACE7 lpdds, int numRects,LPRECT clipList)
{                    
	LPRGNDATA regionData;            
	if (FAILED(lpdd->CreateClipper(0,&lpddclipper,NULL)))
		return ; 
	regionData = (LPRGNDATA)malloc(sizeof(RGNDATAHEADER)+numRects*sizeof(RECT));
	memcpy(regionData->Buffer, clipList, sizeof(RECT)*numRects);

	regionData->rdh.dwSize          = sizeof(RGNDATAHEADER);
	regionData->rdh.iType           = RDH_RECTANGLES;
	regionData->rdh.nCount          = numRects;
	regionData->rdh.nRgnSize        = numRects*sizeof(RECT);

	regionData->rdh.rcBound.left    =  64000;
	regionData->rdh.rcBound.top     =  64000;
	regionData->rdh.rcBound.right   = -64000;
	regionData->rdh.rcBound.bottom  = -64000;

	for (int index=0; index<numRects; index++)
	{
		if (clipList[index].left < regionData->rdh.rcBound.left)
			regionData->rdh.rcBound.left = clipList[index].left;

		if (clipList[index].right > regionData->rdh.rcBound.right)
			regionData->rdh.rcBound.right = clipList[index].right;

		if (clipList[index].top < regionData->rdh.rcBound.top)
			regionData->rdh.rcBound.top = clipList[index].top;

		if (clipList[index].bottom > regionData->rdh.rcBound.bottom)
			regionData->rdh.rcBound.bottom = clipList[index].bottom;

	}
	if (FAILED(lpddclipper->SetClipList(regionData, 0)))
	{
		free(regionData);
		return ;
	} 
	if (FAILED(lpdds->SetClipper(lpddclipper)))
	{
		free(regionData);
		return ;
	} 
	free(regionData);

}
int CRenderSystem::FlipSurface(void)
{
	if(!lpddsback)
	{
		MessageBox(hWnd,"后背表面为空!","error",MB_OK);
		return FALSE;
	}
	if(!windowed)
	{
		while(FAILED(lpddsprimary->Flip(lpddsback,DDFLIP_WAIT)))
		{
			MessageBox(hWnd,"表面反转失败!","error",MB_OK);
			return FALSE;
		}
	}
	else
	{
		RECT destRect;
		GetClientRect(hWnd,&destRect);
		POINT point1,point2;
		point1.x=destRect.left;
		point1.y=destRect.top;
		point2.x=destRect.right;
		point2.y=destRect.bottom;
		ClientToScreen(hWnd,&point1);
		ClientToScreen(hWnd,&point2);
		destRect.left=point1.x;
		destRect.top=point1.y;
		destRect.right=point2.x;
		destRect.bottom=point2.y;

		if(FAILED(lpddsprimary->Blt(&destRect,lpddsback,NULL,DDBLT_WAIT,NULL)))
		{
			MessageBox(hWnd,"FlipSurface--->Blt()调用失败!","error",MB_OK);
			return FALSE;
		}
	}
	return TRUE;
}

UCHAR* CRenderSystem::DDrawLockSurface(LPDIRECTDRAWSURFACE7 lpdds, int& lpitch)
{
	if(!lpdds) return NULL;
	DDRAW_INIT_STRUCT(ddsd);
	if(FAILED(lpdds->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL)))
	{
		MessageBox(hWnd,"加锁内存失败!","error",MB_OK);
		return FALSE;
	}
	lpitch=ddsd.lPitch;
	return (UCHAR*)ddsd.lpSurface;
}

int CRenderSystem::DDrawUnLockSurface(LPDIRECTDRAWSURFACE7 lpdds)
{
	if(lpdds)
		return FALSE;
	if(FAILED(lpdds->Unlock(NULL)))
	{
		MessageBox(hWnd,"解锁内存失败!","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}
UCHAR* CRenderSystem::DDrawLockPrimarySurface()
{
	DDRAW_INIT_STRUCT(ddsd);
	if(FAILED(lpddsprimary->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL)))
	{
		MessageBox(hWnd,"加锁主表面失败!","error",MB_OK);
		return FALSE;
	}
	primarylpitch=ddsd.lPitch;
	primaryBuffer=(UCHAR*)ddsd.lpSurface;
	return primaryBuffer;
}

int CRenderSystem::DDrawUnLockPrimarySurface()
{
	if(FAILED(lpddsprimary->Unlock(NULL)))
	{
		MessageBox(hWnd,"解锁主表面失败!","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}
UCHAR* CRenderSystem::DDrawLockBackSurface()
{
	DDRAW_INIT_STRUCT(ddsd);
	if(FAILED(lpddsback->Lock(NULL,&ddsd,DDLOCK_SURFACEMEMORYPTR|DDLOCK_WAIT,NULL)))
	{
		MessageBox(hWnd,"加锁后备表面失败!","error",MB_OK);
		return FALSE;
	}
	backlpitch=ddsd.lPitch;
	backBuffer=(UCHAR*)ddsd.lpSurface;
	return backBuffer;
}

int CRenderSystem::DDrawUnLockBackSurface()
{
	if(FAILED(lpddsback->Unlock(NULL)))
	{
		MessageBox(hWnd,"解锁后备表面失败!","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}

int CRenderSystem::GetClock(void)
{
	return (GetTickCount());
}

DWORD CRenderSystem::StartClock(void)
{
	return (startClockCount=GetTickCount());
}

DWORD CRenderSystem::WaitClock(DWORD count)
{
	while(GetClock()-StartClock()<count);
	return GetClock();
}

int CRenderSystem::FillRectangle(int x1, int y1, int x2, int y2, int color, LPDIRECTDRAWSURFACE7 lpdds)
{
	DDBLTFX bltfx;
	RECT fillRect;
	DDRAW_INIT_STRUCT(bltfx);
	bltfx.dwFillColor=color;
	fillRect.left=x1;
	fillRect.top=y1;
	fillRect.right=x2;
	fillRect.bottom=y2;
	if(FAILED(lpdds->Blt(&fillRect,NULL,NULL, DDBLT_COLORFILL | DDBLT_WAIT,&bltfx)))
	{
		MessageBox(hWnd,"渲染矩形失败!","error",MB_OK);
		return FALSE;
	}
	return TRUE;

}

int CRenderSystem::SetPaletteEntry(int colorIndex, LPPALETTEENTRY colorPalette)
{

	lpddpal->SetEntries(0,colorIndex,1,colorPalette);

	memcpy(&palette[colorIndex],colorPalette,sizeof(PALETTEENTRY));

	return true;
}

int CRenderSystem::GetPaletteEntry(int colorIndex, LPPALETTEENTRY colorPalette)
{
	memcpy(colorPalette,&palette[colorIndex],sizeof(PALETTEENTRY));
	return TRUE;
}

int CRenderSystem::SavePalette(LPPALETTEENTRY savePalette)
{
	memcpy(savePalette,palette,sizeof(PALETTEENTRY)*256);
	return true;
}

int CRenderSystem::SetPalette(LPPALETTEENTRY setPalette)
{
	memcpy(palette,setPalette,sizeof(PALETTEENTRY)*256);
	lpddpal->SetEntries(0,0,256,palette);
	return true;
}


int CRenderSystem::RotateColor(int startIndex, int endIndex)
{
	int colors=endIndex-startIndex+1;
	PALETTEENTRY wrkPal[256];
	lpddpal->GetEntries(0,startIndex,colors,wrkPal);
	lpddpal->SetEntries(0,startIndex+1,colors-1,wrkPal);
	lpddpal->SetEntries(0,startIndex,1,&wrkPal[colors-1]);
	lpddpal->GetEntries(0,0,256,palette);
	return TRUE;
}

int CRenderSystem::BlinkColors(Blinker command, LPBLINKER newLight, int id)
{
	static BLINKER lights[256]; 
	static int initialized = 0; 

	if (!initialized)
	{
		initialized = 1;
		memset((void *)lights,0, sizeof(lights));
	} 
	switch (command)
	{
	case BLINKER_ADD:
		{
			for (int index=0; index < 256; index++)
			{
				if (lights[index].state == 0)
				{
					lights[index] = *newLight;
					lights[index].counter =  0;
					lights[index].state   = -1;
					lpddpal->SetEntries(0,lights[index].color_index,1,&lights[index].off_color);
					return index;
				}
			} 
		} break;

	case BLINKER_DELETE: 
		{
			if (lights[id].state != 0)
			{
				memset((void *)&lights[id],0,sizeof(BLINKER));
				return id;
			} 
			else
				return -1; 
		} break;

	case BLINKER_UPDATE: 
		{ 
			if (lights[id].state != 0)
			{
				lights[id].on_color  = newLight->on_color; 
				lights[id].off_color = newLight->off_color;
				lights[id].on_time   = newLight->on_time;
				lights[id].off_time  = newLight->off_time; 
				if (lights[id].state == -1)
					lpddpal->SetEntries(0,lights[id].color_index,1,&lights[id].off_color);
				else
					lpddpal->SetEntries(0,lights[id].color_index,1,&lights[id].on_color);
				return id;
			}
			else
				return -1;
		} break;

	case BLINKER_RUN: 
		{
			for (int index=0; index < 256; index++)
			{
				if (lights[index].state == -1)
				{
					if (++lights[index].counter >= lights[index].off_time)
					{
						lights[index].counter = 0;
						lights[index].state = -lights[index].state;               
						lpddpal->SetEntries(0,lights[index].color_index,1,&lights[index].on_color);
					} 
				} 
				else
					if (lights[index].state == 1)
					{
						if (++lights[index].counter >= lights[index].on_time)
						{
							lights[index].counter = 0;
							lights[index].state = -lights[index].state;               
							lpddpal->SetEntries(0,lights[index].color_index,1,&lights[index].off_color);
						}
					} 
			} 
		} break;
	default: break;
	} 
	return TRUE;
}

int CRenderSystem::ScanBitmapImage(CBitmapFile* lpbitmap, LPDIRECTDRAWSURFACE7 lpdds, int cx, int cy)
{
	UCHAR* dest;
	UCHAR* sour;
	DDSURFACEDESC2 ddsd;
	DDRAW_INIT_STRUCT(ddsd);
	if(lpdds->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL))
	{
		MessageBoxA(hWnd,"离屏表面加载位图失败!","Tips",MB_OK);
		return 0;
	}
	cx=cx*(ddsd.dwWidth+1)+1;
	cy=cy*(ddsd.dwHeight+1)+1;
	sour=lpbitmap->buffer+cy*lpbitmap->bitmapinfoheader.biWidth+cx;
	dest=(UCHAR*)ddsd.lpSurface;
	for(DWORD index=0; index<ddsd.dwHeight; index++)
	{
		memcpy(dest,sour,ddsd.dwWidth);
		dest+=ddsd.lPitch;
		sour+=lpbitmap->bitmapinfoheader.biWidth;
	}
	lpdds->Unlock(NULL);
	return true;
}

int CRenderSystem::ScreenTransitions(ScreenAttr effect,UCHAR* vbuffer,int lpitch)
{
	int red,green,blue;
	PALETTEENTRY color;
	PALETTEENTRY wrkPal[256];
	switch(effect)
	{
	case SCREEN_DARKNESS:
		{
			for(int i=0; i<80; i++)
			{
				SavePalette(wrkPal);
				for(int palreg=1; palreg<256; palreg++)
				{
					color=wrkPal[palreg];
					if(color.peRed>4)
						color.peRed-=3;
					else
						color.peRed=0;
					if(color.peGreen>4)
						color.peGreen-=3;
					else
						color.peGreen=0;
					if(color.peBlue>4)
						color.peBlue-=3;
					else
						color.peBlue=0;
					wrkPal[palreg]=color;
				}
				SetPalette(wrkPal);
				StartClock();
				WaitClock(12);
			}
			return TRUE;
		}
	case SCREEN_WHITENESS:
		{
			for(int i=0; i<64; i++)
			{
				SavePalette(wrkPal);
				for(int palreg=0; palreg<256; palreg++)
				{
					color=wrkPal[palreg];
					red=color.peRed;
					green=color.peGreen;
					blue=color.peBlue;

					if((red+=4)>=255)
						red=255;

					if((green+=4)>=255)
						green=255;

					if((blue+=4)>=255)
						blue=255;

					color.peRed=red;
					color.peGreen=green;
					color.peBlue=blue;

					wrkPal[palreg]=color;
				}
				SetPalette(wrkPal);
				StartClock();
				WaitClock(12);
			}
			return TRUE;
		}
	case SCREEN_REDNESS:
		{
			for(int i=0; i<64; i++)
			{
				SavePalette(wrkPal);
				for(int palreg=0; palreg<256; palreg++)
				{
					color=wrkPal[palreg];
					red=color.peRed;
					green=color.peGreen;
					blue=color.peBlue;

					if((red+6)>=255)
						red=255;

					if((green-=4)<=0)
						green=0;

					if((blue-=4)<=0)
						blue=0;

					color.peRed=red;
					color.peGreen=green;
					color.peBlue=blue;

					wrkPal[palreg]=color;
				}
				SetPalette(wrkPal);
				StartClock();
				WaitClock(12);
			}
			return TRUE;
		}
	case SCREEN_BLUENESS:
		{
			for(int i=0; i<64; i++)
			{
				SavePalette(wrkPal);
				for(int palreg=0; palreg<256; palreg++)
				{
					color=wrkPal[palreg];
					red=color.peRed;
					green=color.peGreen;
					blue=color.peBlue;

					if((red-=4)<=0)
						red=0;

					if((green-=4)<=0)
						green=0;

					if((blue+=6)>=255)
						blue=255;

					color.peRed=red;
					color.peGreen=green;
					color.peBlue=blue;

					wrkPal[palreg]=color;
				}
				SetPalette(wrkPal);
				StartClock();
				WaitClock(12);
			}
			return TRUE;
		}
	case SCREEN_GREENNESS:
		{
			for(int i=0; i<64; i++)
			{
				SavePalette(wrkPal);
				for(int palreg=0; palreg<256; palreg++)
				{
					color=wrkPal[palreg];
					red=color.peRed;
					green=color.peGreen;
					blue=color.peBlue;

					if((red-=4)<=0)
						red=0;

					if((green+=6)>=255)
						green=255;

					if((blue-=4)<=0)
						blue=0;

					color.peRed=red;
					color.peGreen=green;
					color.peBlue=blue;

					wrkPal[palreg]=color;
				}
				SetPalette(wrkPal);
				StartClock();
				WaitClock(12);
			}
			return TRUE;
		}
	case SCREEN_SWIPE_X:
		{
			for(int index=0; index<(screenWidth/2); index+=2)
			{
				StartClock();
				WaitClock(12);
				if(screenBPP==8)
				{
					VLine(0,(screenHeight-1),screenWidth-1-index,0,vbuffer,lpitch);
					VLine(0,(screenHeight-1),index,0,vbuffer,lpitch);
					VLine(0,(screenHeight-1),screenWidth-1-(index+1),0,vbuffer,lpitch);
					VLine(0,(screenHeight-1),index+1,0,vbuffer,lpitch);
				}
				else if(screenBPP==16)
				{
					VLine16(0,(screenHeight-1),screenWidth-1-index,0,vbuffer,lpitch);
					VLine16(0,(screenHeight-1),index,0,vbuffer,lpitch);
					VLine16(0,(screenHeight-1),screenWidth-1-(index+1),0,vbuffer,lpitch);
					VLine16(0,(screenHeight-1),index+1,0,vbuffer,lpitch);
				}
			}
			return TRUE;
		}
	case SCREEN_SWIPE_Y:
		{
			for(int index=0; index<(screenHeight/2); index+=2)
			{
				StartClock();
				WaitClock(12);
				if(screenBPP==8)
				{
					HLine(0,(screenWidth-1),screenHeight-1-index,0,vbuffer,lpitch);
					HLine(0,(screenWidth-1),index,0,vbuffer,lpitch);
					HLine(0,(screenWidth-1),screenHeight-1-(index+1),0,vbuffer,lpitch);
					HLine(0,(screenWidth-1),index+1,0,vbuffer,lpitch);
				}
				else if(screenBPP==16)
				{
					HLine16(0,(screenWidth-1),screenHeight-1-index,0,vbuffer,lpitch);
					HLine16(0,(screenWidth-1),index,0,vbuffer,lpitch);
					HLine16(0,(screenWidth-1),screenHeight-1-(index+1),0,vbuffer,lpitch);
					HLine16(0,(screenWidth-1),index+1,0,vbuffer,lpitch);
				}
			}
			return TRUE;
		}
	default:
		return TRUE;
	}
	return TRUE;
}

void CRenderSystem::VLine(int y1, int y2, int x, int color, UCHAR* vbuffer, int lpitch)
{
	UCHAR* startoffset;
	if(x>g_rRectClip.right||x<g_rRectClip.left)
		return ;
	if(y1>y2)
		swap(y1,y2);
	if(y1>g_rRectClip.bottom||y2<g_rRectClip.top)
		return ;
	y1=(y1<g_rRectClip.top)?g_rRectClip.top:y1;
	y2=(y2>g_rRectClip.bottom)?g_rRectClip.bottom:y2;
	startoffset=vbuffer+y1*lpitch+x;
	for(int index=0; index<y2-y1; index++)
	{
		*startoffset=(UCHAR)color;
		startoffset+=lpitch;
	}
}

void CRenderSystem::VLine16(int y1, int y2, int x, int color, UCHAR* vbuffer, int lpitch)
{
	USHORT* startoffset;
	lpitch>>=1;
	if(x>g_rRectClip.right||x<g_rRectClip.left)
		return ;
	if(y1>y2)
		swap(y1,y2);
	if(y1>g_rRectClip.bottom||y2<g_rRectClip.top)
		return ;
	y1=(y1<g_rRectClip.top)?g_rRectClip.top:y1;
	y2=(y2>g_rRectClip.bottom)?g_rRectClip.bottom:y2;
	startoffset=(USHORT*)vbuffer+y1*lpitch+x;
	for(int index=0; index<y2-y1; index++)
	{
		*startoffset=(USHORT)color;
		startoffset+=lpitch;
	}
}

void CRenderSystem::HLine(int x1, int x2, int y, int color, UCHAR* vbuffer, int lpitch)
{
	if(y<g_rRectClip.top||y>g_rRectClip.bottom)
		return ;
	if(x1>x2)
		swap(x1,x2);
	if(x1>g_rRectClip.right||x2<g_rRectClip.left)
		return ;
	x1=(x1<g_rRectClip.left)?g_rRectClip.left:x1;
	x2=(x2>g_rRectClip.right)?g_rRectClip.right:x2;
	memset((UCHAR*)(vbuffer+lpitch*y+x1),(UCHAR)color,x2-x1+1);
}
void CRenderSystem::HLine16(int x1, int x2, int y, int color, UCHAR* vbuffer, int lpitch)
{
	lpitch>>=1;
	if(y<g_rRectClip.top||y>g_rRectClip.bottom)
		return ;
	if(x1>x2)
		swap(x1,x2);
	if(x1>g_rRectClip.right||x2<g_rRectClip.left)
		return ;
	x1=(x1<g_rRectClip.left)?g_rRectClip.left:x1;
	x2=(x2>g_rRectClip.right)?g_rRectClip.right:x2;
	memset((USHORT*)(vbuffer+lpitch*y+x1),(USHORT)color,x2-x1+1);
}



int CRenderSystem::RenderRectangle(int sourceX1, int sourceY1, int sourceX2, int sourceY2, 
								   int destX1, int destY1, int destX2, int destY2, LPDIRECTDRAWSURFACE7 sourceSurface,
								   LPDIRECTDRAWSURFACE7 destSurface)
{
	RECT sourRect;
	RECT destRect;

	sourRect.left=sourceX1;
	sourRect.top=sourceY1;
	sourRect.right=sourceX2;
	sourRect.bottom=sourceY2;

	destRect.left=destX1;
	destRect.top=destY1;
	destRect.right=destX2;
	destRect.bottom=destY2;

	if(FAILED(destSurface->Blt(&destRect,sourceSurface,&sourRect,(DDBLT_WAIT | DDBLT_KEYSRC),NULL)))
	{
		MessageBox(hWnd,"RenderRectangle调用失败,blt失败!","error",MB_OK);
		return FALSE;
	}
	return TRUE;
}

void CRenderSystem::Release(void)
{
	if (ptrRenderList!=NULL)
	{
		delete ptrRenderList;
		ptrRenderList=NULL;
	}

	if(lpddclipperwin!=NULL)
	{
		lpddclipperwin->Release();
		lpddclipperwin=NULL;
	}
	if(lpddclipper!=NULL)
	{
		lpddclipper->Release();
		lpddclipper=NULL;
	}
	if(lpddpal!=NULL)
	{
		lpddpal->Release();
		lpddpal=NULL;
	}
	if(lpddsback!=NULL)
	{
		lpddsback->Release();
		lpddsback=NULL;
	}
	if(lpddsprimary!=NULL)
	{
		lpddsprimary->Release();
		lpddsprimary=NULL;
	}
	if(lpdd!=NULL)
	{
		lpdd->Release();
		lpdd=NULL;
	}
}

void CRenderSystem::DrawPixel(int x,int y,int color)
{
	DDrawLockBackSurface();
	if (screenBPP==8)
	{
		*(backBuffer+y*backlpitch+x)=(UCHAR)color;
	}
	else if (screenBPP==16)
	{
		*((USHORT*)backBuffer+y*(backlpitch>>1)+x)=color;
	}
	DDrawUnLockBackSurface();
}

void CRenderSystem::ScaleOBJ4DV1(CCOBModel* ptrModel,const CVector4& scale,BOOL allFrames)
{
	float maxScale=max(scale.x,scale.y);
	maxScale=max(maxScale,scale.z);
	if (!allFrames)
	{
		for (int i=0;i<ptrModel->numVertices;i++)
		{
			ptrModel->vlocalList[i].x*=scale.x;
			ptrModel->vlocalList[i].y*=scale.y;
			ptrModel->vlocalList[i].z*=scale.z;
		}
		ptrModel->maxRadius[ptrModel->currFrame]*=maxScale;
		ptrModel->avgRadius[ptrModel->currFrame]*=maxScale;
	}
	else
	{
		for (int i=0;i<ptrModel->totalVertices;i++)
		{
			ptrModel->vlocalListHead[i].x*=scale.x;
			ptrModel->vlocalListHead[i].y*=scale.y;
			ptrModel->vlocalListHead[i].z*=scale.z;
		}

		for (int curr=0;curr<ptrModel->numFrames;curr++)
		{
			ptrModel->avgRadius[curr]*=maxScale;
			ptrModel->maxRadius[curr]*=maxScale;
		}
	}

	for (int tri=0;tri<ptrModel->nTriangles;tri++)
	{
		ptrModel->plist[tri].nlength*=((scale.x*scale.x+scale.y*scale.y+scale.z*scale.z)/3);
	}
}
void CRenderSystem::TransOBJModel4DV1(CCOBModel* ptrModel,const CMatrix4x4& mt,int type,BOOL tranbasis,BOOL allFrames)
{
	if (!allFrames)
	{
		switch (type)
		{
		case TRANSFORM_LOCAL_ONLY:
			{
				for (int vert=0; vert<ptrModel->numVertices;vert++)
				{
					ptrModel->vlocalList[vert].v=ptrModel->vlocalList[vert].v*mt;
					if (ptrModel->vlocalList[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vlocalList[vert].n=ptrModel->vlocalList[vert].n*mt;
					}
				}
			}break;
		case TRANSFORM_TRANS_ONLY:
			{
				for (int vert=0;vert<ptrModel->numVertices;vert++)
				{
					ptrModel->vtranslist[vert].v=ptrModel->vtranslist[vert].v*mt;
					if (ptrModel->vtranslist[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vtranslist[vert].n=ptrModel->vtranslist[vert].n*mt;
					}
				}
			}break;
		case TRANSFORM_LOCAL_TO_TRANS:
			{
				for (int vert=0;vert<ptrModel->numVertices;vert++)
				{
					ptrModel->vtranslist[vert].v=ptrModel->vlocalList[vert].v*mt;
					if (ptrModel->vlocalList[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vtranslist[vert].n=ptrModel->vlocalList[vert].n*mt;
					}
				}

			}break;
		}
	}
	else
	{
		switch (type)
		{
		case TRANSFORM_LOCAL_ONLY:
			{
				for (int vert=0; vert<ptrModel->totalVertices;vert++)
				{
					ptrModel->vlocalListHead[vert].v=ptrModel->vlocalListHead[vert].v*mt;
					if (ptrModel->vlocalListHead[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vlocalListHead[vert].n=ptrModel->vlocalListHead[vert].n*mt;
					}
				}

			}break;
		case TRANSFORM_TRANS_ONLY:
			{
				for (int vert=0;vert<ptrModel->totalVertices;vert++)
				{
					ptrModel->vtranslistHead[vert].v=ptrModel->vtranslistHead[vert].v*mt;
					if (ptrModel->vtranslistHead[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vtranslistHead[vert].n=ptrModel->vtranslistHead[vert].n*mt;
					}
				}
			}break;
		case TRANSFORM_LOCAL_TO_TRANS:
			{
				for (int vert=0;vert<ptrModel->numVertices;vert++)
				{
					ptrModel->vtranslistHead[vert].v=ptrModel->vlocalListHead[vert].v*mt;
					if (ptrModel->vlocalListHead[vert].attr & VERTEX_ATTR_NORMAL)
					{
						ptrModel->vtranslistHead[vert].n=ptrModel->vlocalListHead[vert].n*mt;
					}
				}

			}break;
		}
	}
	//检查是否要对朝向向量进行变换
	//如果不变换，朝向向量不再有效
	if (tranbasis)
	{
		ptrModel->ux=ptrModel->ux*mt;
		ptrModel->uy=ptrModel->uy*mt;
		ptrModel->uz=ptrModel->uz*mt;
	}
}
void CRenderSystem::TransRenderList4DV1(const CMatrix4x4& mt,int type)
{
	switch (type)
	{
	case TRANSFORM_LOCAL_ONLY:
		{
			for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
			{
				LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

				if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
					||(triangle->state & TRI4DV1_STATE_CLIPPED)
					||(triangle->state & TRI4DV1_STATE_BACKFACE))
				{
					continue;
				}

				for (int vert=0;vert<3;vert++)
				{
					triangle->vlist[vert].v=triangle->vlist[vert].v*mt;
					if (triangle->vlist[vert].attr & VERTEX_ATTR_NORMAL)
					{
						triangle->vlist[vert].n=triangle->vlist[vert].n*mt;
					}
				}
			}
		}break;
	case TRANSFORM_TRANS_ONLY:
		{
			for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
			{
				LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

				if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
					||(triangle->state & TRI4DV1_STATE_CLIPPED)
					||(triangle->state & TRI4DV1_STATE_BACKFACE))
				{
					continue;
				}

				for (int vert=0;vert<3;vert++)
				{
					triangle->tvlist[vert].v=triangle->tvlist[vert].v*mt;
					if (triangle->tvlist[vert].attr & VERTEX_ATTR_NORMAL)
					{
						triangle->tvlist[vert].n=triangle->tvlist[vert].n*mt;
					}
				}
			}
		}break;
	case TRANSFORM_LOCAL_TO_TRANS:
		{
			for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
			{
				LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

				if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
					||(triangle->state & TRI4DV1_STATE_CLIPPED)
					||(triangle->state & TRI4DV1_STATE_BACKFACE))
				{
					continue;
				}

				for (int vert=0;vert<3;vert++)
				{
					triangle->tvlist[vert].v=triangle->vlist[vert].v*mt;
					if (triangle->vlist[vert].attr & VERTEX_ATTR_NORMAL)
					{
						triangle->tvlist[vert].n=triangle->vlist[vert].n*mt;
					}
				}
			}
		}break;
	default:
		break;
	}
}

void CRenderSystem::LoadTerrainModel(CTerrain* terrain,int insertLocal)
{
	for (int tri=0; tri<terrain->nTriangles; tri++)
	{
		LPTri4DV1 triangle=&terrain->plist[tri];
		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		LPVERTEX oldlist=triangle->vlist;
		if (insertLocal)
		{
			triangle->vlist=terrain->vlocalList;
		}
		else
		{
			triangle->vlist=terrain->vtranslist;
		}
		InsertTri4DV1ToRenderList4DV1(triangle);
	}
}

void CRenderSystem::LoadRenderListFromModel(CCOBModel* ptrModel,BOOL insertLocal)
{
	if (!(ptrModel->state & OBJMODEL4DV1_STATE_ACTIVE)||
		(ptrModel->state & OBJMODEL4DV1_STATE_CULLED)||
		!(ptrModel->state & OBJMODEL4DV1_STATE_VISIBLE))
	{
		return ;
	}

	for (int tri=0; tri<ptrModel->nTriangles; tri++)
	{
		LPTri4DV1 triangle=&ptrModel->plist[tri];
		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		LPVERTEX oldlist=triangle->vlist;
		if (insertLocal)
		{
			triangle->vlist=ptrModel->vlocalList;
		}
		else
		{
			triangle->vlist=ptrModel->vtranslist;
		}
		InsertTri4DV1ToRenderList4DV1(triangle);

		triangle->vlist=oldlist;
	}
}

void CRenderSystem::InsertTri4DV1ToRenderList4DV1(LPTri4DV1 triangle)
{
	if (ptrRenderList->nTriangles>=MAX_RENDERLIST4DV1_TRIANGLES)
	{
		return;
	}

	ptrRenderList->triPtrs[ptrRenderList->nTriangles]=&ptrRenderList->triData[ptrRenderList->nTriangles];

	ptrRenderList->triData[ptrRenderList->nTriangles].attr=triangle->attr;
	ptrRenderList->triData[ptrRenderList->nTriangles].state=triangle->state;
	ptrRenderList->triData[ptrRenderList->nTriangles].color=triangle->color;
	ptrRenderList->triData[ptrRenderList->nTriangles].nlength=triangle->nlength;
	ptrRenderList->triData[ptrRenderList->nTriangles].ptrTexture=triangle->ptrTexture;

	ptrRenderList->triData[ptrRenderList->nTriangles].litColor[0]=triangle->litColor[0];
	ptrRenderList->triData[ptrRenderList->nTriangles].litColor[1]=triangle->litColor[1];
	ptrRenderList->triData[ptrRenderList->nTriangles].litColor[2]=triangle->litColor[2];

	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[0]=triangle->vlist[triangle->vert[0]];
	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[1]=triangle->vlist[triangle->vert[1]];
	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[2]=triangle->vlist[triangle->vert[2]];

	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[0]=triangle->vlist[triangle->vert[0]];
	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[1]=triangle->vlist[triangle->vert[1]];
	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[2]=triangle->vlist[triangle->vert[2]];

	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[0].t=triangle->textlist[triangle->text[0]];
	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[1].t=triangle->textlist[triangle->text[1]];
	ptrRenderList->triData[ptrRenderList->nTriangles].vlist[2].t=triangle->textlist[triangle->text[2]];

	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[0].t=triangle->textlist[triangle->text[0]];
	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[1].t=triangle->textlist[triangle->text[1]];
	ptrRenderList->triData[ptrRenderList->nTriangles].tvlist[2].t=triangle->textlist[triangle->text[2]];

	if (ptrRenderList->nTriangles==0)
	{
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->next=NULL;
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->prev=NULL;
	}
	else
	{
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->prev=ptrRenderList->triPtrs[ptrRenderList->nTriangles-1];
		ptrRenderList->triPtrs[ptrRenderList->nTriangles-1]->next=ptrRenderList->triPtrs[ptrRenderList->nTriangles];
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->next=NULL;
	}
	ptrRenderList->nTriangles++;
}

void CRenderSystem::InsertTriF4DV1ToRenderList4DV1(LPTriF4DV1 triangle)
{
	if (ptrRenderList->nTriangles>=MAX_RENDERLIST4DV1_TRIANGLES)
	{
		return;
	}

	ptrRenderList->triPtrs[ptrRenderList->nTriangles]=&ptrRenderList->triData[ptrRenderList->nTriangles];
	memcpy(&ptrRenderList->triData[ptrRenderList->nTriangles],triangle,sizeof(TriF4DV1));

	if (ptrRenderList->nTriangles==0)
	{
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->next=NULL;
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->prev=NULL;
	}
	else
	{
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->prev=ptrRenderList->triPtrs[ptrRenderList->nTriangles-1];
		ptrRenderList->triPtrs[ptrRenderList->nTriangles-1]->next=ptrRenderList->triPtrs[ptrRenderList->nTriangles];
		ptrRenderList->triPtrs[ptrRenderList->nTriangles]->next=NULL;
	}
	ptrRenderList->nTriangles++;
}

void CRenderSystem::CreateRotateMatrix4x4(float thetaX,float thetaY,float thetaZ,CMatrix4x4& mrot)
{
	CMatrix4x4 mx,my,mz;
	float sinTheta=0,cosTheta=0;
	int rotSeq=0;  //1  x, 2  y, 4  z

	mrot.Identity();
	if (fabs(thetaX)>EPSILON_E5)
	{
		rotSeq|=1;
	}
	if (fabs(thetaY)>EPSILON_E5)
	{
		rotSeq|=2;
	}
	if (fabs(thetaZ)>EPSILON_E5)
	{
		rotSeq|=4;
	}

	switch (rotSeq)
	{
	case 0:
		return;
	case 1:    //x rotation
		{
			sinTheta=FastSin(thetaX);
			cosTheta=FastCos(thetaX);
			mrot=CMatrix4x4(1,    0,          0,         0,
				0,    cosTheta,  sinTheta, 0,
				0,   -sinTheta,  cosTheta, 0,
				0,    0,          0,         1);
			return;
		}
	case 2:  //y rotation
		{
			sinTheta=FastSin(thetaY);
			cosTheta=FastCos(thetaY);
			mrot=CMatrix4x4(cosTheta, 0, -sinTheta, 0,  
				0,         1,  0,         0,
				sinTheta, 0, cosTheta,  0,
				0,         0, 0,          1);
			return;
		}
	case 3:  //xy rotation
		{
			sinTheta=FastSin(thetaX);
			cosTheta=FastCos(thetaX);
			mx=CMatrix4x4(1,    0,          0,         0,
				0,    cosTheta,  sinTheta, 0,
				0,   -sinTheta,  cosTheta, 0,
				0,    0,          0,         1);

			sinTheta=FastSin(thetaY);
			cosTheta=FastCos(thetaY);
			my=CMatrix4x4(cosTheta, 0, -sinTheta, 0,  
				0,         1,  0,         0,
				sinTheta, 0, cosTheta,  0,
				0,         0, 0,          1);
			mrot=mx*my;
			return;
		}
	case 4:  //z rotation
		{
			sinTheta=FastSin(thetaZ);
			cosTheta=FastCos(thetaZ);
			mrot=CMatrix4x4(cosTheta, sinTheta, 0, 0,  
				-sinTheta, cosTheta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);
			return;
		}
	case 5:  //xz rotation
		{
			sinTheta=FastSin(thetaX);
			cosTheta=FastCos(thetaX);
			mx=CMatrix4x4(1,    0,          0,         0,
				0,    cosTheta,  sinTheta, 0,
				0,   -sinTheta,  cosTheta, 0,
				0,    0,          0,         1);

			sinTheta=FastSin(thetaZ);
			cosTheta=FastCos(thetaZ);
			mz=CMatrix4x4(cosTheta, sinTheta, 0, 0,  
				-sinTheta, cosTheta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);
			mrot=mx*mz;
			return;
		}
	case 6:  //yz rotation
		{
			sinTheta=FastSin(thetaY);
			cosTheta=FastCos(thetaY);
			my=CMatrix4x4(cosTheta, 0, -sinTheta, 0,  
				0,         1,  0,         0,
				sinTheta, 0, cosTheta,  0,
				0,         0, 0,          1);

			sinTheta=FastSin(thetaZ);
			cosTheta=FastCos(thetaZ);
			mz=CMatrix4x4(cosTheta, sinTheta, 0, 0,  
				-sinTheta, cosTheta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);
			mrot=my*mz;
			return;
		}
	case 7:
		{
			sinTheta=FastSin(thetaX);
			cosTheta=FastCos(thetaX);
			mx=CMatrix4x4(1,    0,          0,         0,
				0,    cosTheta,  sinTheta, 0,
				0,   -sinTheta,  cosTheta, 0,
				0,    0,          0,         1);

			sinTheta=FastSin(thetaY);
			cosTheta=FastCos(thetaY);
			my=CMatrix4x4(cosTheta, 0, -sinTheta, 0,  
				0,         1,  0,         0,
				sinTheta, 0, cosTheta,  0,
				0,         0, 0,          1);
			CMatrix4x4 temp;
			temp=mx*my;
			sinTheta=FastSin(thetaZ);
			cosTheta=FastCos(thetaZ);
			mz=CMatrix4x4(cosTheta, sinTheta, 0, 0,  
				-sinTheta, cosTheta, 0, 0,
				0,         0,         1, 0,
				0,         0,         0, 1);
			mrot=temp*mz;
			return;
		}
	default:
		break;
	}
}
void CRenderSystem::ModelToWorldOBJ4DV1(CCOBModel* ptrModel,int type,BOOL allFrames)
{
	if (!allFrames)
	{
		if (type==TRANSFORM_LOCAL_TO_TRANS)
		{
			for (int vert=0;vert<ptrModel->numVertices;vert++)
			{
				ptrModel->vtranslist[vert].v=ptrModel->vlocalList[vert].v
					+ptrModel->worldPos;
				ptrModel->vtranslist[vert].n=ptrModel->vlocalList[vert].n;
			}
		}
		else
		{
			for (int vert=0;vert<ptrModel->numVertices;vert++)
			{
				ptrModel->vtranslist[vert].v=ptrModel->vtranslist[vert].v
					+ptrModel->worldPos;
			}
		}
	}
	else
	{
		if (type==TRANSFORM_LOCAL_TO_TRANS)
		{
			for (int vert=0;vert<ptrModel->totalVertices;vert++)
			{
				ptrModel->vtranslistHead[vert].v=ptrModel->vlocalListHead[vert].v
					+ptrModel->worldPos;
				ptrModel->vtranslistHead[vert].n=ptrModel->vlocalListHead[vert].n;
			}
		}
		else
		{
			for (int vert=0;vert<ptrModel->totalVertices;vert++)
			{
				ptrModel->vtranslistHead[vert].v=ptrModel->vtranslistHead[vert].v
					+ptrModel->worldPos;
			}
		}
	}
}

void CRenderSystem::CreateWorldMatrix4x4(CVector4& vpos,CMatrix4x4& Worldmatrix)
{
	Worldmatrix=CMatrix4x4(1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		vpos.x,vpos.y,vpos.z,1);
}

void CRenderSystem::ModelToWorldRenderList4DV1(CVector4& Worldpos,int type)
{
	if (type==TRANSFORM_LOCAL_TO_TRANS)
	{
		for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
		{
			LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

			if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
				||(triangle->state & TRI4DV1_STATE_CLIPPED)
				||(triangle->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			for (int vert=0;vert<3;vert++)
			{
				triangle->tvlist[vert].v=triangle->vlist[vert].v+Worldpos;
				triangle->tvlist[vert].n=triangle->vlist[vert].n;
			}
		}
	}
	else
	{
		for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
		{
			LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

			if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
				||(triangle->state & TRI4DV1_STATE_CLIPPED)
				||(triangle->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			for (int vert=0;vert<3;vert++)
			{
				triangle->tvlist[vert].v=triangle->tvlist[vert].v+Worldpos;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//世界坐标到相机坐标专用变换矩阵
//////////////////////////////////////////////////////////////////////////
void CRenderSystem::WorldToCamOBJ4DV1(CCOBModel* ptrModel,const CCamera& cam)
{
	for (int vert=0; vert<ptrModel->numVertices;vert++)
	{
		ptrModel->vtranslist[vert].v=ptrModel->vtranslist[vert].v*cam.mcam;
	}
}


void CRenderSystem::WorldToCamRenderList4DV1(const CCamera& cam)
{
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];
		if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
			||(triangle->state & TRI4DV1_STATE_CLIPPED)
			||(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		for (int vert=0;vert<3;vert++)
		{
			triangle->tvlist[vert].v=triangle->tvlist[vert].v*cam.mcam;
		}
	}
}

int CRenderSystem::CullOBJ4DV1(CCOBModel* ptrModel,const CCamera& cam,int cullFlag)
{

	//第1步：将物体的包围球球心变换为相机坐标
	CVector4 spherePos;
	spherePos=ptrModel->worldPos*cam.mcam;

	//第2步：根据删除标记对物体执行删除操作
	if (cullFlag & CULLED_Z_PLANE)
	{
		//只根据远近截面来删除物体
		if (((spherePos.z-ptrModel->maxRadius[ptrModel->currFrame])>cam.farZ) 
			||((spherePos.z+ptrModel->maxRadius[ptrModel->currFrame])<cam.nearZ))
		{
			SET_BIT(ptrModel->state,OBJMODEL4DV1_STATE_CULLED);
			return TRUE;
		}
	}

	if (cullFlag & CULLED_X_PLANE)
	{
		//只根据左右截面对物体进行裁剪

		//使用右裁剪面和左裁剪面对包围球最左边点和最右边点进行测试
		float testZ=0.5*cam.viewPlaneWidth*spherePos.z/cam.viewDist;  //利用三角形相似
		if (((spherePos.x-ptrModel->maxRadius[ptrModel->currFrame])>testZ)
			||((spherePos.x+ptrModel->maxRadius[ptrModel->currFrame])<-testZ))
		{
			SET_BIT(ptrModel->state,OBJMODEL4DV1_STATE_CULLED);
			return TRUE;
		}
	}

	if (cullFlag & CULLED_Y_PLANE)
	{
		//只根据上下截面对物体进行裁剪

		//使用上裁剪面和下裁剪面对包围球最下边点和最上边点进行测试
		float testZ=0.5*cam.viewPlaneHeight*spherePos.z/cam.viewDist;   //利用三角形相似 
		if (((spherePos.y-ptrModel->maxRadius[ptrModel->currFrame])>testZ)
			||((spherePos.y+ptrModel->maxRadius[ptrModel->currFrame])<-testZ))
		{
			SET_BIT(ptrModel->state,OBJMODEL4DV1_STATE_CULLED);
			return TRUE;
		}
	}

	return FALSE;
}

void CRenderSystem::ResetOBJ4DV1(CCOBModel* ptrModel)
{

	RESET_BIT(ptrModel->state,OBJMODEL4DV1_STATE_CULLED);
	//重置多边形的被裁剪掉和背面标记
	for (int tri=0;tri<ptrModel->nTriangles;tri++)
	{
		LPTri4DV1 triangle=&ptrModel->plist[tri];

		if (!(triangle->state & TRI4DV1_STATE_ACTIVE))
		{
			continue;
		}

		RESET_BIT(triangle->state,TRI4DV1_STATE_CLIPPED);
		RESET_BIT(triangle->state,TRI4DV1_STATE_BACKFACE);
		RESET_BIT(triangle->state,TRI4DV1_STATE_LIT);
	}
}

void CRenderSystem::RemoveBackfaceOBJ4DV1(CCOBModel* ptrModel,const CCamera& cam)
{
	//此函数根据数组vtranslist中的顶点数据以及相机位置消除
	//物体的背面多边形
	if (ptrModel->state & OBJMODEL4DV1_STATE_CULLED)
	{
		return ;
	}

	for (int tri=0;tri<ptrModel->nTriangles;tri++)
	{
		LPTri4DV1 triangle=&ptrModel->plist[tri];

		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE)||
			(triangle->attr & TRI4DV1_ATTR_2SIDED))
		{
			continue;
		}

		//多边形非自包含
		int vert0=triangle->vert[0];
		int vert1=triangle->vert[1];
		int vert2=triangle->vert[2];

		//计算多边形的面法线
		//u=p0->p1,v=p0->p2,n=uxv;
		CVector4 u,v,n;
		u.CreateVector(ptrModel->vtranslist[vert0].v,ptrModel->vtranslist[vert1].v);
		u.Normalize();
		v.CreateVector(ptrModel->vtranslist[vert0].v,ptrModel->vtranslist[vert2].v);
		v.Normalize();
		n=u.CrossProduct(v);

		//创建指向视点的向量
		CVector4 view;
		view.CreateVector(ptrModel->vtranslist[vert0].v,cam.pos);

		float dp=view.DotProduct(n);
		if (dp<=EPSILON_E5)
		{
			SET_BIT(triangle->state,TRI4DV1_STATE_BACKFACE);
		}
	}

}

void CRenderSystem::RemoveBackfaceRenderList4DV1(const CCamera& cam)
{
	for (int tri=0; tri<ptrRenderList->nTriangles; tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE)||
			(triangle->attr & TRI4DV1_ATTR_2SIDED))
		{
			continue;
		}

		//计算多边形的面法线
		//u=p0->p1,v=p0->p2,n=uxv;
		CVector4 u,v,n;
		u.CreateVector(triangle->tvlist[0].v,triangle->tvlist[1].v);
		v.CreateVector(triangle->tvlist[0].v,triangle->tvlist[2].v);
		n=u.CrossProduct(v);

		//创建指向视点的向量
		CVector4 viewPos;
		viewPos.CreateVector(triangle->tvlist[0].v,cam.pos);

		float dp=n.DotProduct(viewPos);
		if (dp<=EPSILON_E5)
		{
			SET_BIT(triangle->state,TRI4DV1_STATE_BACKFACE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//将物体的每个顶点变换为透视坐标
//这里假设物体已经变换为相机坐标存储在vtranslist[]中
//////////////////////////////////////////////////////////////////////////
void CRenderSystem::CameraToPersOBJ4DV1(CCOBModel* ptrModel,const CCamera& cam)
{
	for (int vert=0;vert<ptrModel->numVertices;vert++)
	{
		float z=ptrModel->vtranslist[vert].z;
		ptrModel->vtranslist[vert].x=cam.viewDist*ptrModel->vtranslist[vert].x/z;
		ptrModel->vtranslist[vert].y=cam.viewDist*ptrModel->vtranslist[vert].y*cam.aspectRatio/z;
	}
}

void CRenderSystem::PersToScreenOBJ4DV1(CCOBModel* ptrModel,const CCamera& cam)
{
	float alpha=(0.5*cam.viewPortWidth-0.5);
	float beta=(0.5*cam.viewPortHeight-0.5);

	for (int vert=0;vert<ptrModel->numVertices;vert++)
	{
		ptrModel->vtranslist[vert].x=alpha+alpha*ptrModel->vtranslist[vert].x;
		ptrModel->vtranslist[vert].y=beta-beta*ptrModel->vtranslist[vert].y;
	}

}

void CRenderSystem::CameraToPersRenderList4DV1(const CCamera& cam)
{
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

		if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}

		for (int vert=0; vert<3; vert++)
		{
			float z=triangle->tvlist[vert].z;
			triangle->tvlist[vert].x=cam.viewDist*triangle->tvlist[vert].x/z;
			triangle->tvlist[vert].y=cam.viewDist*triangle->tvlist[vert].y*cam.aspectRatio/z;
		}
	}
}

void CRenderSystem::PersToScreenRenderList4DV1(const CCamera& cam)
{
	float alpha=(0.5*cam.viewPortWidth-0.5);
	float beta=(0.5*cam.viewPortHeight-0.5);

	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

		if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}

		for (int vert=0; vert<3; vert++)
		{
			triangle->tvlist[vert].x=alpha+alpha*triangle->tvlist[vert].x;
			triangle->tvlist[vert].y=beta-beta*triangle->tvlist[vert].y;
		}
	}
}
void CRenderSystem::CreateCameraToPersMatrix4x4(const CCamera& cam,CMatrix4x4& mt)
{
	mt=CMatrix4x4(cam.viewDist,       0,        0,         0,
		0,cam.viewDist*cam.aspectRatio,0,        0,
		0,                  0,         1,        1,
		0,                  0,         0,        0);
}

void CRenderSystem::ConvertToNotHomogeneous4DOBJ4DV1(CCOBModel* ptrModel)
{

	for (int vert=0; vert<ptrModel->numVertices;vert++)
	{
		ptrModel->vtranslist[vert].v.ConvertToNotHomogeneous();
	}

}

void CRenderSystem::ConvertToNotHomogeneous4DRenderList4DV1()
{
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];

		if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}

		for (int vert=0; vert<3; vert++)
		{
			triangle->tvlist[vert].v.ConvertToNotHomogeneous();
		}
	}
}

void CRenderSystem::CreatePersToScreen4DMatrix4x4(const CCamera&cam,CMatrix4x4& mt)
{
	float alpha=(0.5*cam.viewPortWidth-0.5);
	float beta=(0.5*cam.viewPortHeight-0.5);

	mt=CMatrix4x4(alpha,0,0,0,
		0,-beta,0,0,
		alpha,beta,1,0,
		0,0,0,1);
}
void CRenderSystem::DrawOBJ4DV1Wire16(CCOBModel* ptrModel)
{
	CLine2D line2D;
	for (int i=0;i<numModels;i++)
	{
		for (int tri=0; tri<ptrModel->nTriangles; tri++)
		{
			if (!(ptrModel->plist[tri].state & TRI4DV1_STATE_ACTIVE)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_CLIPPED)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			int vert0=ptrModel->plist[tri].vert[0];
			int vert1=ptrModel->plist[tri].vert[1];
			int vert2=ptrModel->plist[tri].vert[2];

			line2D.DrawClipLine162D(ptrModel->vtranslist[vert0].x,ptrModel->vtranslist[vert0].y,
				ptrModel->vtranslist[vert1].x,ptrModel->vtranslist[vert1].y,ptrModel->plist[tri].litColor[0],
				backBuffer,backlpitch);
			line2D.DrawClipLine162D(ptrModel->vtranslist[vert1].x,ptrModel->vtranslist[vert1].y,
				ptrModel->vtranslist[vert2].x,ptrModel->vtranslist[vert2].y,ptrModel->plist[tri].litColor[0],
				backBuffer,backlpitch);
			line2D.DrawClipLine162D(ptrModel->vtranslist[vert2].x,ptrModel->vtranslist[vert2].y,
				ptrModel->vtranslist[vert0].x,ptrModel->vtranslist[vert0].y,ptrModel->plist[tri].litColor[0],
				backBuffer,backlpitch);
		}
	}
}

void CRenderSystem::DrawRenderList4DV1Wire16()
{
	CLine2D line2D;
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		if (!(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		line2D.DrawClipLine162D(ptrRenderList->triPtrs[tri]->tvlist[0].x,
			ptrRenderList->triPtrs[tri]->tvlist[0].y,ptrRenderList->triPtrs[tri]->tvlist[1].x,
			ptrRenderList->triPtrs[tri]->tvlist[1].y,ptrRenderList->triPtrs[tri]->litColor[0],
			backBuffer,backlpitch);
		line2D.DrawClipLine162D(ptrRenderList->triPtrs[tri]->tvlist[1].x,
			ptrRenderList->triPtrs[tri]->tvlist[1].y,ptrRenderList->triPtrs[tri]->tvlist[2].x,
			ptrRenderList->triPtrs[tri]->tvlist[2].y,ptrRenderList->triPtrs[tri]->litColor[0],
			backBuffer,backlpitch);
		line2D.DrawClipLine162D(ptrRenderList->triPtrs[tri]->tvlist[2].x,
			ptrRenderList->triPtrs[tri]->tvlist[2].y,ptrRenderList->triPtrs[tri]->tvlist[0].x,
			ptrRenderList->triPtrs[tri]->tvlist[0].y,ptrRenderList->triPtrs[tri]->litColor[0],
			backBuffer,backlpitch);
	}
}

//z缓存不支持alpha
void CRenderSystem::DrawOBJ4DV1SolidZB16(CCOBModel* ptrModel,CZBuffer& zb)
{
	CTriangle2D triangle;
	TriF4DV1 face;
	for (int i=0;i<numModels;i++)
	{
		for (int tri=0; tri<ptrModel->nTriangles; tri++)
		{
			if (!(ptrModel->plist[tri].state & TRI4DV1_STATE_ACTIVE)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_CLIPPED)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			int vert0=ptrModel->plist[tri].vert[0];
			int vert1=ptrModel->plist[tri].vert[1];
			int vert2=ptrModel->plist[tri].vert[2];

			face.tvlist[0].v=ptrModel->vtranslist[vert0].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert0].t;
			face.litColor[0]=ptrModel->plist[tri].litColor[0];

			face.tvlist[1].v=ptrModel->vtranslist[vert1].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert1].t;
			face.litColor[1]=ptrModel->plist[tri].litColor[1];

			face.tvlist[2].v=ptrModel->vtranslist[vert2].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert2].t;
			face.litColor[2]=ptrModel->plist[tri].litColor[2];

			face.ptrTexture=ptrModel->ptrTexture;

			if ((ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
				(ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
			{
				triangle.DrawTriangleZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				triangle.DrawGouraudTriangleZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					triangle.DrawTextureTriangleZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					triangle.DrawTextureTriangleZBFS16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}		
		}
	}

}

void CRenderSystem::DrawRenderList4DV1SolidZB16(CZBuffer& zb)
{
	CTriangle2D triangle;
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		if (!(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
		{
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
			{
				triangle.DrawTextureTriangleZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
			{
				triangle.DrawTextureTriangleZBFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			if(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				triangle.DrawTextureTriangleZBGS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
		}		
		else if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			triangle.DrawGouraudTriangleZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
		}
		else if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
			(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			triangle.DrawTriangleZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
		}
	}
}

//z缓存支持alpha
void CRenderSystem::DrawOBJ4DV1SolidZBAlpha16(CCOBModel* ptrModel,CZBuffer& zb,int alphaOverride)
{
	CTriangle2D triangle;
	TriF4DV1 face;
	int alpha;
	for (int i=0;i<numModels;i++)
	{
		for (int tri=0; tri<ptrModel->nTriangles; tri++)
		{
			if (!(ptrModel->plist[tri].state & TRI4DV1_STATE_ACTIVE)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_CLIPPED)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			if (alphaOverride>=0)
			{
				alpha=alphaOverride;
			}
			else
			{
				alpha=(ptrRenderList->triPtrs[tri]->color & 0xff000000)>>24;
			}

			int vert0=ptrModel->plist[tri].vert[0];
			int vert1=ptrModel->plist[tri].vert[1];
			int vert2=ptrModel->plist[tri].vert[2];

			face.tvlist[0].v=ptrModel->vtranslist[vert0].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert0].t;
			face.litColor[0]=ptrModel->plist[tri].litColor[0];

			face.tvlist[1].v=ptrModel->vtranslist[vert1].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert1].t;
			face.litColor[1]=ptrModel->plist[tri].litColor[1];

			face.tvlist[2].v=ptrModel->vtranslist[vert2].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert2].t;
			face.litColor[2]=ptrModel->plist[tri].litColor[2];

			face.ptrTexture=ptrModel->ptrTexture;

			if ((ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
				(ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
			{

				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTriangleZBAlpha162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTriangleZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawGouraudTriangleZBAlpha16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
					{
						triangle.DrawTextureTriangleZBAlpha16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
					}
					else
					{
						triangle.DrawTextureTriangleZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
					}	
				}
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
					{
						triangle.DrawTextureTriangleZBAlphaFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
					}
					else
					{
						triangle.DrawTextureTriangleZBFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
					}
				}
			}		
		}
	}
}

void CRenderSystem::DrawRenderList4DV1SolidZBAlpha16(CZBuffer& zb,int alphaOverride)
{
	CTriangle2D triangle;
	int alpha;
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		if (!(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}

		if (alphaOverride>=0)
		{
			alpha=alphaOverride;
		}
		else
		{
			alpha=(ptrRenderList->triPtrs[tri]->color & 0xff000000)>>24;
		}

		if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
		{
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleZBAlpha16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}	
			}
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleZBAlphaFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleZBFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			if(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleZBAlphaGS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleZBGS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
		}		
		else if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
			{
				triangle.DrawGouraudTriangleZBAlpha16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
			}
			else
			{
				triangle.DrawGouraudTriangleZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}

		}
		else if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
			(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
			{
				triangle.DrawTriangleZBAlpha162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
			}
			else
			{
				triangle.DrawTriangleZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
		}
	}
}




//1/z缓存不支持alpha
void CRenderSystem::DrawOBJ4DV1SolidINVZB16(CCOBModel* ptrModel,CZBuffer& zb)
{
	CTriangle2D triangle;
	TriF4DV1 face;
	for (int i=0;i<numModels;i++)
	{
		for (int tri=0; tri<ptrModel->nTriangles; tri++)
		{
			if (!(ptrModel->plist[tri].state & TRI4DV1_STATE_ACTIVE)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_CLIPPED)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			int vert0=ptrModel->plist[tri].vert[0];
			int vert1=ptrModel->plist[tri].vert[1];
			int vert2=ptrModel->plist[tri].vert[2];

			face.tvlist[0].v=ptrModel->vtranslist[vert0].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert0].t;
			face.litColor[0]=ptrModel->plist[tri].litColor[0];

			face.tvlist[1].v=ptrModel->vtranslist[vert1].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert1].t;
			face.litColor[1]=ptrModel->plist[tri].litColor[1];

			face.tvlist[2].v=ptrModel->vtranslist[vert2].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert2].t;
			face.litColor[2]=ptrModel->plist[tri].litColor[2];

			face.ptrTexture=ptrModel->ptrTexture;

			if ((ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
				(ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
			{
				triangle.DrawTriangleINVZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				triangle.DrawGouraudTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					triangle.DrawTextureTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					triangle.DrawTextureTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}		
		}
	}

}

void CRenderSystem::DrawRenderList4DV1SolidINVZB16(CZBuffer& zb)
{
	CTriangle2D triangle;
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		if (!(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
		{
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
			{
				triangle.DrawTextureTriangleINVZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
			{
				triangle.DrawTextureTriangleINVZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
			if(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				triangle.DrawTextureTriangleINVZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
		}		
		else if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			triangle.DrawGouraudTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
		}
		else if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
			(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			triangle.DrawTriangleINVZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
		}
	}
}

//1/z缓存支持alpha
void CRenderSystem::DrawOBJ4DV1SolidINVZBAlpha16(CCOBModel* ptrModel,CZBuffer& zb,int alphaOverride)
{
	CTriangle2D triangle;
	TriF4DV1 face;
	int alpha;
	for (int i=0;i<numModels;i++)
	{
		for (int tri=0; tri<ptrModel->nTriangles; tri++)
		{
			if (!(ptrModel->plist[tri].state & TRI4DV1_STATE_ACTIVE)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_CLIPPED)||
				(ptrModel->plist[tri].state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}

			if (alphaOverride>=0)
			{
				alpha=alphaOverride;
			}
			else
			{
				alpha=(ptrRenderList->triPtrs[tri]->color & 0xff000000)>>24;
			}

			int vert0=ptrModel->plist[tri].vert[0];
			int vert1=ptrModel->plist[tri].vert[1];
			int vert2=ptrModel->plist[tri].vert[2];

			face.tvlist[0].v=ptrModel->vtranslist[vert0].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert0].t;
			face.litColor[0]=ptrModel->plist[tri].litColor[0];

			face.tvlist[1].v=ptrModel->vtranslist[vert1].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert1].t;
			face.litColor[1]=ptrModel->plist[tri].litColor[1];

			face.tvlist[2].v=ptrModel->vtranslist[vert2].v;
			face.tvlist[0].t=ptrModel->vtranslist[vert2].t;
			face.litColor[2]=ptrModel->plist[tri].litColor[2];

			face.ptrTexture=ptrModel->ptrTexture;

			if ((ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
				(ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
			{

				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTriangleINVZBAlpha162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTriangleINVZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawGouraudTriangleINVZBAlpha16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			else if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
					{
						triangle.DrawTextureTriangleINVZBAlpha16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
					}
					else
					{
						triangle.DrawTextureTriangleINVZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
					}	
				}
				if (ptrModel->plist[tri].attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
					{
						triangle.DrawTextureTriangleINVZBAlphaFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
					}
					else
					{
						triangle.DrawTextureTriangleINVZBFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
					}
				}
			}		
		}
	}
}

void CRenderSystem::DrawRenderList4DV1SolidINVZBAlpha16(CZBuffer& zb,int alphaOverride)
{
	CTriangle2D triangle;
	int alpha;
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		if (!(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
			(ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}

		if (alphaOverride>=0)
		{
			alpha=alphaOverride;
		}
		else
		{
			alpha=(ptrRenderList->triPtrs[tri]->color & 0xff000000)>>24;
		}

		if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
		{
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleINVZBAlpha16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleINVZB16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}	
			}
			if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleINVZBAlphaFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleINVZBFS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
			if(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
				{
					triangle.DrawTextureTriangleINVZBAlphaGS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
				}
				else
				{
					triangle.DrawTextureTriangleINVZBGS16(ptrRenderList->triPtrs[tri],backBuffer,backlpitch,zb.zbuffer,zb.width*4);
				}
			}
		}		
		else if (ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
			{
				triangle.DrawGouraudTriangleINVZBAlpha16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
			}
			else
			{
				triangle.DrawGouraudTriangleINVZB16(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}

		}
		else if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)||
			(ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT))
		{
			TriF4DV1 face;
			face.tvlist[0].v=ptrRenderList->triPtrs[tri]->tvlist[0].v;
			face.tvlist[1].v=ptrRenderList->triPtrs[tri]->tvlist[1].v;
			face.tvlist[2].v=ptrRenderList->triPtrs[tri]->tvlist[2].v;
			face.litColor[0]=ptrRenderList->triPtrs[tri]->litColor[0];
			face.litColor[1]=ptrRenderList->triPtrs[tri]->litColor[1];
			face.litColor[2]=ptrRenderList->triPtrs[tri]->litColor[2];
			if ((ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||(alphaOverride>=0))
			{
				triangle.DrawTriangleINVZBAlpha162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4,alpha);
			}
			else
			{
				triangle.DrawTriangleINVZB162D(&face,backBuffer,backlpitch,zb.zbuffer,zb.width*4);
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//物体反射环境光、散射光、镜面反射
//////////////////////////////////////////////////////////////////////////
int CRenderSystem::LightOBJ4DV1World16(CCOBModel* ptrModel,const CCamera& camera,const CLight& light)
{
	UINT rBase=0,gBase=0,bBase=0;
	UINT rSum=0,gSum=0,bSum=0;
	UINT rSum0=0,gSum0=0,bSum0=0;
	UINT rSum1=0,gSum1=0,bSum1=0;
	UINT rSum2=0,gSum2=0,bSum2=0;
	UINT ShadeColor=0;        //最终着色的颜色
	float dp=0;                //向量点积
	float dist=0;              //光到表面的距离
	float I=0;                 //光的总强度
	float length=0;            //向量的长度
	float atten=0;             //光的衰减

	if (!(ptrModel->state & OBJMODEL4DV1_STATE_ACTIVE)||
		(ptrModel->state & OBJMODEL4DV1_STATE_CULLED)||
		!(ptrModel->state & OBJMODEL4DV1_STATE_VISIBLE))
	{
		return FALSE;
	}
	//处理每个三角形
	for (int tri=0;tri<ptrModel->nTriangles;tri++)
	{
		rSum=0;gSum=0;bSum=0;
		LPTri4DV1 triangle=&ptrModel->plist[tri];
		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_BACKFACE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_LIT))
		{
			continue;
		}
		int vert0=triangle->vert[0];
		int vert1=triangle->vert[1];
		int vert2=triangle->vert[2];
		//检查着色模式
		SET_BIT(triangle->state,TRI4DV1_STATE_LIT);

		//检查着色模式
		if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
		{
			//提取三角形颜色的RGB值
			if (windowed)
			{
				_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

				//转化为8.8.8格式
				rBase<<=3;
				gBase<<=2;
				bBase<<=3;
			}
			else
			{
				if (ddpixelformat==DD_PIXEL_FORMAT565)
				{
					_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					//转化为8.8.8格式
					rBase<<=3;
					gBase<<=2;
					bBase<<=3;
				}
				else if (ddpixelformat==DD_PIXEL_FORMAT555)
				{
					_RGB555FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					rBase<<=3;
					gBase<<=3;
					bBase<<=3;
				}
			}

			if (light.state==LIGHTV1_STATE_OFF)
			{
				return FALSE;
			}

			if (light.attr & LIGHTV1_ATTR_INFINITE)
			{
				//无穷远处光源
				//无穷远处光源 I=Idir*Cdir(Idir 原始方向光强度,Cdir 原始方向光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=Idir*Cdir

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert1].v);
				v.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				dp=n.DotProduct(light.tdir);
				dp=-dp;

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=Idir*Cdir

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						I=128*dp/length;
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=Idir*Cdir
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						n.Normalize();
						tempv=n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=n*f-tempdir;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum+=light.Cs.r*rBase*max/256;
						gSum+=light.Cs.g*gBase*max/256;
						bSum+=light.Cs.b*bBase*max/256;
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}

			}
			else if (light.attr & LIGHTV1_ATTR_POINT)
			{
				//点光源
				//点光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert1].v);
				v.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(triangle->vlist[vert0].v,light.tpos);
				float dist=l.GetLength();   //计算衰减d

				dp=n.DotProduct(l);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(length*dist*atten);              //乘以衰减并归一化
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						n.Normalize();
						tempv=n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=n*f-l;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum+=light.Cs.r*rBase*I/(256*128);
						gSum+=light.Cs.g*gBase*I/(256*128);
						bSum+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_SIMPLE_SPOTLIGHT)
			{
				//简单聚光灯光源
				//简单聚光灯光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert1].v);
				v.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(triangle->vlist[vert0].v,light.tpos);
				float dist=l.GetLength();   //计算衰减d

				CVector4 tempDir=light.tdir*(-1);
				dp=n.DotProduct(tempDir);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(length*atten);              //乘以衰减并归一化
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						n.Normalize();
						tempv=n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=n*f-tempDir;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum+=light.Cs.r*rBase*I/(256*128);
						gSum+=light.Cs.g*gBase*I/(256*128);
						bSum+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_COMPLEX_SPOTLIGHT)
			{
				//复杂聚光灯光源
				//复杂聚光灯光源 I=I0point*Clpoint*Max(cos0,0)fp/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert1].v);
				v.CreateVector(triangle->vlist[vert0].v,triangle->vlist[vert2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(light.tpos,triangle->vlist[vert0].v);
				float dist=l.GetLength();   //计算衰减d

				CVector4 tempDir=light.tdir*(-1);
				dp=n.DotProduct(tempDir);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(length*atten);              //乘以衰减并归一化
							rSum+=light.Cd.r*rBase*I/(256*128);
							gSum+=light.Cd.g*gBase*I/(256*128);
							bSum+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							n.Normalize();
							tempv=n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=n*f-tempDir;

							tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum+=light.Cd.r*rBase*I/(256*128);
							gSum+=light.Cd.g*gBase*I/(256*128);
							bSum+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			if (rSum>255)
			{
				rSum=255;
			}
			if (gSum>255)
			{
				gSum=255;
			}
			if (bSum>255)
			{
				bSum=255;
			}
			triangle->litColor[0]=_RGB16BIT565(rSum,gSum,bSum);
		}
		else if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			//Gouraud着色
			//提取三角形颜色的RGB值

			rSum0  = 0;
			gSum0  = 0;
			bSum0  = 0;

			rSum1  = 0;
			gSum1  = 0;
			bSum1  = 0;

			rSum2  = 0;
			gSum2  = 0;
			bSum2  = 0;

			int ri=0,gi=0,bi=0;

			if (windowed)
			{
				_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

				//转化为8.8.8格式
				rBase<<=3;
				gBase<<=2;
				bBase<<=3;
			}
			else
			{
				if (ddpixelformat==DD_PIXEL_FORMAT565)
				{
					_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					//转化为8.8.8格式
					rBase<<=3;
					gBase<<=2;
					bBase<<=3;
				}
				else if (ddpixelformat==DD_PIXEL_FORMAT555)
				{
					_RGB555FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					rBase<<=3;
					gBase<<=3;
					bBase<<=3;
				}
			}

			if (light.state==LIGHTV1_STATE_OFF)
			{
				return FALSE;
			}

			if (light.attr & LIGHTV1_ATTR_INFINITE)
			{
				//无穷远处光源
				//无穷远处光源 I=Idir*Cdir(Idir 原始方向光强度,Cdir 原始方向光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=Idir*Cdir

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=Idir*Cdir

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间

					dp=triangle->vlist[vert0].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.g*gBase*I/(256*128);
						bSum0+=light.Cd.b*bBase*I/(256*128);
					}
					//
					dp=triangle->vlist[vert1].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.g*gBase*I/(256*128);
						bSum1+=light.Cd.b*bBase*I/(256*128);
					}
					//
					dp=triangle->vlist[vert0].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.g*gBase*I/(256*128);
						bSum2+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=Idir*Cdir
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					dp=triangle->vlist[vert0].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->vlist[vert0].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->vlist[vert0].n*f-tempdir;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum0+=light.Cs.r*rBase*max/256;
						gSum0+=light.Cs.g*gBase*max/256;
						bSum0+=light.Cs.b*bBase*max/256;
					}

					dp=triangle->vlist[vert1].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->vlist[vert1].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->vlist[vert1].n*f-tempdir;

						tempu.CreateVector(triangle->vlist[vert1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum1+=light.Cs.r*rBase*max/256;
						gSum1+=light.Cs.g*gBase*max/256;
						bSum1+=light.Cs.b*bBase*max/256;
					}

					dp=triangle->vlist[vert2].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->vlist[vert2].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->vlist[vert2].n*f-tempdir;

						tempu.CreateVector(triangle->vlist[vert2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum2+=light.Cs.r*rBase*max/256;
						gSum2+=light.Cs.g*gBase*max/256;
						bSum2+=light.Cs.b*bBase*max/256;
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;

				}

			}
			else if (light.attr & LIGHTV1_ATTR_POINT)
			{
				//点光源
				//点光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(triangle->vlist[vert0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert0].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.g*gBase*I/(256*128);
						bSum0+=light.Cd.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->vlist[vert1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert1].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.g*gBase*I/(256*128);
						bSum1+=light.Cd.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->vlist[vert2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert2].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.g*gBase*I/(256*128);
						bSum2+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(triangle->vlist[vert0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert0].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert0].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->vlist[vert0].n*f-l;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum0+=light.Cs.r*rBase*I/(256*128);
						gSum0+=light.Cs.g*gBase*I/(256*128);
						bSum0+=light.Cs.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->vlist[vert1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert1].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert1].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->vlist[vert1].n*f-l;

						tempu.CreateVector(triangle->vlist[vert1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum1+=light.Cs.r*rBase*I/(256*128);
						gSum1+=light.Cs.g*gBase*I/(256*128);
						bSum1+=light.Cs.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->vlist[vert2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->vlist[vert2].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert2].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->vlist[vert2].n*f-l;

						tempu.CreateVector(triangle->vlist[vert2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum2+=light.Cs.r*rBase*I/(256*128);
						gSum2+=light.Cs.g*gBase*I/(256*128);
						bSum2+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_SIMPLE_SPOTLIGHT)
			{
				//简单聚光灯光源
				//简单聚光灯光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(triangle->vlist[vert0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->vlist[vert0].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.r*gBase*I/(256*128);
						bSum0+=light.Cd.r*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->vlist[vert1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert1].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.r*gBase*I/(256*128);
						bSum1+=light.Cd.r*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->vlist[vert2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert2].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.r*gBase*I/(256*128);
						bSum2+=light.Cd.r*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(triangle->vlist[vert0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->vlist[vert0].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert0].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->vlist[vert0].n*f-tempDir;

						tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum0+=light.Cs.r*rBase*I/(256*128);
						gSum0+=light.Cs.g*gBase*I/(256*128);
						bSum0+=light.Cs.b*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->vlist[vert1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert1].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert1].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->vlist[vert1].n*f-tempDir;

						tempu.CreateVector(triangle->vlist[vert1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum1+=light.Cs.r*rBase*I/(256*128);
						gSum1+=light.Cs.g*gBase*I/(256*128);
						bSum1+=light.Cs.b*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->vlist[vert2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert2].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->vlist[vert2].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->vlist[vert2].n*f-tempDir;

						tempu.CreateVector(triangle->vlist[vert2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum2+=light.Cs.r*rBase*I/(256*128);
						gSum2+=light.Cs.g*gBase*I/(256*128);
						bSum2+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_COMPLEX_SPOTLIGHT)
			{
				//复杂聚光灯光源
				//复杂聚光灯光源 I=I0point*Clpoint*Max(cos0,0)fp/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(light.tpos,triangle->vlist[vert0].v);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->vlist[vert0].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum0+=light.Cd.r*rBase*I/(256*128);
							gSum0+=light.Cd.g*gBase*I/(256*128);
							bSum0+=light.Cd.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->vlist[vert1].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert1].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum1+=light.Cd.r*rBase*I/(256*128);
							gSum1+=light.Cd.g*gBase*I/(256*128);
							bSum1+=light.Cd.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->vlist[vert2].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert2].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum2+=light.Cd.r*rBase*I/(256*128);
							gSum2+=light.Cd.g*gBase*I/(256*128);
							bSum2+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(light.tpos,triangle->vlist[vert0].v);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->vlist[vert0].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->vlist[vert0].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->vlist[vert0].n*f-tempDir;

							tempu.CreateVector(triangle->vlist[vert0].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum0+=light.Cs.r*rBase*I/(256*128);
							gSum0+=light.Cs.g*gBase*I/(256*128);
							bSum0+=light.Cs.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->vlist[vert1].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert1].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->vlist[vert1].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->vlist[vert1].n*f-tempDir;

							tempu.CreateVector(triangle->vlist[vert1].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum1+=light.Cs.r*rBase*I/(256*128);
							gSum1+=light.Cs.g*gBase*I/(256*128);
							bSum1+=light.Cs.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->vlist[vert2].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->vlist[vert2].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->vlist[vert2].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->vlist[vert2].n*f-tempDir;

							tempu.CreateVector(triangle->vlist[vert2].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum2+=light.Cs.r*rBase*I/(256*128);
							gSum2+=light.Cs.g*gBase*I/(256*128);
							bSum2+=light.Cs.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			if (rSum0>255)
			{
				rSum0=255;
			}
			if (gSum0>255)
			{
				gSum0=255;
			}
			if (bSum0>255)
			{
				bSum0=255;
			}

			if (rSum1>255)
			{
				rSum1=255;
			}
			if (gSum1>255)
			{
				gSum1=255;
			}
			if (bSum1>255)
			{
				bSum1=255;
			}

			if (rSum2>255)
			{
				rSum2=255;
			}
			if (gSum2>255)
			{
				gSum2=255;
			}
			if (bSum2>255)
			{
				bSum2=255;
			}
			triangle->litColor[0]=_RGB16BIT565(rSum0,gSum0,bSum0);
			triangle->litColor[1]=_RGB16BIT565(rSum1,gSum1,bSum1);
			triangle->litColor[2]=_RGB16BIT565(rSum2,gSum2,bSum2);
		}
		else
		{
			//恒定着色
			triangle->litColor[0]=triangle->color;
		}
	}	
	return TRUE;
}

int CRenderSystem::LightRenderList4DV1World16(const CCamera &camera, const CLight &light)
{
	UINT rBase=0,gBase=0,bBase=0;
	UINT rSum=0,gSum=0,bSum=0;
	UINT rSum0=0,gSum0=0,bSum0=0;
	UINT rSum1=0,gSum1=0,bSum1=0;
	UINT rSum2=0,gSum2=0,bSum2=0;
	UINT ShadeColor=0;        //最终着色的颜色
	float dp=0;                //向量点积
	float dist=0;              //光到表面的距离
	float I=0;                 //光的总强度
	float length=0;            //向量的长度
	float atten=0;             //光的衰减

	//处理每个三角形
	for (int tri=0;tri<ptrRenderList->nTriangles;tri++)
	{
		rSum=0;gSum=0;bSum=0;
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];
		if (!(triangle->state & TRI4DV1_STATE_ACTIVE)||
			(triangle->state & TRI4DV1_STATE_BACKFACE)||
			(triangle->state & TRI4DV1_STATE_CLIPPED)||
			(triangle->state & TRI4DV1_STATE_LIT))
		{
			continue;
		}

		SET_BIT(triangle->state,TRI4DV1_STATE_LIT);

		//检查着色模式
		if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
		{
			//提取三角形颜色的RGB值
			if (windowed)
			{
				_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

				//转化为8.8.8格式
				rBase<<=3;
				gBase<<=2;
				bBase<<=3;
			}
			else
			{
				if (ddpixelformat==DD_PIXEL_FORMAT565)
				{
					_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					//转化为8.8.8格式
					rBase<<=3;
					gBase<<=2;
					bBase<<=3;
				}
				else if (ddpixelformat==DD_PIXEL_FORMAT555)
				{
					_RGB555FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					rBase<<=3;
					gBase<<=3;
					bBase<<=3;
				}
			}

			if (light.state==LIGHTV1_STATE_OFF)
			{
				return FALSE;
			}

			if (light.attr & LIGHTV1_ATTR_INFINITE)
			{
				//无穷远处光源
				//无穷远处光源 I=Idir*Cdir(Idir 原始方向光强度,Cdir 原始方向光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=Idir*Cdir

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->tvlist[0].v,triangle->tvlist[1].v);
				v.CreateVector(triangle->tvlist[0].v,triangle->tvlist[2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				dp=n.DotProduct(light.tdir);
				dp=-dp;


				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=Idir*Cdir

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						I=128*dp/length;
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=Idir*Cdir
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						n.Normalize();
						tempv=n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=n*f-tempdir;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum+=light.Cs.r*rBase*max/256;
						gSum+=light.Cs.g*gBase*max/256;
						bSum+=light.Cs.b*bBase*max/256;
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}

			}
			else if (light.attr & LIGHTV1_ATTR_POINT)
			{
				//点光源
				//点光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->tvlist[0].v,triangle->tvlist[1].v);
				v.CreateVector(triangle->tvlist[0].v,triangle->tvlist[2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(triangle->tvlist[0].v,light.tpos);
				float dist=l.GetLength();   //计算衰减d

				dp=n.DotProduct(l);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(length*dist*atten);              //乘以衰减并归一化
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						n.Normalize();
						tempv=n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=n*f-l;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum+=light.Cs.r*rBase*I/(256*128);
						gSum+=light.Cs.g*gBase*I/(256*128);
						bSum+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_SIMPLE_SPOTLIGHT)
			{
				//简单聚光灯光源
				//简单聚光灯光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->tvlist[0].v,triangle->tvlist[1].v);
				v.CreateVector(triangle->tvlist[0].v,triangle->tvlist[2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(triangle->tvlist[0].v,light.tpos);
				float dist=l.GetLength();   //计算衰减d

				CVector4 tempDir=light.tdir*(-1);
				dp=n.DotProduct(tempDir);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(length*atten);              //乘以衰减并归一化
						rSum+=light.Cd.r*rBase*I/(256*128);
						gSum+=light.Cd.g*gBase*I/(256*128);
						bSum+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						n.Normalize();
						tempv=n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=n*f-tempDir;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum+=light.Cs.r*rBase*I/(256*128);
						gSum+=light.Cs.g*gBase*I/(256*128);
						bSum+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_COMPLEX_SPOTLIGHT)
			{
				//复杂聚光灯光源
				//复杂聚光灯光源 I=I0point*Clpoint*Max(cos0,0)fp/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					rSum+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gSum+=((light.Ca.g*gBase)>>8);
					bSum+=((light.Ca.b*bBase)>>8);
				}

				CVector4 u,v,n;
				u.CreateVector(triangle->tvlist[0].v,triangle->tvlist[1].v);
				v.CreateVector(triangle->tvlist[0].v,triangle->tvlist[2].v);
				n=u.CrossProduct(v); //面法线

				//需要对向量进行归一化
				//提前计算出所有三角形的法线长度，进行优化
				length=triangle->nlength;

				CVector4 l;
				l.CreateVector(light.tpos,triangle->tvlist[0].v);
				float dist=l.GetLength();   //计算衰减d

				CVector4 tempDir=light.tdir*(-1);
				dp=n.DotProduct(tempDir);

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(length*atten);              //乘以衰减并归一化
							rSum+=light.Cd.r*rBase*I/(256*128);
							gSum+=light.Cd.g*gBase*I/(256*128);
							bSum+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							n.Normalize();
							tempv=n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=n*f-tempDir;

							tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum+=light.Cd.r*rBase*I/(256*128);
							gSum+=light.Cd.g*gBase*I/(256*128);
							bSum+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					rSum+=rBase/256;
					gSum+=gBase/256;
					bSum+=bBase/256;
				}
			}
			if (rSum>255)
			{
				rSum=255;
			}
			if (gSum>255)
			{
				gSum=255;
			}
			if (bSum>255)
			{
				bSum=255;
			}
			triangle->litColor[0]=_RGB16BIT565(rSum,gSum,bSum);
		}
		else if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
		{
			//Gouraud着色
			//提取三角形颜色的RGB值

			rSum0  = 0;
			gSum0  = 0;
			bSum0  = 0;

			rSum1  = 0;
			gSum1  = 0;
			bSum1  = 0;

			rSum2  = 0;
			gSum2  = 0;
			bSum2  = 0;

			int ri=0,gi=0,bi=0;

			if (windowed)
			{
				_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

				//转化为8.8.8格式
				rBase<<=3;
				gBase<<=2;
				bBase<<=3;
			}
			else
			{
				if (ddpixelformat==DD_PIXEL_FORMAT565)
				{
					_RGB565FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					//转化为8.8.8格式
					rBase<<=3;
					gBase<<=2;
					bBase<<=3;
				}
				else if (ddpixelformat==DD_PIXEL_FORMAT555)
				{
					_RGB555FROM16BIT(triangle->color,&rBase,&gBase,&bBase);

					rBase<<=3;
					gBase<<=3;
					bBase<<=3;
				}
			}

			if (light.state==LIGHTV1_STATE_OFF)
			{
				return FALSE;
			}

			if (light.attr & LIGHTV1_ATTR_INFINITE)
			{
				//无穷远处光源
				//无穷远处光源 I=Idir*Cdir(Idir 原始方向光强度,Cdir 原始方向光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=Idir*Cdir

					ri=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi=((light.Ca.g*gBase)>>8);
					bi=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=Idir*Cdir

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间

					dp=triangle->tvlist[0].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.g*gBase*I/(256*128);
						bSum0+=light.Cd.b*bBase*I/(256*128);
					}
					//
					dp=triangle->tvlist[1].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.g*gBase*I/(256*128);
						bSum1+=light.Cd.b*bBase*I/(256*128);
					}
					//
					dp=triangle->tvlist[0].n.DotProduct(light.tdir);
					dp=-dp;

					if (dp>0)
					{
						I=128*dp;
						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.g*gBase*I/(256*128);
						bSum2+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=Idir*Cdir
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					dp=triangle->tvlist[0].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->tvlist[0].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->tvlist[0].n*f-tempdir;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum0+=light.Cs.r*rBase*max/256;
						gSum0+=light.Cs.g*gBase*max/256;
						bSum0+=light.Cs.b*bBase*max/256;
					}

					dp=triangle->tvlist[1].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->tvlist[1].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->tvlist[1].n*f-tempdir;

						tempu.CreateVector(triangle->tvlist[1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum1+=light.Cs.r*rBase*max/256;
						gSum1+=light.Cs.g*gBase*max/256;
						bSum1+=light.Cs.b*bBase*max/256;
					}

					dp=triangle->tvlist[2].n.DotProduct(light.tdir);
					dp=-dp;
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu,tempdir;
						float f=0;
						tempv=triangle->tvlist[2].n*2;
						tempdir=light.tdir*(-1);
						f=tempv.DotProduct(tempdir);
						Rdir=triangle->tvlist[2].n*f-tempdir;

						tempu.CreateVector(triangle->tvlist[2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						rSum2+=light.Cs.r*rBase*max/256;
						gSum2+=light.Cs.g*gBase*max/256;
						bSum2+=light.Cs.b*bBase*max/256;
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;

				}

			}
			else if (light.attr & LIGHTV1_ATTR_POINT)
			{
				//点光源
				//点光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(triangle->tvlist[0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[0].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.g*gBase*I/(256*128);
						bSum0+=light.Cd.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->tvlist[1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[1].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.g*gBase*I/(256*128);
						bSum1+=light.Cd.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->tvlist[2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[2].n.DotProduct(l);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(dist*atten);              //乘以衰减并归一化

						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.g*gBase*I/(256*128);
						bSum2+=light.Cd.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(triangle->tvlist[0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[0].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[0].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->tvlist[0].n*f-l;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum0+=light.Cs.r*rBase*I/(256*128);
						gSum0+=light.Cs.g*gBase*I/(256*128);
						bSum0+=light.Cs.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->tvlist[1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[1].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[1].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->tvlist[1].n*f-l;

						tempu.CreateVector(triangle->tvlist[1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum1+=light.Cs.r*rBase*I/(256*128);
						gSum1+=light.Cs.g*gBase*I/(256*128);
						bSum1+=light.Cs.b*bBase*I/(256*128);
					}

					l.CreateVector(triangle->tvlist[2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d

					dp=triangle->tvlist[2].n.DotProduct(l);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[2].n*2;
						l.Normalize();
						f=tempv.DotProduct(l);
						Rdir=triangle->tvlist[2].n*f-l;

						tempu.CreateVector(triangle->tvlist[2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum2+=light.Cs.r*rBase*I/(256*128);
						gSum2+=light.Cs.g*gBase*I/(256*128);
						bSum2+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_SIMPLE_SPOTLIGHT)
			{
				//简单聚光灯光源
				//简单聚光灯光源 I=I0point*Clpoint/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(triangle->tvlist[0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d

					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->tvlist[0].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum0+=light.Cd.r*rBase*I/(256*128);
						gSum0+=light.Cd.r*gBase*I/(256*128);
						bSum0+=light.Cd.r*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->tvlist[1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[1].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum1+=light.Cd.r*rBase*I/(256*128);
						gSum1+=light.Cd.r*gBase*I/(256*128);
						bSum1+=light.Cd.r*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->tvlist[2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[2].n.DotProduct(tempDir);
					if (dp>0)
					{
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*dp/(atten);              //乘以衰减并归一化
						rSum2+=light.Cd.r*rBase*I/(256*128);
						gSum2+=light.Cd.r*gBase*I/(256*128);
						bSum2+=light.Cd.r*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(triangle->tvlist[0].v,light.tpos);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->tvlist[0].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[0].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->tvlist[0].n*f-tempDir;

						tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum0+=light.Cs.r*rBase*I/(256*128);
						gSum0+=light.Cs.g*gBase*I/(256*128);
						bSum0+=light.Cs.b*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->tvlist[1].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[1].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[1].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->tvlist[1].n*f-tempDir;

						tempu.CreateVector(triangle->tvlist[1].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum1+=light.Cs.r*rBase*I/(256*128);
						gSum1+=light.Cs.g*gBase*I/(256*128);
						bSum1+=light.Cs.b*bBase*I/(256*128);
					}
					//
					l.CreateVector(triangle->tvlist[2].v,light.tpos);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[2].n.DotProduct(tempDir);
					if (dp>0)
					{
						CVector4 Rdir,tempv,tempu;
						float f=0;
						tempv=triangle->tvlist[2].n*2;
						f=tempv.DotProduct(tempDir);
						Rdir=triangle->tvlist[2].n*f-tempDir;

						tempu.CreateVector(triangle->tvlist[2].v,camera.pos);
						tempu.Normalize();
						float max=max(Rdir.DotProduct(tempu),0);
						for (int i=0;i<light.sp;i++)
						{
							max*=max;
						}
						atten=light.kc+light.kl*dist+light.kq*dist*dist;
						I=128*max/atten;
						rSum2+=light.Cs.r*rBase*I/(256*128);
						gSum2+=light.Cs.g*gBase*I/(256*128);
						bSum2+=light.Cs.b*bBase*I/(256*128);
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			else if (light.attr & LIGHTV1_ATTR_COMPLEX_SPOTLIGHT)
			{
				//复杂聚光灯光源
				//复杂聚光灯光源 I=I0point*Clpoint*Max(cos0,0)fp/(kc+kl*d+kq*d*d) 
				//(I0point 点光源光强度,Clpoint 点光源光颜色)

				if (light.attr & LIGHTV1_ATTR_AMBIENT)
				{
					//环境光  I_tatal=Cs*I_ambient
					//I_ambient=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					ri+=((light.Ca.r*rBase)>>8);  //避免颜色溢出
					gi+=((light.Ca.g*gBase)>>8);
					bi+=((light.Ca.b*bBase)>>8);

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}

				if (light.attr & LIGHTV1_ATTR_DIFFUSE)
				{
					//漫反射 I_tatal=Cs*I_diffuse*(n*l)
					//I_diffuse=I0point*Clpoint/(kc+kl*d+kq*d*d) 

					//当dp>0时才考虑该光源的影响
					//乘以128是为了避免浮点数转换为整数
					//那会浪费多个CPU时间
					CVector4 l;
					l.CreateVector(light.tpos,triangle->tvlist[0].v);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->tvlist[0].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum0+=light.Cd.r*rBase*I/(256*128);
							gSum0+=light.Cd.g*gBase*I/(256*128);
							bSum0+=light.Cd.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->tvlist[1].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[1].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum1+=light.Cd.r*rBase*I/(256*128);
							gSum1+=light.Cd.g*gBase*I/(256*128);
							bSum1+=light.Cd.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->tvlist[2].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[2].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}
							I=128*dp*dpsl/(atten);              //乘以衰减并归一化
							rSum2+=light.Cd.r*rBase*I/(256*128);
							gSum2+=light.Cd.g*gBase*I/(256*128);
							bSum2+=light.Cd.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_SPECULAR)
				{
					//镜面反射 I_tatal=Cs*I_specular*MAX(R*V,0)sp(sp为指数)*[(n*l)>0?1:0]
					//I_specular=I0point*Clpoint/(kc+kl*d+kq*d*d) 
					//R=(2N*L)*N-L(L为照射点到入射点的向量);    反射向量
					CVector4 l;
					l.CreateVector(light.tpos,triangle->tvlist[0].v);
					float dist=l.GetLength();   //计算衰减d
					CVector4 tempDir=light.tdir*(-1);
					dp=triangle->tvlist[0].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->tvlist[0].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->tvlist[0].n*f-tempDir;

							tempu.CreateVector(triangle->tvlist[0].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum0+=light.Cs.r*rBase*I/(256*128);
							gSum0+=light.Cs.g*gBase*I/(256*128);
							bSum0+=light.Cs.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->tvlist[1].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[1].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->tvlist[1].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->tvlist[1].n*f-tempDir;

							tempu.CreateVector(triangle->tvlist[1].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum1+=light.Cs.r*rBase*I/(256*128);
							gSum1+=light.Cs.g*gBase*I/(256*128);
							bSum1+=light.Cs.b*bBase*I/(256*128);
						}
					}

					//
					l.CreateVector(light.tpos,triangle->tvlist[2].v);
					dist=l.GetLength();   //计算衰减d
					dp=triangle->tvlist[2].n.DotProduct(tempDir);
					if (dp>0)
					{
						l.Normalize();
						float dpsl=l.DotProduct(light.tdir);
						if (dpsl>0)
						{
							CVector4 Rdir,tempv,tempu;
							float f=0;
							tempv=triangle->tvlist[2].n*2;
							f=tempv.DotProduct(tempDir);
							Rdir=triangle->tvlist[2].n*f-tempDir;

							tempu.CreateVector(triangle->tvlist[2].v,camera.pos);
							tempu.Normalize();
							float max=max(Rdir.DotProduct(tempu),0);
							for (int i=0;i<light.sp;i++)
							{
								max*=max;
							}

							atten=light.kc+light.kl*dist+light.kq*dist*dist;
							for (int i=0;i<light.pf;i++)
							{
								dpsl*=dpsl;
							}

							I=128*max*dpsl/(atten);       //乘以衰减并归一化
							rSum2+=light.Cs.r*rBase*I/(256*128);
							gSum2+=light.Cs.g*gBase*I/(256*128);
							bSum2+=light.Cs.b*bBase*I/(256*128);
						}
					}
				}
				if (light.attr & LIGHTV1_ATTR_EMISSION)
				{
					ri+=rBase/256;
					gi+=gBase/256;
					bi+=bBase/256;

					rSum0+=ri;
					gSum0+=gi;
					bSum0+=bi;

					rSum1+=ri;
					gSum1+=gi;
					bSum1+=bi;

					rSum2+=ri;
					gSum2+=gi;
					bSum2+=bi;
				}
			}
			if (rSum0>255)
			{
				rSum0=255;
			}
			if (gSum0>255)
			{
				gSum0=255;
			}
			if (bSum0>255)
			{
				bSum0=255;
			}

			if (rSum1>255)
			{
				rSum1=255;
			}
			if (gSum1>255)
			{
				gSum1=255;
			}
			if (bSum1>255)
			{
				bSum1=255;
			}

			if (rSum2>255)
			{
				rSum2=255;
			}
			if (gSum2>255)
			{
				gSum2=255;
			}
			if (bSum2>255)
			{
				bSum2=255;
			}
			triangle->litColor[0]=_RGB16BIT565(rSum0,gSum0,bSum0);
			triangle->litColor[1]=_RGB16BIT565(rSum1,gSum1,bSum1);
			triangle->litColor[2]=_RGB16BIT565(rSum2,gSum2,bSum2);
		}
		else
		{
			//恒定着色
			triangle->litColor[0]=triangle->color;
		}
	}	
	return TRUE;
}

int compareAVGZ( const void *arg1, const void *arg2 )
{
	LPTriF4DV1 tri1=*((LPTriF4DV1*)arg1);
	LPTriF4DV1 tri2=*((LPTriF4DV1*)arg2);
	float z1,z2;
	z1=0.333333*(tri1->tvlist[0].z+tri1->tvlist[1].z+tri1->tvlist[2].z);
	z2=0.333333*(tri2->tvlist[0].z+tri2->tvlist[1].z+tri2->tvlist[2].z);
	if (z1>z2)
	{
		return -1;
	}
	else if (z1<z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int compareNEARZ( const void *arg1, const void *arg2 )
{
	LPTriF4DV1 tri1=*((LPTriF4DV1*)arg1);
	LPTriF4DV1 tri2=*((LPTriF4DV1*)arg2);
	float z1,z2;
	z1=min(tri1->tvlist[0].z,tri1->tvlist[1].z);
	z1=min(z1,tri1->tvlist[2].z);
	z2=min(tri2->tvlist[0].z,tri2->tvlist[1].z);
	z2=min(z1,tri2->tvlist[2].z);
	if (z1>z2)
	{
		return -1;
	}
	else if (z1<z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int compareFARZ( const void *arg1, const void *arg2 )
{
	LPTriF4DV1 tri1=*((LPTriF4DV1*)arg1);
	LPTriF4DV1 tri2=*((LPTriF4DV1*)arg2);
	float z1,z2;
	z1=max(tri1->tvlist[0].z,tri1->tvlist[1].z);
	z1=max(z1,tri1->tvlist[2].z);
	z2=min(tri2->tvlist[0].z,tri2->tvlist[1].z);
	z2=min(z1,tri2->tvlist[2].z);
	if (z1>z2)
	{
		return -1;
	}
	else if (z1<z2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void CRenderSystem::SortRenderList4DV1(int sortType)
{
	switch (sortType)
	{
	case SORT_RENDERLIST_NEARZ:
		{
			qsort(ptrRenderList->triPtrs,ptrRenderList->nTriangles,sizeof(LPTriF4DV1),
				compareNEARZ);
		}break;
	case SORT_RENDERLIST_FARZ:
		{
			qsort(ptrRenderList->triPtrs,ptrRenderList->nTriangles,sizeof(LPTriF4DV1),
				compareFARZ);
		}break;
	case SORT_RENDERLIST_AVGZ:
		{
			qsort(ptrRenderList->triPtrs,ptrRenderList->nTriangles,sizeof(LPTriF4DV1),
				compareAVGZ);
		}break;
	default:
		break;
	}
}
void CRenderSystem::ClipTriangleRenderList4DV1(const CCamera& cam,int clipFlags)
{
	int VertexCodes[3];//用来存储顶点的裁剪标志
	int numVerts;  //多少个顶点在近裁剪面内
	int v0,v1,v2;
	float zFactor,zTest;
	float xi,yi,x01i,y01i,x02i,y02i,t1,t2;
	float ui,vi,u01i,v01i,u02i,v02i;
	int lastTriindex,insertindex;
	CVector4 u,v,n;
	TriF4DV1 tempTri;

	insertindex=lastTriindex=ptrRenderList->nTriangles;
	for (int tri=0;tri<lastTriindex;tri++)
	{
		LPTriF4DV1 triangle=ptrRenderList->triPtrs[tri];
		if ((triangle==NULL)||!(triangle->state & TRI4DV1_STATE_ACTIVE)
			||(triangle->state & TRI4DV1_STATE_CLIPPED)
			||(triangle->state & TRI4DV1_STATE_BACKFACE))
		{
			continue;
		}
		if (clipFlags & CLIP_X_PLANE)
		{
			zFactor=(0.5)*cam.viewPlaneWidth/cam.viewDist;
			//顶点0
			zTest=zFactor*triangle->tvlist[0].z;
			if (triangle->tvlist[0].x>zTest)
			{
				VertexCodes[0]=CLIP_CODE_GX;
			}
			else if (triangle->tvlist[0].x<-zTest)
			{
				VertexCodes[0]=CLIP_CODE_LX;
			}
			else
			{
				VertexCodes[0]=CLIP_CODE_IX;
			}
			//顶点1
			zTest=zFactor*triangle->tvlist[1].z;
			if (triangle->tvlist[1].x>zTest)
			{
				VertexCodes[1]=CLIP_CODE_GX;
			}
			else if (triangle->tvlist[1].x<-zTest)
			{
				VertexCodes[1]=CLIP_CODE_LX;
			}
			else
			{
				VertexCodes[1]=CLIP_CODE_IX;
			}
			//顶点2
			zTest=zFactor*triangle->tvlist[2].z;
			if (triangle->tvlist[2].x>zTest)
			{
				VertexCodes[2]=CLIP_CODE_GX;
			}
			else if (triangle->tvlist[2].x<-zTest)
			{
				VertexCodes[2]=CLIP_CODE_LX;
			}
			else
			{
				VertexCodes[2]=CLIP_CODE_IX;
			}
			if (((VertexCodes[0]==CLIP_CODE_GX)&&
				(VertexCodes[1]==CLIP_CODE_GX)&&
				(VertexCodes[2]==CLIP_CODE_GX))||
				((VertexCodes[0]==CLIP_CODE_LX)&&
				(VertexCodes[1]==CLIP_CODE_LX)&&
				(VertexCodes[2]==CLIP_CODE_LX)))
			{
				SET_BIT(triangle->state,TRI4DV1_STATE_CLIPPED);
				continue;
			}
		}
		//Y
		if (clipFlags & CLIP_Y_PLANE)
		{
			zFactor=(0.5)*cam.viewPlaneHeight/cam.viewDist;
			//顶点0
			zTest=zFactor*triangle->tvlist[0].z;
			if (triangle->tvlist[0].y>zTest)
			{
				VertexCodes[0]=CLIP_CODE_GY;
			}
			else if (triangle->tvlist[0].y<-zTest)
			{
				VertexCodes[0]=CLIP_CODE_LY;
			}
			else
			{
				VertexCodes[0]=CLIP_CODE_IY;
			}
			//顶点1
			zTest=zFactor*triangle->tvlist[1].z;
			if (triangle->tvlist[1].y>zTest)
			{
				VertexCodes[1]=CLIP_CODE_GY;
			}
			else if (triangle->tvlist[1].y<-zTest)
			{
				VertexCodes[1]=CLIP_CODE_LY;
			}
			else
			{
				VertexCodes[1]=CLIP_CODE_IY;
			}
			//顶点2
			zTest=zFactor*triangle->tvlist[2].z;
			if (triangle->tvlist[2].y>zTest)
			{
				VertexCodes[2]=CLIP_CODE_GY;
			}
			else if (triangle->tvlist[2].y<-zTest)
			{
				VertexCodes[2]=CLIP_CODE_LY;
			}
			else
			{
				VertexCodes[2]=CLIP_CODE_IY;
			}
			if (((VertexCodes[0]==CLIP_CODE_GY)&&
				(VertexCodes[1]==CLIP_CODE_GY)&&
				(VertexCodes[2]==CLIP_CODE_GY))||
				((VertexCodes[0]==CLIP_CODE_LY)&&
				(VertexCodes[1]==CLIP_CODE_LY)&&
				(VertexCodes[2]==CLIP_CODE_LY)))
			{
				SET_BIT(triangle->state,TRI4DV1_STATE_CLIPPED);
				continue;
			}
		}
		if (clipFlags & CLIP_Z_PLANE)
		{
			numVerts=0;
			//顶点0
			if (triangle->tvlist[0].z>cam.farZ)
			{
				VertexCodes[0]=CLIP_CODE_GZ;
			}
			else if (triangle->tvlist[0].z<cam.nearZ)
			{
				VertexCodes[0]=CLIP_CODE_LZ;
			}
			else
			{
				VertexCodes[0]=CLIP_CODE_IZ;
				numVerts++;
			}
			//顶点1
			if (triangle->tvlist[1].z>cam.farZ)
			{
				VertexCodes[1]=CLIP_CODE_GZ;
			}
			else if (triangle->tvlist[1].z<cam.nearZ)
			{
				VertexCodes[1]=CLIP_CODE_LZ;
			}
			else
			{
				VertexCodes[1]=CLIP_CODE_IZ;
				numVerts++;
			}
			//顶点2
			if (triangle->tvlist[2].z>cam.farZ)
			{
				VertexCodes[2]=CLIP_CODE_GZ;
			}
			else if (triangle->tvlist[2].z<cam.nearZ)
			{
				VertexCodes[2]=CLIP_CODE_LZ;
			}
			else
			{
				VertexCodes[2]=CLIP_CODE_IZ;
				numVerts++;
			}
			if (((VertexCodes[0]==CLIP_CODE_GZ)&&
				(VertexCodes[1]==CLIP_CODE_GZ)&&
				(VertexCodes[2]==CLIP_CODE_GZ))||
				((VertexCodes[0]==CLIP_CODE_LZ)&&
				(VertexCodes[1]==CLIP_CODE_LZ)&&
				(VertexCodes[2]==CLIP_CODE_LZ)))
			{
				SET_BIT(triangle->state,TRI4DV1_STATE_CLIPPED);
				continue;
			}
			if ((VertexCodes[0]|VertexCodes[1]|VertexCodes[2])& CLIP_CODE_LZ)
			{
				if (numVerts==1)
				{
					if (VertexCodes[0]==CLIP_CODE_IZ)
					{
						v0=0;v1=1;v2=2;
					}
					else if (VertexCodes[1]==CLIP_CODE_IZ)
					{
						v0=1;v1=2;v2=0;
					}
					else 
					{
						v0=2;v1=0;v2=1;
					}
					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v1].v);
					t1=(cam.nearZ-triangle->tvlist[v0].z)/v.z;
					xi=triangle->tvlist[v0].x+v.x*t1;
					yi=triangle->tvlist[v0].y+v.y*t1;

					triangle->tvlist[v1].x=xi;
					triangle->tvlist[v1].y=yi;
					triangle->tvlist[v1].z=cam.nearZ;

					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v2].v);
					t2=(cam.nearZ-triangle->tvlist[v0].z)/v.z;
					xi=triangle->tvlist[v0].x+v.x*t2;
					yi=triangle->tvlist[v0].y+v.y*t2;

					triangle->tvlist[v2].x=xi;
					triangle->tvlist[v2].y=yi;
					triangle->tvlist[v2].z=cam.nearZ;

					if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
					{
						ui=triangle->tvlist[v0].u0+(triangle->tvlist[v1].u0-triangle->tvlist[v0].u0)*t1;
						vi=triangle->tvlist[v0].v0+(triangle->tvlist[v1].v0-triangle->tvlist[v0].v0)*t1;
						triangle->tvlist[v1].u0=ui;
						triangle->tvlist[v1].v0=vi;
						ui=triangle->tvlist[v0].u0+(triangle->tvlist[v2].u0-triangle->tvlist[v0].u0)*t2;
						vi=triangle->tvlist[v0].v0+(triangle->tvlist[v2].v0-triangle->tvlist[v0].v0)*t2;
						triangle->tvlist[v2].u0=ui;
						triangle->tvlist[v2].v0=vi;
					}
					u.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v1].v);
					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v2].v);
					n=u.CrossProduct(v);
					triangle->nlength=n.GetLength();
				}
				else if (numVerts==2)
				{
					memcpy(&tempTri,triangle,sizeof(TriF4DV1));
					if (VertexCodes[0]==CLIP_CODE_LZ)
					{
						v0=0;v1=1;v2=2;
					}
					else if (VertexCodes[1]==CLIP_CODE_LZ)
					{
						v0=1;v1=2;v2=0;
					}
					else 
					{
						v0=2;v1=0;v2=1;
					}
					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v1].v);
					t1=(cam.nearZ-triangle->tvlist[v0].z)/v.z;
					x01i=triangle->tvlist[v0].x+v.x*t1;
					y01i=triangle->tvlist[v0].y+v.y*t1;
					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v2].v);
					t2=(cam.nearZ-triangle->tvlist[v0].z)/v.z;
					x02i=triangle->tvlist[v0].x+v.x*t2;
					y02i=triangle->tvlist[v0].y+v.y*t2;

					triangle->tvlist[v0].x=x01i;
					triangle->tvlist[v0].y=y01i;
					triangle->tvlist[v0].z=cam.nearZ;

					tempTri.tvlist[v1].x=x01i;
					tempTri.tvlist[v1].y=y01i;
					tempTri.tvlist[v1].z=cam.nearZ;

					tempTri.tvlist[v0].x=x02i;
					tempTri.tvlist[v0].y=y02i;
					tempTri.tvlist[v0].z=cam.nearZ;

					if (triangle->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
					{
						u01i=triangle->tvlist[v0].u0+(triangle->tvlist[v1].u0-triangle->tvlist[v0].u0)*t1;
						v01i=triangle->tvlist[v0].v0+(triangle->tvlist[v1].v0-triangle->tvlist[v0].v0)*t1;
						u02i=triangle->tvlist[v0].u0+(triangle->tvlist[v2].u0-triangle->tvlist[v0].u0)*t2;
						v02i=triangle->tvlist[v0].v0+(triangle->tvlist[v2].v0-triangle->tvlist[v0].v0)*t2;
						triangle->tvlist[v0].u0=u01i;
						triangle->tvlist[v0].v0=v01i;
						tempTri.tvlist[v0].u0=u02i;
						tempTri.tvlist[v0].v0=v02i;
						tempTri.tvlist[v1].u0=u01i;
						tempTri.tvlist[v1].v0=v01i;
					}
					u.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v1].v);
					v.CreateVector(triangle->tvlist[v0].v,triangle->tvlist[v2].v);
					n=u.CrossProduct(v);
					triangle->nlength=n.GetLength();

					u.CreateVector(tempTri.tvlist[v0].v,tempTri.tvlist[v1].v);
					v.CreateVector(tempTri.tvlist[v0].v,tempTri.tvlist[v2].v);
					n=u.CrossProduct(v);
					tempTri.nlength=n.GetLength();
					InsertTriF4DV1ToRenderList4DV1(&tempTri);
				}
			}
		}
	}

}

void CRenderSystem::TransLight4DV1(CLight& light,const CMatrix4x4& mt,int type)
{
	CMatrix4x4 mr=mt;
	mr.M[3][0]=0;mr.M[3][1]=0;mr.M[3][2]=0;
	switch (type)
	{
	case TRANSFORM_LOCAL_ONLY:
		{
			light.pos=light.pos*mr;
			light.dir=light.dir*mr;
		}break;
	case TRANSFORM_TRANS_ONLY:
		{
			light.tpos=light.tpos*mr;
			light.tdir=light.tdir*mr;
		}break;
	case TRANSFORM_LOCAL_TO_TRANS:
		{
			light.tpos=light.pos*mr;
			light.tdir=light.dir*mr;
		}break;
	default:
		break;
	}
}

void CRenderSystem::DrawRenderListRENDERCONTEXT(LPRENDERCONTEXT rc)
{
	CTriangle2D triangle;
	TriF4DV1 face;
	int alpha;

	if (rc->attr & RENDER_ATTR_ZBUFFER)
	{
		for (int tri=0;tri<rc->ptrRenderList->nTriangles;tri++)
		{
			if (!(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}
			if (rc->alphaOverride>=0)
			{
				alpha=rc->alphaOverride;
			}
			else
			{
				alpha=(rc->ptrRenderList->triPtrs[tri]->color && 0xff000000)>>24;
			}
			if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_MIPMAP)
				{
					if (rc->attr & RENDER_ATTR_MIPMAP)
					{
						//计算mip纹理链等级数
						int tmipLevels=
							gLog2[((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]->width];
						//根据最大mip距离和多边形z坐标确定使用哪个mip等级
						//这里使用多边形一个顶点的z坐标
						int miplevel=(tmipLevels*rc->ptrRenderList->triPtrs[tri]->tvlist[0].z/rc->mipDist);
						if (miplevel>tmipLevels)
						{
							miplevel=tmipLevels;
						}
						face.ptrTexture=((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[miplevel];
						for (int ts=0;ts<miplevel;ts++)
						{
							face.tvlist[0].u0*=0.5;
							face.tvlist[0].v0*=0.5;

							face.tvlist[1].u0*=0.5;
							face.tvlist[1].v0*=0.5;

							face.tvlist[2].u0*=0.5;
							face.tvlist[2].v0*=0.5;
						}
					}
					else
					{
						//用户没有请求使用mipmapping，因此使用0级别纹理
						face.ptrTexture=(((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]);
					}
				}
				else
				{
					face.ptrTexture=rc->ptrRenderList->triPtrs[tri]->ptrTexture;
				}
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							if (rc->attr & RENDER_ATTR_BILERP)
							{
								triangle.DrawTextureTriangleBilerpZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
							else
							{
								triangle.DrawTextureTriangleZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else  //Gauroud 着色
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
					face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
			}
			else if ((rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)||
				(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT))
			{
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawTriangleZBAlpha162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawTriangleZB162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
			else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
				face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawGouraudTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
		}
	}
	else if (rc->attr & RENDER_ATTR_INVZBUFFER)
	{
		for (int tri=0;tri<rc->ptrRenderList->nTriangles;tri++)
		{
			if (!(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}
			if (rc->alphaOverride>=0)
			{
				alpha=rc->alphaOverride;
			}
			else
			{
				alpha=(rc->ptrRenderList->triPtrs[tri]->color && 0xff000000)>>24;
			}
			if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_MIPMAP)
				{
					if (rc->attr & RENDER_ATTR_MIPMAP)
					{
						//计算mip纹理链等级数
						int tmipLevels=
							gLog2[((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]->width];
						//根据最大mip距离和多边形z坐标确定使用哪个mip等级
						//这里使用多边形一个顶点的z坐标
						int miplevel=(tmipLevels*rc->ptrRenderList->triPtrs[tri]->tvlist[0].z/rc->mipDist);
						if (miplevel>tmipLevels)
						{
							miplevel=tmipLevels;
						}
						face.ptrTexture=((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[miplevel];
						for (int ts=0;ts<miplevel;ts++)
						{
							face.tvlist[0].u0*=0.5;
							face.tvlist[0].v0*=0.5;

							face.tvlist[1].u0*=0.5;
							face.tvlist[1].v0*=0.5;

							face.tvlist[2].u0*=0.5;
							face.tvlist[2].v0*=0.5;
						}
					}
					else
					{
						//用户没有请求使用mipmapping，因此使用0级别纹理
						face.ptrTexture=(((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]);
					}
				}
				else
				{
					face.ptrTexture=rc->ptrRenderList->triPtrs[tri]->ptrTexture;
				}
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTrianglePerINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTrianglePerLPINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							if (rc->attr & RENDER_ATTR_BILERP)
							{
								triangle.DrawTextureTriangleBilerpINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
							else
							{
								triangle.DrawTextureTriangleINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTrianglePerINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTrianglePerLPINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTrianglePerINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTrianglePerLPINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTrianglePerINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							triangle.DrawTextureTrianglePerLPINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else  //Gauroud 着色ww
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
					face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							triangle.DrawTextureTriangleINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
			}
			else if ((rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)||
				(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT))
			{
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawTriangleINVZBAlpha162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawTriangleINVZB162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
			else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
				face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawGouraudTriangleINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
		}
	}
	else if (rc->attr & RENDER_ATTR_WRITETHRUZBUFFER)
	{
		for (int tri=0;tri<rc->ptrRenderList->nTriangles;tri++)
		{
			if (!(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}
			if (rc->alphaOverride>=0)
			{
				alpha=rc->alphaOverride;
			}
			else
			{
				alpha=(rc->ptrRenderList->triPtrs[tri]->color && 0xff000000)>>24;
			}
			if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_MIPMAP)
				{
					if (rc->attr & RENDER_ATTR_MIPMAP)
					{
						//计算mip纹理链等级数
						int tmipLevels=
							gLog2[((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]->width];
						//根据最大mip距离和多边形z坐标确定使用哪个mip等级
						//这里使用多边形一个顶点的z坐标
						int miplevel=(tmipLevels*rc->ptrRenderList->triPtrs[tri]->tvlist[0].z/rc->mipDist);
						if (miplevel>tmipLevels)
						{
							miplevel=tmipLevels;
						}
						face.ptrTexture=((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[miplevel];
						for (int ts=0;ts<miplevel;ts++)
						{
							face.tvlist[0].u0*=0.5;
							face.tvlist[0].v0*=0.5;

							face.tvlist[1].u0*=0.5;
							face.tvlist[1].v0*=0.5;

							face.tvlist[2].u0*=0.5;
							face.tvlist[2].v0*=0.5;
						}
					}
					else
					{
						//用户没有请求使用mipmapping，因此使用0级别纹理
						face.ptrTexture=(((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]);
					}
				}
				else
				{
					face.ptrTexture=rc->ptrRenderList->triPtrs[tri]->ptrTexture;
				}
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							if (rc->attr & RENDER_ATTR_BILERP)
							{
								triangle.DrawTextureTriangleBilerpZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
							else
							{
								triangle.DrawTextureTriangleWTZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleWTZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleWTZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleWTZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleWTZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleWTZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else  //Gauroud 着色
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
					face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleWTZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持
							triangle.DrawTextureTriangleWTZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTriangleWTZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
			}
			else if ((rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)||
				(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT))
			{
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawTriangleZBAlpha162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawTriangleWTZB162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
			else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
				face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawGouraudTriangleZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleWTZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
		}
	}
	else if (rc->attr & RENDER_ATTR_WRITETHRUINVZBUFFER)
	{
		for (int tri=0;tri<rc->ptrRenderList->nTriangles;tri++)
		{
			if (!(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_ACTIVE)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_CLIPPED)||
				(rc->ptrRenderList->triPtrs[tri]->state & TRI4DV1_STATE_BACKFACE))
			{
				continue;
			}
			if (rc->alphaOverride>=0)
			{
				alpha=rc->alphaOverride;
			}
			else
			{
				alpha=(rc->ptrRenderList->triPtrs[tri]->color && 0xff000000)>>24;
			}
			if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_TEXTURE)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_MIPMAP)
				{
					if (rc->attr & RENDER_ATTR_MIPMAP)
					{
						//计算mip纹理链等级数
						int tmipLevels=
							gLog2[((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]->width];
						//根据最大mip距离和多边形z坐标确定使用哪个mip等级
						//这里使用多边形一个顶点的z坐标
						int miplevel=(tmipLevels*rc->ptrRenderList->triPtrs[tri]->tvlist[0].z/rc->mipDist);
						if (miplevel>tmipLevels)
						{
							miplevel=tmipLevels;
						}
						face.ptrTexture=((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[miplevel];
						for (int ts=0;ts<miplevel;ts++)
						{
							face.tvlist[0].u0*=0.5;
							face.tvlist[0].v0*=0.5;

							face.tvlist[1].u0*=0.5;
							face.tvlist[1].v0*=0.5;

							face.tvlist[2].u0*=0.5;
							face.tvlist[2].v0*=0.5;
						}
					}
					else
					{
						//用户没有请求使用mipmapping，因此使用0级别纹理
						face.ptrTexture=(((CBitmapImage**)(rc->ptrRenderList->triPtrs[tri]->ptrTexture))[0]);
					}
				}
				else
				{
					face.ptrTexture=rc->ptrRenderList->triPtrs[tri]->ptrTexture;
				}
				if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT)
				{
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							//不支持z缓存的透视修正
							triangle.DrawTextureTrianglePerINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTrianglePerLPINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							if (rc->attr & RENDER_ATTR_BILERP)
							{
								triangle.DrawTextureTriangleBilerpINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
							else
							{
								triangle.DrawTextureTriangleWTINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
							}
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleWTPerINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							//不支持
							triangle.DrawTextureTrianglePerLPINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTrianglePerINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTrianglePerLPINVZBAlphaFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleWTINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleWTPerINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							triangle.DrawTextureTrianglePerLPINVZBFS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
				else  //Gauroud 着色ww
				{
					face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
					face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
					face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
					if ((rc->attr & RENDER_ATTR_ALPHA)&&(rc->ptrRenderList->triPtrs[tri]->attr&TRI4DV1_ATTR_TRANSPARENT)||
						rc->alphaOverride>=0)
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)      
						{
							triangle.DrawTextureTriangleINVZBAlphaGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
						}
					}
					else
					{
						if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_AFFINE)
						{
							triangle.DrawTextureTriangleWTINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_CORRECT)
						{
							triangle.DrawTextureTriangleWTINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
						else if (rc->attr & RENDER_ATTR_TEXTURE_PERSPECTIVE_LINEAR)
						{
							triangle.DrawTextureTriangleWTINVZBGS16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
						}
					}
				}
			}
			else if ((rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_FLAT)||
				(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_CONSTANT))
			{
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawTriangleINVZBAlpha162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawTriangleWTINVZB162D(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
			else if (rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_SHADE_MODE_GOURAUD)
			{
				face.tvlist[0]=rc->ptrRenderList->triPtrs[tri]->tvlist[0];
				face.tvlist[1]=rc->ptrRenderList->triPtrs[tri]->tvlist[1];
				face.tvlist[2]=rc->ptrRenderList->triPtrs[tri]->tvlist[2];
				face.litColor[0]=rc->ptrRenderList->triPtrs[tri]->litColor[0];
				face.litColor[1]=rc->ptrRenderList->triPtrs[tri]->litColor[1];
				face.litColor[2]=rc->ptrRenderList->triPtrs[tri]->litColor[2];
				if ((rc->attr & RENDER_ATTR_ALPHA)&&
					(rc->ptrRenderList->triPtrs[tri]->attr & TRI4DV1_ATTR_TRANSPARENT)||rc->alphaOverride>=0)
				{
					triangle.DrawGouraudTriangleINVZBAlpha16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch,alpha);
				}
				else
				{
					triangle.DrawGouraudTriangleWTINVZB16(&face,rc->videoBuffer,rc->lpitch,rc->zbuffer,rc->zpitch);
				}
			}
		}
	}
}

void CRenderSystem::TraversalBSPInsertRENDERLIST(LPBSPNODE root, CCamera &camera, int insertLocal)
{
	//该函数根据参数camera中指定的视点，按从后到前的顺序递归遍历BSP
	//BSP树中多边形的坐标必须是世界坐标
	//该函数检测视点位于当前墙面的前面还是后面，然后执行相应的处理
	//并将每个多边形插入渲染列表

	static CVector4 v;
	static float dot_wall;

	if (root==NULL)
	{
		return;
	}
	//检测视点位于当前墙面的前面还是后面
	v.CreateVector(root->wall.vlist[0].v,camera.pos);
	dot_wall=v.DotProduct(root->wall.n);
	//如果dot_wall>0，视点位于当前墙面的前面
	//因此递归地处理back子树，然后再递归处理front子树
	//否则执行相反处理
	if (dot_wall>0)
	{
		TraversalBSPInsertRENDERLIST(root->back,camera,insertLocal);
		//将四边形分割成两个三角形
		TriF4DV1 triangle1,triangle2;
		triangle1.state= root->wall.state ;
		triangle1.attr = root->wall.attr;
		triangle1.color = root->wall.color;
		triangle1.ptrTexture= root->wall.ptrTexture;
		triangle1.mati = root->wall.mati;
		triangle1.nlength = root->wall.nlength;
		triangle1.normal = root->wall.n;

		triangle2.state= root->wall.state ;
		triangle2.attr = root->wall.attr;
		triangle2.color = root->wall.color;
		triangle2.ptrTexture= root->wall.ptrTexture;
		triangle2.mati = root->wall.mati;
		triangle2.nlength = root->wall.nlength;
		triangle2.normal = root->wall.n;

		//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
		if (insertLocal)
		{
			triangle1.vlist[0]= root->wall.vlist[0];
			triangle1.tvlist[0]= root->wall.vlist[0];

			triangle1.vlist[1]= root->wall.vlist[1];
			triangle1.tvlist[1]= root->wall.vlist[1];

			triangle1.vlist[2]= root->wall.vlist[3];
			triangle1.tvlist[2]= root->wall.vlist[3];

			triangle2.vlist[0]= root->wall.vlist[1];
			triangle2.tvlist[0]= root->wall.vlist[1];

			triangle2.vlist[1]= root->wall.vlist[2];
			triangle2.tvlist[1]= root->wall.vlist[2];

			triangle2.vlist[2]= root->wall.vlist[3];
			triangle2.tvlist[2]= root->wall.vlist[3];
		}
		else
		{
			triangle1.vlist[0]= root->wall.tvlist[0];
			triangle1.tvlist[0]= root->wall.tvlist[0];

			triangle1.vlist[1]= root->wall.tvlist[1];
			triangle1.tvlist[1]= root->wall.tvlist[1];

			triangle1.vlist[2]= root->wall.tvlist[3];
			triangle1.tvlist[2]= root->wall.tvlist[3];

			triangle2.vlist[0]= root->wall.tvlist[1];
			triangle2.tvlist[0]= root->wall.tvlist[1];

			triangle2.vlist[1]= root->wall.tvlist[2];
			triangle2.tvlist[1]= root->wall.tvlist[2];

			triangle2.vlist[2]= root->wall.tvlist[3];
			triangle2.tvlist[2]= root->wall.tvlist[3];
		}
		InsertTriF4DV1ToRenderList4DV1(&triangle1);
		InsertTriF4DV1ToRenderList4DV1(&triangle2);
		TraversalBSPInsertRENDERLIST(root->front,camera,insertLocal);
	}
	else
	{
		//视点位于当前墙面的后面
		TraversalBSPInsertRENDERLIST(root->front,camera,insertLocal);
		TriF4DV1 triangle1,triangle2;
		triangle1.state= root->wall.state ;
		triangle1.attr = root->wall.attr;
		triangle1.color = root->wall.color;
		triangle1.ptrTexture= root->wall.ptrTexture;
		triangle1.mati = root->wall.mati;
		triangle1.nlength = root->wall.nlength;
		triangle1.normal = root->wall.n;

		triangle2.state= root->wall.state ;
		triangle2.attr = root->wall.attr;
		triangle2.color = root->wall.color;
		triangle2.ptrTexture= root->wall.ptrTexture;
		triangle2.mati = root->wall.mati;
		triangle2.nlength = root->wall.nlength;
		triangle2.normal = root->wall.n;

		//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
		if (insertLocal)
		{
			triangle1.vlist[0]= root->wall.vlist[0];
			triangle1.tvlist[0]= root->wall.vlist[0];

			triangle1.vlist[1]= root->wall.vlist[1];
			triangle1.tvlist[1]= root->wall.vlist[1];

			triangle1.vlist[2]= root->wall.vlist[3];
			triangle1.tvlist[2]= root->wall.vlist[3];

			triangle2.vlist[0]= root->wall.vlist[1];
			triangle2.tvlist[0]= root->wall.vlist[1];

			triangle2.vlist[1]= root->wall.vlist[2];
			triangle2.tvlist[1]= root->wall.vlist[2];

			triangle2.vlist[2]= root->wall.vlist[3];
			triangle2.tvlist[2]= root->wall.vlist[3];
		}
		else
		{
			triangle1.vlist[0]= root->wall.tvlist[0];
			triangle1.tvlist[0]= root->wall.tvlist[0];

			triangle1.vlist[1]= root->wall.tvlist[1];
			triangle1.tvlist[1]= root->wall.tvlist[1];

			triangle1.vlist[2]= root->wall.tvlist[3];
			triangle1.tvlist[2]= root->wall.tvlist[3];

			triangle2.vlist[0]= root->wall.tvlist[1];
			triangle2.tvlist[0]= root->wall.tvlist[1];

			triangle2.vlist[1]= root->wall.tvlist[2];
			triangle2.tvlist[1]= root->wall.tvlist[2];

			triangle2.vlist[2]= root->wall.tvlist[3];
			triangle2.tvlist[2]= root->wall.tvlist[3];
		}
		InsertTriF4DV1ToRenderList4DV1(&triangle1);
		InsertTriF4DV1ToRenderList4DV1(&triangle2);
		TraversalBSPInsertRENDERLIST(root->back,camera,insertLocal);
	}
}

void CRenderSystem::TraversalBSPInsertRemoveBFRENDERLIST(LPBSPNODE root, CCamera &camera, int insertLocal)
{
	//该函数根据参数camera中指定的视点，按从后到前的顺序递归遍历BSP
	//BSP树中多边形的坐标必须是世界坐标
	//该函数检测视点位于当前墙面的前面还是后面，然后执行相应的处理
	//并将每个多边形插入渲染列表

	static CVector4 v;
	static float dot_wall;

	if (root==NULL)
	{
		return;
	}
	//检测视点位于当前墙面的前面还是后面
	v.CreateVector(root->wall.vlist[0].v,camera.pos);
	dot_wall=v.DotProduct(root->wall.n);
	//如果dot_wall>0，视点位于当前墙面的前面
	//因此递归地处理back子树，然后再递归处理front子树
	//否则执行相反处理
	if (dot_wall>0)
	{
		TraversalBSPInsertRemoveBFRENDERLIST(root->back,camera,insertLocal);
		
		float dp=root->wall.n.DotProduct(camera.n);
		if (FastInvcos(dp)>(90-camera.fov/2))
		{
			//将四边形分割成两个三角形
			TriF4DV1 triangle1,triangle2;
			triangle1.state= root->wall.state ;
			triangle1.attr = root->wall.attr;
			triangle1.color = root->wall.color;
			triangle1.ptrTexture= root->wall.ptrTexture;
			triangle1.mati = root->wall.mati;
			triangle1.nlength = root->wall.nlength;
			triangle1.normal = root->wall.n;

			triangle2.state= root->wall.state ;
			triangle2.attr = root->wall.attr;
			triangle2.color = root->wall.color;
			triangle2.ptrTexture= root->wall.ptrTexture;
			triangle2.mati = root->wall.mati;
			triangle2.nlength = root->wall.nlength;
			triangle2.normal = root->wall.n;

			//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
			if (insertLocal)
			{
				triangle1.vlist[0]= root->wall.vlist[0];
				triangle1.tvlist[0]= root->wall.vlist[0];

				triangle1.vlist[1]= root->wall.vlist[1];
				triangle1.tvlist[1]= root->wall.vlist[1];

				triangle1.vlist[2]= root->wall.vlist[3];
				triangle1.tvlist[2]= root->wall.vlist[3];

				triangle2.vlist[0]= root->wall.vlist[1];
				triangle2.tvlist[0]= root->wall.vlist[1];

				triangle2.vlist[1]= root->wall.vlist[2];
				triangle2.tvlist[1]= root->wall.vlist[2];

				triangle2.vlist[2]= root->wall.vlist[3];
				triangle2.tvlist[2]= root->wall.vlist[3];
			}
			else
			{
				triangle1.vlist[0]= root->wall.tvlist[0];
				triangle1.tvlist[0]= root->wall.tvlist[0];

				triangle1.vlist[1]= root->wall.tvlist[1];
				triangle1.tvlist[1]= root->wall.tvlist[1];

				triangle1.vlist[2]= root->wall.tvlist[3];
				triangle1.tvlist[2]= root->wall.tvlist[3];

				triangle2.vlist[0]= root->wall.tvlist[1];
				triangle2.tvlist[0]= root->wall.tvlist[1];

				triangle2.vlist[1]= root->wall.tvlist[2];
				triangle2.tvlist[1]= root->wall.tvlist[2];

				triangle2.vlist[2]= root->wall.tvlist[3];
				triangle2.tvlist[2]= root->wall.tvlist[3];
			}
			InsertTriF4DV1ToRenderList4DV1(&triangle1);
			InsertTriF4DV1ToRenderList4DV1(&triangle2);
		}
		TraversalBSPInsertRemoveBFRENDERLIST(root->front,camera,insertLocal);
	}
	else
	{
		//视点位于当前墙面的后面
		TraversalBSPInsertRemoveBFRENDERLIST(root->front,camera,insertLocal);
		float dp=root->wall.n.DotProduct(camera.n);
		if (FastInvcos(dp)<(90+camera.fov/2))
		{
			TriF4DV1 triangle1,triangle2;
			triangle1.state= root->wall.state ;
			triangle1.attr = root->wall.attr;
			triangle1.color = root->wall.color;
			triangle1.ptrTexture= root->wall.ptrTexture;
			triangle1.mati = root->wall.mati;
			triangle1.nlength = root->wall.nlength;
			triangle1.normal = root->wall.n;

			triangle2.state= root->wall.state ;
			triangle2.attr = root->wall.attr;
			triangle2.color = root->wall.color;
			triangle2.ptrTexture= root->wall.ptrTexture;
			triangle2.mati = root->wall.mati;
			triangle2.nlength = root->wall.nlength;
			triangle2.normal = root->wall.n;

			//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
			if (insertLocal)
			{
				triangle1.vlist[0]= root->wall.vlist[0];
				triangle1.tvlist[0]= root->wall.vlist[0];

				triangle1.vlist[1]= root->wall.vlist[1];
				triangle1.tvlist[1]= root->wall.vlist[1];

				triangle1.vlist[2]= root->wall.vlist[3];
				triangle1.tvlist[2]= root->wall.vlist[3];

				triangle2.vlist[0]= root->wall.vlist[1];
				triangle2.tvlist[0]= root->wall.vlist[1];

				triangle2.vlist[1]= root->wall.vlist[2];
				triangle2.tvlist[1]= root->wall.vlist[2];

				triangle2.vlist[2]= root->wall.vlist[3];
				triangle2.tvlist[2]= root->wall.vlist[3];
			}
			else
			{
				triangle1.vlist[0]= root->wall.tvlist[0];
				triangle1.tvlist[0]= root->wall.tvlist[0];

				triangle1.vlist[1]= root->wall.tvlist[1];
				triangle1.tvlist[1]= root->wall.tvlist[1];

				triangle1.vlist[2]= root->wall.tvlist[3];
				triangle1.tvlist[2]= root->wall.tvlist[3];

				triangle2.vlist[0]= root->wall.tvlist[1];
				triangle2.tvlist[0]= root->wall.tvlist[1];

				triangle2.vlist[1]= root->wall.tvlist[2];
				triangle2.tvlist[1]= root->wall.tvlist[2];

				triangle2.vlist[2]= root->wall.tvlist[3];
				triangle2.tvlist[2]= root->wall.tvlist[3];
			}
			InsertTriF4DV1ToRenderList4DV1(&triangle1);
			InsertTriF4DV1ToRenderList4DV1(&triangle2);
		}
		
		TraversalBSPInsertRemoveBFRENDERLIST(root->back,camera,insertLocal);
	}
}
void CRenderSystem::TraversalBSPInsertCullFrustrumRENDERLIST(LPBSPNODE root, CCamera &camera, int insertLocal)
{
	//该函数根据参数camera中指定的视点，按从后到前的顺序递归遍历BSP
	//BSP树中多边形的坐标必须是世界坐标
	//该函数检测视点位于当前墙面的前面还是后面，然后执行相应的处理
	//并将每个多边形插入渲染列表

	static CVector4 v;
	static float dot_wall;

	if (root==NULL)
	{
		return;
	}
	//检测视点位于当前墙面的前面还是后面
	v.CreateVector(root->wall.vlist[0].v,camera.pos);
	dot_wall=v.DotProduct(root->wall.n);
	//如果dot_wall>0，视点位于当前墙面的前面
	//因此递归地处理back子树，然后再递归处理front子树
	//否则执行相反处理
	if (dot_wall>0)
	{
		float dp=root->wall.n.DotProduct(camera.n);
		if (FastInvcos(dp)>(90-camera.fov/2))
		{
			TraversalBSPInsertCullFrustrumRENDERLIST(root->back,camera,insertLocal);
			//将四边形分割成两个三角形
			TriF4DV1 triangle1,triangle2;
			triangle1.state= root->wall.state ;
			triangle1.attr = root->wall.attr;
			triangle1.color = root->wall.color;
			triangle1.ptrTexture= root->wall.ptrTexture;
			triangle1.mati = root->wall.mati;
			triangle1.nlength = root->wall.nlength;
			triangle1.normal = root->wall.n;

			triangle2.state= root->wall.state ;
			triangle2.attr = root->wall.attr;
			triangle2.color = root->wall.color;
			triangle2.ptrTexture= root->wall.ptrTexture;
			triangle2.mati = root->wall.mati;
			triangle2.nlength = root->wall.nlength;
			triangle2.normal = root->wall.n;

			//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
			if (insertLocal)
			{
				triangle1.vlist[0]= root->wall.vlist[0];
				triangle1.tvlist[0]= root->wall.vlist[0];

				triangle1.vlist[1]= root->wall.vlist[1];
				triangle1.tvlist[1]= root->wall.vlist[1];

				triangle1.vlist[2]= root->wall.vlist[3];
				triangle1.tvlist[2]= root->wall.vlist[3];

				triangle2.vlist[0]= root->wall.vlist[1];
				triangle2.tvlist[0]= root->wall.vlist[1];

				triangle2.vlist[1]= root->wall.vlist[2];
				triangle2.tvlist[1]= root->wall.vlist[2];

				triangle2.vlist[2]= root->wall.vlist[3];
				triangle2.tvlist[2]= root->wall.vlist[3];
			}
			else
			{
				triangle1.vlist[0]= root->wall.tvlist[0];
				triangle1.tvlist[0]= root->wall.tvlist[0];

				triangle1.vlist[1]= root->wall.tvlist[1];
				triangle1.tvlist[1]= root->wall.tvlist[1];

				triangle1.vlist[2]= root->wall.tvlist[3];
				triangle1.tvlist[2]= root->wall.tvlist[3];

				triangle2.vlist[0]= root->wall.tvlist[1];
				triangle2.tvlist[0]= root->wall.tvlist[1];

				triangle2.vlist[1]= root->wall.tvlist[2];
				triangle2.tvlist[1]= root->wall.tvlist[2];

				triangle2.vlist[2]= root->wall.tvlist[3];
				triangle2.tvlist[2]= root->wall.tvlist[3];
			}
			InsertTriF4DV1ToRenderList4DV1(&triangle1);
			InsertTriF4DV1ToRenderList4DV1(&triangle2);
		}
		TraversalBSPInsertCullFrustrumRENDERLIST(root->front,camera,insertLocal);
	}
	else
	{
		//视点位于当前墙面的后面
		float dp=root->wall.n.DotProduct(camera.n);
		if (FastInvcos(dp)<(90+camera.fov/2))
		{
			TraversalBSPInsertCullFrustrumRENDERLIST(root->front,camera,insertLocal);
			TriF4DV1 triangle1,triangle2;
			triangle1.state= root->wall.state ;
			triangle1.attr = root->wall.attr;
			triangle1.color = root->wall.color;
			triangle1.ptrTexture= root->wall.ptrTexture;
			triangle1.mati = root->wall.mati;
			triangle1.nlength = root->wall.nlength;
			triangle1.normal = root->wall.n;

			triangle2.state= root->wall.state ;
			triangle2.attr = root->wall.attr;
			triangle2.color = root->wall.color;
			triangle2.ptrTexture= root->wall.ptrTexture;
			triangle2.mati = root->wall.mati;
			triangle2.nlength = root->wall.nlength;
			triangle2.normal = root->wall.n;

			//创建的三角形的顺序为v0,v1,v3   v1,v2,v3
			if (insertLocal)
			{
				triangle1.vlist[0]= root->wall.vlist[0];
				triangle1.tvlist[0]= root->wall.vlist[0];

				triangle1.vlist[1]= root->wall.vlist[1];
				triangle1.tvlist[1]= root->wall.vlist[1];

				triangle1.vlist[2]= root->wall.vlist[3];
				triangle1.tvlist[2]= root->wall.vlist[3];

				triangle2.vlist[0]= root->wall.vlist[1];
				triangle2.tvlist[0]= root->wall.vlist[1];

				triangle2.vlist[1]= root->wall.vlist[2];
				triangle2.tvlist[1]= root->wall.vlist[2];

				triangle2.vlist[2]= root->wall.vlist[3];
				triangle2.tvlist[2]= root->wall.vlist[3];
			}
			else
			{
				triangle1.vlist[0]= root->wall.tvlist[0];
				triangle1.tvlist[0]= root->wall.tvlist[0];

				triangle1.vlist[1]= root->wall.tvlist[1];
				triangle1.tvlist[1]= root->wall.tvlist[1];

				triangle1.vlist[2]= root->wall.tvlist[3];
				triangle1.tvlist[2]= root->wall.tvlist[3];

				triangle2.vlist[0]= root->wall.tvlist[1];
				triangle2.tvlist[0]= root->wall.tvlist[1];

				triangle2.vlist[1]= root->wall.tvlist[2];
				triangle2.tvlist[1]= root->wall.tvlist[2];

				triangle2.vlist[2]= root->wall.tvlist[3];
				triangle2.tvlist[2]= root->wall.tvlist[3];
			}
			InsertTriF4DV1ToRenderList4DV1(&triangle1);
			InsertTriF4DV1ToRenderList4DV1(&triangle2);
		}

		TraversalBSPInsertCullFrustrumRENDERLIST(root->back,camera,insertLocal);
	}
}

BOOL CRenderSystem::BHVFrustrumCull(LPBHVNODE bhvTree, const CCamera cam, int cullFlags)
{
	if (!bhvTree)
	{
		return FALSE;
	}
	//1.将节点的包围球球心变换为相机坐标
	CPoint4D spherePos;
	spherePos=bhvTree->pos*cam.mcam;

	//2.根据剔除标记对节点进行剔除
	if (cullFlags & CULLED_Z_PLANE)
	{
		if (((spherePos.z-bhvTree->radius.z)>cam.farZ)||
			((spherePos.z+bhvTree->radius.z)<cam.nearZ))
		{
			//对整个节点进行删除
			for (int i=0;i<bhvTree->numObjects;i++)
			{
				SET_BIT(bhvTree->objects[i]->state,OBJMODEL4DV1_STATE_CULLED);
			}
			return TRUE;
		}
	}

	if (cullFlags & CULLED_X_PLANE)
	{
		//只根据左右截面对物体进行裁剪

		//使用右裁剪面和左裁剪面对包围球最左边点和最右边点进行测试
		float testZ=0.5*cam.viewPlaneWidth*spherePos.z/cam.viewDist;  //利用三角形相似
		if (((spherePos.x-bhvTree->radius.x)>testZ)
			||((spherePos.x+bhvTree->radius.x)<-testZ))
		{
			//对整个节点进行删除
			for (int i=0;i<bhvTree->numObjects;i++)
			{
				SET_BIT(bhvTree->objects[i]->state,OBJMODEL4DV1_STATE_CULLED);
			}
			return TRUE;
		}
	}

	if (cullFlags & CULLED_Y_PLANE)
	{
		//只根据上下截面对物体进行裁剪

		//使用上裁剪面和下裁剪面对包围球最下边点和最上边点进行测试
		float testZ=0.5*cam.viewPlaneHeight*spherePos.z/cam.viewDist;   //利用三角形相似 
		if (((spherePos.y-bhvTree->radius.y)>testZ)
			||((spherePos.y+bhvTree->radius.y)<-testZ))
		{
			//对整个节点进行删除
			for (int i=0;i<bhvTree->numObjects;i++)
			{
				SET_BIT(bhvTree->objects[i]->state,OBJMODEL4DV1_STATE_CULLED);
			}
			return TRUE;
		}
	}
	
	//至此，我们知道该BHV节点没有被删除
	//因此遍历子节点，看是否能删除它们
	for (int ichild=0;ichild<bhvTree->numChildren;ichild++)
	{
		BHVFrustrumCull(bhvTree->links[ichild],cam,cullFlags);
	}

	return FALSE;
}