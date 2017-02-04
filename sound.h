// sound.h
//

#pragma once

#include <dsound.h>
#include <stdio.h>
#include <SDL_mixer.h>


/////////////////////////////////////////////////////////////////////////////

#define MAXSOUND	100
#define MAXVOLUME	20
#define MAXBLUPI	100

class CSound
{
public:
	CSound();
	~CSound();

	bool	Create(HWND hWnd);
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
	bool	PlayMusic(HWND hWnd, LPSTR lpszMIDIFilename);
	bool	RestartMusic();
	void	SuspendMusic();
	void	StopMusic();
	bool	IsPlayingMusic();
	void	AdaptVolumeMusic();
	void	SetSuspendSkip(int nb);
	bool	StopAllSounds();

protected:
	HWND				m_hWnd;
	bool				m_bEnable;
	bool				m_bState;
	LPDIRECTSOUND		m_lpDS;
	LPDIRECTSOUNDBUFFER	m_lpDSB[MAXSOUND];
	Mix_Chunk			*m_lpSDL[MAXSOUND];
	short				m_channelBlupi[MAXBLUPI];
	UINT				m_MidiDeviceID;
	char				m_MIDIFilename[50];
	int					m_audioVolume;
	int					m_midiVolume;
	int					m_lastMidiVolume;
	int					m_nbSuspendSkip;
};

/////////////////////////////////////////////////////////////////////////////
