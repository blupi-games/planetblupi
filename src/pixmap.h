/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2018, Mathieu Schroeter
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

#include <SDL.h>
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
  std::string   wideName;
  Point         dimTotal;
  Point         dimIcon;

  TextureInfo ()
    : texMask (nullptr)
    , texture (nullptr)
    , target (false)
    , dimTotal{0}
    , dimIcon{0}
  {
  }
};

class CEvent;

class CPixmap
{
public:
  enum Mode {
    FIX = 0,
    FIX_REVERSABLE,
    EXPAND,
    EXPAND_REVERSABLE,
  };

  CPixmap (CEvent * event);
  ~CPixmap ();

  bool Create (Point dim);

  void CreateMainTexture ();
  bool ReloadTargetTextures ();
  bool Cache (size_t channel, Point totalDim);
  bool Cache (
    size_t channel, const std::string & pFilename, Point totalDim,
    Point iconDim, Mode mode = FIX, std::string wideName = "");
  bool Cache (size_t channel, const std::string & pFilename, Point totalDim);
  bool Cache (size_t channel, SDL_Surface * surface, Point totalDim);
  SDL_Texture * getTexture (size_t channel);
  void          SetClipping (Rect clip);
  Rect          GetClipping ();

  bool IsIconPixel (size_t channel, Sint32 rank, Point pos);

  bool DrawIcon (Sint32 chDst, size_t channel, Sint32 rank, Point pos);
  bool DrawIconDemi (Sint32 chDst, size_t channel, Sint32 rank, Point pos);
  bool DrawIconPart (
    Sint32 chDst, size_t channel, Sint32 rank, Point pos, Sint32 startY,
    Sint32 endY);
  bool DrawPart (
    Sint32 chDst, size_t channel, Point dest, Rect rect,
    SDL_RendererFlip flip = SDL_FLIP_NONE);
  bool DrawImage (Sint32 chDst, size_t channel, Rect rect);

  bool BuildIconMask (
    size_t channelMask, Sint32 rankMask, size_t channel, Sint32 rankSrc,
    Sint32 rankDst);

  bool Display ();

  void SetMouseSprite (MouseSprites sprite);
  void MouseShow (bool bShow);
  void LoadCursors ();
  void ChangeSprite (MouseSprites sprite);

public:
  double GetDisplayScale ();
  void   FromDisplayToGame (Sint32 & x, Sint32 & y, double prevScale = 1);
  void   FromGameToDisplay (Sint32 & x, Sint32 & y);

  Sint32 Blit (
    Sint32 dstCh, SDL_Texture * src, const SDL_Rect & dstRect, double angle,
    SDL_RendererFlip flip);

protected:
  Sint32 BltFast (
    Sint32 dstCh, size_t srcCh, Rect dstR, Rect srcR,
    SDL_RendererFlip flip = SDL_FLIP_NONE);
  Sint32 BltFast (
    Sint32 chDst, size_t channel, Point dst, Rect rcRect,
    SDL_RendererFlip flip = SDL_FLIP_NONE);
  Sint32 BltFast (
    SDL_Texture * lpSDL, size_t channel, Point dst, Rect rcRect,
    SDL_BlendMode = SDL_BLENDMODE_BLEND);

  Rect      MouseRectSprite ();
  SDL_Point GetCursorHotSpot (MouseSprites sprite);
  SDL_Rect  GetCursorRect (MouseSprites sprite);

protected:
  bool  m_bDebug;
  bool  m_bPalette;
  Point m_dim;      // dimensions totales
  Rect  m_clipRect; // rectangle de clipping

  MouseSprites m_mouseSprite;
  bool         m_bBackDisplayed;

  CEvent * event;

  SDL_Cursor *  m_lpCurrentCursor;
  SDL_Cursor *  m_lpSDLCursors[MAXCURSORS];
  SDL_Surface * m_lpSDLBlupi;

public:
  SDL_Texture * mainTexture;

protected:
  std::unordered_map<size_t, TextureInfo> m_SDLTextureInfo;
};
