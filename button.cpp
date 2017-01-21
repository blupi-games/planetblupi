// Button.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include "def.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "button.h"
#include "misc.h"



/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CButton::CButton()
{
	m_type            = 0;
	m_bEnable         = true;
	m_bHide           = false;
	m_state           = 0;
	m_mouseState      = 0;
	m_nbMenu          = 0;
	m_nbToolTips      = 0;
	m_selMenu         = 0;
	m_bMouseDown      = false;
	m_bMinimizeRedraw = false;
	m_bRedraw         = false;
}

// Destructeur.

CButton::~CButton()
{
}


// Crée un nouveau bouton.

bool CButton::Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
					 POINT pos, int type, bool bMinimizeRedraw,
					 int *pMenu, int nbMenu,
					 int *pToolTips, int nbToolTips,
					 int region, UINT message)
{
	POINT		iconDim;
	int			i, icon;

	static int ttypes[] =
	{
		DIMBUTTONX,DIMBUTTONY,		// button00.bmp
	};

	if ( type < 0 || type > 0 )  return false;

	iconDim.x  = ttypes[type*2+0];
	iconDim.y  = ttypes[type*2+1];

	m_hWnd            = hWnd;
	m_pPixmap         = pPixmap;
	m_pSound          = pSound;
	m_type            = type;
	m_bMinimizeRedraw = bMinimizeRedraw;
	m_bEnable         = true;
	m_bHide           = false;
	m_message         = message;
	m_pos             = pos;
	m_dim             = iconDim;
	m_nbMenu          = nbMenu;
	m_nbToolTips      = nbToolTips;
	m_selMenu         = 0;
	m_state           = 0;
	m_mouseState      = 0;
	m_bMouseDown      = false;
	m_bRedraw         = true;

	for ( i=0 ; i<nbMenu ; i++ )
	{
		icon = pMenu[i];

		if ( region == 1 )  // palmiers ?
		{
			if ( icon ==  0 )  icon = 90;  // sol normal
			if ( icon ==  1 )  icon = 91;  // sol inflammable
			if ( icon ==  2 )  icon = 92;  // sol inculte
			if ( icon ==  7 )  icon =  9;  // plante
			if ( icon ==  8 )  icon = 10;  // arbre
		}

		if ( region == 2 )  // hiver ?
		{
			if ( icon ==  0 )  icon = 96;  // sol normal
			if ( icon ==  1 )  icon = 97;  // sol inflammable
			if ( icon ==  2 )  icon = 98;  // sol inculte
			if ( icon ==  8 )  icon = 99;  // arbre
		}

		if ( region == 3 )  // sapin ?
		{
			if ( icon ==  0 )  icon = 102;  // sol normal
			if ( icon ==  1 )  icon = 103;  // sol inflammable
			if ( icon ==  2 )  icon = 104;  // sol inculte
			if ( icon ==  8 )  icon = 105;  // arbre
		}

		m_iconMenu[i] = icon;
	}

	for ( i=0 ; i<nbToolTips ; i++ )
	{
		m_toolTips[i] = pToolTips[i];
	}

	return true;
}

// Dessine un bouton dans son état.

void CButton::Draw()
{
	int			i;
	POINT		pos;
	RECT		rect;

	if ( m_bMinimizeRedraw && !m_bRedraw )  return;
	m_bRedraw = false;

	if ( m_bHide )  // bouton caché ?
	{
		rect.left   = m_pos.x;
		rect.right  = m_pos.x+m_dim.x;
		rect.top    = m_pos.y;
		rect.bottom = m_pos.y+m_dim.y;
		m_pPixmap->DrawPart(-1, CHBACK, m_pos, rect, 1);  // dessine le fond
		return;
	}

	if ( m_bEnable )  // bouton actif ?
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, m_mouseState, m_pos);
	}
	else
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, 4, m_pos);
	}

	if ( m_nbMenu == 0 )  return;

	pos = m_pos;
	if ( m_nbMenu > 0 )
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type,
							m_iconMenu[m_selMenu]+6, pos);
	}

	if ( m_nbMenu == 1 || !m_bEnable || !m_bMouseDown )  return;

	pos = m_pos;
	pos.x += m_dim.x+2;
	for ( i=0 ; i<m_nbMenu ; i++ )
	{
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, i==m_selMenu?1:0, pos);
		m_pPixmap->DrawIcon(-1, CHBUTTON+m_type, m_iconMenu[i]+6, pos);
		pos.x += m_dim.x-1;
	}
}

void CButton::Redraw()
{
	m_bRedraw = true;
}

int CButton::GetState()
{
	return m_state;
}

void CButton::SetState(int state)
{
	if ( m_state      != state ||
		 m_mouseState != state )
	{
		m_bRedraw = true;
	}

	m_state      = state;
	m_mouseState = state;
}

int CButton::GetMenu()
{
	return m_selMenu;
}

