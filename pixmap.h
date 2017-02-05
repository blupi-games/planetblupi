// CPixmap.h

#pragma once

#include <ddraw.h>
#include <SDL.h>

/////////////////////////////////////////////////////////////////////////////

#define MAXIMAGE	100
#define MAXCURSORS	14

class CPixmap
{
public:
	CPixmap();
	~CPixmap();

	void	SetDebug(bool bDebug);
	
	bool	Create(POINT dim, bool bFullScreen, int mouseType);
	bool	Flush();
	void	Fill(RECT rect, COLORREF color);
	
	bool	Cache(int channel, char *pFilename, POINT totalDim, POINT iconDim, bool bUsePalette);
	bool	Cache(int channel, char *pFilename, POINT totalDim, bool bUsePalette);
	bool	Cache(int channel, SDL_Surface *surface, POINT totalDim);
	void	SetTransparent(int channel, COLORREF color);
	void	SetTransparent2(int channel, COLORREF color1, COLORREF color2);
	void	SetClipping(RECT clip);
	RECT	GetClipping();

	bool	IsIconPixel(int channel, int rank, POINT pos);

	bool	DrawIcon(int chDst, int channel, int rank, POINT pos, int mode=0, bool bMask=false);
	bool	DrawIconDemi(int chDst, int channel, int rank, POINT pos, int mode=0, bool bMask=false);
	bool	DrawIconPart(int chDst, int channel, int rank, POINT pos, int startY, int endY, int mode=0, bool bMask=false);
	bool	DrawPart(int chDst, int channel, POINT dest, RECT rect, int mode=0, bool bMask=false);
	bool	DrawImage(int chDst, int channel, RECT rect, int mode=0);

	bool	BuildIconMask(int channelMask, int rankMask,
						  int channel, int rankSrc, int rankDst);
	
	bool	Display();

	void	SetMouseSprite(int sprite, bool bDemoPlay);
	void	MouseShow(bool bShow);
	void	LoadCursors ();
	void	ChangeSprite (MouseSprites sprite);

protected:
	HRESULT	BltFast(int chDst, int channel, POINT dst, RECT rcRect, int mode);
	HRESULT	BltFast(SDL_Texture *lpSDL,
					int channel, POINT dst, RECT rcRect, int mode);

	RECT	MouseRectSprite();
	SDL_Point GetCursorHotSpot (int sprite);
	SDL_Rect GetCursorRect (int sprite);

protected:
	bool					m_bFullScreen;
	int						m_mouseType;
	bool					m_bDebug;
	bool					m_bPalette;
	POINT					m_dim;					// dimensions totales
	RECT					m_clipRect;				// rectangle de clipping

	int						m_mouseSprite;
	bool					m_bBackDisplayed;

	LPDIRECTDRAW			m_lpDD;					// DirectDraw object
	LPDIRECTDRAWSURFACE		m_lpDDSPrimary;			// DirectDraw primary surface
	SDL_Surface *			m_lpSDLPrimary;
	LPDIRECTDRAWSURFACE		m_lpDDSBack;			// DirectDraw back surface
	SDL_Surface *			m_lpSDLBack;
	LPDIRECTDRAWSURFACE		m_lpDDSMouse;			// DirectDraw back surface
	SDL_Cursor *			m_lpCurrentCursor;
	SDL_Cursor *			m_lpSDLCursors[MAXCURSORS];
	SDL_Surface *			m_lpSDLBlupi;
	LPDIRECTDRAWPALETTE		m_lpDDPal;				// the primary surface palette
	SDL_Surface *			m_lpSDLSurface[MAXIMAGE];
	SDL_Texture *			m_lpSDLTexture[MAXIMAGE];
	LPDIRECTDRAWCLIPPER		m_lpClipper;
    PALETTEENTRY			m_pal[256];				// sauvegarde palette
    PALETTEENTRY			m_sysPal[256];			// sauvegarde palette
	COLORREF				m_colorSurface[2*MAXIMAGE];

	char					m_filename[MAXIMAGE][20];
	POINT					m_totalDim[MAXIMAGE];	// dimensions totale image
	POINT					m_iconDim[MAXIMAGE];	// dimensions d'une icône
	DDBLTFX					m_DDbltfx;
};

/////////////////////////////////////////////////////////////////////////////
