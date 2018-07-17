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

#include <SDL_stdinc.h>
#include <stdlib.h>

#include "fifo.h"

// gestion d'une pile classée en valeur croissantes
// typiquement reprend les coordonnées les plus proches
// du but en premier lieu

CPileTriee::CPileTriee (Sint32 taille)
{
  m_taille = taille;
  m_max = m_out = 0;
  m_data        = (Element *) malloc (sizeof (Element) * taille);
}

CPileTriee::~CPileTriee ()
{
  free (m_data);
}

Sint32
CPileTriee::get ()
{
  if (m_out == m_max)
    return -1;
  Sint32 val = m_data[m_out].pos;
  m_out++;
  if (m_out >= m_taille)
    m_out = 0;
  return val;
}

void
CPileTriee::put (Sint32 pos, Sint32 dist)
{
  Sint32 i = m_out;
  Sint32 p, d, m;

  while (i != m_max)
  {
    // le point est-il plus proche que celui-là ?
    if (dist < m_data[i].dist)
    {
      // oui, insert et décale le suivant
      p = m_data[i].pos;
      d = m_data[i].dist;

      m_data[i].pos  = pos;
      m_data[i].dist = dist;

      pos  = p;
      dist = d;
    }
    i++;
    if (i >= m_taille)
      i = 0;
  }

  // ajoute le point éloigné à la suite
  m = m_max + 1;
  if (m >= m_taille)
    m = 0;
  if (m != m_out)
  {
    m_data[m_max].pos  = pos;
    m_data[m_max].dist = dist;

    m_max = m;
  }
}
