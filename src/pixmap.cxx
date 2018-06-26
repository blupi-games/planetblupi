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
#include <string>

#ifdef _WIN32
#include <io.h>
#define access _access
#else /* _WIN32 */
#include <unistd.h>
#endif /* !_WIN32 */

#include <SDL2/SDL_image.h>

#include "blupi.h"
#include "def.h"
#include "event.h"
#include "misc.h"
#include "pixmap.h"

/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CPixmap::CPixmap (CEvent * event)
{
  Sint32 i;

  m_bDebug   = true;
  m_bPalette = true;

  m_mouseSprite    = SPRITE_WAIT;
  m_bBackDisplayed = false;

  m_lpSDLBlupi = nullptr;

  for (i = 0; i < MAXCURSORS; i++)
    m_lpSDLCursors[i] = nullptr;

  m_lpCurrentCursor = nullptr;
  this->mainTexture = nullptr;
  this->event       = event;
}

// Destructeur.

CPixmap::~CPixmap ()
{
  unsigned int i;

  for (i = 0; i < countof (m_lpSDLCursors); i++)
  {
    if (m_lpSDLCursors[i])
    {
      SDL_FreeCursor (m_lpSDLCursors[i]);
      m_lpSDLCursors[i] = nullptr;
    }
  }

  for (auto tex : m_SDLTextureInfo)
  {
    if (tex.second.texture)
      SDL_DestroyTexture (tex.second.texture);
    if (tex.second.texMask)
      SDL_DestroyTexture (tex.second.texMask);
  }

  if (m_lpSDLBlupi)
    SDL_FreeSurface (m_lpSDLBlupi);

  if (this->mainTexture)
    SDL_DestroyTexture (this->mainTexture);
}

// Crï¿½e l'objet DirectDraw principal.
// Retourne false en cas d'erreur.

bool
CPixmap::Create (Point dim)
{
  m_dim = dim;

  m_clipRect.left   = 0;
  m_clipRect.top    = 0;
  m_clipRect.right  = dim.x;
  m_clipRect.bottom = dim.y;

  return true;
}

Sint32
CPixmap::BltFast (Sint32 dstCh, size_t srcCh, Rect dstR, Rect srcR)
{
  Sint32 res;

  SDL_Rect srcRect, dstRect;
  srcRect.x = srcR.left;
  srcRect.y = srcR.top;
  srcRect.w = srcR.right - srcR.left;
  srcRect.h = srcR.bottom - srcR.top;
  dstRect.x = dstR.left;
  dstRect.y = dstR.top;
  dstRect.w = dstR.right - dstR.left;
  dstRect.h = dstR.bottom - dstR.top;

  auto target = SDL_GetRenderTarget (g_renderer);

  if (dstCh < 0)
  {
    if (!this->mainTexture && g_bFullScreen && g_zoom == 1)
    {
      SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "best");
      this->mainTexture = SDL_CreateTexture (
        g_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, LXIMAGE,
        LYIMAGE);
      SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    }
    else if (this->mainTexture && !(g_bFullScreen && g_zoom == 1))
    {
      SDL_DestroyTexture (this->mainTexture);
      this->mainTexture = nullptr;
    }

    if (this->mainTexture)
      SDL_SetRenderTarget (g_renderer, target ? target : this->mainTexture);
    res = SDL_RenderCopy (
      g_renderer, m_SDLTextureInfo[srcCh].texture, &srcRect, &dstRect);
    if (this->mainTexture)
      SDL_SetRenderTarget (g_renderer, target);
  }
  else
  {
    SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[dstCh].texture);
    res = SDL_RenderCopy (
      g_renderer, m_SDLTextureInfo[srcCh].texture, &srcRect, &dstRect);
    SDL_SetRenderTarget (g_renderer, target);
  }

  return res;
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

