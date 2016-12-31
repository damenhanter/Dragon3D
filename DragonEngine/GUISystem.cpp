#include "StdAfx.h"
#include "GUISystem.h"
#include "datatype.h"


CGUISystem::CGUISystem(void)
{
}

CGUISystem::~CGUISystem(void)
{
}

int CGUISystem::DrawTextGDI(char* text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds)
{
	HDC hdc;
	if(FAILED(lpdds->GetDC(&hdc)))
		return false;
	SetTextColor(hdc,color);
	SetBkMode(hdc,TRANSPARENT);
	TextOut(hdc,x,y,text,strlen(text));
	lpdds->ReleaseDC(hdc);
	return true;
}
