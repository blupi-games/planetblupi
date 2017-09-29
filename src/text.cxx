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

#include "def.h"
#include "pixmap.h"
#include "text.h"

/**
 * \brief Return the character offset for the sprite.
 *
 * \param[in] c - The character (incremented if 0xC3 UTF-8).
 * \returns the offset.
 */
static Sint32
GetOffset (const char *& c)
{
  /* clang-format off */
  static const unsigned char table_accents[] = {
  /*  ü     à     â     é     è     ë     ê     ï            */
    0xBC, 0xA0, 0xA2, 0xA9, 0xA8, 0xAB, 0xAA, 0xAF, /* UTF-8 */
  /*  î     ô     ù     û     ä     ö     ç                  */
    0xAE, 0xB4, 0xB9, 0xBB, 0xA4, 0xB6, 0xA7,       /* UTF-8 */
  };

  static const unsigned char table_extended[] = {
  /*  ò     ì                                                */
    0xB2, 0xAC,                                     /* UTF-8 */
  };
  /* clang-format on */

  if (static_cast<unsigned char> (*c) == 0xC3)
    c++;

  for (unsigned int i = 0; i < countof (table_accents); ++i)
    if ((unsigned char) *c == table_accents[i])
      return 15 + i;

  for (unsigned int i = 0; i < countof (table_extended); ++i)
    if ((unsigned char) *c == table_extended[i])
      return 127 + i;

  if (*c < 0)
    return 1; // square

  return *c;
}

/**
 * \brief Return the character length.
 *
 * \param[in] c - The character (can be incremented).
 * \param[in] font - The font used (little or normal).
 * \returns the length.
 */
Sint32
GetCharWidth (const char *& c, Sint32 font)
{
  // clang-format off
  static const unsigned char table_width[] =
  {
     9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  8,
     9,  9,  8,  8,  8,  8,  5,  5,  8,  8,  8,  9,  8,  8, 10, 10,
     5,  6,  9, 13, 11, 12, 12,  6,  6,  6, 12, 12,  5,  9,  6,  9,
     8,  8,  9,  9,  8,  9,  8,  8,  9,  9,  6,  6,  8,  9, 10, 11,
    12,  8,  9,  9,  9,  8,  8,  8,  9,  4,  8,  9,  8, 10,  9,  9,
     8,  9,  8,  9, 10,  8,  9, 11,  9,  8, 10,  7, 10,  7, 13, 13,
     9,  9,  8,  8,  8,  8,  6,  8,  8,  4,  6,  8,  4, 12,  8,  8,
     8,  8,  7,  6,  7,  8,  8, 10,  8,  8,  7,  6,  6,  6, 10,  8,
     5,
  };

  static const unsigned char table_width_little[] =
  {
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  6,  6,  7,
     6,  6,  6,  6,  6,  6,  3,  3,  6,  6,  6,  6,  6,  6,  5,  5,
     3,  3,  5,  8,  5, 11,  9,  3,  4,  4,  6,  6,  3,  4,  3,  6,
     5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  3,  3,  7,  6,  7,  6,
     9,  8,  6,  7,  7,  5,  5,  8,  7,  2,  4,  7,  5, 10,  7,  8,
     6,  8,  7,  6,  6,  6,  8, 12,  7,  6,  6,  3,  5,  3,  6,  8,
     4,  6,  6,  6,  6,  6,  4,  6,  6,  2,  3,  5,  2, 10,  6,  6,
     6,  6,  3,  5,  3,  6,  6,  8,  6,  6,  5,  4,  6,  4,  7,  6,
     3,
  };
  // clang-format on

  if (font == FONTLITTLE)
    return table_width_little[GetOffset (c)];
  else
    return table_width[GetOffset (c)] - 1;
}

/**
 * \brief Draw a text in a pixmap to a specific position.
 *
 * \param[in] pPixmap - The pixmap where it must be drawn.
 * \param[in] pos - The coordinates for the text.
 * \param[in] pText - The text.
 * \param[in] font - The font style (little or normal).
 */
void
DrawText (CPixmap * pPixmap, Point pos, const char * pText, Sint32 font)
{
  Sint32 rank;

  while (*pText != '\0')
  {
    rank = GetOffset (pText);

    if (font != FONTLITTLE)
    {
      rank += (128 + 16) * font;
      pPixmap->DrawIcon (-1, CHTEXT, rank, pos);
    }
    else
      pPixmap->DrawIcon (-1, CHLITTLE, rank, pos);

    pos.x += GetCharWidth (pText, font);
    pText++;
  }
}

// Affiche un texte penché.

