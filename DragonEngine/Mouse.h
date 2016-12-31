#pragma once
#include "datatype.h"

class CMouse
{
public:
	CMouse(LPDIRECTINPUT8 input8,BOOL exclusive);
	~CMouse(void);

	BOOL UpdateDevice();
	POINT GetPosition();
	POINT GetZPosition();
	int ButtonDown(UINT key);
	int ButtonUp(UINT key);
	void Release();
protected:
	LPDIRECTINPUTDEVICE8 m_pMouse;
	DIMOUSESTATE m_MouseState;
	DIMOUSESTATE m_oldMouseState;
	long x;
	long y;
	long z;
};
