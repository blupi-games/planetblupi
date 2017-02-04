// Jauge.h

#pragma once

class CPixmap;
class CDecor;
class CSound;

/////////////////////////////////////////////////////////////////////////////

class CJauge
{
public:
	CJauge();
	~CJauge();

	bool	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, bool bMinimizeRedraw);
	void	Draw();
	void	Redraw();

	void	SetLevel(int level);
	void	SetType(int type);

	bool	GetHide();
	void	SetHide(bool bHide);

	POINT	GetPos();
	void	SetRedraw();
	
protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	bool		m_bHide;		// true si bouton caché
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_type;
	int			m_level;
	bool		m_bMinimizeRedraw;
	bool		m_bRedraw;		// true -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