Sint32
CPixmap::BltFast (Sint32 chDst, size_t channel, Point dst, Rect rcRect)
{
  Sint32 limit;

  // Effectue un peu de clipping.
  if (dst.x < m_clipRect.left)
  {
    rcRect.left += m_clipRect.left - dst.x;
    dst.x = m_clipRect.left;
  }
  limit = (m_clipRect.right - dst.x) + rcRect.left;
  if (rcRect.right > limit)
    rcRect.right = limit;
  if (dst.y < m_clipRect.top)
  {
    rcRect.top += m_clipRect.top - dst.y;
    dst.y = m_clipRect.top;
  }
  limit = (m_clipRect.bottom - dst.y) + rcRect.top;
  if (rcRect.bottom > limit)
    rcRect.bottom = limit;

  if (rcRect.left >= rcRect.right || rcRect.top >= rcRect.bottom)
    return 0;

  Rect dstRect;
  dstRect.left   = dst.x;
  dstRect.top    = dst.y;
  dstRect.right  = dstRect.left + rcRect.right - rcRect.left;
  dstRect.bottom = dstRect.top + rcRect.bottom - rcRect.top;
  return this->BltFast (chDst, channel, dstRect, rcRect);
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

Sint32
CPixmap::BltFast (
  SDL_Texture * lpSDL, size_t channel, Point dst, Rect rcRect,
  SDL_BlendMode mode)
{
  Sint32 res;

  SDL_Rect srcRect, dstRect;
  srcRect.x = rcRect.left;
  srcRect.y = rcRect.top;
  srcRect.w = rcRect.right - rcRect.left;
  srcRect.h = rcRect.bottom - rcRect.top;
  dstRect   = srcRect;
  dstRect.x = dst.x;
  dstRect.y = dst.y;

  SDL_BlendMode oMode;
  SDL_GetTextureBlendMode (m_SDLTextureInfo[channel].texture, &oMode);
  if (oMode != mode)
    SDL_SetTextureBlendMode (m_SDLTextureInfo[channel].texture, mode);

  SDL_SetRenderTarget (g_renderer, lpSDL);
  res = SDL_RenderCopy (
    g_renderer, m_SDLTextureInfo[channel].texture, &srcRect, &dstRect);
  SDL_SetRenderTarget (g_renderer, nullptr);

  if (oMode != mode)
    SDL_SetTextureBlendMode (m_SDLTextureInfo[channel].texture, oMode);

  return res;
}

/**
 * \brief Reload textures created with access target flag.
 *
 * \returns true on success.
 */
bool
CPixmap::ReloadTargetTextures ()
{
  if (this->mainTexture)
  {
    SDL_DestroyTexture (this->mainTexture);
    this->mainTexture = nullptr;
  }

  for (auto & tex : m_SDLTextureInfo)
  {
    if (!tex.second.target)
      continue;

    if (tex.second.file.empty ())
    {
      if (!Cache (tex.first, tex.second.dimTotal))
        return false;
    }
    else if (!Cache (
               tex.first, tex.second.file, tex.second.dimTotal,
               tex.second.dimIcon))
      return false;
  }

  return true;
}

bool
CPixmap::Cache (size_t channel, Point totalDim)
{
  if (m_SDLTextureInfo.find (channel) == m_SDLTextureInfo.end ())
  {
    m_SDLTextureInfo[channel].texture = SDL_CreateTexture (
      g_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, totalDim.x,
      totalDim.y);

    if (!m_SDLTextureInfo[channel].texture)
    {
      SDL_LogError (
        SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture: %s",
        SDL_GetError ());
      return false;
    }

    SDL_SetTextureBlendMode (
      m_SDLTextureInfo[channel].texture, SDL_BLENDMODE_BLEND);
  }
  else
  {
    SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[channel].texture);
    SDL_SetRenderDrawColor (g_renderer, 0, 0, 0, 0);
    SDL_RenderClear (g_renderer);
    SDL_SetRenderTarget (g_renderer, nullptr);
  }

  m_SDLTextureInfo[channel].texMask  = nullptr;
  m_SDLTextureInfo[channel].target   = true;
  m_SDLTextureInfo[channel].dimIcon  = Point{0, 0};
  m_SDLTextureInfo[channel].dimTotal = totalDim;
  m_SDLTextureInfo[channel].file     = "";

  return true;
}

