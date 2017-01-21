// Menu.cpp
//

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include "def.h"
#include "resource.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "button.h"
#include "menu.h"
#include "text.h"
#include "misc.h"


/////////////////////////////////////////////////////////////////////////////


#define MARGMENU	0


static short table_button_icon[] =
{
	24,		// go
	40,		// stop
	32,		// mange
	30,		// carry
	31,		// depose
	22,		// abat
	27,		// roc
	28,		// cultive
	19,		// build1 (cabane)
	25,		// build2 (couveuse)
	35,		// build3 (laboratoire)
	61,		// build4 (mine)
	59,		// build5 (usine)
	101,	// build6 (téléporteur)
	20,		// mur
	26,		// palis
	42,		// abat n
	43,		// roc n
	23,		// pont
	33,		// tour
	34,		// boit
	39,		// labo
	54,		// fleur
	55,		// fleur n
	41,		// dynamite
	58,		// bateau
	60,		// djeep
	64,		// drapeau
	62,		// extrait du fer
	65,		// fabrique jeep
	63,		// fabrique mine
	83,		// fabrique disciple
	100,	// repeat
	107,	// qarmure
	106,	// fabarmure
};

void GetText(int rank, char *pBuffer, int lgBuffer)
{
	LoadString(TX_ACTION_GO+rank, pBuffer, lgBuffer);
}

void GetErr(int rank, char *pBuffer, int lgBuffer)
{
	LoadString(TX_ERROR_MISC+rank, pBuffer, lgBuffer);
}


/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CMenu::CMenu()
{
	m_nbButtons = 0;
	m_selRank = -1;
}

// Destructeur.

CMenu::~CMenu()
{
}


// Crée un nouveau bouton.

BOOL CMenu::Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int nb, int *pButtons, int *pErrors,
				   int perso)
{
	pos.x -= DIMBUTTONX/2;
	pos.y -= DIMBUTTONY/2;

	m_hWnd      = hWnd;
	m_pPixmap   = pPixmap;
	m_pSound    = pSound;
	m_nbButtons = nb;
	m_pos       = pos;
	m_perso     = perso;

	Update(nb, pButtons, pErrors);

	if ( m_pos.x < POSDRAWX )  m_pos.x = POSDRAWX;
	if ( m_pos.y < POSDRAWY )  m_pos.y = POSDRAWY;

	if ( m_pos.x > POSDRAWX+DIMDRAWX-2-m_dim.x )  m_pos.x = POSDRAWX+DIMDRAWX-2-m_dim.x;
	if ( m_pos.y > POSDRAWY+DIMDRAWY-2-m_dim.y )  m_pos.y = POSDRAWY+DIMDRAWY-2-m_dim.y;

	if ( m_pos.x != pos.x || m_pos.y != pos.y )
	{
		pos = m_pos;
		pos.x += DIMBUTTONX/2;
		pos.y += DIMBUTTONY/2;
		ClientToScreen(m_hWnd, &pos);
		SetCursorPos(pos.x, pos.y);
	}

	m_selRank = Detect(pos);

	return TRUE;
}

// Met à jour le menu.

void CMenu::Update(int nb, int *pButtons, int *pErrors)
{
	int			i;

	m_nbButtons = nb;

	if ( nb < 5 )  m_nbCel.x = 1;
	else           m_nbCel.x = 2;

	m_nbCel.y = (nb+m_nbCel.x-1)/m_nbCel.x;

	m_dim.x = (DIMBUTTONX+MARGMENU)*m_nbCel.x;
	m_dim.y = (DIMBUTTONY+MARGMENU)*m_nbCel.y;

	for ( i=0 ; i<nb ; i++ )
	{
		m_buttons[i] = pButtons[i];
		m_errors[i]  = pErrors[i];
	}
}

// Détruit le menu.

void CMenu::Delete()
{
	m_nbButtons = 0;
	m_selRank = -1;
}

// Dessine un bouton dans son état.

