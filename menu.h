// Menu.h

#pragma once

/////////////////////////////////////////////////////////////////////////////

class CMenu
{
public:
	CMenu();
	~CMenu();

	bool	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int nb, int *pButtons, int *pErrors,
				   int perso);
	void	Update(int nb, int *pButtons, int *pErrors);
	void	Delete();
	void	Draw();
	int		GetSel();
	int		GetRank();
	bool	IsError();
	bool	IsExist();
	void	Message();

	bool	TreatEvent(const SDL_Event &event);

protected:
	int		Detect(POINT pos);
	bool	MouseDown(POINT pos);
	bool	MouseMove(POINT pos);
	bool	MouseUp(POINT pos);

protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_nbButtons;
	POINT		m_nbCel;
	int			m_perso;
	int			m_buttons[MAXBUTTON];
	int			m_errors[MAXBUTTON];
	UINT		m_messages[MAXBUTTON];
	int			m_selRank;
};

/////////////////////////////////////////////////////////////////////////////
