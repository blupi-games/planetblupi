// CPixmap.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include <string>

#ifdef _WIN32
#include <io.h>
#define access _access
#else /* _WIN32 */
#include <unistd.h>
#endif /* !_WIN32 */

#include <SDL2/SDL_image.h>

#include "def.h"
#include "pixmap.h"
#include "misc.h"
#include "blupi.h"



/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CPixmap::CPixmap()
{
    Sint32      i;

    m_bFullScreen  = false;
    m_mouseType    = MOUSETYPEGRA;
    m_bDebug       = true;
    m_bPalette     = true;

    m_mouseSprite  = SPRITE_WAIT;
    m_bBackDisplayed = false;

    for (i = 0; i < MAXCURSORS; i++)
        m_lpSDLCursors[i] = nullptr;

    for (i = 0 ; i < MAXIMAGE ; i++)
        m_lpSDLTexture[i] = nullptr;

    m_lpCurrentCursor = nullptr;
}

// Destructeur.

CPixmap::~CPixmap()
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

    for (i = 0 ; i < countof (m_lpSDLTexture) ; i++)
    {
        if (m_lpSDLTexture[i] != nullptr)
        {
            SDL_DestroyTexture (m_lpSDLTexture[i]);
            m_lpSDLTexture[i] = nullptr;
        }
    }
}

// Crï¿½e l'objet DirectDraw principal.
// Retourne false en cas d'erreur.

bool CPixmap::Create (POINT dim,
                      bool bFullScreen, Sint32 mouseType)
{
    m_bFullScreen = bFullScreen;
    m_mouseType   = mouseType;
    m_dim         = dim;

    m_clipRect.left   = 0;
    m_clipRect.top    = 0;
    m_clipRect.right  = dim.x;
    m_clipRect.bottom = dim.y;

    return true;
}

// Libï¿½re les bitmaps.

bool CPixmap::Flush()
{
    return true;
}

// Rempli une zone rectangulaire avec une couleur uniforme.

