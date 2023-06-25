/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2019, Mathieu Schroeter
 * https://epsitec.ch; https://www.blupi.org; https://github.com/blupi-games
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

#include "button.h"
#include "decor.h"
#include "def.h"
#include "event.h"
#include "gettext.h"
#include "misc.h"
#include "pixmap.h"
#include "sound.h"

CButton::CButton ()
{
  m_type       = 0;
  m_bEnable    = true;
  m_bHide      = false;
  m_state      = 0;
  m_mouseState = 0;
  m_nbMenu     = 0;
  m_nbToolTips = 0;
  m_selMenu    = 0;
  m_bMouseDown = false;
  m_message    = static_cast<Uint32> (-1);
  m_pPixmap    = nullptr;
  m_pDecor     = nullptr;
  m_pSound     = nullptr;
  m_toolTips   = nullptr;
}

CButton::~CButton () {}

// Crée un nouveau bouton.

bool
CButton::Create (
  CPixmap * pPixmap, CSound * pSound, Point pos, Sint32 type, Sint32 * pMenu,
  Sint32 nbMenu, const char ** pToolTips, Sint32 region, Uint32 message,
  bool isRightReading)
{
  Point  iconDim;
  Sint32 i, icon;

  static Sint32 ttypes[] = {
    DIMBUTTONX, DIMBUTTONY, // button00.bmp
  };

  if (type < 0 || type > 0)
    return false;

  iconDim.x = ttypes[type * 2 + 0];
  iconDim.y = ttypes[type * 2 + 1];

  m_pPixmap    = pPixmap;
  m_pSound     = pSound;
  m_type       = type;
  m_bEnable    = true;
  m_bHide      = false;
  m_message    = message;
  m_pos        = pos;
  m_dim        = iconDim;
  m_nbMenu     = nbMenu;
  m_selMenu    = 0;
  m_state      = 0;
  m_mouseState = 0;
  m_bMouseDown = false;

  m_nbToolTips = 0;
  while (pToolTips[m_nbToolTips])
    ++m_nbToolTips;

  for (i = 0; i < nbMenu; i++)
  {
    icon = pMenu[i];

    if (isRightReading)
    {
      if (icon == 51) // right arrow
        icon = 50;
      else if (icon == 50) // left arrow
        icon = 51;
      else if (icon == 40) // stop
        icon = 109;
    }

    if (region == 1) // palmiers ?
    {
      if (icon == 0)
        icon = 90; // sol normal
      if (icon == 1)
        icon = 91; // sol inflammable
      if (icon == 2)
        icon = 92; // sol inculte
      if (icon == 7)
        icon = 9; // plante
      if (icon == 8)
        icon = 10; // arbre
    }

    if (region == 2) // hiver ?
    {
      if (icon == 0)
        icon = 96; // sol normal
      if (icon == 1)
        icon = 97; // sol inflammable
      if (icon == 2)
        icon = 98; // sol inculte
      if (icon == 8)
        icon = 99; // arbre
    }

    if (region == 3) // sapin ?
    {
      if (icon == 0)
        icon = 102; // sol normal
      if (icon == 1)
        icon = 103; // sol inflammable
      if (icon == 2)
        icon = 104; // sol inculte
      if (icon == 8)
        icon = 105; // arbre
    }

    m_iconMenu[i] = icon;
  }

  m_toolTips = pToolTips;

  return true;
}

// Dessine un bouton dans son état.

void
CButton::Draw ()
{
  Sint32 i;
  Point  pos = this->m_pos;
  Rect   rect;

  if (m_bHide) // bouton caché ?
  {
    rect.left   = pos.x;
    rect.right  = pos.x + m_dim.x;
    rect.top    = pos.y;
    rect.bottom = pos.y + m_dim.y;
    m_pPixmap->DrawPart (-1, CHBACK, pos, rect); // dessine le fond
    return;
  }

  if (m_bEnable) // bouton actif ?
    m_pPixmap->DrawIcon (-1, CHBUTTON + m_type, m_mouseState, pos);
  else
    m_pPixmap->DrawIcon (-1, CHBUTTON + m_type, 4, pos);

  if (m_nbMenu == 0)
    return;

  if (m_nbMenu > 0)
  {
    m_pPixmap->DrawIcon (-1, CHBUTTON + m_type, m_iconMenu[m_selMenu] + 6, pos);
  }

  if (m_nbMenu == 1 || !m_bEnable || !m_bMouseDown)
    return;

  pos.x += IsRightReading () ? -m_dim.x - 2 : m_dim.x + 2;
  for (i = 0; i < m_nbMenu; i++)
  {
    m_pPixmap->DrawIcon (-1, CHBUTTON + m_type, i == m_selMenu ? 1 : 0, pos);
    m_pPixmap->DrawIcon (-1, CHBUTTON + m_type, m_iconMenu[i] + 6, pos);
    pos.x += IsRightReading () ? -m_dim.x - 1 : m_dim.x - 1;
  }
}

Sint32
CButton::GetState ()
{
  return m_state;
}

void
CButton::SetState (Sint32 state)
{
  m_state      = state;
  m_mouseState = state;
}

Sint32
CButton::GetMenu ()
{
  return m_selMenu;
}

