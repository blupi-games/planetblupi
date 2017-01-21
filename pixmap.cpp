// CPixmap.cpp
//

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include "def.h"
#include "pixmap.h"
#include "misc.h"
#include "ddutil.h"



/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CPixmap::CPixmap()
{
	int		i;
	
	m_bFullScreen  = FALSE;
	m_mouseType    = MOUSETYPEGRA;
	m_bDebug       = TRUE;
	m_bPalette     = TRUE;

	m_mouseSprite  = SPRITE_WAIT;
	MouseHotSpot();
	m_mousePos.x   = LXIMAGE/2;
	m_mousePos.y   = LYIMAGE/2;
	m_mouseBackPos = m_mousePos;
	m_bMouseBack   = FALSE;
	m_bMouseShow   = TRUE;
	m_bBackDisplayed = FALSE;

	m_lpDD         = NULL;
	m_lpDDSPrimary = NULL;
	m_lpDDSBack    = NULL;
	m_lpDDSMouse   = NULL;
	m_lpDDPal      = NULL;
	m_lpClipper    = NULL;

	for ( i=0 ; i<MAXIMAGE ; i++ )
	{
		m_lpDDSurface[i] = NULL;
	}

	// initialize special effects structure
	ZeroMemory(&m_DDbltfx, sizeof(m_DDbltfx));
	m_DDbltfx.dwSize = sizeof(m_DDbltfx);   
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
			if ( m_lpDDSurface[i] != NULL )
			{
				m_lpDDSurface[i]->Release();
				m_lpDDSurface[i]= NULL;
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


void CPixmap::SetDebug(BOOL bDebug)
{
	m_bDebug = bDebug;
	DDSetDebug(bDebug);
}


// Crée l'objet DirectDraw principal.
// Retourne FALSE en cas d'erreur.

BOOL CPixmap::Create(HWND hwnd, POINT dim,
					 BOOL bFullScreen, int mouseType)
{
	DDSURFACEDESC		ddsd;
	HRESULT				ddrval;
	POINT				pos;

	m_hWnd        = hwnd;
	m_bFullScreen = bFullScreen;
	m_mouseType   = mouseType;
	m_dim         = dim;

	if ( m_mouseType == MOUSETYPEGRA )
	{
		// Cache définitivement la vilaine souris Windows.
		ShowCursor(FALSE);

		pos = m_mousePos;
		ClientToScreen(m_hWnd, &pos);
		SetCursorPos(pos.x, pos.y);  // met la souris au centre
	}

	m_clipRect.left   = 0;
	m_clipRect.top    = 0;
	m_clipRect.right  = dim.x;
	m_clipRect.bottom = dim.y;

	// Create the main DirectDraw object
    ddrval = DirectDrawCreate(NULL, &m_lpDD, NULL);
    if ( ddrval != DD_OK )
    {
		OutputDebug("Fatal error: DirectDrawCreate\n");
        return FALSE;
    }

    // Get exclusive mode.
	if ( m_bFullScreen )
	{
		ddrval = m_lpDD->SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
	}
	else
	{
	    ddrval = m_lpDD->SetCooperativeLevel(hwnd, DDSCL_NORMAL);
	}
    if ( ddrval != DD_OK )
    {
		OutputDebug("Fatal error: SetCooperativeLevel\n");
        return FALSE;
    }

    // Set the video mode to 640x480x8.
	if ( m_bFullScreen )
	{
		ddrval = m_lpDD->SetDisplayMode(dim.x, dim.y, 8);
		if ( ddrval != DD_OK )
		{
			OutputDebug("Fatal error: SetDisplayMode\n");
			return FALSE;
		}
	}

    // Create the primary surface with 1 back buffer.
	ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

    ddrval = m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL);
    if ( ddrval != DD_OK )
    {
		TraceErrorDD(ddrval, "pixmap", 0);
		OutputDebug("Fatal error: CreateSurface\n");
        return FALSE;
    }
	
	// Create the back buffer.
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize         = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
//?	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth        = dim.x;
	ddsd.dwHeight       = dim.y;

	ddrval = m_lpDD->CreateSurface(&ddsd, &m_lpDDSBack, NULL);
	if ( ddrval != DD_OK )
	{
		TraceErrorDD(ddrval, "pixmap", 0);
		OutputDebug("Fatal error: CreateBackSurface\n");
		return FALSE;
	}

	// Create the mouse buffer.
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize         = sizeof(ddsd);
	ddsd.dwFlags        = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH;
//?	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth        = DIMBLUPIX;
	ddsd.dwHeight       = DIMBLUPIY;

	ddrval = m_lpDD->CreateSurface(&ddsd, &m_lpDDSMouse, NULL);
	if ( ddrval != DD_OK )
	{
		TraceErrorDD(ddrval, "pixmap", 0);
		OutputDebug("Fatal error: CreateMouseSurface\n");
		return FALSE;
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
			return FALSE;
		}

		ddrval = m_lpClipper->SetHWnd(0, hwnd);
		if ( ddrval != DD_OK )
		{
			TraceErrorDD(ddrval, "pixmap", 0);
			OutputDebug("Can't set clipper window handle\n");
			return FALSE;
		}

		ddrval = m_lpDDSPrimary->SetClipper(m_lpClipper);
		if ( ddrval != DD_OK )
		{
			TraceErrorDD(ddrval, "pixmap", 0);
			OutputDebug("Can't attach clipper to primary surface\n");
			return FALSE;
		}
    }

    return TRUE;
}