void CPixmap::Fill (RECT rect, COLORREF color)
{
    // ï¿½ faire si nï¿½cessaire ...
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

Sint32 CPixmap::BltFast (Sint32 chDst, Sint32 channel, POINT dst, RECT rcRect)
{
    Sint32          res, limit;

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

    if (rcRect.left >= rcRect.right ||
        rcRect.top  >= rcRect.bottom)
        return 0;

    if (chDst < 0)
    {
        SDL_Rect srcRect, dstRect;
        srcRect.x = rcRect.left;
        srcRect.y = rcRect.top;
        srcRect.w = rcRect.right - rcRect.left;
        srcRect.h = rcRect.bottom - rcRect.top;
        dstRect = srcRect;
        dstRect.x = dst.x;
        dstRect.y = dst.y;

        res = SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
    }
    else
    {
        SDL_Rect srcRect, dstRect;
        srcRect.x = rcRect.left;
        srcRect.y = rcRect.top;
        srcRect.w = rcRect.right - rcRect.left;
        srcRect.h = rcRect.bottom - rcRect.top;
        dstRect = srcRect;
        dstRect.x = dst.x;
        dstRect.y = dst.y;

        SDL_SetRenderTarget (g_renderer, m_lpSDLTexture[chDst]);
        res = SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
        SDL_SetRenderTarget (g_renderer, nullptr);
    }

    return res;
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

Sint32 CPixmap::BltFast (SDL_Texture *lpSDL, Sint32 channel, POINT dst,
                         RECT rcRect)
{
    Sint32          res;

    SDL_Rect srcRect, dstRect;
    srcRect.x = rcRect.left;
    srcRect.y = rcRect.top;
    srcRect.w = rcRect.right - rcRect.left;
    srcRect.h = rcRect.bottom - rcRect.top;
    dstRect = srcRect;
    dstRect.x = dst.x;
    dstRect.y = dst.y;

    SDL_SetRenderTarget (g_renderer, lpSDL);
    res = SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
    SDL_SetRenderTarget (g_renderer, nullptr);

    return res;
}

// Cache une image contenant des icï¿½nes.

bool CPixmap::Cache (Sint32 channel, const char *pFilename, POINT totalDim,
                     POINT iconDim)
{
    if (channel < 0 || channel >= MAXIMAGE)
        return false;

    std::string file = GetBaseDir() + pFilename;
    SDL_Surface *surface = IMG_Load (file.c_str());

    if (channel == CHBLUPI)
        m_lpSDLBlupi = surface;

    SDL_Texture *texture = SDL_CreateTextureFromSurface (g_renderer, surface);
    Uint32 format;
    Sint32 access, w, h;
    SDL_QueryTexture (texture, &format, &access, &w, &h);

    if (!m_lpSDLTexture[channel])
    {
        m_lpSDLTexture[channel] = SDL_CreateTexture (g_renderer, SDL_PIXELFORMAT_RGBA32,
                                  SDL_TEXTUREACCESS_TARGET, w, h);
        SDL_SetTextureBlendMode (m_lpSDLTexture[channel], SDL_BLENDMODE_BLEND);
    }
    else
    {
        SDL_SetRenderTarget (g_renderer, m_lpSDLTexture[channel]);
        SDL_SetRenderDrawColor (g_renderer, 0, 0, 0, 0);
        SDL_RenderClear (g_renderer);
        SDL_SetRenderTarget (g_renderer, nullptr);
    }

    SDL_SetRenderTarget (g_renderer, m_lpSDLTexture[channel]);
    SDL_RenderCopy (g_renderer, texture, nullptr, nullptr);
    SDL_SetRenderTarget (g_renderer, nullptr);

    SDL_DestroyTexture (texture);

    //m_lpSDLTexture[channel] = SDL_CreateTextureFromSurface (g_renderer, surface);
    if (!m_lpSDLTexture[channel])
    {
        SDL_LogError (SDL_LOG_CATEGORY_APPLICATION,
                      "Couldn't create texture from surface: %s", SDL_GetError());
        return false;
    }

    if (channel != CHBLUPI)
        SDL_FreeSurface (surface);

    strcpy (m_filename[channel], pFilename);

    m_totalDim[channel] = totalDim;
    m_iconDim[channel]  = iconDim;

    return true;
}

// Cache une image globale.

bool CPixmap::Cache (Sint32 channel, const char *pFilename, POINT totalDim)
{
    POINT       iconDim;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;

    iconDim.x = 0;
    iconDim.y = 0;

    return Cache (channel, pFilename, totalDim, iconDim);
}

// Cache une image provenant d'un bitmap.

bool CPixmap::Cache (Sint32 channel, SDL_Surface *surface, POINT totalDim)
{
    if (channel < 0 || channel >= MAXIMAGE)
        return false;

    // Create the offscreen surface, by loading our bitmap.
    if (m_lpSDLTexture[channel])
        SDL_DestroyTexture (m_lpSDLTexture[channel]);

    m_lpSDLTexture[channel] = SDL_CreateTextureFromSurface (g_renderer, surface);

    if (!m_lpSDLTexture[channel])
        return false;

    m_totalDim[channel] = totalDim;
    m_iconDim[channel]  = totalDim;

    return true;
}

// Modifie la rï¿½gion de clipping.

void CPixmap::SetClipping (RECT clip)
{
    m_clipRect = clip;
}

// Retourne la rï¿½gion de clipping.

RECT CPixmap::GetClipping()
{
    return m_clipRect;
}


// Teste si un point fait partie d'une icï¿½ne.

bool CPixmap::IsIconPixel (Sint32 channel, Sint32 rank, POINT pos)
{
    Sint32          nbx, nby;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
        return false;

    if (m_iconDim[channel].x == 0 ||
        m_iconDim[channel].y == 0)
        return false;

    nbx = m_totalDim[channel].x / m_iconDim[channel].x;
    nby = m_totalDim[channel].y / m_iconDim[channel].y;

    if (rank < 0 || rank >= nbx * nby)
        return false;

    pos.x += (rank % nbx) * m_iconDim[channel].x;
    pos.y += (rank / nbx) * m_iconDim[channel].y;

    SDL_Rect rect;
    rect.x = pos.x;
    rect.y = pos.y;
    rect.w = 1;
    rect.h = 1;
    Uint32 pixel = 0;

    SDL_SetRenderTarget (g_renderer, m_lpSDLTexture[channel]);
    SDL_RenderReadPixels (g_renderer, &rect, 0, &pixel, 4);
    SDL_SetRenderTarget (g_renderer, nullptr);

    return !!pixel;
}


// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

bool CPixmap::DrawIcon (Sint32 chDst, Sint32 channel, Sint32 rank, POINT pos,
                        bool bMask)
{
    Sint32          nbx, nby;
    RECT        rect;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (channel != CHMAP && m_lpSDLTexture[channel] == nullptr)
        return false;

    if (m_iconDim[channel].x == 0 ||
        m_iconDim[channel].y == 0)
        return false;

    nbx = m_totalDim[channel].x / m_iconDim[channel].x;
    nby = m_totalDim[channel].y / m_iconDim[channel].y;

    if (rank < 0 || rank >= nbx * nby)
        return false;

    rect.left   = (rank % nbx) * m_iconDim[channel].x;
    rect.top    = (rank / nbx) * m_iconDim[channel].y;
    rect.right  = rect.left + m_iconDim[channel].x;
    rect.bottom = rect.top  + m_iconDim[channel].y;

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

bool CPixmap::DrawIconDemi (Sint32 chDst, Sint32 channel, Sint32 rank,
                            POINT pos, bool bMask)
{
    Sint32          nbx, nby;
    RECT        rect;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
        return false;

    if (m_iconDim[channel].x == 0 ||
        m_iconDim[channel].y == 0)
        return false;

    nbx = m_totalDim[channel].x /  m_iconDim[channel].x;
    nby = m_totalDim[channel].y / (m_iconDim[channel].y / 2);

    rank = (rank / 32) * 32 + ((rank % 32) / 2) + ((rank % 2) * 16);

    if (rank < 0 || rank >= nbx * nby)
        return false;

    rect.left   = (rank % nbx) * m_iconDim[channel].x;
    rect.top    = (rank / nbx) * (m_iconDim[channel].y / 2);
    rect.right  = rect.left + m_iconDim[channel].x;
    rect.bottom = rect.top  + (m_iconDim[channel].y / 2);

    return !BltFast (chDst, channel, pos, rect);
}

// Dessine une partie d'image rectangulaire.

bool CPixmap::DrawIconPart (Sint32 chDst, Sint32 channel, Sint32 rank,
                            POINT pos,
                            Sint32 startY, Sint32 endY, bool bMask)
{
    Sint32          nbx, nby;
    RECT        rect;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
        return false;

    if (m_iconDim[channel].x == 0 ||
        m_iconDim[channel].y == 0)
        return false;

    nbx = m_totalDim[channel].x / m_iconDim[channel].x;
    nby = m_totalDim[channel].y / m_iconDim[channel].y;

    if (rank < 0 || rank >= nbx * nby)
        return false;

    rect.left   = (rank % nbx) * m_iconDim[channel].x;
    rect.top    = (rank / nbx) * m_iconDim[channel].y;
    rect.right  = rect.left + m_iconDim[channel].x;
    rect.bottom = rect.top  + endY;

    pos.y    += startY;
    rect.top += startY;

    return !BltFast (chDst, channel, pos, rect);
}

// Dessine une partie d'image n'importe oï¿½.

bool CPixmap::DrawPart (Sint32 chDst, Sint32 channel, POINT dest, RECT rect,
                        bool bMask)
{
    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
        return false;

    return !BltFast (chDst, channel, dest, rect);
}

// Dessine une partie d'image rectangulaire.

bool CPixmap::DrawImage (Sint32 chDst, Sint32 channel, RECT rect)
{
    POINT       dst;
    Sint32          res;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
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

bool CPixmap::BuildIconMask (Sint32 channelMask, Sint32 rankMask,
                             Sint32 channel, Sint32 rankSrc, Sint32 rankDst)
{
    Sint32          nbx, nby;
    POINT       posDst;
    RECT        rect;
    Sint32          res;

    if (channel < 0 || channel >= MAXIMAGE)
        return false;
    if (m_lpSDLTexture[channel] == nullptr)
        return false;

    if (m_iconDim[channel].x == 0 ||
        m_iconDim[channel].y == 0)
        return false;

    nbx = m_totalDim[channel].x / m_iconDim[channel].x;
    nby = m_totalDim[channel].y / m_iconDim[channel].y;

    if (rankSrc < 0 || rankSrc >= nbx * nby)
        return false;
    if (rankDst < 0 || rankDst >= nbx * nby)
        return false;

    rect.left   = (rankSrc % nbx) * m_iconDim[channel].x;
    rect.top    = (rankSrc / nbx) * m_iconDim[channel].y;
    rect.right  = rect.left + m_iconDim[channel].x;
    rect.bottom = rect.top  + m_iconDim[channel].y;
    posDst.x    = (rankDst % nbx) * m_iconDim[channel].x;
    posDst.y    = (rankDst / nbx) * m_iconDim[channel].y;
    res = BltFast (m_lpSDLTexture[channel], channel, posDst, rect);
    if (res)
        return false;

    if (m_iconDim[channelMask].x == 0 ||
        m_iconDim[channelMask].y == 0)
        return false;

    nbx = m_totalDim[channelMask].x / m_iconDim[channelMask].x;
    nby = m_totalDim[channelMask].y / m_iconDim[channelMask].y;

    if (rankMask < 0 || rankMask >= nbx * nby)
        return false;

    rect.left   = (rankMask % nbx) * m_iconDim[channelMask].x;
    rect.top    = (rankMask / nbx) * m_iconDim[channelMask].y;
    rect.right  = rect.left + m_iconDim[channelMask].x;
    rect.bottom = rect.top  + m_iconDim[channelMask].y;
    res = BltFast (m_lpSDLTexture[channel], channelMask, posDst, rect);

    return !res;
}


// Affiche le pixmap ï¿½ l'ï¿½cran.
// Retourne false en cas d'erreur.

bool CPixmap::Display()
{
    m_bBackDisplayed = true;
    SDL_RenderPresent (g_renderer);
    return true;
}

// Change le lutin de la souris.

void CPixmap::SetMouseSprite (Sint32 sprite, bool bDemoPlay)
{
    if (m_mouseSprite == sprite)
        return;

    m_mouseSprite = sprite;

    SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
}

// Montre ou cache la souris.

void CPixmap::MouseShow (bool bShow)
{
    SDL_ShowCursor (bShow);
}

// Retourne le rectangle correspondant au sprite
// de la souris dans CHBLUPI.

RECT CPixmap::MouseRectSprite()
{
    Sint32      rank, nbx;
    RECT    rcRect;

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

    nbx = m_totalDim[CHBLUPI].x / m_iconDim[CHBLUPI].x;

    rcRect.left   = (rank % nbx) * m_iconDim[CHBLUPI].x;
    rcRect.top    = (rank / nbx) * m_iconDim[CHBLUPI].y;
    rcRect.right  = rcRect.left + m_iconDim[CHBLUPI].x;
    rcRect.bottom = rcRect.top + m_iconDim[CHBLUPI].y;

    return rcRect;
}

SDL_Point CPixmap::GetCursorHotSpot (Sint32 sprite)
{
    static const Sint32 hotspots[MAXCURSORS * 2] =
    {
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

    SDL_Point hotspot = { 0, 0 };

    if (sprite >= SPRITE_BEGIN && sprite <= SPRITE_END)
    {
        const Sint32 rank = sprite - SPRITE_BEGIN;  // rank <- 0..n

        hotspot.x = hotspots[rank * 2 + 0];
        hotspot.y = hotspots[rank * 2 + 1];
    }

    return hotspot;
}

SDL_Rect CPixmap::GetCursorRect (Sint32 sprite)
{
    Sint32 rank;
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

    Sint32 nbx = m_totalDim[CHBLUPI].x / m_iconDim[CHBLUPI].x;

    rcRect.x = (rank % nbx) * m_iconDim[CHBLUPI].x;
    rcRect.y = (rank / nbx) * m_iconDim[CHBLUPI].y;
    rcRect.w = m_iconDim[CHBLUPI].x;
    rcRect.h = m_iconDim[CHBLUPI].y;

    return rcRect;
}

void CPixmap::LoadCursors()
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

    for (Sint32 sprite = SPRITE_BEGIN; sprite <= SPRITE_END; ++sprite)
    {
        SDL_Point hotspot = this->GetCursorHotSpot (sprite);
        SDL_Rect rect = this->GetCursorRect (sprite);

        SDL_Surface *surface = SDL_CreateRGBSurface (0, rect.w, rect.h, 32, rmask,
                               gmask, bmask, amask);
        SDL_BlitSurface (m_lpSDLBlupi, &rect, surface, nullptr);

        // FIXME: change cursor first value to 0
        m_lpSDLCursors[sprite - 1] = SDL_CreateColorCursor (surface, hotspot.x,
                                     hotspot.y);
    }
}

void CPixmap::ChangeSprite (MouseSprites sprite)
{
    if (m_lpCurrentCursor == m_lpSDLCursors[sprite - 1])
        return;

    SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
    m_lpCurrentCursor = m_lpSDLCursors[sprite - 1];
}
