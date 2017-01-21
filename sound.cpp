// sound.cpp
//

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




// Creates a DirectSound buffer.

bool CSound::CreateSoundBuffer(int dwBuf, DWORD dwBufSize, DWORD dwFreq, DWORD dwBitsPerSample, DWORD dwBlkAlign, bool bStereo)
{
    PCMWAVEFORMAT	pcmwf;
    DSBUFFERDESC	dsbdesc;
    
    // Set up wave format structure.
    memset( &pcmwf, 0, sizeof(PCMWAVEFORMAT) );
    pcmwf.wf.wFormatTag      = WAVE_FORMAT_PCM;      
    pcmwf.wf.nChannels       = bStereo ? 2 : 1;
    pcmwf.wf.nSamplesPerSec  = dwFreq;
    pcmwf.wf.nBlockAlign     = (WORD)dwBlkAlign;
    pcmwf.wf.nAvgBytesPerSec = pcmwf.wf.nSamplesPerSec * pcmwf.wf.nBlockAlign;
    pcmwf.wBitsPerSample     = (WORD)dwBitsPerSample;

    // Set up DSBUFFERDESC structure.
    memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));  // Zero it out. 
    dsbdesc.dwSize           = sizeof(DSBUFFERDESC);
    dsbdesc.dwFlags          = DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME;
    dsbdesc.dwBufferBytes    = dwBufSize; 
    dsbdesc.lpwfxFormat      = (LPWAVEFORMATEX)&pcmwf;

    TRY_DS(m_lpDS->CreateSoundBuffer(&dsbdesc, &m_lpDSB[dwBuf], NULL))
    return true;
}

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

// Creates a DirectSound buffer from a wave file.

bool CSound::CreateBufferFromWaveFile(int dwBuf, char *pFileName)
{
    // Open the wave file       
    FILE* pFile = fopen(pFileName, "rb");
    if ( pFile == NULL ) return false;

    // Read in the wave header          
    WaveHeader wavHdr;
    if ( fread(&wavHdr, sizeof(wavHdr), 1, pFile) != 1 ) 
    {
        fclose(pFile);
        return NULL;
    }

    // Figure out the size of the data region
    DWORD dwSize = wavHdr.dwDSize;

    // Is this a stereo or mono file?
    bool bStereo = wavHdr.wChnls > 1 ? true : false;

    // Create the sound buffer for the wave file
    if ( !CreateSoundBuffer(dwBuf, dwSize, wavHdr.dwSRate,
							wavHdr.BitsPerSample, wavHdr.wBlkAlign, bStereo) )
    {
        // Close the file
        fclose(pFile);
        
        return false;
    }

    // Read the data for the wave file into the sound buffer
    if ( !ReadData(m_lpDSB[dwBuf], pFile, dwSize, sizeof(wavHdr)) )
    {
        fclose(pFile);
        return false;
    }

    // Close out the wave file
    fclose(pFile);

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

// Plays a sound using direct sound.

bool CSound::PlaySoundDS(DWORD dwSound, DWORD dwFlags)
{
    // Make sure the sound is valid
    if ( dwSound >= MAXSOUND )  return false;    

    // Make sure we have a valid sound buffer
    if ( m_lpDSB[dwSound] )
    {
        DWORD dwStatus;
        TRY_DS(m_lpDSB[dwSound]->GetStatus(&dwStatus));

        if ( (dwStatus & DSBSTATUS_PLAYING) != DSBSTATUS_PLAYING )
        {
            // Play the sound
           TRY_DS(m_lpDSB[dwSound]->Play(0, 0, dwFlags));
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
	if ( !DirectSoundCreate(NULL, &m_lpDS, NULL) == DS_OK )
	{
		OutputDebug("Fatal error: DirectSoundCreate\n");
		m_bEnable = false;
		return false;
	}

	m_lpDS->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
	m_bEnable = true;
	m_hWnd    = hWnd;
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
	
	return CreateBufferFromWaveFile(channel, pFilename);
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
}

// Fait entendre un son.
// Le volume est compris entre 0 (max) et -10000 (silence).
// Le panoramique est compris entre -10000 (gauche), 0 (centre)
// et +10000 (droite).

bool CSound::Play(int channel, int volume, int pan)
{
	if ( !m_bEnable )  return true;
	if ( !m_bState || m_audioVolume == 0 )  return true;

	volume -= (MAXVOLUME-m_audioVolume)*((10000/4)/MAXVOLUME);

//?	if ( volume == -10000 )  return true;
	if ( volume <= -10000/4 )  return true;

	if ( channel < 0 || channel >= MAXSOUND )  return false;
	if ( m_lpDSB[channel] == NULL )            return false;

	m_lpDSB[channel]->SetVolume(volume);
	m_lpDSB[channel]->SetPan(pan);
	m_lpDSB[channel]->Play(0, 0, 0);

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
		if ( stopCh >= 0 && m_lpDSB[stopCh] != NULL )
		{
			m_lpDSB[stopCh]->Stop();  // stoppe le son précédent
			m_lpDSB[stopCh]->SetCurrentPosition(0);
		}

		m_channelBlupi[rank] = channel;
	}

//?	pan = (int)(((long)pos.x*20000L)/LXIMAGE)-10000L;
//?	pan = (int)(((long)pos.x*10000L)/LXIMAGE)-5000L;
	pan = (int)(((long)pos.x*5000L)/LXIMAGE)-2500L;

	volumex = 0;  // volume maximum
	if ( pos.x < 0 )
	{
		volumex = (pos.x*2500)/LXIMAGE;
	}
	if ( pos.x > LXIMAGE )
	{
		pos.x -= LXIMAGE;
		volumex = (-pos.x*2500)/LXIMAGE;
	}
	if ( volumex < -10000 )  volumex = -10000;

	volumey = 0;  // volume maximum
	if ( pos.y < 0 )
	{
		volumey = (pos.y*2500)/LYIMAGE;
	}
	if ( pos.y > LYIMAGE )
	{
		pos.y -= LYIMAGE;
		volumey = (-pos.y*2500)/LYIMAGE;
	}
	if ( volumey < -10000 )  volumey = -10000;

	if ( volumex < volumey )  volume = volumex;
	else                      volume = volumey;

	return Play(channel, volume, pan);
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
							  (DWORD)(LPVOID)&mciOpenParms);
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
	mciPlayParms.dwCallback = (DWORD)hWnd;
	dwReturn = mciSendCommand(m_MidiDeviceID,
							  MCI_PLAY,
							  MCI_NOTIFY, 
							  (DWORD)(LPVOID)&mciPlayParms);
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

