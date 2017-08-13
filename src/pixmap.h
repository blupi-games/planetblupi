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

#include <SDL2/SDL.h>
#include <string>
#include <unordered_map>

#include "blupi.h"
#include "def.h"

#define MAXCURSORS 14

struct TextureInfo {
  SDL_Texture * texMask;
  SDL_Texture * texture;
  bool          target; // can be used as a render target
  std::string   file;
  POINT         dimTotal;
  POINT         dimIcon;

  TextureInfo ()
    : texMask (nullptr)
    , texture (nullptr)
    , target (false)
    , dimTotal{0}
    , dimIcon{0}
  {
  }
};

class CPixmap
{
public:
  CPixmap ();
  ~CPixmap ();

  bool Create (POINT dim);
  void Fill (RECT rect, COLORREF color);

  bool ReloadTargetTextures ();
  bool Cache (
    size_t channel, const std::string & pFilename, POINT totalDim,
    POINT iconDim);
  bool Cache (size_t channel, const std::string & pFilename, POINT totalDim);
  bool Cache (size_t channel, SDL_Surface * surface, POINT totalDim);
  void SetClipping (RECT clip);
  RECT GetClipping ();

  bool IsIconPixel (size_t channel, Sint32 rank, POINT pos);

  bool DrawIcon (
    Sint32 chDst, size_t channel, Sint32 rank, POINT pos, bool bMask = false);
  bool DrawIconDemi (
    Sint32 chDst, size_t channel, Sint32 rank, POINT pos, bool bMask = false);
  bool DrawIconPart (
    Sint32 chDst, size_t channel, Sint32 rank, POINT pos, Sint32 startY,
    Sint32 endY, bool bMask = false);
  bool DrawPart (
    Sint32 chDst, size_t channel, POINT dest, RECT rect, bool bMask = false);
  bool DrawImage (Sint32 chDst, size_t channel, RECT rect);

  bool BuildIconMask (
    size_t channelMask, Sint32 rankMask, size_t channel, Sint32 rankSrc,
    Sint32 rankDst);

  bool Display ();

  void SetMouseSprite (MouseSprites sprite);
  void MouseShow (bool bShow);
  void LoadCursors ();
  void ChangeSprite (MouseSprites sprite);

protected:
  Sint32 BltFast (Sint32 chDst, size_t channel, POINT dst, RECT rcRect);
  Sint32 BltFast (
    SDL_Texture * lpSDL, size_t channel, POINT dst, RECT rcRect,
    SDL_BlendMode = SDL_BLENDMODE_BLEND);

  RECT      MouseRectSprite ();
  SDL_Point GetCursorHotSpot (Sint32 sprite);
  SDL_Rect  GetCursorRect (Sint32 sprite);

protected:
  bool  m_bDebug;
  bool  m_bPalette;
  POINT m_dim;      // dimensions totales
  RECT  m_clipRect; // rectangle de clipping

  MouseSprites m_mouseSprite;
  bool         m_bBackDisplayed;

  SDL_Cursor *                            m_lpCurrentCursor;
  SDL_Cursor *                            m_lpSDLCursors[MAXCURSORS];
  SDL_Surface *                           m_lpSDLBlupi;
  std::unordered_map<size_t, TextureInfo> m_SDLTextureInfo;
};
