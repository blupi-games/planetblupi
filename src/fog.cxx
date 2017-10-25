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

#include "decor.h"

// clang-format off
// Cette table indique les quarts de cases contenant du
// brouillard lorsque la valeur est à un.
//      0  1
//      2  3
static char tableFog[15 * 4] =
{
    1, 1, 1, 0, // 0
    1, 1, 0, 0, // 1
    1, 1, 0, 1, // 2
    1, 0, 1, 0, // 3
    1, 1, 1, 1, // 4
    0, 1, 0, 1, // 5
    1, 0, 1, 1, // 6
    0, 0, 1, 1, // 7
    0, 1, 1, 1, // 8
    0, 0, 0, 1, // 9
    0, 0, 1, 0, // 10
    1, 0, 0, 1, // 11
    0, 1, 0, 0, // 12
    1, 0, 0, 0, // 13
    0, 1, 1, 0, // 14
};
// clang-format on

// Retourne les bits contenant du brouillard.

bool
GetFogBits (Sint32 icon, char * pBits)
{
  pBits[0] = 0;
  pBits[1] = 0;
  pBits[2] = 0;
  pBits[3] = 0;

  if (icon < 0 || icon >= 15)
    return true;

  pBits[0] = tableFog[icon * 4 + 0];
  pBits[1] = tableFog[icon * 4 + 1];
  pBits[2] = tableFog[icon * 4 + 2];
  pBits[3] = tableFog[icon * 4 + 3];

  return true;
}

// Retourne l'icône correspondant aux bits de brouillard.

Sint32
GetFogIcon (char * pBits)
{
  Sint32 i;

  for (i = 0; i < 15; i++)
  {
    if (
      tableFog[i * 4 + 0] == pBits[0] && tableFog[i * 4 + 1] == pBits[1] &&
      tableFog[i * 4 + 2] == pBits[2] && tableFog[i * 4 + 3] == pBits[3])
      return i;
  }

  return -1;
}

// Table donnant la "vision" d'un blupi dans le
// brouillard.
// clang-format off
static Sint8 table_fog[17 * 17] =
{
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  4,  2,  4,  4,  4,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  1, -1,  5,  4,  4,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  1, -1, -1, -1,  5,  4,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  1, -1, -1, -1, -1, -1,  5,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  1, -1, -1, -1, -1, -1, -1, -1,  5,  4,  4,  4,  4,  4,
  4,  4,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  5,  4,  4,  4,  4,
  4,  0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  5,  4,  4,  4,
  4,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  5,  4,  4,
  4,  4,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  8,  4,
  4,  4,  4,  4,  3, -1, -1, -1, -1, -1, -1, -1, -1, -1,  7,  4,  4,
  4,  4,  4,  4,  4,  3, -1, -1, -1, -1, -1, -1, -1,  7,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  3, -1, -1, -1, -1, -1,  7,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  4,  3, -1, -1, -1,  7,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  4,  4,  3, -1,  7,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  4,  4,  4,  6,  4,  4,  4,  4,  4,  4,  4,
  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
};
// clang-format on

// Ecarte le brouillard autour d'un blupi.

void
CDecor::BlupiPushFog (Sint32 rank)
{
  Sint32 x, y, i;
  Point  cel;
  char   cBits[4];
  char   nBits[4];

  if (m_blupi[rank].perso != 0 && m_blupi[rank].perso != 8)
    return;

  for (y = 0; y < 17; y++)
  {
    for (x = 0; x < 17; x++)
    {
      if (x % 2 != y % 2)
        continue;
      if (table_fog[x + y * 17] == FOGHIDE)
        continue;

      cel.x = (x + ((m_blupi[rank].cel.x + 1) / 4) * 2 - 8) * 2;
      cel.y = (y + ((m_blupi[rank].cel.y + 1) / 4) * 2 - 8) * 2;

      // Ne pas utiliser IsValid pour permettre d'aller
      // jusqu'au bord !
      if (cel.x >= 0 && cel.x < MAXCELX && cel.y >= 0 && cel.y < MAXCELX)
      {
        if (m_decor[cel.x / 2][cel.y / 2].fog != -1)
        {
          GetFogBits (m_decor[cel.x / 2][cel.y / 2].fog, cBits);
          GetFogBits (table_fog[x + y * 17], nBits);

          for (i = 0; i < 4; i++)
          {
            nBits[i] &= cBits[i]; // "ou" visibilité
          }

          m_decor[cel.x / 2][cel.y / 2].fog = GetFogIcon (nBits);
        }
      }
    }
  }
}
