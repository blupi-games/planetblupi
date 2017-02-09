// sound.h
//

#pragma once

#include <stdio.h>
#include <SDL_mixer.h>
#include "blupi.h"

/////////////////////////////////////////////////////////////////////////////

#define MAXSOUND	100
#define MAXVOLUME	20
#define MAXBLUPI	100

class CSound
{
public:
	CSound();
	~CSound();

	bool	Create();
	void	SetState(bool bState);
	bool	GetEnable();

	void	SetAudioVolume(int volume);
	int		GetAudioVolume();
	void	SetMidiVolume(int volume);
	int		GetMidiVolume();

	void	CacheAll();
	bool	Cache(int channel, char *pFilename);
	void	Flush(int channel);

	bool	Play(int channel, int volume=0, Uint8 panLeft = 255, Uint8 panRight = 255);
	bool	PlayImage(int channel, POINT pos, int rank=-1);
	bool	PlayMusic(const char *lpszMIDIFilename);
	bool	RestartMusic();
	void	SuspendMusic();
	void	StopMusic();
	bool	IsPlayingMusic();
	bool	IsStoppedOnDemand ();
	void	AdaptVolumeMusic();
	void	SetSuspendSkip(int nb);
	bool	StopAllSounds();

protected:
	bool				m_bEnable;
	bool				m_bState;
	bool				m_bStopped;
	Mix_Music			*m_pMusic;
	Mix_Chunk			*m_lpSDL[MAXSOUND];
	short				m_channelBlupi[MAXBLUPI];
	char				m_MIDIFilename[50];
	int					m_audioVolume;
	int					m_midiVolume;
	int					m_lastMidiVolume;
	int					m_nbSuspendSkip;
};

/////////////////////////////////////////////////////////////////////////////
