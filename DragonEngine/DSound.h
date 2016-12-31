#pragma once
#include "datatype.h"

typedef struct PCM_SOUND
{
	LPDIRECTSOUNDBUFFER dsbuffer;
	int state;
	int rate;
	int size;
	int id;
}PCMSound,*LPPCMSound;

class CDSound
{
public:
	CDSound(void);
	~CDSound(void);

	int DSoundInit(void);
	int DSoundLoadWave(char* filename,int controlflags=DSBCAPS_CTRLDEFAULT);
	int play(int id,int flag);
	int SetVolume(int id,int vol);
	int SetFrequence(int id,int frequ);
	int SetPan(int id,long lpan);
	int StopSound(int id);
	int StopAllSound();
	int DeleteSound(int id);
	int DeleteAllSound();
	DWORD GetStatus(int id);
	int ReplicateSound(int sourceId);
	void ShutDown();
public:
	LPDIRECTSOUND lpds;
protected:
	DSBUFFERDESC dsbd;
	DSCAPS dscaps;
	DSBCAPS dsbcaps;
	WAVEFORMATEX pcmwf;
	PCMSound sound[MAX_SOUNDS];
};
