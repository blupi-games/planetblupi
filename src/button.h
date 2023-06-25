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

#pragma once

#include <vector>

#include "blupi.h"

class CSound;
class CDecor;
class CPixmap;

class CButton
{
public:
  CButton ();
  ~CButton ();

  bool Create (
    CPixmap * pPixmap, CSound * pSound, Point pos, Sint32 type, Sint32 * pMenu,
    Sint32 nbMenu, const char ** pToolTips, Sint32 region, Uint32 message,
    bool isRightReading);
  void Draw ();

  Sint32 GetState ();
  void   SetState (Sint32 state);

  Sint32 GetMenu ();
  void   SetMenu (Sint32 menu);

  bool GetEnable ();
  void SetEnable (bool bEnable);

  bool GetHide ();
  void SetHide (bool bHide);

  bool TreatEvent (const SDL_Event & event);
  bool MouseOnButton (Point pos);

  const char * GetToolTips (Point pos);

protected:
  bool Detect (Point pos);
  bool IsInMenu (const Point & pos);
  bool MouseDown (Point pos);
  bool MouseMove (Point pos);
  bool MouseUp (Point pos);

protected:
  CPixmap * m_pPixmap;
  CDecor *  m_pDecor;
  CSound *  m_pSound;

  Sint32 m_type;         // type de bouton
  bool   m_bEnable;      // true si bouton actif
  bool   m_bHide;        // true si bouton caché
  Uint32 m_message;      // message envoyé si bouton actionné
  Point  m_pos;          // up/left corner
  Point  m_dim;          // dimensions
  Sint32 m_state;        // 0=relâché, 1=pressé, +2=survollé
  Sint32 m_mouseState;   // 0=relâché, 1=pressé, +2=survollé
  Sint32 m_iconMenu[20]; // icônes du sous-menu

  const char ** m_toolTips; // info-bulles

  Sint32 m_nbMenu;     // nb de case du sous-menu
  Sint32 m_nbToolTips; // nb d'info-bulles
  Sint32 m_selMenu;    // sous-menu sélectionné

  bool m_bMouseDown; // true -> bouton souris pressé
};