void
DrawTextPente (
  CPixmap * pPixmap, Point pos, const char * pText, Sint32 pente, Sint32 font)
{
  Sint32 rank, lg, rel, start;

  start = pos.y;
  rel   = 0;
  while (*pText != 0)
  {
    rank = GetOffset (pText);
    rank += (128 + 16) * font;
    pPixmap->DrawIcon (-1, CHTEXT, rank, pos);

    lg = GetCharWidth (pText, font);
    pText++;
    rel += lg;
    pos.x += lg;
    pos.y = start + rel / pente;
  }
}

// Affiche un pavé de texte.
// Une ligne vide est affichée avec un demi interligne !
// Si part != -1, n'affiche que les lignes qui commencent
// par "n|", avec n=part.

void
DrawTextRect (
  CPixmap * pPixmap, Point pos, char * pText, Sint32 pente, Sint32 font,
  Sint32 part)
{
  char   text[100];
  char * pDest;
  Sint32 itl;

  if (font == FONTLITTLE)
    itl = DIMLITTLEY;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest = text;
    if (text[0] != 0 && text[1] == '|') // commence par "n|" ?
    {
      if (part != -1 && part != text[0] - '0')
        continue;
      pDest += 2; // saute "n|"
    }
    else
    {
      if (part != -1)
        continue;
    }

    if (pente == 0)
      DrawText (pPixmap, pos, pDest, font);
    else
      DrawTextPente (pPixmap, pos, pDest, pente, font);

    if (pDest[0] == 0) // ligne vide ?
    {
      pos.y += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      pos.y += itl; // passe à la ligne suivante
    }
  }
}

// Affiche un texte centré pouvant éventuellement
// contenir plusieurs lignes séparées par des '\n'.

void
DrawTextCenter (CPixmap * pPixmap, Point pos, const char * pText, Sint32 font)
{
  char   text[100];
  char * pDest;
  Sint32 itl;
  Point  start;

  if (font == FONTLITTLE)
    itl = DIMLITTLEY;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest   = text;
    start.x = pos.x - GetTextWidth (pDest) / 2;
    start.y = pos.y;
    DrawText (pPixmap, start, pDest, font);

    if (pDest[0] == 0) // ligne vide ?
    {
      pos.y += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      pos.y += itl; // passe à la ligne suivante
    }
  }
}

// Retourne la hauteur d'un texte.

Sint32
GetTextHeight (char * pText, Sint32 font, Sint32 part)
{
  char   text[100];
  char * pDest;
  Sint32 itl;
  Sint32 h = 0;

  if (font == FONTLITTLE)
    itl = DIMLITTLEY;
  else
    itl = DIMTEXTY;

  while (*pText != 0)
  {
    pDest = text;
    while (*pText != 0 && *pText != '\r' && *pText != '\n')
      *pDest++ = *pText++;
    *pDest = 0;
    if (*pText == '\r')
      pText++; // saute '\r'
    if (*pText == '\n')
      pText++; // saute '\n'

    pDest = text;
    if (text[0] != 0 && text[1] == '|') // commence par "n|" ?
    {
      if (part != -1 && part != text[0] - '0')
        continue;
      pDest += 2; // saute "n|"
    }
    else
    {
      if (part != -1)
        continue;
    }

    if (pDest[0] == 0) // ligne vide ?
    {
      h += itl / 2; // descend de 1/2 ligne
    }
    else
    {
      h += itl; // passe à la ligne suivante
    }
  }

  return h;
}

// Retourne la longueur d'un texte.

Sint32
GetTextWidth (const char * pText, Sint32 font)
{
  Sint32 width = 0;

  while (*pText != 0)
  {
    width += GetCharWidth (pText, font);
    pText++;
  }

  return width;
}

// Retourne la longueur d'un grand chiffre.

void
GetBignumInfo (Sint32 num, Sint32 & start, Sint32 & lg)
{
  static Sint32 table[11] = {0, 53, 87, 133, 164, 217, 253, 297, 340, 382, 426};

  start = table[num];
  lg    = table[num + 1] - table[num];
}

// Affiche un grand nombre.

void
DrawBignum (CPixmap * pPixmap, Point pos, Sint32 num)
{
  char   string[10];
  Sint32 i = 0;
  Sint32 start, lg;
  Rect   rect;

  snprintf (string, sizeof (string), "%d", num);

  rect.top    = 0;
  rect.bottom = 52;
  while (string[i] != 0)
  {
    GetBignumInfo (string[i] - '0', start, lg);

    rect.left  = start;
    rect.right = start + lg;
    pPixmap->DrawPart (-1, CHBIGNUM, pos, rect);
    pos.x += lg + 4;

    i++;
  }
}

// Retourne la longueur d'un grand nombre.

Sint32
GetBignumWidth (Sint32 num)
{
  char   string[10];
  Sint32 i = 0;
  Sint32 start, lg;
  Sint32 width = -4;

  snprintf (string, sizeof (string), "%d", num);

  while (string[i] != 0)
  {
    GetBignumInfo (string[i] - '0', start, lg);
    width += lg + 4;
    i++;
  }

  return width;
}