// Cache une image contenant des icï¿½nes.

bool
CPixmap::Cache (
  size_t channel, const std::string & pFilename, Point totalDim, Point iconDim,
  Mode mode, size_t chBackWide)
{
  std::string   file       = GetBaseDir () + pFilename;
  SDL_Surface * surface    = IMG_Load (file.c_str ());
  bool          blupiChSet = false;

  if (channel == CHBLUPI && !m_lpSDLBlupi)
  {
    m_lpSDLBlupi = surface;
    blupiChSet   = true;
  }

  SDL_Texture * texture = SDL_CreateTextureFromSurface (g_renderer, surface);
  Uint32        format;
  Sint32        access, ow, w, oh, h;
  SDL_QueryTexture (texture, &format, &access, &ow, &oh);

  auto m = mode == EXPAND || channel == CHBACK;
  w      = m ? LXIMAGE : ow;
  h      = m ? LYIMAGE : oh;

  if (m_SDLTextureInfo.find (channel) == m_SDLTextureInfo.end ())
  {
    m_SDLTextureInfo[channel].texture = SDL_CreateTexture (
      g_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);

    if (!m_SDLTextureInfo[channel].texture)
    {
      SDL_LogError (
        SDL_LOG_CATEGORY_APPLICATION,
        "Couldn't create texture from surface: %s", SDL_GetError ());
      return false;
    }

    if (channel != CHBACK)
      SDL_SetTextureBlendMode (
        m_SDLTextureInfo[channel].texture, SDL_BLENDMODE_BLEND);
  }

  SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[channel].texture);
  SDL_SetRenderDrawColor (g_renderer, 0, 0, 0, 0);
  SDL_RenderClear (g_renderer);
  SDL_SetRenderTarget (g_renderer, nullptr);

  m_SDLTextureInfo[channel].texMask  = channel == CHMASK2 ? texture : nullptr;
  m_SDLTextureInfo[channel].target   = true;
  m_SDLTextureInfo[channel].dimIcon  = iconDim;
  m_SDLTextureInfo[channel].dimTotal = totalDim;
  m_SDLTextureInfo[channel].file     = pFilename;

  SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[channel].texture);

  switch (mode)
  {
  case FIX:
  {
    if (channel == CHBACK && (ow < LXIMAGE || oh < LYIMAGE))
    {
      if (chBackWide > 0)
      {
        Rect srcRect;
        srcRect.left   = 0;
        srcRect.right  = LXIMAGE;
        srcRect.top    = 0;
        srcRect.bottom = LYIMAGE;
        this->DrawImage (-1, chBackWide, srcRect);
      }

      SDL_Rect dst;
      dst.x = (LXIMAGE - ow) / 2;
      dst.y = 0;
      dst.w = ow;
      dst.h = oh;
      SDL_RenderCopy (g_renderer, texture, nullptr, &dst);
    }
    else
      SDL_RenderCopy (g_renderer, texture, nullptr, nullptr);
    break;
  }

  case EXPAND:
  {
    SDL_Rect src, dst;
    src.x = 0;
    src.y = 0;
    src.w = POSDRAWX - 1;
    src.h = LYIMAGE;
    dst   = src;
    SDL_RenderCopy (g_renderer, texture, &src, &dst);
    src.x = ow - 16;
    src.w = 16;
    dst.x = LXIMAGE - 16;
    dst.w = src.w;
    SDL_RenderCopy (g_renderer, texture, &src, &dst);
    src.x = POSDRAWX - 1;
    src.w = ow - src.x - 16;
    dst.x = src.x;
    dst.w = DIMDRAWX + 1;
    SDL_RenderCopy (g_renderer, texture, &src, &dst);
    break;
  }
  }

  SDL_SetRenderTarget (g_renderer, nullptr);

  if (!m_SDLTextureInfo[channel].texMask)
    SDL_DestroyTexture (texture);

  if (!blupiChSet)
    SDL_FreeSurface (surface);

  return true;
}

