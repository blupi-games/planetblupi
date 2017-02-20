
#pragma once

#include <SDL2/SDL.h>

#include "blupi.h"

#define MAXIMAGE    100
#define MAXCURSORS  14

class CPixmap
{
public:
    CPixmap();
    ~CPixmap();

    bool    Create (POINT dim, bool bFullScreen, Sint32 mouseType);
    bool    Flush();
    void    Fill (RECT rect, COLORREF color);

    bool    Cache (Sint32 channel, const char *pFilename, POINT totalDim,
                   POINT iconDim);
    bool    Cache (Sint32 channel, const char *pFilename, POINT totalDim);
    bool    Cache (Sint32 channel, SDL_Surface *surface, POINT totalDim);
    void    SetClipping (RECT clip);
    RECT    GetClipping();

    bool    IsIconPixel (Sint32 channel, Sint32 rank, POINT pos);

    bool    DrawIcon (Sint32 chDst, Sint32 channel, Sint32 rank, POINT pos,
                      bool bMask = false);
    bool    DrawIconDemi (Sint32 chDst, Sint32 channel, Sint32 rank, POINT pos,
                          bool bMask = false);
    bool    DrawIconPart (Sint32 chDst, Sint32 channel, Sint32 rank, POINT pos,
                          Sint32 startY, Sint32 endY, bool bMask = false);
    bool    DrawPart (Sint32 chDst, Sint32 channel, POINT dest, RECT rect,
                      bool bMask = false);
    bool    DrawImage (Sint32 chDst, Sint32 channel, RECT rect);

    bool    BuildIconMask (Sint32 channelMask, Sint32 rankMask,
                           Sint32 channel, Sint32 rankSrc, Sint32 rankDst);

    bool    Display();

    void    SetMouseSprite (Sint32 sprite, bool bDemoPlay);
    void    MouseShow (bool bShow);
    void    LoadCursors();
    void    ChangeSprite (MouseSprites sprite);

protected:
    Sint32  BltFast (Sint32 chDst, Sint32 channel, POINT dst, RECT rcRect);
    Sint32  BltFast (SDL_Texture *lpSDL, Sint32 channel, POINT dst, RECT rcRect);

    RECT    MouseRectSprite();
    SDL_Point GetCursorHotSpot (Sint32 sprite);
    SDL_Rect GetCursorRect (Sint32 sprite);

protected:
    bool                    m_bFullScreen;
    Sint32                      m_mouseType;
    bool                    m_bDebug;
    bool                    m_bPalette;
    POINT                   m_dim;                  // dimensions totales
    RECT                    m_clipRect;             // rectangle de clipping

    Sint32                      m_mouseSprite;
    bool                    m_bBackDisplayed;

    SDL_Cursor             *m_lpCurrentCursor;
    SDL_Cursor             *m_lpSDLCursors[MAXCURSORS];
    SDL_Surface            *m_lpSDLBlupi;
    SDL_Texture            *m_lpSDLTexture[MAXIMAGE];
    COLORREF                m_colorSurface[2 * MAXIMAGE];

    char                    m_filename[MAXIMAGE][20];
    POINT                   m_totalDim[MAXIMAGE];   // dimensions totale image
    POINT                   m_iconDim[MAXIMAGE];    // dimensions d'une icï¿½ne
};

/////////////////////////////////////////////////////////////////////////////
