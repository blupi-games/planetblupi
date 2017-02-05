// CPixmap.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string>
#include <ddraw.h>
#include <SDL.h>
#include <SDL_surface.h>
#include <SDL_log.h>
#include "def.h"
#include "pixmap.h"
#include "misc.h"
#include "ddutil.h"
#include "blupi.h"



/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CPixmap::CPixmap()
{
	int		i;
	
	m_bFullScreen  = false;
	m_mouseType    = MOUSETYPEGRA;
	m_bDebug       = true;
	m_bPalette     = true;

	m_mouseSprite  = SPRITE_WAIT;
	m_bBackDisplayed = false;

	m_lpDD         = NULL;
	m_lpDDSPrimary = NULL;
	m_lpDDSBack    = NULL;
	m_lpDDSMouse   = NULL;
	m_lpDDPal      = NULL;
	m_lpClipper    = NULL;

	for ( i=0 ; i<MAXIMAGE ; i++ )
	{
		m_lpSDLTexture[i] = NULL;
	}

	// initialize special effects structure
	ZeroMemory(&m_DDbltfx, sizeof(m_DDbltfx));
	m_DDbltfx.dwSize = sizeof(m_DDbltfx);

	m_lpCurrentCursor = nullptr;
}

// Destructeur.

CPixmap::~CPixmap()
{
	int		i;

    if ( m_lpDD != NULL )
    {
        if ( m_lpDDSPrimary != NULL )
        {
            m_lpDDSPrimary->Release();
            m_lpDDSPrimary = NULL;
        }

		if ( m_lpDDSBack != NULL )
		{
			m_lpDDSBack->Release();
			m_lpDDSBack = NULL;
		}

		if ( m_lpDDSMouse != NULL )
		{
			m_lpDDSMouse->Release();
			m_lpDDSMouse = NULL;
		}

        if ( m_lpDDPal != NULL )
        {
            m_lpDDPal->Release();
            m_lpDDPal = NULL;
        }

		for ( i=0 ; i<MAXIMAGE ; i++ )
		{
			if ( m_lpSDLTexture[i] != NULL )
			{
				SDL_DestroyTexture (m_lpSDLTexture[i]);
				m_lpSDLTexture[i] = NULL;
			}
		}

		if ( m_lpClipper != NULL )
		{
			m_lpClipper->Release();
			m_lpClipper = NULL;
		}

        m_lpDD->Release();
        m_lpDD = NULL;
    }
}


void CPixmap::SetDebug(bool bDebug)
{
	m_bDebug = bDebug;
	DDSetDebug(bDebug);
}


// Crée l'objet DirectDraw principal.
// Retourne false en cas d'erreur.

bool CPixmap::Create(POINT dim,
					 bool bFullScreen, int mouseType)
{
	HRESULT				ddrval;

	m_bFullScreen = bFullScreen;
	m_mouseType   = mouseType;
	m_dim         = dim;

	m_clipRect.left   = 0;
	m_clipRect.top    = 0;
	m_clipRect.right  = dim.x;
	m_clipRect.bottom = dim.y;

	// Create the main DirectDraw object
    ddrval = DirectDrawCreate(NULL, &m_lpDD, NULL);
    if ( ddrval != DD_OK )
    {
		OutputDebug("Fatal error: DirectDrawCreate\n");
        return false;
    }

    if ( ddrval != DD_OK )
    {
		OutputDebug("Fatal error: SetCooperativeLevel\n");
        return false;
    }

    // Set the video mode to 640x480x8.
	if ( m_bFullScreen )
	{
		ddrval = m_lpDD->SetDisplayMode(dim.x, dim.y, 8);
		if ( ddrval != DD_OK )
		{
			OutputDebug("Fatal error: SetDisplayMode\n");
			return false;
		}
	}

	// Create a DirectDrawClipper object. The object enables clipping to the 
	// window boundaries in the IDirectDrawSurface::Blt function for the 
	// primary surface.
	if ( !m_bFullScreen )
	{
		ddrval = m_lpDD->CreateClipper(0, &m_lpClipper, NULL);
		if ( ddrval != DD_OK )
		{
			TraceErrorDD(ddrval, "pixmap", 0);
			OutputDebug("Can't create clipper\n");
			return false;
		}

		ddrval = m_lpClipper->SetHWnd(0, nullptr);
		if ( ddrval != DD_OK )
		{
			TraceErrorDD(ddrval, "pixmap", 0);
			OutputDebug("Can't set clipper window handle\n");
			return false;
		}
    }

    return true;
}

