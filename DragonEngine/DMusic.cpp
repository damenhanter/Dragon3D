#include "StdAfx.h"
#include "DMusic.h"

extern HWND hWnd;
extern HINSTANCE hInstance;
CDMusic::CDMusic(CDSound* pSound)
{
	this->pSound=pSound;
	activeId=-1;
}

CDMusic::CDMusic()
{
	this->pSound=NULL;
	activeId=-1;
}

CDMusic::~CDMusic(void)
{
}

int CDMusic::DMusicInit(void)
{
	if(FAILED(CoInitialize(NULL)))
	{
		MessageBox(hWnd,"初始化组建对象失败!","error",MB_OK);
		return FALSE;
	}
	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance,
		NULL,CLSCTX_INPROC,IID_IDirectMusicPerformance,(void**)&pPerf)))
	{
		MessageBox(hWnd,"创建DirectMusicPerformance组建失败!","error",MB_OK);
		return FALSE;
	}
	if(pSound!=NULL)
	{
		if(FAILED(pPerf->Init(NULL,pSound->lpds,hWnd)))
		{
			MessageBox(hWnd,"初始化DirectMusic失败!","error",MB_OK);
			return FALSE;
		}
	}
	else
	{
		if(FAILED(pPerf->Init(NULL,NULL,hWnd)))
		{
			MessageBox(hWnd,"初始化DirectMusic失败!","error",MB_OK);
			return FALSE;
		}
	}
	if(FAILED(pPerf->AddPort(NULL)))
	{
		MessageBox(hWnd,"添加数据输出端口失败!","error",MB_OK);
		return FALSE;
	}
	if(FAILED(CoCreateInstance(CLSID_DirectMusicLoader,NULL,
		CLSCTX_INPROC,IID_IDirectMusicLoader,(void**)&pLoader)))
	{
		MessageBox(hWnd,"创建DirectMusicPerformance组建失败!","error",MB_OK);
		return NULL;
	}
	for(int index=0; index<DM_NUM_SEGMENTS; index++)
	{
		midi[index].dm_segment=NULL;
		midi[index].dm_segstate=NULL;
		midi[index].state=MIDI_NULL;
		midi[index].id=index;
	}
	return TRUE;
}

int CDMusic::LoadMIDISegment(WCHAR *wszMidifilename)
{
	DMUS_OBJECTDESC objDesc;
	WCHAR wszDir[MAX_PATH];
	int id=-1;
	for(int index=0; index<DM_NUM_SEGMENTS; index++)
	{
		if(midi[index].state==MIDI_NULL)
		{
			id=index;
			break;
		}
	}
	if(id==-1)
		return FALSE;

	if(NULL==_wgetcwd(wszDir,MAX_PATH))
	{
		MessageBox(hWnd,"获得当前目录失败!","error",MB_OK);
		return NULL;
	}
	if(FAILED(pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes,wszDir,FALSE)))
	{
		MessageBox(hWnd,"搜索目录失败!","error",MB_OK);
		return NULL;
	}
	ZeroMemory(&objDesc,sizeof(objDesc));
	objDesc.dwSize=sizeof(objDesc);
	objDesc.guidClass=CLSID_DirectMusicSegment;
	wcscpy_s(objDesc.wszFileName,wszMidifilename);
	objDesc.dwValidData= DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
	if(FAILED(pLoader->GetObjectA(&objDesc,IID_IDirectMusicSegment,(void**)&midi[id].dm_segment)))
	{
		MessageBox(hWnd,"DirectMusic获得数据失败!","error",MB_OK);
		return NULL;
	}
	if(midi[id].dm_segment->SetParam(GUID_StandardMIDIFile,-1,0,0,(void*)pPerf))
	{
		MessageBox(hWnd,"DirectMusic设置执行参数失败!","error",MB_OK);
		return NULL;
	}
	if(midi[id].dm_segment->SetParam(GUID_Download,-1,0,0,(void*)pPerf))
	{
		MessageBox(hWnd,"DirectMusic设置执行参数失败!","error",MB_OK);
		return NULL;
	}
	midi[id].dm_segstate=NULL;
	midi[id].id=id;
	midi[id].state=MIDI_LOADED;
	return id;
}
int CDMusic::play(int id)
{
	if(midi[id].dm_segment && (midi[id].state!=MIDI_NULL))
	{
		if(activeId!=-1)
			StopMusic(activeId);
		if(FAILED(pPerf->PlaySegment(midi[id].dm_segment,0,0,&midi[id].dm_segstate)))
		{
			MessageBox(hWnd,"播放Music失败!","error",MB_OK);
			return FALSE;
		}
		midi[id].state=MIDI_PLAYING;
		activeId=id;
	}
	return TRUE;
}
int CDMusic::StopMusic(int id)
{
	if(midi[id].dm_segment && (midi[id].state!=MIDI_NULL))
	{
		if(FAILED(pPerf->Stop(midi[id].dm_segment,NULL,0,0)))
		{
			MessageBox(hWnd,"Stop Music失败!","error",MB_OK);
			return FALSE;
		}
		midi[id].state=MIDI_STOPPED;
		activeId=-1;
	}
	return TRUE;
}
int CDMusic::DeleteMusic(int id)
{
	if(midi[id].dm_segment)
	{
		midi[id].dm_segment->SetParam(GUID_Unload,-1,0,0,(void*)pPerf);
		midi[id].dm_segment->Release();
		midi[id].dm_segment=NULL;
		midi[id].dm_segstate=NULL;
		midi[id].state=MIDI_NULL;
	}
	return TRUE;
}
int CDMusic::DeleteAllMusic()
{
	for(int index=0; index<DM_NUM_SEGMENTS; index++)
		DeleteMusic(index);
	return TRUE;
}
int CDMusic::GetStatus(int id)
{
	if(midi[id].dm_segment && midi[id].state!=MIDI_NULL)
	{
		if(pPerf->IsPlaying(midi[id].dm_segment,NULL)==S_OK)
			midi[id].state=MIDI_PLAYING;
		else
			midi[id].state=MIDI_STOPPED;
		return midi[id].state;
	}
	else
		return FALSE;
}
void CDMusic::ShutDown()
{
	if(pPerf)
		pPerf->Stop(NULL,NULL,0,0);
	DeleteAllMusic();
	if(pPerf)
	{
		pPerf->CloseDown();
		pPerf->Release();
		pPerf=NULL;
	}
	if(pLoader)
	{
		pLoader->Release();
		pLoader=NULL;
	}

	CoUninitialize();
}