void CMenu::Draw()
{
	int			i, state, icon;
	POINT		pos;
	RECT		oldClip, clipRect;
	BOOL		bLeft  = TRUE;
	BOOL		bRight = TRUE;
	char		text[50];
	char*		pText;

	if ( m_nbButtons == 0 )  return;

	oldClip = m_pPixmap->GetClipping();
	clipRect.left   = POSDRAWX;
	clipRect.top    = POSDRAWY;
	clipRect.right  = POSDRAWX+DIMDRAWX;
	clipRect.bottom = POSDRAWY+DIMDRAWY;
	m_pPixmap->SetClipping(clipRect);

	if ( m_pos.x-150         < POSDRAWX          )  bLeft  = FALSE;
	if ( m_pos.x+m_dim.x+150 > POSDRAWX+DIMDRAWX )  bRight = FALSE;

	for ( i=0 ; i<m_nbButtons ; i++ )
	{
		pos.x = m_pos.x+((i/m_nbCel.y)*(DIMBUTTONX+MARGMENU));
		pos.y = m_pos.y+((i%m_nbCel.y)*(DIMBUTTONY+MARGMENU));

		if ( i == m_selRank )  state = 2;  // hilite
		else                   state = 0;  // release
		if ( m_errors[i] != 0 &&
			 m_errors[i] != ERROR_TOURISOL &&
			 m_errors[i] < 100 )
		{
			state = 4;  // disable
		}
		m_pPixmap->DrawIcon(-1, CHBUTTON, state, pos);

		icon = table_button_icon[m_buttons[i]];
		if ( m_perso == 8 )  // disciple ?
		{
			if ( icon ==  30 )  icon = 88;  // prend
			if ( icon ==  31 )  icon = 89;  // dépose
		}
		m_pPixmap->DrawIcon(-1, CHBUTTON, icon+6, pos);
	}

	// Affiche le texte d'aide.
	if ( m_selRank != -1 )
	{
		i = m_selRank;
		pos.y = m_pos.y+((i%m_nbCel.y)*(DIMBUTTONY+MARGMENU));

		if ( m_errors[i] == 0 )
		{
			pos.y += (DIMBUTTONY-DIMTEXTY)/2;
		}
		else
		{
			pos.y += (DIMBUTTONY-DIMTEXTY*2)/2;
		}

		if ( m_errors[i] >= 100 )  // no ressource au lieu erreur ?
		{
			LoadString(m_errors[i], text, 50);
			pText = strchr(text, '\n');
			if ( pText != NULL )  *pText = 0;
		}
		else
		{
			GetText(m_buttons[i], text, 50);
		}

		if ( m_nbCel.x > 1 && i < m_nbCel.y )
		{
//			if ( bLeft )
//			{
				pos.x = m_pos.x-4-GetTextWidth(text);  // texte à gauche
//			}
//			else
//			{
//				pos.x = m_pos.x+((i/m_nbCel.y)+1)+(DIMBUTTONX+MARGMENU)+4;
//			}
		}
		else
		{
//			if ( bRight )
//			{
				pos.x = m_pos.x+m_dim.x+4;  // texte à droite
//			}
//			else
//			{
//				pos.x = m_pos.x+(i/m_nbCel.y)*(DIMBUTTONX+MARGMENU)-4-GetTextWidth(text);
//			}
		}

		DrawText(m_pPixmap, pos, text, FONTWHITE);

		if ( m_errors[i] != 0 )
		{
			if ( m_errors[i] >= 100 )  // no ressource au lieu erreur ?
			{
				LoadString(m_errors[i], text, 50);
				pText = strchr(text, '\n');
				if ( pText != NULL )  strcpy(text, pText+1);
			}
			else
			{
				GetErr(m_errors[i]-1, text, 50);  // impossible ici
			}

			if ( m_nbCel.x > 1 && i < m_nbCel.y )
			{
//				if ( bLeft )
//				{
					pos.x = m_pos.x-4-GetTextWidth(text);  // texte à gauche
//				}
//				else
//				{
//					pos.x = m_pos.x+((i/m_nbCel.y)+1)+(DIMBUTTONX+MARGMENU)+4;
//				}
			}
			else
			{
//				if ( bRight )
//				{
					pos.x = m_pos.x+m_dim.x+4;  // texte à droite
//				}
//				else
//				{
//					pos.x = m_pos.x+(i/m_nbCel.y)*(DIMBUTTONX+MARGMENU)-4-GetTextWidth(text);
//				}
			}

			pos.y += DIMTEXTY;
			if ( m_errors[i] >= 100 )  // no ressource au lieu erreur ?
			{
				DrawText(m_pPixmap, pos, text, FONTWHITE);
			}
			else
			{
				DrawText(m_pPixmap, pos, text, FONTRED);
			}
		}
	}

	m_pPixmap->SetClipping(oldClip);
}

