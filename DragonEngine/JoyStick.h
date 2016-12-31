#pragma once
#include "datatype.h"

class CJoyStick
{
public:
	CJoyStick(LPDIRECTINPUT8 input8);
	~CJoyStick(void);
    BOOL EnumJoySticks(LPCDIDEVICEINSTANCE lpdi,LPVOID lpguid);
	BOOL UpdateDevice();
	POINT GetPosition();
	int ButtonDown(UINT key);
	int ButtonUp(UINT key);
	void Release();
protected:
	LPDIRECTINPUTDEVICE8 m_pJoyStick;
	LPDIRECTINPUT8 m_input8;
	DIJOYSTATE m_joyState;
	DIJOYSTATE m_oldjoyState;
	GUID JoystickGUID;
	char joyname[260];
	int x;
	int y;
};
