// Menu.h

#pragma once

#include <unordered_map>
#include "def.h"

/////////////////////////////////////////////////////////////////////////////

class CMenu
{
public:
	CMenu();
	~CMenu();

	bool	Create(CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int nb, int *pButtons, int *pErrors,
				   std::unordered_map<int, const char *> &texts,
				   int perso);
	void	Update(int nb, int *pButtons, int *pErrors,
				   std::unordered_map<int, const char *> &texts);
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
	std::unordered_map<int, const char *> m_texts;
	unsigned int		m_messages[MAXBUTTON];
	int			m_selRank;
};

/////////////////////////////////////////////////////////////////////////////