// Retourne le bouton sélectionné.

int CMenu::GetSel()
{
	if ( m_selRank == -1 )  return -1;

	return m_buttons[m_selRank];
}

// Retourne le rang sélectionné.

int CMenu::GetRank()
{
	return m_selRank;
}

// Retourne TRUE si le bouton sélectionné a une erreur.

BOOL CMenu::IsError()
{
	if ( m_selRank == -1 )  return TRUE;

	if ( m_errors[m_selRank] != 0  &&
		 m_errors[m_selRank] < 100 )  return TRUE;

	return FALSE;
}


// Indique si le menu existe.

BOOL CMenu::IsExist()
{
	return ( m_nbButtons == 0 ) ? FALSE:TRUE;
}


// Traitement d'un événement.

BOOL CMenu::TreatEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT		pos;

	if ( m_nbButtons == 0 )  return FALSE;

	pos = ConvLongToPos(lParam);

    switch( message )
    {
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if ( MouseDown(pos) )  return TRUE;
			break;

		case WM_MOUSEMOVE:
			if ( MouseMove(pos) )  return TRUE;
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			if ( MouseUp(pos) )  return TRUE;
			break;
	}

	return FALSE;
}

// Détecte dans quel bouton est la souris.

int CMenu::Detect(POINT pos)
{
	int		rank;

	if ( pos.x < m_pos.x || pos.x > m_pos.x+m_dim.x ||
		 pos.y < m_pos.y || pos.y > m_pos.y+m_dim.y )  return -1;

	rank  =  (pos.y-m_pos.y)/(DIMBUTTONY+MARGMENU);
	rank += ((pos.x-m_pos.x)/(DIMBUTTONX+MARGMENU))*m_nbCel.y;

	if ( rank >= m_nbButtons )  return -1;
	return rank;
}

// Bouton de la souris pressé.

BOOL CMenu::MouseDown(POINT pos)
{
	return FALSE;
}

// Souris déplacés.

BOOL CMenu::MouseMove(POINT pos)
{
	m_mousePos = pos;
	m_selRank = Detect(pos);

	if ( pos.x < m_pos.x-(DIMBUTTONX+MARGMENU) ||
		 pos.x > m_pos.x+m_dim.x+(DIMBUTTONX+MARGMENU) ||
		 pos.y < m_pos.y-(DIMBUTTONY+MARGMENU) ||
		 pos.y > m_pos.y+m_dim.y+(DIMBUTTONY+MARGMENU) )
	{
		Delete();  // enlève le menu si souris trop loin !
	}

	return FALSE;
}

// Bouton de la souris relâché.

BOOL CMenu::MouseUp(POINT pos)
{
	m_mousePos = pos;
	m_selRank = Detect(pos);

	return FALSE;
}

// Envoie le message.

void CMenu::Message()
{
	if ( m_selRank != -1 )
	{
		PostMessage(m_hWnd, WM_BUTTON0+m_selRank, 0, 0);
	}
}


