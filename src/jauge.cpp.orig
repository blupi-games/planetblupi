// Jauge.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include "def.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "jauge.h"
#include "misc.h"



/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CJauge::CJauge()
{
	m_type            = 0;
	m_bHide           = true;
	m_bMinimizeRedraw = false;
	m_bRedraw         = false;
}

// Destructeur.

CJauge::~CJauge()
{
}


// Crée un nouveau bouton.

bool CJauge::Create(CPixmap *pPixmap, CSound *pSound,
					POINT pos, Sint32 type, bool bMinimizeRedraw)
{
	m_pPixmap         = pPixmap;
	m_pSound          = pSound;
	m_type            = type;
	m_bMinimizeRedraw = bMinimizeRedraw;
	m_bHide           = true;
	m_pos             = pos;
	m_dim.x           = DIMJAUGEX;
	m_dim.y           = DIMJAUGEY;
	m_level           = 0;
	m_bRedraw         = true;

	return true;
}

// Dessine un bouton dans son état.

void CJauge::Draw()
{
	Sint32			part;
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

	part = (m_level*(DIMJAUGEX-6-4))/100;

	rect.left   = 0;
	rect.right  = DIMJAUGEX;
	rect.top    = DIMJAUGEY*0;
	rect.bottom = DIMJAUGEY*1;
	m_pPixmap->DrawPart(-1, CHJAUGE, m_pos, rect);  // partie noire

	if ( part > 0 )
	{
		rect.left   = 0;
		rect.right  = 6+part;
		rect.top    = DIMJAUGEY*m_type;
		rect.bottom = DIMJAUGEY*(m_type+1);
		m_pPixmap->DrawPart(-1, CHJAUGE, m_pos, rect);  // partie colorée
	}
}

// Redessine une jauge.

void CJauge::Redraw()
{
	m_bRedraw = true;
}

// Modifie le niveau.

void CJauge::SetLevel(Sint32 level)
{
	if ( level <   0 )  level =   0;
	if ( level > 100 )  level = 100;

	if ( m_level != level )
	{
		m_bRedraw = true;
	}

	m_level = level;
}

// Modifie le type.

void CJauge::SetType(Sint32 type)
{
	if ( m_type != type )
	{
		m_bRedraw = true;
	}

	m_type = type;
}


bool CJauge::GetHide()
{
	return m_bHide;
}

void CJauge::SetHide(bool bHide)
{
	if ( m_bHide != bHide )
	{
		m_bRedraw = true;
	}

	m_bHide = bHide;
}

POINT CJauge::GetPos()
{
	return m_pos;
}

void CJauge::SetRedraw()
{
	m_bRedraw = true;
}