// Libère les bitmaps.

BOOL CPixmap::Flush()
{
	return TRUE;
}

// Restitue les bitmaps.

BOOL CPixmap::Restore()
{
	RestoreAll();
	return TRUE;
}

// Initialise la palette système.

BOOL CPixmap::InitSysPalette()
{
    HDC			hdc;
	int			caps;

    hdc = CreateCompatibleDC(NULL);
    if ( hdc == NULL )  return FALSE;

	if ( !m_bFullScreen )
	{
		caps = GetDeviceCaps(hdc, SIZEPALETTE);
		if ( caps == 0 )  m_bPalette = FALSE;
		else              m_bPalette = TRUE;
	}

	GetSystemPaletteEntries(hdc, 0, 256, m_sysPal);
    DeleteDC(hdc);
	return TRUE;
}

// Indique si l'on utilise une palette.

BOOL CPixmap::IsPalette()
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
	int			i;
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

	for ( i=0 ; i<MAXIMAGE ; i++ )
	{
		if ( m_lpDDSurface[i] && m_lpDDSurface[i]->IsLost() )
		{
			ddrval = m_lpDDSurface[i]->Restore();
			if( ddrval == DD_OK )
			{
				DDReLoadBitmap(m_lpDDSurface[i], m_filename[i]);
			}
		}
	}
	return DD_OK;
}

// Effectue un appel BltFast.
// Les modes sont 0=transparent, 1=opaque.

