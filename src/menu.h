/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017, Mathieu Schroeter
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

#include <unordered_map>

#include "def.h"

class CEvent;

class CMenu
{
public:
  CMenu ();
  ~CMenu ();

  bool Create (
    CPixmap * pPixmap, CSound * pSound, CEvent * pEvent, Point pos, Sint32 nb,
    Buttons * pButtons, Errors * pErrors,
    std::unordered_map<Sint32, const char *> & texts, Sint32 perso);
  void Update (
    Sint32 nb, Buttons * pButtons, Errors * pErrors,
    std::unordered_map<Sint32, const char *> & texts);
  void   Delete ();
  void   Draw ();
  Sint32 GetSel ();
  Sint32 GetRank ();
  bool   IsError ();
  bool   IsExist ();
  void   Message ();

  bool TreatEvent (const SDL_Event & event);

protected:
  Sint32 Detect (Point pos);
  bool   MouseDown (Point pos);
  bool   MouseMove (Point pos);
  bool   MouseUp (Point pos);

protected:
  CPixmap *                                m_pPixmap;
  CDecor *                                 m_pDecor;
  CSound *                                 m_pSound;
  CEvent *                                 m_pEvent;
  Point                                    m_pos; // up/left corner
  Point                                    m_dim; // dimensions
  Sint32                                   m_nbButtons;
  Point                                    m_nbCel;
  Sint32                                   m_perso;
  Sint32                                   m_buttons[MAXBUTTON];
  Errors                                   m_errors[MAXBUTTON];
  std::unordered_map<Sint32, const char *> m_texts;
  Uint32                                   m_messages[MAXBUTTON];
  Sint32                                   m_selRank;
};

/////////////////////////////////////////////////////////////////////////////
