#include "StdAfx.h"
#include "InputSystem.h"
extern HWND hWnd;
extern HINSTANCE hInstance;
CInputSystem::CInputSystem(BOOL exclusive)
{
	if(SUCCEEDED(DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,(void**)&lpdi,NULL)))
	{
		mouse=new CMouse(lpdi,exclusive);
		keyboard=new CKeyboard(lpdi,exclusive);
		joystick=new CJoyStick(lpdi);
	}
	else
	{
		//MessageBox(hWnd,"获取DirectInput8对象失败!","error",MB_OK);
		return ;
	}

}

CInputSystem::~CInputSystem(void)
{
	if(mouse)
	{
		//mouse->Release();
		delete mouse;
		mouse=NULL;
	}
	if(keyboard)
	{
		//keyboard->Release();
		delete keyboard;
		keyboard=NULL;
	}
	if(joystick)
	{
		//joystick->Release();
		delete joystick;
		joystick=NULL;
	}
	if(lpdi!=NULL)
	{
		lpdi->Release();
		lpdi=NULL;
	}
}
BOOL CInputSystem::UpateDevice()
{
	if(mouse)
	{
		mouse->UpdateDevice();
	}
	if(keyboard)
	{
		keyboard->UpdateDevice();
	}
	if(joystick)
	{
		joystick->UpdateDevice();
	}
	return TRUE;
}
int  CInputSystem::KeyDown(UINT key)
{
	if(keyboard)
		return keyboard->ButtonDown(key);
	else
		return 0;
}
int  CInputSystem::KeyUp(UINT key)
{
	if(keyboard)
		return keyboard->ButtonUp(key);
	else
		return 0;
}
int  CInputSystem::MouseButtonDown(UINT key)
{
	if(mouse)
		return mouse->ButtonDown(key);
	else
		return 0;
}
int  CInputSystem::MouseButtonUp(UINT key)
{
	if(mouse)
		return mouse->ButtonUp(key);
	else
		return 	0;
}
POINT  CInputSystem::GetMousePosition()
{
	if(mouse)
		return mouse->GetPosition();
	else
		return POINT(); 

}
POINT  CInputSystem::GetMouseZPosition()
{
	if(mouse)
		return mouse->GetZPosition();
	else
		return POINT(); 
}
int  CInputSystem::JoyStickButtonDown(UINT key)
{
	if(joystick)
		return joystick->ButtonDown(key);
	else
		return 0; 
}
int  CInputSystem::JoyStickButtonUp(UINT key)
{
	if(joystick)
		return joystick->ButtonUp(key);
	else
		return 0; 
}
POINT  CInputSystem::GetJoyStickPosition()
{
	if(joystick)
		return joystick->GetPosition();
	else
		return POINT(); 
}
