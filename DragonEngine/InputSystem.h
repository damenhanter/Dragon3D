#pragma once
#include "Mouse.h"
#include "keyboard.h"
#include "JoyStick.h"

class CInputSystem
{
public:
	CInputSystem(BOOL exclusive);
	~CInputSystem(void);
	BOOL UpateDevice();
	int KeyDown(UINT key);
	int KeyUp(UINT key);
	int MouseButtonDown(UINT key);
	int MouseButtonUp(UINT key);
	POINT GetMousePosition();
	POINT GetMouseZPosition();
	int JoyStickButtonDown(UINT key);
	int JoyStickButtonUp(UINT key);
	POINT GetJoyStickPosition();
public:
	LPDIRECTINPUT8 lpdi;
	CMouse* mouse;
	CKeyboard* keyboard;
	CJoyStick* joystick;
};