HRESULT CPixmap::BltFast(int chDst, int channel,
						 POINT dst, RECT rcRect, int mode)
{
	DWORD		dwTrans;
    HRESULT		ddrval;
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

    while( TRUE )
    {
		if ( chDst < 0 )
		{
			ddrval = m_lpDDSBack->BltFast(dst.x, dst.y,
										  m_lpDDSurface[channel],
										  &rcRect, dwTrans);
		}
		else
		{
			ddrval = m_lpDDSurface[chDst]->BltFast(dst.x, dst.y,
										  m_lpDDSurface[channel],
										  &rcRect, dwTrans);
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

HRESULT CPixmap::BltFast(LPDIRECTDRAWSURFACE lpDD,
						 int channel, POINT dst, RECT rcRect, int mode)
{
	DWORD		dwTrans;
    HRESULT		ddrval;

	if ( mode == 0 )  dwTrans = DDBLTFAST_SRCCOLORKEY;
	else              dwTrans = DDBLTFAST_NOCOLORKEY;

    while( TRUE )
    {
		ddrval = lpDD->BltFast(dst.x, dst.y,
							   m_lpDDSurface[channel],
							   &rcRect, dwTrans);
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


// Sauve toute la palette de couleurs.

BOOL CPixmap::SavePalette()
{
    HRESULT     ddrval;

	if ( m_lpDDPal == NULL )  return FALSE;

    ddrval = m_lpDDPal->GetEntries(0, 0, 256, m_pal);

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}

// Restitue toute la palette de couleurs.

BOOL CPixmap::RestorePalette()
{
    HRESULT     ddrval;

    ddrval = m_lpDDPal->SetEntries(0, 0, 256, m_pal);

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
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

BOOL CPixmap::Cache(int channel, char *pFilename, POINT totalDim, POINT iconDim,
					BOOL bUsePalette)
{
    HRESULT     ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;

	if ( m_lpDDSurface[channel] != NULL )
	{
		Flush(channel);
	}

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

    // Create the offscreen surface, by loading our bitmap.
    m_lpDDSurface[channel] = DDLoadBitmap(m_lpDD, pFilename, 0, 0);

    if ( m_lpDDSurface[channel] == NULL )
    {
		OutputDebug("Fatal error: DDLoadBitmap\n");
        return FALSE;
    }

    // Set the color key to white
	if ( m_bDebug )  OutputDebug("DDSetColorKey\n");
    DDSetColorKey(m_lpDDSurface[channel], RGB(255,255,255));  // blanc
	
	strcpy(m_filename[channel], pFilename);

	m_totalDim[channel] = totalDim;
	m_iconDim[channel]  = iconDim;

	return TRUE;
}

// Cache une image globale.

BOOL CPixmap::Cache(int channel, char *pFilename, POINT totalDim, BOOL bUsePalette)
{
	POINT		iconDim;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;

	iconDim.x = 0;
	iconDim.y = 0;

	return Cache(channel, pFilename, totalDim, iconDim, bUsePalette);
}

// Cache une image provenant d'un bitmap.

BOOL CPixmap::Cache(int channel, HBITMAP hbm, POINT totalDim)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;

	if ( m_lpDDSurface[channel] != NULL )
	{
		Flush(channel);
	}

    // Create the offscreen surface, by loading our bitmap.
    m_lpDDSurface[channel] = DDConnectBitmap(m_lpDD, hbm);

    if ( m_lpDDSurface[channel] == NULL )
    {
		OutputDebug("Fatal error: DDLoadBitmap\n");
        return FALSE;
    }

    // Set the color key to white
    DDSetColorKey(m_lpDDSurface[channel], RGB(255,255,255));  // blanc
	
	m_totalDim[channel] = totalDim;
	m_iconDim[channel]  = totalDim;

	return TRUE;
}

// Purge une image.

void CPixmap::Flush(int channel)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return;
	if (  m_lpDDSurface[channel] == NULL )     return;

	m_lpDDSurface[channel]->Release();
	m_lpDDSurface[channel]= NULL;
}

// Définition de la couleur transparente.

void CPixmap::SetTransparent(int channel, COLORREF color)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return;
	if (  m_lpDDSurface[channel] == NULL )     return;

    DDSetColorKey(m_lpDDSurface[channel], color);
	m_colorSurface[2*channel+0] = color;
	m_colorSurface[2*channel+1] = color;
}

// Définition de la couleur transparente.

void CPixmap::SetTransparent2(int channel, COLORREF color1, COLORREF color2)
{
	if ( channel < 0 || channel >= MAXIMAGE )  return;
	if (  m_lpDDSurface[channel] == NULL )     return;

    DDSetColorKey2(m_lpDDSurface[channel], color1, color2);
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

BOOL CPixmap::IsIconPixel(int channel, int rank, POINT pos)
{
	int			nbx, nby;
    COLORREF	rgb;
    HDC			hDC;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return FALSE;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return FALSE;

	pos.x += (rank%nbx)*m_iconDim[channel].x;
	pos.y += (rank/nbx)*m_iconDim[channel].y;

	if ( m_lpDDSurface[channel]->GetDC(&hDC) != DD_OK )  return FALSE;
	rgb = GetPixel(hDC, pos.x, pos.y);
	m_lpDDSurface[channel]->ReleaseDC(hDC);

	if ( rgb == m_colorSurface[2*channel+0] ||
		 rgb == m_colorSurface[2*channel+1] )  return FALSE;

	return TRUE;
}


// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

BOOL CPixmap::DrawIcon(int chDst, int channel, int rank, POINT pos,
					   int mode, BOOL bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return FALSE;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return FALSE;

	rect.left   = (rank%nbx)*m_iconDim[channel].x;
	rect.top    = (rank/nbx)*m_iconDim[channel].y;
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  + m_iconDim[channel].y;

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, pos, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
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

BOOL CPixmap::DrawIconDemi(int chDst, int channel, int rank, POINT pos,
						   int mode, BOOL bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return FALSE;

	nbx = m_totalDim[channel].x /  m_iconDim[channel].x;
	nby = m_totalDim[channel].y / (m_iconDim[channel].y/2);

	rank = (rank/32)*32+((rank%32)/2)+((rank%2)*16);

	if ( rank < 0 || rank >= nbx*nby )  return FALSE;

	rect.left   = (rank%nbx)* m_iconDim[channel].x;
	rect.top    = (rank/nbx)*(m_iconDim[channel].y/2);
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  +(m_iconDim[channel].y/2);

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, pos, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

BOOL CPixmap::DrawIconPart(int chDst, int channel, int rank, POINT pos,
						   int startY, int endY,
						   int mode, BOOL bMask)
{
	int			nbx, nby;
	RECT		rect;
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return FALSE;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rank < 0 || rank >= nbx*nby )  return FALSE;

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

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}

// Dessine une partie d'image n'importe où.
// Les modes sont 0=transparent, 1=opaque.

BOOL CPixmap::DrawPart(int chDst, int channel, POINT dest, RECT rect,
					   int mode, BOOL bMask)
{
	HRESULT		ddrval;
	COLORREF	oldColor1, oldColor2;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	oldColor1 = m_colorSurface[2*channel+0];
	oldColor2 = m_colorSurface[2*channel+1];
	if ( bMask )  SetTransparent(channel, RGB(255,255,255));  // blanc
	ddrval = BltFast(chDst, channel, dest, rect, mode);
	if ( bMask )  SetTransparent2(channel, oldColor1, oldColor2);

	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}

// Dessine une partie d'image rectangulaire.
// Les modes sont 0=transparent, 1=opaque.

BOOL CPixmap::DrawImage(int chDst, int channel, RECT rect, int mode)
{
	POINT		dst;
	HRESULT		ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	dst.x = rect.left;
	dst.y = rect.top;

	ddrval = BltFast(chDst, channel, dst, rect, mode);

	if ( ddrval != DD_OK )  return FALSE;

	if ( channel == CHBACK )
	{
		MouseBackSave();  // sauve ce qui sera sous la souris
		m_bBackDisplayed = FALSE;
	}

	return TRUE;
}


// Construit une icône en utilisant un masque.

BOOL CPixmap::BuildIconMask(int channelMask, int rankMask,
							int channel, int rankSrc, int rankDst)
{
	int			nbx, nby;
	POINT		posDst;
	RECT		rect;
	HRESULT		ddrval;

	if ( channel < 0 || channel >= MAXIMAGE )  return FALSE;
	if (  m_lpDDSurface[channel] == NULL )     return FALSE;

	if ( m_iconDim[channel].x == 0 ||
		 m_iconDim[channel].y == 0 )  return FALSE;

	nbx = m_totalDim[channel].x / m_iconDim[channel].x;
	nby = m_totalDim[channel].y / m_iconDim[channel].y;

	if ( rankSrc < 0 || rankSrc >= nbx*nby )  return FALSE;
	if ( rankDst < 0 || rankDst >= nbx*nby )  return FALSE;

	rect.left   = (rankSrc%nbx)*m_iconDim[channel].x;
	rect.top    = (rankSrc/nbx)*m_iconDim[channel].y;
	rect.right  = rect.left + m_iconDim[channel].x;
	rect.bottom = rect.top  + m_iconDim[channel].y;
	posDst.x    = (rankDst%nbx)*m_iconDim[channel].x;
	posDst.y    = (rankDst/nbx)*m_iconDim[channel].y;
	ddrval = BltFast(m_lpDDSurface[channel], channel, posDst, rect, 1);
	if ( ddrval != DD_OK )  return FALSE;

	if ( m_iconDim[channelMask].x == 0 ||
		 m_iconDim[channelMask].y == 0 )  return FALSE;

	nbx = m_totalDim[channelMask].x / m_iconDim[channelMask].x;
	nby = m_totalDim[channelMask].y / m_iconDim[channelMask].y;

	if ( rankMask < 0 || rankMask >= nbx*nby )  return FALSE;

	rect.left   = (rankMask%nbx)*m_iconDim[channelMask].x;
	rect.top    = (rankMask/nbx)*m_iconDim[channelMask].y;
	rect.right  = rect.left + m_iconDim[channelMask].x;
	rect.bottom = rect.top  + m_iconDim[channelMask].y;
	ddrval = BltFast(m_lpDDSurface[channel], channelMask, posDst, rect, 0);
	if ( ddrval != DD_OK )  return FALSE;

	return TRUE;
}


// Affiche le pixmap à l'écran.
// Retourne FALSE en cas d'erreur.

BOOL CPixmap::Display()
{
	HRESULT		ddrval;
	RECT		DestRect, MapRect;

	m_bBackDisplayed = TRUE;

	// Get screen coordinates of client window for blit
	GetClientRect(m_hWnd, &DestRect);
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect);
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect+1);
	
	MapRect.left   = 0;
	MapRect.top    = 0;
	MapRect.right  = m_dim.x;
	MapRect.bottom = m_dim.y;

	// do the blit from back surface
	ddrval = m_lpDDSPrimary->Blt
				(
					&DestRect,		// destination rect
					m_lpDDSBack,
					&MapRect,		// source rect     
					DDBLT_WAIT,
					&m_DDbltfx
				);
    if ( ddrval == DDERR_SURFACELOST )
    {
        ddrval = RestoreAll();
    }
	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}


// Positionne la souris et change le lutin.

void CPixmap::SetMousePosSprite(POINT pos, int sprite, BOOL bDemoPlay)
{
	if ( m_mousePos.x == pos.x &&
		 m_mousePos.y == pos.y &&
		 m_mouseSprite == sprite )  return;

	m_mousePos    = pos;
	m_mouseSprite = sprite;
	MouseHotSpot();

	if ( !bDemoPlay )
	{
		MouseUpdate();
	}
}

// Positionne la souris.

void CPixmap::SetMousePos(POINT pos, BOOL bDemoPlay)
{
	if ( m_mousePos.x == pos.x &&
		 m_mousePos.y == pos.y )  return;

	m_mousePos = pos;

	if ( !bDemoPlay )
	{
		MouseUpdate();
	}
}

// Change le lutin de la souris.

void CPixmap::SetMouseSprite(int sprite, BOOL bDemoPlay)
{
	if ( m_mouseSprite == sprite )  return;

	m_mouseSprite = sprite;
	MouseHotSpot();

	if ( !bDemoPlay )
	{
		MouseUpdate();
	}
}

// Montre ou cache la souris.

void CPixmap::MouseShow(BOOL bShow)
{
	m_bMouseShow = bShow;
}

// Met à jour le dessin de la souris.

void CPixmap::MouseUpdate()
{
	RECT	oldRect, newRect, rcRect;

	if ( m_lpDDSurface[CHBLUPI] == NULL )  return;
	if ( m_mouseType != MOUSETYPEGRA )  return;
	if ( m_mouseSprite == SPRITE_EMPTY )  return;
	if ( !m_bMouseShow )  return;

	oldRect.left   = m_mouseBackPos.x;
	oldRect.top    = m_mouseBackPos.y;
	oldRect.right  = m_mouseBackPos.x + DIMBLUPIX;
	oldRect.bottom = m_mouseBackPos.y + DIMBLUPIY;

	newRect.left   = m_mousePos.x - m_mouseHotSpot.x;
	newRect.top    = m_mousePos.y - m_mouseHotSpot.y;
	newRect.right  = newRect.left + DIMBLUPIX;
	newRect.bottom = newRect.top  + DIMBLUPIY;

	MouseBackRestore();  // enlève la souris dans m_lpDDSBack
	MouseBackDraw();     // dessine la souris dans m_lpDDSBack

	if ( m_bBackDisplayed )
	{
		if ( IntersectRect(&rcRect, &oldRect, &newRect) )
		{
			UnionRect(&rcRect, &oldRect, &newRect);
			MouseQuickDraw(rcRect);
		}
		else
		{
			MouseQuickDraw(oldRect);
			MouseQuickDraw(newRect);
		}
	}
}

// Dessine rapidement la souris dans l'écran.
// Il s'agit en fait de dessiner un petit morceau rectangulaire
// de m_lpDDSBack dans l'écran.

BOOL CPixmap::MouseQuickDraw(RECT rect)
{
	HRESULT		ddrval;
	RECT		DestRect;

	if ( rect.left   < 0       )  rect.left   = 0;
	if ( rect.right  > LXIMAGE )  rect.right  = LXIMAGE;
	if ( rect.top    < 0       )  rect.top    = 0;
	if ( rect.bottom > LYIMAGE )  rect.bottom = LYIMAGE;

	// Get screen coordinates of client window for blit
	DestRect = rect;
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect);
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect+1);
	
	// do the blit from back surface
	ddrval = m_lpDDSPrimary->Blt
				(
					&DestRect,		// destination rect
					m_lpDDSBack,
					&rect,			// source rect     
					DDBLT_WAIT,
					&m_DDbltfx
				);
    if ( ddrval == DDERR_SURFACELOST )
    {
        ddrval = RestoreAll();
    }
	if ( ddrval != DD_OK )  return FALSE;
	return TRUE;
}

// Invalide la copie sous la souris.

void CPixmap::MouseInvalidate()
{
	m_bMouseBack = FALSE;
}

// Enlève la souris dans m_lpDDSBack.

void CPixmap::MouseBackClear()
{
	if ( m_mouseType != MOUSETYPEGRA )  return;
	MouseBackRestore();  // enlève la souris dans m_lpDDSBack
}

// Dessine la souris dans m_lpDDSBack.

void CPixmap::MouseBackDraw()
{
	POINT	dst;
	RECT	rcRect;

	if ( m_lpDDSurface[CHBLUPI] == NULL )  return;
	if ( m_mouseType != MOUSETYPEGRA )  return;
	if ( m_mouseSprite == SPRITE_EMPTY )  return;
	if ( !m_bMouseShow )  return;

	MouseBackSave();  // sauve ce qui sera sous la souris

	dst.x  = m_mousePos.x - m_mouseHotSpot.x;
	dst.y  = m_mousePos.y - m_mouseHotSpot.y;
	rcRect = MouseRectSprite();

	if ( dst.x < 0 )
	{
		rcRect.left -= dst.x;
		dst.x = 0;
	}
	if ( dst.x+DIMBLUPIX > LXIMAGE )
	{
		rcRect.right -= (dst.x+DIMBLUPIX)-LXIMAGE;
	}
	if ( dst.y < 0 )
	{
		rcRect.top -= dst.y;
		dst.y = 0;
	}
	if ( dst.y+DIMBLUPIY > LYIMAGE )
	{
		rcRect.bottom -= (dst.y+DIMBLUPIY)-LYIMAGE;
	}

	// Dessine le lutin dans m_lpDDSBack.
	BltFast(m_lpDDSBack, CHBLUPI, dst, rcRect, 0);
}

// Sauve le fond sous la souris.
// m_lpDDSMouse <- m_lpDDSBack

void CPixmap::MouseBackSave()
{
    HRESULT		ddrval;
	POINT		dst;
	RECT		rcRect;

	if ( m_lpDDSurface[CHBLUPI] == NULL )  return;
	if ( m_mouseType != MOUSETYPEGRA )  return;
	if ( m_mouseSprite == SPRITE_EMPTY )  return;
	if ( !m_bMouseShow )  return;

	m_mouseBackPos.x = m_mousePos.x - m_mouseHotSpot.x;
	m_mouseBackPos.y = m_mousePos.y - m_mouseHotSpot.y;
	m_bMouseBack = TRUE;

	dst.x = 0;
	dst.y = 0;

	rcRect.left   = m_mouseBackPos.x;
	rcRect.top    = m_mouseBackPos.y;
	rcRect.right  = m_mouseBackPos.x + DIMBLUPIX;
	rcRect.bottom = m_mouseBackPos.y + DIMBLUPIY;

	if ( rcRect.left < 0 )
	{
		dst.x -= rcRect.left;
		rcRect.left = 0;
	}
	if ( rcRect.right > LXIMAGE )
	{
		rcRect.right = LXIMAGE;
	}
	if ( rcRect.top < 0 )
	{
		dst.y -= rcRect.top;
		rcRect.top = 0;
	}
	if ( rcRect.bottom > LYIMAGE )
	{
		rcRect.bottom = LYIMAGE;
	}

    while( TRUE )
    {
		ddrval = m_lpDDSMouse->BltFast(dst.x, dst.y,
									   m_lpDDSBack,
									   &rcRect, DDBLTFAST_NOCOLORKEY);
        if ( ddrval == DD_OK )  break;
        
        if ( ddrval == DDERR_SURFACELOST )
        {
            ddrval = RestoreAll();
            if ( ddrval != DD_OK )  break;
        }

        if ( ddrval != DDERR_WASSTILLDRAWING )  break;
    }
}

// Restitue le fond sous la souris.
// m_lpDDSBack <- m_lpDDSMouse

void CPixmap::MouseBackRestore()
{
    HRESULT		ddrval;
	POINT		dst;
	RECT		rcRect;

	if ( m_lpDDSurface[CHBLUPI] == NULL )  return;
	if ( !m_bMouseBack )  return;

	dst.x = m_mouseBackPos.x;
	dst.y = m_mouseBackPos.y;

	rcRect.left   = 0;
	rcRect.top    = 0;
	rcRect.right  = DIMBLUPIX;
	rcRect.bottom = DIMBLUPIY;

	if ( dst.x < 0 )
	{
		rcRect.left -= dst.x;
		dst.x = 0;
	}
	if ( dst.x+DIMBLUPIX > LXIMAGE )
	{
		rcRect.right -= (dst.x+DIMBLUPIX)-LXIMAGE;
	}
	if ( dst.y < 0 )
	{
		rcRect.top -= dst.y;
		dst.y = 0;
	}
	if ( dst.y+DIMBLUPIY > LYIMAGE )
	{
		rcRect.bottom -= (dst.y+DIMBLUPIY)-LYIMAGE;
	}

    while( TRUE )
    {
		ddrval = m_lpDDSBack->BltFast(dst.x, dst.y,
									  m_lpDDSMouse,
									  &rcRect, DDBLTFAST_NOCOLORKEY);
        if ( ddrval == DD_OK )  break;
        
        if ( ddrval == DDERR_SURFACELOST )
        {
            ddrval = RestoreAll();
            if ( ddrval != DD_OK )  break;
        }

        if ( ddrval != DDERR_WASSTILLDRAWING )  break;
    }
}

// Affiche le contenu de m_lpDDSMouse dans le
// coin sup/gauche.

void CPixmap::MouseBackDebug()
{
	HRESULT		ddrval;
	RECT		DestRect, MapRect;

	// Get screen coordinates of client window for blit
	GetClientRect(m_hWnd, &DestRect);
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect);
	ClientToScreen(m_hWnd, (LPPOINT)&DestRect+1);
	
	MapRect.left   = 0;
	MapRect.top    = 0;
	MapRect.right  = DIMBLUPIX;
	MapRect.bottom = DIMBLUPIY;

	DestRect.right  = DestRect.left + DIMBLUPIX;
	DestRect.bottom = DestRect.top  + DIMBLUPIY;

	// do the blit from back surface
	ddrval = m_lpDDSPrimary->Blt
				(
					&DestRect,		// destination rect
					m_lpDDSMouse,
					&MapRect,		// source rect     
					DDBLT_WAIT,
					&m_DDbltfx
				);
    if ( ddrval == DDERR_SURFACELOST )
    {
        ddrval = RestoreAll();
    }
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

// Initialise le hot spot selon le sprite en cours.

void CPixmap::MouseHotSpot()
{
	int		rank;

	static int table_mouse_hotspot[14*2] =
	{
		30, 30,		// SPRITE_ARROW
		20, 15,		// SPRITE_POINTER
		31, 26,		// SPRITE_MAP
		25, 14,		// SPRITE_ARROWU
		24, 35,		// SPRITE_ARROWD
		15, 24,		// SPRITE_ARROWL
		35, 24,		// SPRITE_ARROWR
		18, 16,		// SPRITE_ARROWUL
		32, 18,		// SPRITE_ARROWUR
		17, 30,		// SPRITE_ARROWDL
		32, 32,		// SPRITE_ARROWDR
		30, 30,		// SPRITE_WAIT
		30, 30,		// SPRITE_EMPTY
		21, 51,		// SPRITE_FILL
	};

	if ( m_mouseSprite >= SPRITE_ARROW &&
		 m_mouseSprite <= SPRITE_FILL  )
	{
		rank = m_mouseSprite - SPRITE_ARROW;  // rank <- 0..n

		m_mouseHotSpot.x = table_mouse_hotspot[rank*2+0];
		m_mouseHotSpot.y = table_mouse_hotspot[rank*2+1];
	}
	else
	{
		m_mouseHotSpot.x = 0;
		m_mouseHotSpot.y = 0;
	}
}


