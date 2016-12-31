#pragma once

#include "../DragonEngine/engine.h"
#pragma comment(lib,"../lib/DragonEngine.lib")



int GameInit(void* parms=NULL);
int GameShutdown(void* parms=NULL);
int GameMain(void* params=NULL);

LRESULT APIENTRY WndProc (HWND, UINT, WPARAM, LPARAM);

