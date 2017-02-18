// sound.cpp
//

#include <stdio.h>
#include "sound.h"
#include "event.h"
#include "misc.h"
#include "def.h"
#include "resource.h"


// Stops all sounds.

bool CSound::StopAllSounds()
{
    for (Sint32 i = 0; i < MAXSOUND; i ++)
    {
        if (!m_lpSDL[i])
            continue;

        if (Mix_Playing (i + 1) == SDL_TRUE)
            Mix_FadeOutChannel (i + 1, 500);
    }

    return true;
}

CSound::CSound()
{
    Sint32      i;

    m_bEnable         = false;
    m_bState          = false;
    m_MIDIFilename[0] = 0;
    m_audioVolume     = 20;
    m_midiVolume      = 15;
    m_lastMidiVolume  = 0;
    m_nbSuspendSkip   = 0;
    m_pMusic          = nullptr;
    m_bStopped        = false;

    for (i = 0 ; i < MAXBLUPI ; i++)
        m_channelBlupi[i] = -1;

    memset (m_lpSDL, 0, sizeof (m_lpSDL));
}

// Destructeur.

CSound::~CSound()
{
    Sint32      i;

    for (i = 0 ; i < MAXSOUND ; i++)
    {
        if (!m_lpSDL[i])
            continue;

        Mix_FreeChunk (m_lpSDL[i]);
        m_lpSDL[i] = nullptr;
    }
}


// Initialisation de DirectSound.

bool CSound::Create()
{
    m_bEnable = true;

    if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1)
        return false;

    Mix_AllocateChannels (MAXSOUND);

    Mix_HookMusicFinished ([]()
    {
        CEvent::PushUserEvent (WM_MUSIC_STOP);
    });

    return true;
}


// Retourne l'ï¿½tat de DirectSound.

bool CSound::GetEnable()
{
    return m_bEnable;
}


// Enclenche ou dï¿½clenche le son.

void CSound::SetState (bool bState)
{
    m_bState = bState;
}


// Gestion des volumes audio (.wav) et midi (.mid).

void CSound::SetAudioVolume (Sint32 volume)
{
    m_audioVolume = volume;
}

Sint32 CSound::GetAudioVolume()
{
    if (!m_bEnable)
        return 0;
    return m_audioVolume;
}

void CSound::SetMidiVolume (Sint32 volume)
{
    m_midiVolume = volume;
}

Sint32 CSound::GetMidiVolume()
{
    if (!m_bEnable)
        return 0;
    return m_midiVolume;
}


// Cache tous les ficheirs son (.wav).

void CSound::CacheAll()
{
    Sint32          i;
    char        name[50];

    if (!m_bEnable)
        return;

    for (i = 0 ; i < MAXSOUND ; i++)
    {
        sprintf (name, "sound%.3d.blp", i);
        if (!Cache (i, name))
            break;
    }
}

// Charge un fichier son (.wav).

bool CSound::Cache (Sint32 channel, const std::string &pFilename)
{
    if (!m_bEnable)
        return false;
    if (channel < 0 || channel >= MAXSOUND)
        return false;

    if (m_lpSDL[channel])
        Flush (channel);

    auto sound = "sound/" + GetLocale () + "/" + pFilename;
    auto file = GetBaseDir() + sound;
    m_lpSDL[channel] = Mix_LoadWAV (file.c_str());
    if (!m_lpSDL[channel])
    {
        if (GetLocale () != "en")
        {
            /* Try with the fallback locale */
            sound = "sound/en/" + pFilename;
            file = GetBaseDir() + sound;
            m_lpSDL[channel] = Mix_LoadWAV (file.c_str());
            if (m_lpSDL[channel])
                goto out;
        }

        SDL_Log ("Mix_LoadWAV: %s\n", Mix_GetError());
        return false;
    }

out:
    SDL_Log ("Load sound: %s\n", sound.c_str ());
    return true;
}

// Dï¿½charge un son.

void CSound::Flush (Sint32 channel)
{
    if (!m_bEnable)
        return;
    if (channel < 0 || channel >= MAXSOUND)
        return;

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

bool CSound::Play (Sint32 channel, Sint32 volume, Uint8 panLeft, Uint8 panRight)
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
// ï¿½ventuellement stopper le dernier son en cours !

bool CSound::PlayImage (Sint32 channel, POINT pos, Sint32 rank)
{
    Sint32 stopCh, volumex, volumey, volume;

    if (rank >= 0 && rank < MAXBLUPI)
    {
        stopCh = m_channelBlupi[rank];
        if (stopCh >= 0 && m_lpSDL[stopCh] != nullptr)
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

    return Play (channel, volume, panLeft, panRight);
}


// Uses MCI to play a MIDI file. The window procedure
// is notified when playback is complete.

bool CSound::PlayMusic (const char *lpszMIDIFilename)
{
    std::string path = GetBaseDir();

    if (!m_bEnable)
        return true;
    if (m_midiVolume == 0)
        return true;

    Mix_VolumeMusic (MIX_MAX_VOLUME * 100 * m_midiVolume / 20 / 100);
    m_lastMidiVolume = m_midiVolume;

    path += lpszMIDIFilename;

    m_pMusic = Mix_LoadMUS (path.c_str());
    if (!m_pMusic)
    {
        printf ("%s\n", Mix_GetError());
        return false;
    }

    if (Mix_PlayMusic (m_pMusic, 0) == -1)
    {
        printf ("%s\n", Mix_GetError());
        return false;
    }

    m_bStopped = false;
    strcpy (m_MIDIFilename, lpszMIDIFilename);
    return true;
}

// Restart the MIDI player.

bool CSound::RestartMusic()
{
    OutputDebug ("RestartMusic\n");
    if (!m_bEnable)
        return true;
    if (m_midiVolume == 0)
        return true;
    if (m_MIDIFilename[0] == 0)
        return false;

    return PlayMusic (m_MIDIFilename);
}

// Shuts down the MIDI player.

void CSound::SuspendMusic()
{
    if (!m_bEnable)
        return;

    if (m_nbSuspendSkip != 0)
    {
        m_nbSuspendSkip --;
        return;
    }

    m_bStopped = true;
    Mix_HaltMusic();
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

bool CSound::IsStoppedOnDemand()
{
    return m_bStopped;
}

// Adapte le volume de la musique en cours, si nï¿½cessaire.

void CSound::AdaptVolumeMusic()
{
    if (m_midiVolume != m_lastMidiVolume)
    {
        Mix_VolumeMusic (MIX_MAX_VOLUME * 100 * m_midiVolume / 20 / 100);
        m_lastMidiVolume = m_midiVolume;
    }
}

// Indique le nombre de suspend ï¿½ sauter.

void CSound::SetSuspendSkip (Sint32 nb)
{
    m_nbSuspendSkip = nb;
}
