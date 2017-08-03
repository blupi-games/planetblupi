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

#pragma once

class CPixmap;
class CDecor;
class CSound;

class CJauge
{
public:
  CJauge ();
  ~CJauge ();

  bool Create (CPixmap * pPixmap, CSound * pSound, POINT pos, Sint32 type);
  void Draw ();

  void SetLevel (Sint32 level);
  void SetType (Sint32 type);

  bool GetHide ();
  void SetHide (bool bHide);

  POINT GetPos ();

protected:
  CPixmap * m_pPixmap;
  CDecor *  m_pDecor;
  CSound *  m_pSound;
  bool      m_bHide; // true si bouton caché
  POINT     m_pos;   // coin sup/gauche
  POINT     m_dim;   // dimensions
  Sint32    m_type;
  Sint32    m_level;
};

/////////////////////////////////////////////////////////////////////////////
