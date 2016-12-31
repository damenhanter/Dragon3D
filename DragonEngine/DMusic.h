#pragma once
#include "datatype.h"
#include "DSound.h"


typedef struct DMUSIC_MIDI_TYP
{
	IDirectMusicSegment* dm_segment;
	IDirectMusicSegmentState* dm_segstate;
	int id;
	int state;
}MIDISegment,*LPMIDISegment;


class CDMusic
{
public:
	CDMusic(void);
	CDMusic(CDSound* pSound);
	~CDMusic(void);
	int DMusicInit(void);
	int LoadMIDISegment(WCHAR* wszMidifilename);
	int play(int id);
	int StopMusic(int id);
	int DeleteMusic(int id);
	int DeleteAllMusic();
	int GetStatus(int id);
	void ShutDown();
protected:
	int activeId;
	CDSound* pSound;
	IDirectMusicPerformance* pPerf;
	IDirectMusicLoader*      pLoader;
	MIDISegment midi[DM_NUM_SEGMENTS];
};
