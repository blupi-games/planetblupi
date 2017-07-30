
#pragma once

#include <string>
#include <stdio.h>
#include <SDL2/SDL_mixer.h>

#include "blupi.h"


#define MAXSOUND    100
#define MAXVOLUME   20
#define MAXBLUPI    100

class CSound
{
public:
    CSound();
    ~CSound();

    bool    Create();
    void    SetState (bool bState);
    bool    GetEnable();

    void    SetAudioVolume (Sint32 volume);
    Sint32      GetAudioVolume();
    void    SetMidiVolume (Sint32 volume);
    Sint32      GetMidiVolume();

    void    CacheAll();
    bool    Cache (Sint32 channel, const std::string &pFilename);
    void    FlushAll ();
    void    Flush (Sint32 channel);

    bool    Play (Sint32 channel, Sint32 volume = 0, Uint8 panLeft = 255,
                  Uint8 panRight = 255);
    bool    PlayImage (Sint32 channel, POINT pos, Sint32 rank = -1);
    bool    PlayMusic (const std::string &lpszMIDIFilename);
    bool    RestartMusic();
    void    SuspendMusic();
    void    StopMusic();
    bool    IsPlayingMusic();
    bool    IsStoppedOnDemand();
    void    AdaptVolumeMusic();
    void    SetSuspendSkip (Sint32 nb);
    bool    StopAllSounds(bool immediat);

protected:
    bool                m_bState;
    bool                m_bStopped;
    Mix_Music           *m_pMusic;
    Mix_Chunk           *m_lpSDL[MAXSOUND];
    Sint16              m_channelBlupi[MAXBLUPI];
    std::string         m_MIDIFilename;
    Sint32                  m_audioVolume;
    Sint32                  m_midiVolume;
    Sint32                  m_lastMidiVolume;
    Sint32                  m_nbSuspendSkip;
};

/////////////////////////////////////////////////////////////////////////////
