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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kitchensink/kitchensink.h"

#include "blupi.h"
#include "def.h"
#include "event.h"
#include "misc.h"
#include "movie.h"

// Initialize avi libraries.

bool
CMovie::initAVI ()
{
  // Initialize Kitchensink with network support and all formats.
  Sint32 err = Kit_Init (KIT_INIT_FORMATS);
  if (err != 0)
  {
    fprintf (stderr, "Unable to initialize Kitchensink: %s", Kit_GetError ());
    return false;
  }

  return true;
}

// Closes the opened AVI file and the opened device type. |

void
CMovie::termAVI ()
{
  Kit_Quit ();
}

// Close the movie and anything associated with it.                 |
// This function clears the <m_fPlaying> and <m_fMovieOpen> flags   |

void
CMovie::fileCloseMovie ()
{
  m_fPlaying   = false; // can't be playing any longer
  m_fMovieOpen = false; // no more movies open

  if (m_videoTex)
  {
    SDL_DestroyTexture (m_videoTex);
    m_videoTex = nullptr;
  }

  if (m_player)
  {
    SDL_CloseAudioDevice (m_audioDev);
    Kit_ClosePlayer (m_player);
    m_player = nullptr;
  }

  if (m_movie)
  {
    Kit_CloseSource (m_movie);
    m_movie = nullptr;
  }
}

// Open an AVI movie. Use CommDlg open box to
// open and then handle the initialization to
// show the movie and position it properly.  Keep
// the movie paused when opened.
// Sets <m_fMovieOpen> on success.

bool
CMovie::fileOpenMovie (const std::string & pFilename)
{
  const auto path = GetBaseDir () + pFilename;

  // we got a filename, now close any old movie and open the new one. */
  if (m_fMovieOpen)
    fileCloseMovie ();

  // Open up the sourcefile.
  // This can be a local file, network url, ...
  m_movie = Kit_CreateSourceFromUrl (path.c_str ());
  if (m_movie)
  {
    // Create the player
    m_player = Kit_CreatePlayer (m_movie);
    if (m_player == nullptr)
      return false;

    pinfo = new Kit_PlayerInfo;
    Kit_GetPlayerInfo (m_player, pinfo);

    SDL_AudioSpec wanted_spec, audio_spec;

    SDL_memset (&wanted_spec, 0, sizeof (wanted_spec));
    wanted_spec.freq     = pinfo->audio.samplerate;
    wanted_spec.format   = pinfo->audio.format;
    wanted_spec.channels = pinfo->audio.channels;
    m_audioDev = SDL_OpenAudioDevice (nullptr, 0, &wanted_spec, &audio_spec, 0);
    SDL_PauseAudioDevice (m_audioDev, 0);

    m_videoTex = SDL_CreateTexture (
      g_renderer, pinfo->video.format, SDL_TEXTUREACCESS_STATIC,
      pinfo->video.width, pinfo->video.height);
    if (m_videoTex == nullptr)
      return false;

    return true;
  }
  else
  {
    // generic error for open
    m_fMovieOpen = false;
    return false;
  }
}

// Play/pause the movie depending on the state

void
CMovie::playMovie ()
{
  m_fPlaying = !m_fPlaying; // swap the play flag

  // play/pause the AVI movie
  if (m_fPlaying)
    Kit_PlayerPlay (m_player);
  else
    Kit_PlayerPause (m_player);
}

CMovie::CMovie ()
{
  m_bEnable    = false;
  m_fPlaying   = false;
  m_fMovieOpen = false;

  m_movie    = nullptr;
  m_player   = nullptr;
  m_videoTex = nullptr;
  pinfo      = nullptr;

  memset (m_audiobuf, 0, sizeof (m_audiobuf));

  m_ret = 0;
}

CMovie::~CMovie ()
{
  termAVI ();
}

// Ouvre la librairie avi.

bool
CMovie::Create ()
{
  m_bEnable = initAVI ();
  return m_bEnable;
}

// Retourne l'état de DirectMovie.

bool
CMovie::GetEnable ()
{
  return m_bEnable;
}

// Indique si un film existe.

bool
CMovie::IsExist (const std::string & pFilename)
{
  const auto path = GetBaseDir () + pFilename;
  FILE *     file;

  file = fopen (path.c_str (), "rb");
  if (file == nullptr)
    return false;

  fclose (file);
  return true;
}

// Montre un film avi.

bool
CMovie::Play (const std::string & pFilename)
{
  if (!m_bEnable)
    return false;

  if (!fileOpenMovie (pFilename))
    return false;

  playMovie ();
  CEvent::PushUserEvent (EV_MOVIE_PLAY);

  return true;
}

// Stoppe le film avi.

void
CMovie::Stop ()
{
  if (!m_bEnable)
    return;

  fileCloseMovie ();
}

void
CMovie::Pause ()
{
  if (!m_bEnable || !m_fPlaying)
    return;

  if (Kit_GetPlayerState (m_player) != KIT_PLAYING)
    return;

  Kit_PlayerPause (m_player);
}

void
CMovie::Resume ()
{
  if (!m_bEnable || !m_fPlaying)
    return;

  if (Kit_GetPlayerState (m_player) != KIT_PAUSED)
    return;

  Kit_PlayerPlay (m_player);
}

bool
CMovie::Render ()
{
  if (!m_bEnable || !m_fPlaying)
    return false;

  if (Kit_GetPlayerState (m_player) == KIT_STOPPED)
    return false;

  // Refresh audio
  if (SDL_GetQueuedAudioSize (m_audioDev) < AUDIOBUFFER_SIZE)
  {
    Sint32 need = AUDIOBUFFER_SIZE - m_ret;

    SDL_LockAudio ();
    while (need > 0)
    {
      m_ret = Kit_GetAudioData (
        m_player, (unsigned char *) m_audiobuf, AUDIOBUFFER_SIZE,
        (size_t) SDL_GetQueuedAudioSize (m_audioDev));
      need -= m_ret;
      if (m_ret > 0)
        SDL_QueueAudio (m_audioDev, m_audiobuf, m_ret);
      else
        break;
    }
    SDL_UnlockAudio ();
    SDL_PauseAudioDevice (m_audioDev, 0);
  }

  // Clear screen with black
  SDL_SetRenderDrawColor (g_renderer, 0, 0, 0, 255);
  SDL_RenderClear (g_renderer);

  // Refresh videotexture and render it
  Kit_GetVideoData (m_player, m_videoTex);

  SDL_Rect dstRect;
  dstRect.x = (LXIMAGE - LXLOGIC) / 2;
  dstRect.y = 0;
  dstRect.w = LXLOGIC;
  dstRect.h = LYLOGIC;
  SDL_RenderCopy (g_renderer, m_videoTex, nullptr, &dstRect);

  SDL_RenderPresent (g_renderer);
  CEvent::PushUserEvent (EV_MOVIE_PLAY);
  return true;
}
