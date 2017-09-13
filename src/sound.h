/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017, Mathieu Schroeter
 * http://epsitec.ch; http://www.blupi.org; http://github.com/blupi-games
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#pragma once

#include <set>
#include <stdio.h>
#include <string>
#include <vector>

#include <SDL2/SDL_mixer.h>

#include "blupi.h"

#define MAXSOUND 100
#define MAXVOLUME 20
#define MAXBLUPI 100

class CSound
{
public:
  CSound ();
  ~CSound ();

  bool Create ();
  void SetState (bool bState);
  bool GetEnable ();

  void   SetAudioVolume (Sint32 volume);
  Sint32 GetAudioVolume ();
  void   SetMidiVolume (Sint32 volume);
  Sint32 GetMidiVolume ();

  void CacheAll ();
  bool Cache (Sint32 channel, const std::string & pFilename);
  void FlushAll ();
  void Flush (Sint32 channel);

  bool Play (
    Sint32 channel, Sint32 volume = 0, Uint8 panLeft = 255,
    Uint8 panRight = 255);
  bool PlayImage (Sounds channel, Point pos, Sint32 rank = -1);
  bool PlayMusic (const std::string & lpszMIDIFilename);
  bool RestartMusic ();
  void SuspendMusic ();
  void StopMusic ();
  bool IsPlayingMusic ();
  bool IsStoppedOnDemand ();
  void AdaptVolumeMusic ();
  void SetSuspendSkip (Sint32 nb);
  bool StopAllSounds (bool immediat, const std::set<Sint32> * except = nullptr);

protected:
  bool                     m_bState;
  bool                     m_bStopped;
  Mix_Music *              m_pMusic;
  Mix_Chunk *              m_lpSDL[MAXSOUND];
  std::vector<std::string> m_sndFiles;
  Sounds                   m_channelBlupi[MAXBLUPI];
  std::string              m_MIDIFilename;
  Sint32                   m_audioVolume;
  Sint32                   m_midiVolume;
  Sint32                   m_lastMidiVolume;
  Sint32                   m_nbSuspendSkip;
};
