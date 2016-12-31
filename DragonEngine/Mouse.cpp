#include "StdAfx.h"
#include "Mouse.h"

extern HWND hWnd;
extern HINSTANCE hInstance;

CMouse::CMouse(LPDIRECTINPUT8 input8,BOOL exclusive)
{
	x=y=z=0;
	if(FAILED(input8->CreateDevice(GUID_SysMouse,&m_pMouse,NULL)))
	{
		//MessageBox(hWnd,"鼠标设备创建失败!","error",MB_OK);
		m_pMouse=NULL;
		return ;
	}
	if(exclusive)
	{
		if(FAILED(m_pMouse->SetCooperativeLevel(hWnd,DISCL_EXCLUSIVE | DISCL_BACKGROUND)))
		{
			//MessageBox(hWnd,"鼠标设置合作级别失败!","error",MB_OK);
			return ;
		}
	}
	else
	{
		if(FAILED(m_pMouse->SetCooperativeLevel(hWnd,DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		{
			//MessageBox(hWnd,"鼠标设置合作级别失败!","error",MB_OK);
			return ;
		}
	}
	if(FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
	{
		//MessageBox(hWnd,"鼠标格式化数据失败!","error",MB_OK);
		return ;
	}
}

CMouse::~CMouse(void)
{
	Release();
}

BOOL CMouse::UpdateDevice()
{
	if(m_pMouse)
	{
		memcpy(&m_oldMouseState,&m_MouseState,sizeof(DIMOUSESTATE));
		HRESULT hr;
		while((hr=m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE),&m_MouseState))==DIERR_INPUTLOST)
		{
			if(FAILED(m_pMouse->Acquire()))
			{
				break;
			}
		}

		x+=m_MouseState.lX;
		y+=m_MouseState.lY;
		z=m_MouseState.lZ;
	}
	return TRUE;
}
POINT CMouse::GetPosition()
{
	if(m_pMouse)
	{
		POINT point={x,y};
		return point;
	}
	else
		return POINT();
}
POINT CMouse::GetZPosition()
{
	if(m_pMouse)
	{
		POINT point={z,z};
		return point;
	}
	else
		return POINT();
}
int CMouse::ButtonDown(UINT key)
{
	if(m_pMouse)
	{
		return (m_MouseState.rgbButtons[key] & 0x80);
	}
	else
		return 0;
}
int CMouse::ButtonUp(UINT key)
{
	if(m_pMouse)
	{
		return (!(m_MouseState.rgbButtons[key] & 0x80) && 
			(m_oldMouseState.rgbButtons[key]!=m_MouseState.rgbButtons[key]));
	}
	else
		return 0;
}
void CMouse::Release()
{
	if(m_pMouse!=NULL)
	{
		m_pMouse->Acquire();
		m_pMouse->Release();
		m_pMouse=NULL;
	}
}