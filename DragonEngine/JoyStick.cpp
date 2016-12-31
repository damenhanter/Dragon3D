#include "StdAfx.h"
#include "JoyStick.h"
#pragma warning (disable:4996)
extern HWND hWnd;
extern HINSTANCE hInstance;

CJoyStick* g_pJoyStick=NULL;
BOOL EnumDeviceCallBack(const DIDEVICEINSTANCE *inst, void* pData)
{
	return g_pJoyStick->EnumJoySticks(inst,pData);
}

CJoyStick::CJoyStick(LPDIRECTINPUT8 input8)
{
	x=y=0;
	g_pJoyStick=this;
	m_input8=input8;
	if(FAILED(m_input8->EnumDevices(DI8DEVCLASS_GAMECTRL,(LPDIENUMDEVICESCALLBACK)EnumDeviceCallBack,
		&JoystickGUID,DIEDFL_ATTACHEDONLY)))
	{
		m_pJoyStick=NULL;
		return ;
	}

	if(FAILED(m_input8->CreateDevice(JoystickGUID,&m_pJoyStick,NULL)))
	{
		//MessageBox(hWnd,"创建游戏杆设备失败!","error",MB_OK);
		return ;
	}
	if(FAILED(m_pJoyStick->SetDataFormat(&c_dfDIJoystick)))
	{
		//MessageBox(hWnd,"游戏杆格式化失败!","error",MB_OK);
		return ;
	}
	if(FAILED(m_pJoyStick->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
	{
		//MessageBox(hWnd,"设置游戏杆协作级别失败!","error",MB_OK);
		return ;
	}
	
	DIPROPRANGE joyAxisRange;

	joyAxisRange.lMin=-24;
	joyAxisRange.lMax=24;
	joyAxisRange.diph.dwSize=sizeof(DIPROPRANGE);
	joyAxisRange.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	joyAxisRange.diph.dwObj=DIJOFS_X;
	joyAxisRange.diph.dwHow=DIPH_BYOFFSET;
	m_pJoyStick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);
	
	joyAxisRange.lMin=-24;
	joyAxisRange.lMax=24;
	joyAxisRange.diph.dwSize=sizeof(DIPROPRANGE);
	joyAxisRange.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	joyAxisRange.diph.dwObj=DIJOFS_Y;
	joyAxisRange.diph.dwHow=DIPH_BYOFFSET;
	m_pJoyStick->SetProperty(DIPROP_RANGE,&joyAxisRange.diph);

	DIPROPDWORD deadZone;
	deadZone.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	deadZone.diph.dwHow=DIPH_BYOFFSET;
	deadZone.diph.dwObj=DIJOFS_X;
	deadZone.diph.dwSize=sizeof(DIPROPDWORD);
	deadZone.dwData=1000;
	m_pJoyStick->SetProperty(DIPROP_DEADZONE,&deadZone.diph);

	deadZone.diph.dwHeaderSize=sizeof(DIPROPHEADER);
	deadZone.diph.dwHow=DIPH_BYOFFSET;
	deadZone.diph.dwObj=DIJOFS_Y;
	deadZone.diph.dwSize=sizeof(DIPROPDWORD);
	deadZone.dwData=1000;
	m_pJoyStick->SetProperty(DIPROP_DEADZONE,&deadZone.diph);
	
	if(FAILED(m_pJoyStick->Acquire()))
	{
		//MessageBox(hWnd,"获得设备失败!","error",MB_OK);
		return ;
	}
}

CJoyStick::~CJoyStick(void)
{
	Release();
}
BOOL CJoyStick::EnumJoySticks(LPCDIDEVICEINSTANCE lpdi, LPVOID lpguid)
{
	*(GUID*)lpguid=lpdi->guidInstance;
	strcpy(joyname,lpdi->tszProductName);
	return DIENUM_STOP;
}
BOOL CJoyStick::UpdateDevice()
{
	if(m_pJoyStick)
	{
		m_pJoyStick->Poll();
		memcpy(&m_oldjoyState,&m_joyState,sizeof(DIJOYSTATE));
		if(FAILED(m_pJoyStick->GetDeviceState(sizeof(DIJOYSTATE),&m_joyState)))
		{
			if(SUCCEEDED(m_pJoyStick->Acquire()))
			{
				m_pJoyStick->GetDeviceState(sizeof(DIJOYSTATE),&m_joyState);
			}
		}
		x+=m_joyState.lX;
		y+=m_joyState.lY;
	}
	return TRUE;
}
POINT CJoyStick::GetPosition()
{
	if(m_pJoyStick)
	{
		POINT point;
		point.x=x;
		point.y=y;
		return point;
	}
	else
		return POINT();
}
int CJoyStick::ButtonDown(UINT key)
{
	if(m_pJoyStick)
	{
		return (m_joyState.rgbButtons[key] & 0x80);
	}
	else
		return 0;
}
int CJoyStick::ButtonUp(UINT key)
{
	if(m_pJoyStick)
	{
		return (!(m_joyState.rgbButtons[key] & 0x80)
			&&(m_oldjoyState.rgbButtons[key]!=m_joyState.rgbButtons[key]));
	}
	else
		return 0;
}
void CJoyStick::Release()
{
	if(m_pJoyStick!=NULL)
	{
		m_pJoyStick->Unacquire();
		m_pJoyStick->Release();
		m_pJoyStick=NULL;
	}

}