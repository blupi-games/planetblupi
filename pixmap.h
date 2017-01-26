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
	
	bool	Create(HWND hwnd, POINT dim, bool bFullScreen, int mouseType);
	bool	Flush();
	bool	Restore();
	bool	InitSysPalette();
	bool	IsPalette();
	void	Fill(RECT rect, COLORREF color);
	
	bool	SavePalette();
	bool	RestorePalette();
	int		SearchColor(int red, int green, int blue);
	bool	Cache(int channel, char *pFilename, POINT totalDim, POINT iconDim, bool bUsePalette);
	bool	Cache(int channel, char *pFilename, POINT totalDim, bool bUsePalette);
	bool	Cache(int channel, HBITMAP hbm, POINT totalDim);
	void	Flush(int channel);
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

	void	SetMousePosSprite(POINT pos, int sprite, bool bDemoPlay);
	void	SetMousePos(POINT pos, bool bDemoPlay);
	void	SetMouseSprite(int sprite, bool bDemoPlay);
	void	MouseShow(bool bShow);
	void	MouseInvalidate();
	void	MouseBackClear();
	void	MouseBackDraw();
	void	LoadCursors ();

protected:
	HRESULT	RestoreAll();
	HRESULT	BltFast(int chDst, int channel, POINT dst, RECT rcRect, int mode);
	HRESULT	BltFast(LPDIRECTDRAWSURFACE lpDD, SDL_Texture *lpSDL,
					int channel, POINT dst, RECT rcRect, int mode);

	void	MouseUpdate();
	bool	MouseQuickDraw(RECT rect);
	void	MouseBackSave();
	void	MouseBackRestore();
	void	MouseBackDebug();
	RECT	MouseRectSprite();
	void	MouseHotSpot();
	SDL_Point GetCursorHotSpot (int sprite);
	SDL_Rect GetCursorRect (int sprite);

protected:
	bool					m_bFullScreen;
	int						m_mouseType;
	bool					m_bDebug;
	bool					m_bPalette;
	HWND					m_hWnd;
	POINT					m_dim;					// dimensions totales
	RECT					m_clipRect;				// rectangle de clipping

	POINT					m_mousePos;
	int						m_mouseSprite;
	POINT					m_mouseHotSpot;
	POINT					m_mouseBackPos;
	bool					m_bMouseBack;
	bool					m_bMouseShow;
	bool					m_bBackDisplayed;

	LPDIRECTDRAW			m_lpDD;					// DirectDraw object
	LPDIRECTDRAWSURFACE		m_lpDDSPrimary;			// DirectDraw primary surface
	SDL_Surface *			m_lpSDLPrimary;
	LPDIRECTDRAWSURFACE		m_lpDDSBack;			// DirectDraw back surface
	SDL_Surface *			m_lpSDLBack;
	LPDIRECTDRAWSURFACE		m_lpDDSMouse;			// DirectDraw back surface
	SDL_Cursor *			m_lpSDLCursors[MAXCURSORS];
	SDL_Surface *			m_lpSDLBlupi;
	LPDIRECTDRAWPALETTE		m_lpDDPal;				// the primary surface palette
	LPDIRECTDRAWSURFACE		m_lpDDSurface[MAXIMAGE]; // offscreen surfaces
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
