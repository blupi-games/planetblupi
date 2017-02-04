// sound.cpp
//

#include <SDL.h>
#include <dsound.h>
#include <stdio.h>
#include "sound.h"
#include "misc.h"
#include "def.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////



// The following macro are used for proper error handling for DirectSound.
#define TRY_DS(exp) { { HRESULT rval = exp; if (rval != DS_OK) { TraceErrorDS(rval, __FILE__, __LINE__); return false; } } }


struct WaveHeader
{
    BYTE        RIFF[4];          // "RIFF"
    DWORD       dwSize;           // Size of data to follow
    BYTE        WAVE[4];          // "WAVE"
    BYTE        fmt_[4];          // "fmt "
    DWORD       dw16;             // 16
    WORD        wOne_0;           // 1
    WORD        wChnls;           // Number of Channels
    DWORD       dwSRate;          // Sample Rate
    DWORD       BytesPerSec;      // Sample Rate
    WORD        wBlkAlign;        // 1
    WORD        BitsPerSample;    // Sample size
    BYTE        DATA[4];          // "DATA"
    DWORD       dwDSize;          // Number of Samples
};

// Reads in data from a wave file.

bool CSound::ReadData(LPDIRECTSOUNDBUFFER lpDSB, FILE* pFile, DWORD dwSize, DWORD dwPos) 
{
    // Seek to correct position in file (if necessary)
    if ( dwPos != 0xffffffff ) 
    {
        if ( fseek(pFile, dwPos, SEEK_SET) != 0 ) 
        {
            return false;
        }
    }

    // Lock data in buffer for writing
    LPVOID	pData1;
    DWORD	dwData1Size;
    LPVOID	pData2;
    DWORD	dwData2Size;
    HRESULT	rval;

    rval = lpDSB->Lock(0, dwSize, &pData1, &dwData1Size, &pData2, &dwData2Size, DSBLOCK_FROMWRITECURSOR);
    if ( rval != DS_OK )
    {
        return false;
    }

    // Read in first chunk of data
    if ( dwData1Size > 0 ) 
    {
        if ( fread(pData1, dwData1Size, 1, pFile) != 1 ) 
        {          
                        char holder[256];
                        wsprintf(holder,"Data1 : %d, dwdata: %d, pFile: %d",pData1,dwData1Size,pFile);
                        OutputDebug(holder);
            return false;
        }
    }

    // read in second chunk if necessary
    if ( dwData2Size > 0 ) 
    {
        if ( fread(pData2, dwData2Size, 1, pFile) != 1 ) 
        {
            return false;
        }
    }

    // Unlock data in buffer
    rval = lpDSB->Unlock(pData1, dwData1Size, pData2, dwData2Size);
    if ( rval != DS_OK )
    {
        return false;
    }

    return true;
}

// Stops all sounds.