void CButton::SetMenu(int menu)
{
	if ( m_selMenu != menu )
	{
		m_bRedraw = true;
	}

	m_selMenu = menu;
}


bool CButton::GetEnable()
{
	return m_bEnable;
}

void CButton::SetEnable(bool bEnable)
{
	if ( m_bEnable != bEnable )
	{
		m_bRedraw = true;
	}

	m_bEnable = bEnable;
}


bool CButton::GetHide()
{
	return m_bHide;
}

void CButton::SetHide(bool bHide)
{
	if ( m_bHide != bHide )
	{
		m_bRedraw = true;
	}

	m_bHide = bHide;
}


// Traitement d'un événement.

bool CButton::TreatEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT		pos;

	if ( m_bHide || !m_bEnable )  return false;

	pos = ConvLongToPos(lParam);

    switch( message )
    {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if ( MouseDown(pos) )  return true;
			break;

		case WM_MOUSEMOVE:
			if ( MouseMove(pos) )  return true;
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			if ( MouseUp(pos) )  return false;  // (*)
			break;
	}

	return false;
}

// (*) Tous les boutons doivent recevoir l'événement BUTTONUP !


// Indique si la souris est sur ce bouton.

bool CButton::MouseOnButton(POINT pos)
{
	return Detect(pos);
}


// Retourne le tooltips pour un bouton, en fonction
// de la position de la souris.

int CButton::GetToolTips(POINT pos)
{
	int		width = m_dim.x;
	int		rank;

	if ( m_bHide || !m_bEnable )  return -1;

	if ( m_nbMenu > 1 && m_bMouseDown )  // sous-menu déroulé ?
	{
		width += 2+(m_dim.x-1)*m_nbMenu;
	}

	if ( pos.x < m_pos.x         ||
		 pos.x > m_pos.x+width   ||
		 pos.y < m_pos.y         ||
		 pos.y > m_pos.y+m_dim.y )  return -1;

	rank = (pos.x-(m_pos.x+2+1))/(m_dim.x-1);
	if ( rank < 0 )  rank = 0;
	if ( rank > m_nbToolTips )  return -1;

	if ( m_nbMenu > 1 )
	{
		if ( m_bMouseDown && rank > 0 )
		{
			rank --;
		}
		else
		{
			rank = m_selMenu;
		}
	}

	return m_toolTips[rank];
}


// Détecte si la souris est dans le bouton.

bool CButton::Detect(POINT pos)
{
	int		width = m_dim.x;

	if ( m_bHide || !m_bEnable )  return false;

	if ( m_nbMenu > 1 && m_bMouseDown )  // sous-menu déroulé ?
	{
		width += 2+(m_dim.x-1)*m_nbMenu;
	}

	if ( pos.x < m_pos.x         ||
		 pos.x > m_pos.x+width   ||
		 pos.y < m_pos.y         ||
		 pos.y > m_pos.y+m_dim.y )  return false;

	return true;
}

// Bouton de la souris pressé.

bool CButton::MouseDown(POINT pos)
{
	if ( !Detect(pos) )  return false;

	m_mouseState = 1;
	m_bMouseDown = true;
	m_bRedraw    = true;
	PostMessage(m_hWnd, WM_UPDATE, 0, 0);

	m_pSound->PlayImage(SOUND_CLICK, pos);
	return true;
}

// Souris déplacés.

bool CButton::MouseMove(POINT pos)
{
	bool	bDetect;
	int		iState, iMenu;

	iState = m_mouseState;
	iMenu  = m_selMenu;

	bDetect = Detect(pos);

	if ( m_bMouseDown )
	{
		if ( bDetect )  m_mouseState = 1;  // pressé
		else            m_mouseState = m_state;
	}
	else
	{
		if ( bDetect )  m_mouseState = m_state+2;  // survollé
		else            m_mouseState = m_state;
	}

	if ( m_nbMenu > 1 &&
		 m_bMouseDown &&
		 pos.x > m_pos.x+m_dim.x+2 )  // dans sous-menu ?
	{
		m_selMenu = (pos.x-(m_pos.x+m_dim.x+2))/(m_dim.x-1);
		if ( m_selMenu >= m_nbMenu )
		{
			m_selMenu = m_nbMenu-1;
		}
	}

	if ( iState != m_mouseState ||
		 iMenu  != m_selMenu    )
	{
		m_bRedraw = true;
		PostMessage(m_hWnd, WM_UPDATE, 0, 0);
	}

	return m_bMouseDown;
}

// Bouton de la souris relâché.

bool CButton::MouseUp(POINT pos)
{
	bool	bDetect;

	bDetect = Detect(pos);

	m_mouseState = m_state;
	m_bMouseDown = false;
	m_bRedraw    = true;

	if ( !bDetect )  return false;

	if ( m_message != -1 )
	{
		PostMessage(m_hWnd, m_message, 0, 0);
	}

	return true;
}


