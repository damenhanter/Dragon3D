#pragma once
#include "datatype.h"

class CGUISystem
{
public:
	CGUISystem(void);
	~CGUISystem(void);
	int DrawTextGDI(char* text, int x, int y, COLORREF color, LPDIRECTDRAWSURFACE7 lpdds);
};