// Libère les bitmaps.

bool CPixmap::Flush()
{
	return true;
}

// Restitue les bitmaps.

bool CPixmap::Restore()
{
	RestoreAll();
	return true;
}

// Initialise la palette système.

bool CPixmap::InitSysPalette()
{
    HDC			hdc;
	int			caps;

    hdc = CreateCompatibleDC(NULL);
    if ( hdc == NULL )  return false;

	if ( !m_bFullScreen )
	{
		caps = GetDeviceCaps(hdc, SIZEPALETTE);
		if ( caps == 0 )  m_bPalette = false;
		else              m_bPalette = true;
	}

	GetSystemPaletteEntries(hdc, 0, 256, m_sysPal);
    DeleteDC(hdc);
	return true;
}

// Indique si l'on utilise une palette.

bool CPixmap::IsPalette()
{
	return m_bPalette;
}


// Rempli une zone rectangulaire avec une couleur uniforme.

void CPixmap::Fill(RECT rect, COLORREF color)
{
	// à faire si nécessaire ...
}


// Restore all lost objects.

HRESULT CPixmap::RestoreAll()
{
	if ( m_bDebug )  OutputDebug("CPixmap::RestoreAll\n");
	HRESULT     ddrval;

	if ( m_lpDDSPrimary && m_lpDDSPrimary->IsLost() )
	{
		ddrval = m_lpDDSPrimary->Restore();
//		if( ddrval != DD_OK )  return ddrval;
	}

	if ( m_lpDDSBack && m_lpDDSBack->IsLost() )
	{
		ddrval = m_lpDDSBack->Restore();
//		if( ddrval != DD_OK )  return ddrval;
	}

	if ( m_lpDDSMouse && m_lpDDSMouse->IsLost() )
	{
		ddrval = m_lpDDSMouse->Restore();
//		if( ddrval != DD_OK )  return ddrval;
	}

	return DD_OK;
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

HRESULT CPixmap::BltFast(int chDst, int channel,
						 POINT dst, RECT rcRect, int mode)
{
	DWORD		dwTrans;
    HRESULT		ddrval = DD_OK;
	int			limit;

	if ( mode == 0 )  dwTrans = DDBLTFAST_SRCCOLORKEY;
	else              dwTrans = DDBLTFAST_NOCOLORKEY;

	// Effectue un peu de clipping.
	if ( dst.x < m_clipRect.left )
	{
		rcRect.left += m_clipRect.left-dst.x;
		dst.x = m_clipRect.left;
	}
	limit = (m_clipRect.right-dst.x)+rcRect.left;
	if ( rcRect.right > limit )
	{
		rcRect.right = limit;
	}
	if ( dst.y < m_clipRect.top )
	{
		rcRect.top += m_clipRect.top-dst.y;
		dst.y = m_clipRect.top;
	}
	limit = (m_clipRect.bottom-dst.y)+rcRect.top;
	if ( rcRect.bottom > limit )
	{
		rcRect.bottom = limit;
	}

	if ( rcRect.left >= rcRect.right ||
		 rcRect.top  >= rcRect.bottom )  return DD_OK;

    while( true )
    {
		if ( chDst < 0 )
		{
			SDL_Rect srcRect, dstRect;
			srcRect.x = rcRect.left;
			srcRect.y = rcRect.top;
			srcRect.w = rcRect.right - rcRect.left;
			srcRect.h = rcRect.bottom - rcRect.top;
			dstRect = srcRect;
			dstRect.x = dst.x;
			dstRect.y = dst.y;
			//SDL_BlitSurface (m_lpSDLSurface[channel], &srcRect, m_lpSDLBack, &dstRect);
			SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
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
			//SDL_BlitSurface (m_lpSDLSurface[channel], &srcRect, m_lpSDLSurface[chDst], &dstRect);

			//SDL_SetTextureBlendMode (m_lpSDLTexture[chDst], SDL_BLENDMODE_BLEND);
			SDL_SetRenderTarget (g_renderer, m_lpSDLTexture[chDst]);
			SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
			SDL_SetRenderTarget (g_renderer, nullptr);
			//SDL_RenderCopy (g_renderer, m_lpSDLTexture[chDst], NULL, NULL);
		}
        if ( ddrval == DD_OK )  break;
        
        if ( ddrval == DDERR_SURFACELOST )
        {
            ddrval = RestoreAll();
            if ( ddrval != DD_OK )  break;
        }

        if ( ddrval != DDERR_WASSTILLDRAWING )  break;
    }

	return ddrval;
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

HRESULT CPixmap::BltFast(SDL_Texture *lpSDL,
						 int channel, POINT dst, RECT rcRect, int mode)
{
	DWORD		dwTrans;
    HRESULT		ddrval = DD_OK;

	if ( mode == 0 )  dwTrans = DDBLTFAST_SRCCOLORKEY;
	else              dwTrans = DDBLTFAST_NOCOLORKEY;

		SDL_Rect srcRect, dstRect;
		srcRect.x = rcRect.left;
		srcRect.y = rcRect.top;
		srcRect.w = rcRect.right - rcRect.left;
		srcRect.h = rcRect.bottom - rcRect.top;
		dstRect = srcRect;
		dstRect.x = dst.x;
		dstRect.y = dst.y;
		//SDL_BlitSurface (m_lpSDLSurface[channel], &srcRect, lpSDL, &dstRect);
		SDL_SetRenderTarget (g_renderer, lpSDL);
		SDL_RenderCopy (g_renderer, m_lpSDLTexture[channel], &srcRect, &dstRect);
		SDL_SetRenderTarget (g_renderer, nullptr);

	return ddrval;
}


// Sauve toute la palette de couleurs.

bool CPixmap::SavePalette()
{
    HRESULT     ddrval;

	if ( m_lpDDPal == NULL )  return false;

    ddrval = m_lpDDPal->GetEntries(0, 0, 256, m_pal);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Restitue toute la palette de couleurs.

bool CPixmap::RestorePalette()
{
    HRESULT     ddrval;

    ddrval = m_lpDDPal->SetEntries(0, 0, 256, m_pal);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Cherche une couleur dans la palette principale.
// En mode plein écran, il faut chercher dans la palette
// correspondant aux images (obtenue avec SavePalette),
// alors qu'en mode fenêtre, il faut chercher dans la
// palette système (obtenue avec InitSysPalette) !!!

int CPixmap::SearchColor(int red, int green, int blue)
{
	int		i, j, delta, min;

	if ( m_bFullScreen )
	{
		for ( i=0 ; i<256 ; i++ )
		{
			if ( red   == m_pal[i].peRed   &&
				 green == m_pal[i].peGreen &&
				 blue  == m_pal[i].peBlue  )  return i;
		}

		// Cherche la couleur la plus proche.
		min = 10000;
		j   = -1;
		for ( i=0 ; i<256 ; i++ )
		{
			delta = abs(red   - m_pal[i].peRed  )+
					abs(green - m_pal[i].peGreen)+
					abs(blue  - m_pal[i].peBlue );

			if ( delta < min )
			{
				min = delta;
				j = i;
			}
		}
	}
	else
	{
		if ( m_bPalette )
		{
			for ( i=0 ; i<256 ; i++ )
			{
				if ( red   == m_sysPal[i].peRed   &&
					 green == m_sysPal[i].peGreen &&
					 blue  == m_sysPal[i].peBlue  )  return i;
			}

			// Cherche la couleur la plus proche.
			min = 10000;
			j   = -1;
			for ( i=0 ; i<256 ; i++ )
			{
				delta = abs(red   - m_sysPal[i].peRed  )+
						abs(green - m_sysPal[i].peGreen)+
						abs(blue  - m_sysPal[i].peBlue );

				if ( delta < min )
				{
					min = delta;
					j = i;
				}
			}
		}
		else
		{
			j  =  (blue >>3)     &0x001F;
			j |= ((green>>2)<< 5)&0x07E0;
			j |= ((red  >>3)<<11)&0xF800;  // mode 5-6-5
//?			j  =  (blue >>3)     &0x001F;
//?			j |= ((green>>3)<< 5)&0x03E0;
//?			j |= ((red  >>3)<<10)&0x7C00;  // mode 5-5-5
		}
	}
	return j;
}


// Cache une image contenant des icônes.

bool CPixmap::Cache(int channel, char *pFilename, POINT totalDim, POINT iconDim,
					bool bUsePalette)
{
    HRESULT     ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;

    // Create and set the palette.
	if ( bUsePalette )
	{
		if ( m_bDebug )  OutputDebug("Use palette\n");
		if ( m_lpDDPal != NULL )
		{
			if ( m_bDebug )  OutputDebug("Release palette\n");
			m_lpDDPal->Release();
			m_lpDDPal = NULL;
		}

		m_lpDDPal = DDLoadPalette(m_lpDD, pFilename);

		if ( m_lpDDPal )
		{
			if ( m_bDebug )  OutputDebug("Set palette\n");
			m_lpDDSPrimary->SetPalette(NULL);  // indispensable !
			ddrval = m_lpDDSPrimary->SetPalette(m_lpDDPal);
			if ( ddrval != DD_OK )
			{
				TraceErrorDD(ddrval, pFilename, 1);
			}
		}
	}

	std::string file = pFilename;
	if (_access ((file + ".bmp").c_str (), 0 /* F_OK */) != -1)
		file += ".bmp";

	SDL_Surface *surface = SDL_LoadBMP (file.c_str ());

	if (channel == CHBLUPI)
		m_lpSDLBlupi = surface;

	SDL_Texture *texture = SDL_CreateTextureFromSurface (g_renderer, surface);
	unsigned int format;
	int access, w, h;
	SDL_QueryTexture (texture, &format, &access, &w, &h);

	if (!m_lpSDLTexture[channel])
	{
		m_lpSDLTexture[channel] = SDL_CreateTexture (g_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
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
		SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError ());
		return false;
	}

	if (channel != CHBLUPI)
		SDL_FreeSurface (surface);

	strcpy(m_filename[channel], pFilename);

	m_totalDim[channel] = totalDim;
	m_iconDim[channel]  = iconDim;

	return true;
}

// Cache une image globale.

bool CPixmap::Cache(int channel, char *pFilename, POINT totalDim, bool bUsePalette)
{
	POINT		iconDim;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;

	iconDim.x = 0;
	iconDim.y = 0;

	return Cache(channel, pFilename, totalDim, iconDim, bUsePalette);
}

// Cache une image provenant d'un bitmap.

bool CPixmap::Cache(int channel, SDL_Surface *surface, POINT totalDim)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return false;

    // Create the offscreen surface, by loading our bitmap.
	if (m_lpSDLTexture[channel])
		SDL_DestroyTexture (m_lpSDLTexture[channel]);

	m_lpSDLTexture[channel] = SDL_CreateTextureFromSurface (g_renderer, surface);

    if (m_lpSDLTexture[channel] == NULL )
    {
		OutputDebug("Fatal error: DDLoadBitmap\n");
        return false;
    }

    // Set the color key to white
    //DDSetColorKey(m_lpDDSurface[channel], RGB(255,255,255));  // blanc
	
	m_totalDim[channel] = totalDim;
	m_iconDim[channel]  = totalDim;

	return true;
}

// Définition de la couleur transparente.

void CPixmap::SetTransparent(int channel, COLORREF color)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return;
	if (m_lpSDLTexture[channel] == NULL )     return;

	m_colorSurface[2*channel+0] = color;
	m_colorSurface[2*channel+1] = color;
}

// Définition de la couleur transparente.

void CPixmap::SetTransparent2(int channel, COLORREF color1, COLORREF color2)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return;
	if (m_lpSDLTexture[channel] == NULL )     return;

	m_colorSurface[2*channel+0] = color1;
	m_colorSurface[2*channel+1] = color2;
}


// Modifie la région de clipping.

void CPixmap::SetClipping(RECT clip)
{
	m_clipRect = clip;
}

// Retourne la région de clipping.

RECT CPixmap::GetClipping()
{
	return m_clipRect;
}


// Teste si un point fait partie d'une icône.

bool CPixmap::IsIconPixel(int channel, int rank, POINT pos)
{
	int			nbx, nby;
    COLORREF	rgb;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return false;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return false;

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

bool CPixmap::DrawIcon(int chDst, int channel, int rank, POINT pos,
					   int mode, bool bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if ( channel != CHMAP && m_lpSDLTexture[channel] == NULL )     return false;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return false;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return false;

	rect.left   = (rank%nbx)*m_iconDim[channel].x;
	rect.top    = (rank/nbx)*m_iconDim[channel].y;
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  + m_iconDim[channel].y;

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if (channel != CHMAP && bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, pos, rect, mode);
	if (channel != CHMAP && bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.
//
// Correspondances in,out :
//	 0,0	 2,1	...
//	 1,16	 3,17
//
//	32,32	34,33
//	33,48	35,49

bool CPixmap::DrawIconDemi(int chDst, int channel, int rank, POINT pos,
						   int mode, bool bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return false;

	nbx = m_totalDim[channel].x /  m_iconDim[channel].x;
	nby = m_totalDim[channel].y / (m_iconDim[channel].y/2);

	rank = (rank/32)*32+((rank%32)/2)+((rank%2)*16);

	if ( rank < 0 || rank >= nbx*nby )  return false;

	rect.left   = (rank%nbx)* m_iconDim[channel].x;
	rect.top    = (rank/nbx)*(m_iconDim[channel].y/2);
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  +(m_iconDim[channel].y/2);

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, pos, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

bool CPixmap::DrawIconPart(int chDst, int channel, int rank, POINT pos,
						   int startY, int endY,
						   int mode, bool bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return false;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return false;

	rect.left   = (rank%nbx)*m_iconDim[channel].x;
	rect.top    = (rank/nbx)*m_iconDim[channel].y;
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  + endY;

	pos.y    += startY;
	rect.top += startY;

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, pos, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Dessine une partie d'image n'importe où.
// Les modes sont 0=transparent, 1=opaque.

bool CPixmap::DrawPart(int chDst, int channel, POINT dest, RECT rect,
					   int mode, bool bMask)
{
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, dest, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return false;
	return true;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

bool CPixmap::DrawImage(int chDst, int channel, RECT rect, int mode)
{
	POINT		dst;
	HRESULT		ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	dst.x = rect.left;
	dst.y = rect.top;

	ddrval = BltFast(chDst, channel, dst, rect, mode);

	if ( ddrval != DD_OK )  return false;

	if ( channel == CHBACK )
	{
		m_bBackDisplayed = false;
	}

	return true;
}


// Construit une icône en utilisant un masque.

bool CPixmap::BuildIconMask(int channelMask, int rankMask,
							int channel, int rankSrc, int rankDst)
{
	int			nbx, nby;
	POINT		posDst;
	RECT		rect;
	HRESULT		ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return false;
	if (m_lpSDLTexture[channel] == NULL )     return false;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return false;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rankSrc < 0 || rankSrc >= nbx*nby )  return false;
	if ( rankDst < 0 || rankDst >= nbx*nby )  return false;

	rect.left   = (rankSrc%nbx)*m_iconDim[channel].x;
	rect.top    = (rankSrc/nbx)*m_iconDim[channel].y;
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  + m_iconDim[channel].y;
	posDst.x    = (rankDst%nbx)*m_iconDim[channel].x;
	posDst.y    = (rankDst/nbx)*m_iconDim[channel].y;
	ddrval = BltFast(m_lpSDLTexture[channel], channel, posDst, rect, 1);
	if ( ddrval != DD_OK )  return false;

	if ( m_iconDim[channelMask].x == 0 ||
		 m_iconDim[channelMask].y == 0 )  return false;

	nbx = m_totalDim[channelMask].x / m_iconDim[channelMask].x;
	nby = m_totalDim[channelMask].y / m_iconDim[channelMask].y;

	if ( rankMask < 0 || rankMask >= nbx*nby )  return false;

	rect.left   = (rankMask%nbx)*m_iconDim[channelMask].x;
	rect.top    = (rankMask/nbx)*m_iconDim[channelMask].y;
	rect.right  = rect.left + m_iconDim[channelMask].x;
	rect.bottom = rect.top  + m_iconDim[channelMask].y;
	ddrval = BltFast(m_lpSDLTexture[channel], channelMask, posDst, rect, 0);
	if ( ddrval != DD_OK )  return false;

	return true;
}


// Affiche le pixmap à l'écran.
// Retourne false en cas d'erreur.

bool CPixmap::Display()
{
	RECT		MapRect;

	m_bBackDisplayed = true;

	MapRect.left   = 0;
	MapRect.top    = 0;
	MapRect.right  = m_dim.x;
	MapRect.bottom = m_dim.y;

	SDL_RenderPresent (g_renderer);
	return true;
}

// Change le lutin de la souris.

void CPixmap::SetMouseSprite(int sprite, bool bDemoPlay)
{
	if ( m_mouseSprite == sprite )  return;

	m_mouseSprite = sprite;

	SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
}

// Montre ou cache la souris.

void CPixmap::MouseShow(bool bShow)
{
	SDL_ShowCursor (bShow);
}

// Retourne le rectangle correspondant au sprite
// de la souris dans CHBLUPI.

RECT CPixmap::MouseRectSprite()
{
	int		rank, nbx;
	RECT	rcRect;

	rank = 348;
	if ( m_mouseSprite == SPRITE_ARROW   )  rank = 348;
	if ( m_mouseSprite == SPRITE_POINTER )  rank = 349;
	if ( m_mouseSprite == SPRITE_MAP     )  rank = 350;
	if ( m_mouseSprite == SPRITE_WAIT    )  rank = 351;
	if ( m_mouseSprite == SPRITE_FILL    )  rank = 352;
	if ( m_mouseSprite == SPRITE_ARROWL  )  rank = 353;
	if ( m_mouseSprite == SPRITE_ARROWR  )  rank = 354;
	if ( m_mouseSprite == SPRITE_ARROWU  )  rank = 355;
	if ( m_mouseSprite == SPRITE_ARROWD  )  rank = 356;
	if ( m_mouseSprite == SPRITE_ARROWDL )  rank = 357;
	if ( m_mouseSprite == SPRITE_ARROWDR )  rank = 358;
	if ( m_mouseSprite == SPRITE_ARROWUL )  rank = 359;
	if ( m_mouseSprite == SPRITE_ARROWUR )  rank = 360;

	nbx = m_totalDim[CHBLUPI].x / m_iconDim[CHBLUPI].x;

	rcRect.left   = (rank%nbx)*m_iconDim[CHBLUPI].x;
	rcRect.top    = (rank/nbx)*m_iconDim[CHBLUPI].y;
	rcRect.right  = rcRect.left+m_iconDim[CHBLUPI].x;
	rcRect.bottom = rcRect.top +m_iconDim[CHBLUPI].y;

	return rcRect;
}

SDL_Point CPixmap::GetCursorHotSpot (int sprite)
{
	static const int hotspots[MAXCURSORS * 2] =
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
		const int rank = sprite - SPRITE_BEGIN;  // rank <- 0..n

		hotspot.x = hotspots[rank * 2 + 0];
		hotspot.y = hotspots[rank * 2 + 1];
	}

	return hotspot;
}

SDL_Rect CPixmap::GetCursorRect (int sprite)
{
	int rank;
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

	int nbx = m_totalDim[CHBLUPI].x / m_iconDim[CHBLUPI].x;

	rcRect.x = (rank % nbx) * m_iconDim[CHBLUPI].x;
	rcRect.y = (rank / nbx) * m_iconDim[CHBLUPI].y;
	rcRect.w = m_iconDim[CHBLUPI].x;
	rcRect.h = m_iconDim[CHBLUPI].y;

	return rcRect;
}

void CPixmap::LoadCursors ()
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

	for (int sprite = SPRITE_BEGIN; sprite <= SPRITE_END; ++sprite)
	{
		SDL_Point hotspot = this->GetCursorHotSpot (sprite);
		SDL_Rect rect = this->GetCursorRect (sprite);

		SDL_Surface *surface = SDL_CreateRGBSurface (0, rect.w, rect.h, 32, rmask, gmask, bmask, amask);
		SDL_BlitSurface (m_lpSDLBlupi, &rect, surface, nullptr);

		// FIXME: change cursor first value to 0
		m_lpSDLCursors[sprite - 1] = SDL_CreateColorCursor (surface, hotspot.x, hotspot.y);
	}
}

void CPixmap::ChangeSprite (MouseSprites sprite)
{
	if (m_lpCurrentCursor == m_lpSDLCursors[sprite - 1])
		return;

	SDL_SetCursor (m_lpSDLCursors[sprite - 1]);
	m_lpCurrentCursor = m_lpSDLCursors[sprite - 1];
}
