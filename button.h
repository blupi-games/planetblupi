// Button.h

#pragma once

/////////////////////////////////////////////////////////////////////////////

class CButton
{
public:
	CButton();
	~CButton();

	bool	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, bool bMinimizeRedraw,
				   int *pMenu, int nbMenu,
				   int *pTooltips, int nbToolTips,
				   int region, UINT message);
	void	Draw();
	void	Redraw();

	int		GetState();
	void	SetState(int state);

	int		GetMenu();
	void	SetMenu(int menu);

	bool	GetEnable();
	void	SetEnable(bool bEnable);

	bool	GetHide();
	void	SetHide(bool bHide);

	bool	TreatEvent(const SDL_Event &event);
	bool	MouseOnButton(POINT pos);
	int		GetToolTips(POINT pos);

	
protected:
	bool	Detect(POINT pos);
	bool	MouseDown(POINT pos);
	bool	MouseMove(POINT pos);
	bool	MouseUp(POINT pos);

protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	int			m_type;			// type de bouton
	bool		m_bEnable;		// true si bouton actif
	bool		m_bHide;		// true si bouton caché
	UINT		m_message;		// message envoyé si bouton actionné
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_state;		// 0=relâché, 1=pressé, +2=survollé
	int			m_mouseState;	// 0=relâché, 1=pressé, +2=survollé
	int			m_iconMenu[20];	// icônes du sous-menu
	int			m_toolTips[20];	// info-bulles
	int			m_nbMenu;		// nb de case du sous-menu
	int			m_nbToolTips;	// nb d'info-bulles
	int			m_selMenu;		// sous-menu sélectionné
	bool		m_bMouseDown;	// true -> bouton souris pressé
	bool		m_bMinimizeRedraw;
	bool		m_bRedraw;		// true -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