void
CButton::SetMenu (Sint32 menu)
{
  m_selMenu = menu;
}

bool
CButton::GetEnable ()
{
  return m_bEnable;
}

void
CButton::SetEnable (bool bEnable)
{
  m_bEnable = bEnable;
}

bool
CButton::GetHide ()
{
  return m_bHide;
}

void
CButton::SetHide (bool bHide)
{
  m_bHide = bHide;
}

// Traitement d'un événement.

bool
CButton::TreatEvent (const SDL_Event & event)
{
  Point pos;

  if (m_bHide || !m_bEnable)
    return false;

  // pos = ConvLongToPos(lParam);

  switch (event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
    if (
      event.button.button != SDL_BUTTON_LEFT &&
      event.button.button != SDL_BUTTON_RIGHT)
      break;

    pos.x = event.button.x;
    pos.y = event.button.y;
    if (MouseDown (pos))
      return true;
    break;

  case SDL_MOUSEMOTION:
    pos.x = event.motion.x;
    pos.y = event.motion.y;
    if (MouseMove (pos))
      return true;
    break;

  case SDL_MOUSEBUTTONUP:
    if (
      event.button.button != SDL_BUTTON_LEFT &&
      event.button.button != SDL_BUTTON_RIGHT)
      break;

    pos.x = event.button.x;
    pos.y = event.button.y;
    if (MouseUp (pos)) // (*)
      return false;
    break;
  }

  return false;
}

// (*) Tous les boutons doivent recevoir l'événement BUTTONUP !

// Indique si la souris est sur ce bouton.

bool
CButton::MouseOnButton (Point pos)
{
  return Detect (pos);
}

bool
CButton::IsInMenu (const Point & pos)
{
  Sint32 width = m_dim.x;

  if (m_nbMenu > 1 && m_bMouseDown) // submenu expanded ?
    width += 2 + (m_dim.x + (IsRightReading () ? 1 : -1)) * m_nbMenu;

  if (IsRightReading () && m_nbMenu > 1 && m_bMouseDown)
  {
    if (pos.x > m_pos.x + m_dim.x || pos.x < m_pos.x - width + m_dim.x)
      return false;
  }
  else
  {
    if (pos.x < m_pos.x || pos.x > m_pos.x + width)
      return false;
  }

  if (pos.y < m_pos.y || pos.y > m_pos.y + m_dim.y)
    return false;

  return true;
}

// Retourne le tooltips pour un bouton, en fonction
// de la position de la souris.

const char *
CButton::GetToolTips (Point pos)
{
  Sint32 rank;

  if (m_bHide || !m_bEnable)
    return nullptr;

  if (!this->IsInMenu (pos))
    return nullptr;

  rank = (pos.x - (m_pos.x + 2 + 1)) / (m_dim.x - 1);
  if (rank < 0)
    rank = 0;
  if (rank > m_nbToolTips)
    return nullptr;

  if (m_nbMenu > 1)
  {
    if (m_bMouseDown && rank > 0)
      rank--;
    else
      rank = m_selMenu;
  }

  return gettext (m_toolTips[rank]);
}

// Détecte si la souris est dans le bouton.

bool
CButton::Detect (Point pos)
{
  if (m_bHide || !m_bEnable)
    return false;

  return this->IsInMenu (pos);
}

// Bouton de la souris pressé.

bool
CButton::MouseDown (Point pos)
{
  if (!Detect (pos))
    return false;

  m_mouseState = 1;
  m_bMouseDown = true;

  CEvent::PushUserEvent (EV_UPDATE);

  m_pSound->PlayImage (SOUND_CLICK, pos);
  return true;
}

// Souris déplacés.

bool
CButton::MouseMove (Point pos)
{
  bool   bDetect;
  Sint32 iState, iMenu;

  iState = m_mouseState;
  iMenu  = m_selMenu;

  bDetect = Detect (pos);

  if (m_bMouseDown)
  {
    if (bDetect)
      m_mouseState = 1; // pressé
    else
      m_mouseState = m_state;
  }
  else
  {
    if (bDetect)
      m_mouseState = m_state + 2; // survollé
    else
      m_mouseState = m_state;
  }

  if (
    m_nbMenu > 1 && m_bMouseDown &&
    (IsRightReading () ? pos.x < m_pos.x
                       : pos.x > m_pos.x + m_dim.x + 2)) // in sub-menu ?
  {
    m_selMenu = IsRightReading ()
                  ? (m_pos.x - 2 - pos.x) / (m_dim.x + 1)
                  : (pos.x - (m_pos.x + m_dim.x + 2)) / (m_dim.x - 1);
    if (m_selMenu >= m_nbMenu)
      m_selMenu = m_nbMenu - 1;
  }

  if (iState != m_mouseState || iMenu != m_selMenu)
    CEvent::PushUserEvent (EV_UPDATE);

  return m_bMouseDown;
}

// Bouton de la souris relâché.

bool
CButton::MouseUp (Point pos)
{
  bool bDetect;

  bDetect = Detect (pos);

  m_mouseState = m_state;
  m_bMouseDown = false;

  if (!bDetect)
    return false;

  if (m_message != static_cast<Uint32> (-1))
    CEvent::PushUserEvent (m_message);

  return true;
}
