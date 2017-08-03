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

// traitement d'une liste en fifo

// stucture pour enpiler des positions
// en fonction de leur distance à la cible
typedef struct {
  Sint32 pos;
  Sint32 dist;
} Element;

// traitement d'une pile triée

class CPileTriee
{
private:
  Sint32    m_taille; // nombre de polongs max
  Sint32    m_max;    // position limite
  Sint32    m_out;    // position pour reprendre
  Element * m_data;   // données

public:
  CPileTriee (Sint32 taille);
  ~CPileTriee ();

  void   put (Sint32 pos, Sint32 dist);
  Sint32 get ();
};