// Cache une image globale.

bool
CPixmap::Cache (size_t channel, const std::string & pFilename, Point totalDim)
{
  Point iconDim;

  iconDim.x = 0;
  iconDim.y = 0;

  return Cache (channel, pFilename, totalDim, iconDim);
}

// Cache une image provenant d'un bitmap.

bool
CPixmap::Cache (size_t channel, SDL_Surface * surface, Point totalDim)
{
  // Create the offscreen surface, by loading our bitmap.
  if (
    m_SDLTextureInfo.find (channel) != m_SDLTextureInfo.end () &&
    m_SDLTextureInfo[channel].texture)
    SDL_DestroyTexture (m_SDLTextureInfo[channel].texture);

  m_SDLTextureInfo[channel].texture =
    SDL_CreateTextureFromSurface (g_renderer, surface);
  m_SDLTextureInfo[channel].target = false;

  if (!m_SDLTextureInfo[channel].texture)
    return false;

  m_SDLTextureInfo[channel].dimTotal = totalDim;
  m_SDLTextureInfo[channel].dimIcon  = totalDim;
  return true;
}

// Modifie la rï¿½gion de clipping.

void
CPixmap::SetClipping (Rect clip)
{
  m_clipRect = clip;
}

// Retourne la rï¿½gion de clipping.

Rect
CPixmap::GetClipping ()
{
  return m_clipRect;
}

// Teste si un point fait partie d'une icï¿½ne.

bool
CPixmap::IsIconPixel (size_t channel, Sint32 rank, Point pos)
{
  Sint32 nbx, nby;

  auto texInfo = m_SDLTextureInfo.find (channel);
  if (texInfo == m_SDLTextureInfo.end ())
    return false;

  if (texInfo->second.dimIcon.x == 0 || texInfo->second.dimIcon.y == 0)
    return false;

  nbx = texInfo->second.dimTotal.x / texInfo->second.dimIcon.x;
  nby = texInfo->second.dimTotal.y / texInfo->second.dimIcon.y;

  if (rank < 0 || rank >= nbx * nby)
    return false;

  pos.x += (rank % nbx) * texInfo->second.dimIcon.x;
  pos.y += (rank / nbx) * texInfo->second.dimIcon.y;

  SDL_Rect rect;
  rect.x       = pos.x;
  rect.y       = pos.y;
  rect.w       = 1;
  rect.h       = 1;
  Uint32 pixel = 0;

  SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[channel].texture);
  SDL_RenderReadPixels (g_renderer, &rect, 0, &pixel, 4);
  SDL_SetRenderTarget (g_renderer, nullptr);

  return !!pixel;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