bool CSound::StopAllSounds()
{
    // Make sure we have a valid sound buffer
    for (int i = 0; i < MAXSOUND; i ++)
    {
        if ( m_lpDSB[i] )
        {
            DWORD dwStatus;
            TRY_DS(m_lpDSB[i]->GetStatus(&dwStatus));

            if ( (dwStatus & DSBSTATUS_PLAYING) == DSBSTATUS_PLAYING )
            {
                TRY_DS(m_lpDSB[i]->Stop())
            }
        }
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////


// Modifie le volume midi.
// Le volume est compris entre 0 et 20 !

void InitMidiVolume(int volume)
{
	int				nb, i, n;
	MMRESULT		result;
	HMIDIOUT		hmo = 0;

	static unsigned int table[21] =
	{
		0x00000000,
		0x11111111,
		0x22222222,
		0x33333333,
		0x44444444,
		0x55555555,
		0x66666666,
		0x77777777,
		0x88888888,
		0x99999999,
		0xAAAAAAAA,
		0xBBBBBBBB,
		0xCCCCCCCC,
		0xDDDDDDDD,
		0xEEEEEEEE,
		0xF222F222,
		0xF555F555,
		0xF777F777,
		0xFAAAFAAA,
		0xFDDDFDDD,
		0xFFFFFFFF,
	};

	if ( volume < 0         )  volume = 0;
	if ( volume > MAXVOLUME )  volume = MAXVOLUME;

	nb = midiOutGetNumDevs();
	for ( i=0 ; i<nb ; i++ )
	{
		result = midiOutOpen((LPHMIDIOUT)&hmo, i, 0L, 0L, 0L);
		if ( result != MMSYSERR_NOERROR )
		{
			continue;
		}

		result = midiOutSetVolume(hmo, table[volume]);
		if ( result != MMSYSERR_NOERROR )
		{
			n = 1;
		}
		midiOutClose(hmo);
		hmo = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CSound::CSound()
{
	int		i;
	
	m_bEnable         = false;
	m_bState          = false;
	m_MidiDeviceID    = 0;
	m_MIDIFilename[0] = 0;
	m_audioVolume     = 20;
	m_midiVolume      = 15;
	m_lastMidiVolume  = 0;
	m_nbSuspendSkip   = 0;

	m_lpDS = NULL;

	for ( i=0 ; i<MAXSOUND ; i++ )
	{
		m_lpDSB[i] = NULL;
	}

	for ( i=0 ; i<MAXBLUPI ; i++ )
	{
		m_channelBlupi[i] = -1;
	}
}

// Destructeur.

CSound::~CSound()
{
	int		i;

	if ( m_bEnable )
	{
		InitMidiVolume(15);  // remet un volume moyen !
	}

	for ( i=0 ; i<MAXSOUND ; i++ )
	{
		if ( m_lpDSB[i] != NULL )
		{
//?			m_lpDSB[i]->Release();
			m_lpDSB[i]= NULL;
		}
	}

	if ( m_lpDS != NULL )
	{
		m_lpDS->Release();
		m_lpDS = NULL;
	}
}


// Initialisation de DirectSound.

bool CSound::Create(HWND hWnd)
{
	m_bEnable = true;

	if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1)
		return false;

	Mix_AllocateChannels (MAXSOUND);
	return true;
}


// Retourne l'état de DirectSound.

bool CSound::GetEnable()
{
	return m_bEnable;
}


// Enclenche ou déclenche le son.

void CSound::SetState(bool bState)
{
	m_bState = bState;
}


// Gestion des volumes audio (.wav) et midi (.mid).

void CSound::SetAudioVolume(int volume)
{
	m_audioVolume = volume;
}

int CSound::GetAudioVolume()
{
	if ( !m_bEnable )  return 0;
	return m_audioVolume;
}

void CSound::SetMidiVolume(int volume)
{
	m_midiVolume = volume;
}

int CSound::GetMidiVolume()
{
	if ( !m_bEnable )  return 0;
	return m_midiVolume;
}


// Cache tous les ficheirs son (.wav).

void CSound::CacheAll()
{
	int			i;
	char		name[50];

	if ( !m_bEnable )  return;

	for ( i=0 ; i<MAXSOUND ; i++ )
	{
		sprintf(name, "sound\\sound%.3d.blp", i);
		if ( !Cache(i, name) )  break;
	}
}

// Charge un fichier son (.wav).
	
bool CSound::Cache(int channel, char *pFilename)
{
	if ( !m_bEnable )  return false;
	if ( channel < 0 || channel >= MAXSOUND )  return false;

	if ( m_lpDSB[channel] != NULL )
	{
		Flush(channel);
	}

	m_lpSDL[channel] = Mix_LoadWAV (pFilename);
	if (!m_lpSDL[channel])
	{
		SDL_Log ("Mix_LoadWAV: %s\n", Mix_GetError ());
		return false;
	}

	return true;
}

// Décharge un son.

void CSound::Flush(int channel)
{
	if ( !m_bEnable )  return;
	if ( channel < 0 || channel >= MAXSOUND )  return;

	if ( m_lpDSB[channel] != NULL )
	{
		m_lpDSB[channel]->Release();
		m_lpDSB[channel]= NULL;
	}

	if (m_lpSDL[channel])
	{
		Mix_FreeChunk (m_lpSDL[channel]);
		m_lpSDL[channel] = nullptr;
	}
}

// Fait entendre un son.
// Le volume est compris entre 128 (max) et 0 (silence).
// Le panoramique est compris entre 255,0 (gauche), 127,128 (centre)
// et 0,255 (droite).

bool CSound::Play(int channel, int volume, Uint8 panLeft, Uint8 panRight)
{
	if (!m_bEnable)
		return true;

	if (!m_bState || !m_audioVolume)
		return true;

	if (channel < 0 || channel >= MAXSOUND)
		return false;

	Mix_SetPanning (channel + 1, panLeft, panRight);

	volume = volume * 100 * m_audioVolume / 20 / 100;
	Mix_Volume (channel + 1, volume);

	if (Mix_Playing (channel + 1) == SDL_FALSE)
		Mix_PlayChannel (channel + 1, m_lpSDL[channel], 0);

	return true;
}

// Fait entendre un son dans une image.
// Si rank != -1, il indique le rang du blupi dont il faudra
// éventuellement stopper le dernier son en cours !

bool CSound::PlayImage(int channel, POINT pos, int rank)
{
	int		stopCh, volumex, volumey, volume, pan;

	if ( rank >= 0 && rank < MAXBLUPI )
	{
		stopCh = m_channelBlupi[rank];
		if ( stopCh >= 0 && m_lpSDL[stopCh] != NULL )
			Mix_FadeOutChannel (stopCh + 1, 500);

		m_channelBlupi[rank] = channel;
	}

	Uint8 panRight, panLeft;
	volumex = MIX_MAX_VOLUME;
	volumey = MIX_MAX_VOLUME;

	if (pos.x < 0)
	{
		panRight = 0;
		panLeft  = 255;
		volumex += pos.x;
		if (volumex < 0)
			volumex = 0;
	}
	else if (pos.x > LXIMAGE)
	{
		panRight = 255;
		panLeft  = 0;
		volumex -= pos.x - LXIMAGE;
		if (volumex < 0)
			volumex = 0;
	}
	else
	{
		panRight = 255 * static_cast<Uint16> (pos.x) / LXIMAGE;
		panLeft  = 255 - panRight;
	}

	if (pos.y < 0)
	{
		volumey += pos.y;
		if (volumey < 0)
			volumey = 0;
	}
	else if (pos.y > LYIMAGE)
	{
		volumey -= pos.y - LYIMAGE;
		if (volumey < 0)
			volumey = 0;
	}

	volume = volumex < volumey ? volumex : volumey;

	return Play(channel, volume, panLeft, panRight);
}


// Uses MCI to play a MIDI file. The window procedure
// is notified when playback is complete.

bool CSound::PlayMusic(HWND hWnd, LPSTR lpszMIDIFilename)
{
	MCI_OPEN_PARMS	mciOpenParms;
	MCI_PLAY_PARMS	mciPlayParms;
	DWORD			dwReturn;
	char			string[MAX_PATH];

	if ( !m_bEnable )  return true;
	if ( m_midiVolume == 0 )  return true;
	InitMidiVolume(m_midiVolume);
	m_lastMidiVolume = m_midiVolume;

	if ( lpszMIDIFilename[1] == ':' )  // nom complet "D:\REP..." ?
	{
		strcpy(string, lpszMIDIFilename);
	}
	else
	{
		GetCurrentDir(string, MAX_PATH-30);
		strcat(string, lpszMIDIFilename);
	}

	// Open the device by specifying the device and filename.
	// MCI will attempt to choose the MIDI mapper as the output port.
	mciOpenParms.lpstrDeviceType = "sequencer";
	mciOpenParms.lpstrElementName = string;
	dwReturn = mciSendCommand(NULL,
							  MCI_OPEN,
							  MCI_OPEN_TYPE|MCI_OPEN_ELEMENT,
							  (DWORD_PTR)(LPVOID)&mciOpenParms);
	if ( dwReturn != 0 )
	{
		OutputDebug("PlayMusic-1\n");
		mciGetErrorString(dwReturn, string, 128);
		OutputDebug(string);
		// Failed to open device. Don't close it; just return error.
		return false;
	}

	// The device opened successfully; get the device ID.
	m_MidiDeviceID = mciOpenParms.wDeviceID;

	// Begin playback. 
	mciPlayParms.dwCallback = (DWORD_PTR)hWnd;
	dwReturn = mciSendCommand(m_MidiDeviceID,
							  MCI_PLAY,
							  MCI_NOTIFY, 
							  (DWORD_PTR)(LPVOID)&mciPlayParms);
	if ( dwReturn != 0 )
	{
		OutputDebug("PlayMusic-2\n");
		mciGetErrorString(dwReturn, string, 128);
		OutputDebug(string);
		StopMusic();
		return false;
	}

	strcpy(m_MIDIFilename, lpszMIDIFilename);

	return true;
}

// Restart the MIDI player.

bool CSound::RestartMusic()
{
	OutputDebug("RestartMusic\n");
	if ( !m_bEnable )  return true;
	if ( m_midiVolume == 0 )  return true;
	if ( m_MIDIFilename[0] == 0 )  return false;

	return PlayMusic(m_hWnd, m_MIDIFilename);
}

// Shuts down the MIDI player.

void CSound::SuspendMusic()
{
	if ( !m_bEnable )  return;

	if ( m_nbSuspendSkip != 0 )
	{
		m_nbSuspendSkip --;
		return;
	}

	if ( m_MidiDeviceID && m_midiVolume != 0 )
	{
		mciSendCommand(m_MidiDeviceID, MCI_CLOSE, 0, NULL);
	}
	m_MidiDeviceID = 0;
}

// Shuts down the MIDI player.

void CSound::StopMusic()
{
	SuspendMusic();
	m_MIDIFilename[0] = 0;
}

// Retourne true si une musique est en cours.

bool CSound::IsPlayingMusic()
{
	return (m_MIDIFilename[0] != 0);
}

// Adapte le volume de la musique en cours, si nécessaire.

void CSound::AdaptVolumeMusic()
{
	if ( m_midiVolume != m_lastMidiVolume )
	{
		InitMidiVolume(m_midiVolume);
		m_lastMidiVolume = m_midiVolume;
		RestartMusic();
	}
}

// Indique le nombre de suspend à sauter.

void CSound::SetSuspendSkip(int nb)
{
	m_nbSuspendSkip = nb;
}

