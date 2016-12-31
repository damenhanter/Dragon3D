#include "StdAfx.h"
#include "Keyboard.h"
extern HWND hWnd;
extern HINSTANCE hInstance;

CKeyboard::CKeyboard(LPDIRECTINPUT8 input8,BOOL exclusive)
{
	ZeroMemory(m_keyboardState,256);
	ZeroMemory(m_oldkeyboardState,256);
	if(FAILED(input8->CreateDevice(GUID_SysKeyboard,&m_pKeyboard,NULL)))
	{
		m_pKeyboard=NULL;
		return ;
		//MessageBox(hWnd,"创建键盘设备失败!","error",MB_OK);
	}
	DWORD flag;
	if(exclusive)
	{
		flag=DISCL_EXCLUSIVE|DISCL_BACKGROUND;
	}
	else
	{
		flag=DISCL_BACKGROUND|DISCL_NONEXCLUSIVE;
	}
	if(FAILED(m_pKeyboard->SetCooperativeLevel(hWnd,flag)))
	{
		return ;
		//MessageBox(hWnd,"键盘设置协作级别失败!","error",MB_OK);
	}
	if(FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return ;
		//MessageBox(hWnd,"键盘格式化数据失败!","error",MB_OK);
	}
}

CKeyboard::~CKeyboard(void)
{
	Release();
}
BOOL CKeyboard::UpdateDevice()
{
	if(m_pKeyboard)
	{
		memcpy(m_oldkeyboardState,m_keyboardState,256);
		if(FAILED(m_pKeyboard->GetDeviceState(256,m_keyboardState)))
		{
			if(FAILED(m_pKeyboard->Acquire()))
			{
				if(FAILED(m_pKeyboard->GetDeviceState(256,m_keyboardState)))
				{
					//MessageBox(hWnd,"键盘获得数据失败!","error",MB_OK);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
int CKeyboard::ButtonDown(UINT key)
{
	if(m_pKeyboard)
	{
		return m_keyboardState[key] & 0x80;
	}
	else
		return 0;
}
int CKeyboard::ButtonUp(UINT key)
{
	if(m_pKeyboard)
	{
		return (!(m_keyboardState[key] & 0x80)&&(m_keyboardState[key]!=m_oldkeyboardState[key]));
	}
	else
		return 0;
}
void CKeyboard::Release()
{
	if(m_pKeyboard!=NULL)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
		m_pKeyboard=NULL;
	}
	ZeroMemory(m_keyboardState,256);
	ZeroMemory(m_oldkeyboardState,256);
}