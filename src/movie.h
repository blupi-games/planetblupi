/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2018, Mathieu Schroeter
 * https://epsitec.ch; https://www.blupi.org; https://github.com/blupi-games
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

#include <string>

struct Kit_Source;
struct Kit_Player;
struct Kit_PlayerInfo;
struct SDL_Texture;
class CPixmap;

#define AUDIOBUFFER_SIZE (32768)

class CMovie
{
public:
  CMovie (CPixmap * pixmap);
  ~CMovie ();

  bool Create ();
  bool GetEnable ();
  bool IsExist (const std::string & pFilename);
  bool Play (const std::string & pFilename);
  void Stop ();
  void Pause ();
  void Resume ();
  bool Render ();

protected:
  void playMovie ();
  bool fileOpenMovie (const std::string & pFilename);
  void fileCloseMovie ();
  void termAVI ();
  bool initAVI ();

protected:
  CPixmap *         pixmap;
  Kit_Source *      m_movie;
  Kit_Player *      m_player;
  SDL_Texture *     m_videoTex;
  SDL_Texture *     backTexture;
  Sint32            m_ret;
  SDL_AudioDeviceID m_audioDev;
  SDL_RWops *       rw_ops;

  char m_audiobuf[AUDIOBUFFER_SIZE];
  bool m_bEnable;
  bool starting;
  bool m_fPlaying;   // Play flag: true == playing, false == paused
  bool m_fMovieOpen; // Open flag: true == movie open, false = none
};