bool
CPixmap::DrawIcon (Sint32 chDst, size_t channel, Sint32 rank, Point pos)
{
  Sint32 nbx, nby;
  Rect   rect;

  auto texInfo = m_SDLTextureInfo.find (channel);
  if (channel != CHMAP && texInfo == m_SDLTextureInfo.end ())
    return false;

  if (texInfo->second.dimIcon.x == 0 || texInfo->second.dimIcon.y == 0)
    return false;

  nbx = texInfo->second.dimTotal.x / texInfo->second.dimIcon.x;
  nby = texInfo->second.dimTotal.y / texInfo->second.dimIcon.y;

  if (rank < 0 || rank >= nbx * nby)
    return false;

  rect.left   = (rank % nbx) * texInfo->second.dimIcon.x;
  rect.top    = (rank / nbx) * texInfo->second.dimIcon.y;
  rect.right  = rect.left + texInfo->second.dimIcon.x;
  rect.bottom = rect.top + texInfo->second.dimIcon.y;

  return !BltFast (chDst, channel, pos, rect);
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.
//
// Correspondances in,out :
//   0,0     2,1    ...
//   1,16    3,17
//
//  32,32   34,33
//  33,48   35,49

bool
CPixmap::DrawIconDemi (Sint32 chDst, size_t channel, Sint32 rank, Point pos)
{
  Sint32 nbx, nby;
  Rect   rect;

  auto texInfo = m_SDLTextureInfo.find (channel);
  if (texInfo == m_SDLTextureInfo.end ())
    return false;

  if (texInfo->second.dimIcon.x == 0 || texInfo->second.dimIcon.y == 0)
    return false;

  nbx = texInfo->second.dimTotal.x / texInfo->second.dimIcon.x;
  nby = texInfo->second.dimTotal.y / (texInfo->second.dimIcon.y / 2);

  rank = (rank / 32) * 32 + ((rank % 32) / 2) + ((rank % 2) * 16);

  if (rank < 0 || rank >= nbx * nby)
    return false;

  rect.left   = (rank % nbx) * texInfo->second.dimIcon.x;
  rect.top    = (rank / nbx) * (texInfo->second.dimIcon.y / 2);
  rect.right  = rect.left + texInfo->second.dimIcon.x;
  rect.bottom = rect.top + (texInfo->second.dimIcon.y / 2);

  return !BltFast (chDst, channel, pos, rect);
}

// Dessine une partie d'image rectangulaire.

bool
CPixmap::DrawIconPart (
  Sint32 chDst, size_t channel, Sint32 rank, Point pos, Sint32 startY,
  Sint32 endY)
{
  Sint32 nbx, nby;
  Rect   rect;

  auto texInfo = m_SDLTextureInfo.find (channel);
  if (texInfo == m_SDLTextureInfo.end ())
    return false;

  if (texInfo->second.dimIcon.x == 0 || texInfo->second.dimIcon.y == 0)
    return false;

  nbx = texInfo->second.dimTotal.x / texInfo->second.dimIcon.x;
  nby = texInfo->second.dimTotal.y / texInfo->second.dimIcon.y;

  if (rank < 0 || rank >= nbx * nby)
    return false;

  rect.left   = (rank % nbx) * texInfo->second.dimIcon.x;
  rect.top    = (rank / nbx) * texInfo->second.dimIcon.y;
  rect.right  = rect.left + texInfo->second.dimIcon.x;
  rect.bottom = rect.top + endY;

  pos.y += startY;
  rect.top += startY;

  return !BltFast (chDst, channel, pos, rect);
}

// Dessine une partie d'image n'importe oï¿½.

bool
CPixmap::DrawPart (Sint32 chDst, size_t channel, Point dest, Rect rect)
{
  if (m_SDLTextureInfo.find (channel) == m_SDLTextureInfo.end ())
    return false;

  return !BltFast (chDst, channel, dest, rect);
}

// Dessine une partie d'image rectangulaire.

bool
CPixmap::DrawImage (Sint32 chDst, size_t channel, Rect rect)
{
  Point  dst;
  Sint32 res;

  if (m_SDLTextureInfo.find (channel) == m_SDLTextureInfo.end ())
    return false;

  dst.x = rect.left;
  dst.y = rect.top;

  res = BltFast (chDst, channel, dst, rect);

  if (res)
    return false;

  if (channel == CHBACK)
    m_bBackDisplayed = false;

  return true;
}

// Construit une icï¿½ne en utilisant un masque.

bool
CPixmap::BuildIconMask (
  size_t channelMask, Sint32 rankMask, size_t channel, Sint32 rankSrc,
  Sint32 rankDst)
{
  Sint32 nbx, nby;
  Point  posDst, posDstMask;
  Rect   rect, rectMask;
  Sint32 res;

  auto texInfo = m_SDLTextureInfo.find (channel);
  if (texInfo == m_SDLTextureInfo.end ())
    return false;

  if (texInfo->second.dimIcon.x == 0 || texInfo->second.dimIcon.y == 0)
    return false;

  nbx = texInfo->second.dimTotal.x / texInfo->second.dimIcon.x;
  nby = texInfo->second.dimTotal.y / texInfo->second.dimIcon.y;

  if (rankSrc < 0 || rankSrc >= nbx * nby)
    return false;
  if (rankDst < 0 || rankDst >= nbx * nby)
    return false;

  rect.left   = (rankSrc % nbx) * texInfo->second.dimIcon.x;
  rect.top    = (rankSrc / nbx) * texInfo->second.dimIcon.y;
  rect.right  = rect.left + texInfo->second.dimIcon.x;
  rect.bottom = rect.top + texInfo->second.dimIcon.y;
  posDst.x    = (rankDst % nbx) * texInfo->second.dimIcon.x;
  posDst.y    = (rankDst / nbx) * texInfo->second.dimIcon.y;

  res = BltFast (m_SDLTextureInfo[channel].texture, channel, posDst, rect);
  if (res)
    return false;

  auto texMaskInfo = m_SDLTextureInfo.find (channelMask);

  if (texMaskInfo->second.dimIcon.x == 0 || texMaskInfo->second.dimIcon.y == 0)
    return false;

  nbx = texMaskInfo->second.dimTotal.x / texMaskInfo->second.dimIcon.x;
  nby = texMaskInfo->second.dimTotal.y / texMaskInfo->second.dimIcon.y;

  /* Support only CHMASK1 (white) because it needs CHMASK2 (black) (hardcoded
   * here) */
  if (rankMask < 0 || rankMask >= nbx * nby || channelMask != CHMASK1)
    return false;

  rectMask.left   = (rankMask % nbx) * texMaskInfo->second.dimIcon.x;
  rectMask.top    = (rankMask / nbx) * texMaskInfo->second.dimIcon.y;
  rectMask.right  = rectMask.left + texMaskInfo->second.dimIcon.x;
  rectMask.bottom = rectMask.top + texMaskInfo->second.dimIcon.y;

  /* Multiply the white mask with the texture (no alpha) */
  res = BltFast (
    m_SDLTextureInfo[channel].texture, channelMask, posDst, rectMask,
    SDL_BLENDMODE_MOD);

  posDstMask.x = (rankMask % nbx) * texMaskInfo->second.dimIcon.x;
  posDstMask.y = (rankMask / nbx) * texMaskInfo->second.dimIcon.y;
  /* Addition the previous texture with the black mask (alpha retrieved) */
  res = BltFast (
    m_SDLTextureInfo[CHMASK2].texture, channel, posDstMask, rect,
    SDL_BLENDMODE_ADD);

  /* Blit the altered mask in the final texture */
  res = BltFast (
    m_SDLTextureInfo[channel].texture, CHMASK2, posDst, rectMask,
    SDL_BLENDMODE_NONE);

  /* Restore the black mask for the next iteration. */
  SDL_SetRenderTarget (g_renderer, m_SDLTextureInfo[CHMASK2].texture);
  SDL_RenderCopy (
    g_renderer, m_SDLTextureInfo[CHMASK2].texMask, nullptr, nullptr);
  SDL_SetRenderTarget (g_renderer, nullptr);

  return !res;
}

// Affiche le pixmap ï¿½ l'ï¿½cran.
// Retourne false en cas d'erreur.

bool
CPixmap::Display ()
{
  m_bBackDisplayed = true;

  if (this->mainTexture)
    SDL_RenderCopy (g_renderer, this->mainTexture, nullptr, nullptr);

  SDL_RenderPresent (g_renderer);
  return true;
}

// Change le lutin de la souris.

void
CPixmap::SetMouseSprite (MouseSprites sprite)
{
  if (m_mouseSprite == sprite)
    return;

  m_mouseSprite = sprite;

  SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
}

// Montre ou cache la souris.

void
CPixmap::MouseShow (bool bShow)
{
  SDL_ShowCursor (bShow);
}

// Retourne le rectangle correspondant au sprite
// de la souris dans CHBLUPI.

Rect
CPixmap::MouseRectSprite ()
{
  Sint32 rank, nbx;
  Rect   rcRect;

  rank = 348;
  if (m_mouseSprite == SPRITE_ARROW)
    rank = 348;
  if (m_mouseSprite == SPRITE_POINTER)
    rank = 349;
  if (m_mouseSprite == SPRITE_MAP)
    rank = 350;
  if (m_mouseSprite == SPRITE_WAIT)
    rank = 351;
  if (m_mouseSprite == SPRITE_FILL)
    rank = 352;
  if (m_mouseSprite == SPRITE_ARROWL)
    rank = 353;
  if (m_mouseSprite == SPRITE_ARROWR)
    rank = 354;
  if (m_mouseSprite == SPRITE_ARROWU)
    rank = 355;
  if (m_mouseSprite == SPRITE_ARROWD)
    rank = 356;
  if (m_mouseSprite == SPRITE_ARROWDL)
    rank = 357;
  if (m_mouseSprite == SPRITE_ARROWDR)
    rank = 358;
  if (m_mouseSprite == SPRITE_ARROWUL)
    rank = 359;
  if (m_mouseSprite == SPRITE_ARROWUR)
    rank = 360;

  auto texMaskInfo = m_SDLTextureInfo.find (CHBLUPI);

  nbx = texMaskInfo->second.dimTotal.x / texMaskInfo->second.dimIcon.x;

  rcRect.left   = (rank % nbx) * texMaskInfo->second.dimIcon.x;
  rcRect.top    = (rank / nbx) * texMaskInfo->second.dimIcon.y;
  rcRect.right  = rcRect.left + texMaskInfo->second.dimIcon.x;
  rcRect.bottom = rcRect.top + texMaskInfo->second.dimIcon.y;

  return rcRect;
}

SDL_Point
CPixmap::GetCursorHotSpot (MouseSprites sprite)
{
  static const Sint32 hotspots[MAXCURSORS * 2] = {
    30, 30, // SPRITE_ARROW
    20, 15, // SPRITE_POINTER
    31, 26, // SPRITE_MAP
    25, 14, // SPRITE_ARROWU
    24, 35, // SPRITE_ARROWD
    15, 24, // SPRITE_ARROWL
    35, 24, // SPRITE_ARROWR
    18, 16, // SPRITE_ARROWUL
    32, 18, // SPRITE_ARROWUR
    17, 30, // SPRITE_ARROWDL
    32, 32, // SPRITE_ARROWDR
    30, 30, // SPRITE_WAIT
    30, 30, // SPRITE_EMPTY
    21, 51, // SPRITE_FILL
  };

  SDL_Point hotspot = {0, 0};

  if (sprite >= SPRITE_BEGIN && sprite <= SPRITE_END)
  {
    const Sint32 rank = sprite - SPRITE_BEGIN; // rank <- 0..n

    hotspot.x = hotspots[rank * 2 + 0];
    hotspot.y = hotspots[rank * 2 + 1];
  }

  return hotspot;
}

SDL_Rect
CPixmap::GetCursorRect (MouseSprites sprite)
{
  Sint32   rank;
  SDL_Rect rcRect;

  switch (sprite)
  {
  default:
  case SPRITE_ARROW:
    rank = 348;
    break;
  case SPRITE_POINTER:
    rank = 349;
    break;
  case SPRITE_MAP:
    rank = 350;
    break;
  case SPRITE_WAIT:
    rank = 351;
    break;
  case SPRITE_FILL:
    rank = 352;
    break;
  case SPRITE_ARROWL:
    rank = 353;
    break;
  case SPRITE_ARROWR:
    rank = 354;
    break;
  case SPRITE_ARROWU:
    rank = 355;
    break;
  case SPRITE_ARROWD:
    rank = 356;
    break;
  case SPRITE_ARROWDL:
    rank = 357;
    break;
  case SPRITE_ARROWDR:
    rank = 358;
    break;
  case SPRITE_ARROWUL:
    rank = 359;
    break;
  case SPRITE_ARROWUR:
    rank = 360;
    break;
  }

  auto texMaskInfo = m_SDLTextureInfo.find (CHBLUPI);

  Sint32 nbx = texMaskInfo->second.dimTotal.x / texMaskInfo->second.dimIcon.x;

  rcRect.x = (rank % nbx) * texMaskInfo->second.dimIcon.x;
  rcRect.y = (rank / nbx) * texMaskInfo->second.dimIcon.y;
  rcRect.w = texMaskInfo->second.dimIcon.x;
  rcRect.h = texMaskInfo->second.dimIcon.y;

  return rcRect;
}

void
CPixmap::LoadCursors ()
{
  Uint32 rmask, gmask, bmask, amask;

/* SDL interprets each pixel as a 32-bit number, so our masks must depend
on the endianness (byte order) of the machine */
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  rmask = 0xff000000;
  gmask = 0x00ff0000;
  bmask = 0x0000ff00;
  amask = 0x000000ff;
#else
  rmask = 0x000000ff;
  gmask = 0x0000ff00;
  bmask = 0x00ff0000;
  amask = 0xff000000;
#endif

  auto scale = this->GetDisplayScale ();

  for (int i = SPRITE_BEGIN; i <= SPRITE_END; ++i)
  {
    MouseSprites sprite = static_cast<MouseSprites> (i);

    if (m_lpSDLCursors[sprite - 1])
      SDL_FreeCursor (m_lpSDLCursors[sprite - 1]);

    SDL_Point hotspot = this->GetCursorHotSpot (sprite);
    SDL_Rect  rect    = this->GetCursorRect (sprite);

    SDL_Surface * surface = SDL_CreateRGBSurface (
      0, rect.w * scale, rect.h * scale, 32, rmask, gmask, bmask, amask);
    SDL_BlitScaled (m_lpSDLBlupi, &rect, surface, nullptr);

    // FIXME: change cursor first value to 0
    m_lpSDLCursors[sprite - 1] =
      SDL_CreateColorCursor (surface, hotspot.x * scale, hotspot.y * scale);
    SDL_FreeSurface (surface);
  }
}

void
CPixmap::ChangeSprite (MouseSprites sprite)
{
  if (m_lpCurrentCursor == m_lpSDLCursors[sprite - 1])
    return;

  SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
  m_lpCurrentCursor = m_lpSDLCursors[sprite - 1];
}

double
CPixmap::GetDisplayScale ()
{
  // SDL_DisplayMode displayMode;
  // SDL_GetWindowDisplayMode (g_window, &displayMode);
  Sint32 w, h;
  SDL_GetWindowSize (g_window, &w, &h);
  return static_cast<double> (h / LYIMAGE);
}

void
CPixmap::FromDisplayToGame (Sint32 & x, Sint32 & y, double prevScale)
{
  if (this->event->IsDemoPlaying ())
    return;

  Sint32 w, h;
  SDL_GetWindowSize (g_window, &w, &h);

  double factor = 1;

  if (!g_bFullScreen)
    factor = prevScale;

  x /= factor;
  y /= factor;

  if (!g_bFullScreen)
    return;
}

void
CPixmap::FromGameToDisplay (Sint32 & x, Sint32 & y)
{
  Sint32 w, h;
  SDL_GetWindowSize (g_window, &w, &h);

  double factor = 1;

  if (!g_bFullScreen)
    factor = g_zoom;

  x *= factor;
  y *= factor;

  if (!g_bFullScreen)
    return;

  double _w = w, _h = h;

  x = x * _w / LXIMAGE;
  y = y * _h / LYIMAGE;
}
