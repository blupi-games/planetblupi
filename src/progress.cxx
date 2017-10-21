/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017, Mathieu Schroeter
 * http://epsitec.ch; http://www.blupi.org; http://github.com/blupi-games
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include <stdio.h>
#include <stdlib.h>

#include "decor.h"
#include "def.h"
#include "misc.h"
#include "pixmap.h"
#include "progress.h"
#include "sound.h"

CJauge::CJauge ()
{
  m_type    = 0;
  m_bHide   = true;
  m_pPixmap = nullptr;
  m_pDecor  = nullptr;
  m_pSound  = nullptr;
}

CJauge::~CJauge ()
{
}

// Crée un nouveau bouton.

bool
CJauge::Create (CPixmap * pPixmap, CSound * pSound, Point pos, Sint32 type)
{
  m_pPixmap = pPixmap;
  m_pSound  = pSound;
  m_type    = type;
  m_bHide   = true;
  m_pos     = pos;
  m_dim.x   = DIMJAUGEX;
  m_dim.y   = DIMJAUGEY;
  m_level   = 0;

  return true;
}

// Dessine un bouton dans son état.

void
CJauge::Draw ()
{
  Sint32 part;
  Rect   rect;

  if (m_bHide) // bouton caché ?
  {
    rect.left   = m_pos.x;
    rect.right  = m_pos.x + m_dim.x;
    rect.top    = m_pos.y;
    rect.bottom = m_pos.y + m_dim.y;
    m_pPixmap->DrawPart (-1, CHBACK, m_pos, rect); // dessine le fond
    return;
  }

  part = (m_level * (DIMJAUGEX - 6 - 4)) / 100;

  rect.left   = 0;
  rect.right  = DIMJAUGEX;
  rect.top    = DIMJAUGEY * 0;
  rect.bottom = DIMJAUGEY * 1;
  m_pPixmap->DrawPart (-1, CHJAUGE, m_pos, rect); // partie noire

  if (part > 0)
  {
    rect.left   = 0;
    rect.right  = 6 + part;
    rect.top    = DIMJAUGEY * m_type;
    rect.bottom = DIMJAUGEY * (m_type + 1);
    m_pPixmap->DrawPart (-1, CHJAUGE, m_pos, rect); // partie colorée
  }
}

// Modifie le niveau.

void
CJauge::SetLevel (Sint32 level)
{
  if (level < 0)
    level = 0;
  if (level > 100)
    level = 100;

  m_level = level;
}

// Modifie le type.

void
CJauge::SetType (Sint32 type)
{
  m_type = type;
}

bool
CJauge::GetHide ()
{
  return m_bHide;
}

void
CJauge::SetHide (bool bHide)
{
  m_bHide = bHide;
}

Point
CJauge::GetPos ()
{
  return m_pos;
}
