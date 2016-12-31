#pragma once
#include "datatype.h"

class CKeyboard
{
public:
	CKeyboard(LPDIRECTINPUT8 input8,BOOL exclusive);
	~CKeyboard(void);
	BOOL UpdateDevice();
	int ButtonDown(UINT key);
	int ButtonUp(UINT key);
	void Release();
protected:
	LPDIRECTINPUTDEVICE8 m_pKeyboard;
	UCHAR m_keyboardState[256]; 
	UCHAR m_oldkeyboardState[256];
};
