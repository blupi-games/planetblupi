/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2019, Mathieu Schroeter
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

#include "blupi.h"
#include "def.h"
#include "event.h"
#include "misc.h"
#include "pixmap.h"
#include "text.h"

/**
 * \brief Return the character offset for the sprite.
 *
 * \param[in] c - The character (incremented if 0xC3 or 0xC4 or 0xC5 UTF-8).
 * \returns the offset.
 */
static Uint8
GetOffset (const char *& c)
{
  /* clang-format off */
  static const unsigned char table_c3[] = {
  /*  ü     à     â     é     è     ë     ê     ï   */
    0xBC, 0xA0, 0xA2, 0xA9, 0xA8, 0xAB, 0xAA, 0xAF,
  /*  î     ô     ù     û     ä     ö     ç     ò   */
    0xAE, 0xB4, 0xB9, 0xBB, 0xA4, 0xB6, 0xA7, 0xB2,
  /*  ì     ó     Ç     Ö     Ü                     */
    0xAC, 0xB3, 0x87, 0x96, 0x9C,
  };

  static const unsigned char table_c4[] = {
  /*  ę     ć     ą     Ğ     ğ     İ     ı         */
    0x99, 0x87, 0x85, 0x9E, 0x9F, 0xB0, 0xB1,
  };

  static const unsigned char table_c5[] = {
  /*  ń     ź     ż     ł     ś     Ş     ş         */
    0x84, 0xBA, 0xBC, 0x82, 0x9B, 0x9E, 0x9F,
  };

  static const unsigned char table_d7[] = {
  /*a ז     ו     ה     ד    ג     ב     א        */
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
  /*a מ     ל     ך     כ    י     ט     ח        */
    0x97, 0x98, 0x99, 0x9B, 0x9A, 0x9C, 0x9E,
  /*a ף     פ     ע     ס    ן     נ     ם        */
    0x9D, 0xA0, 0x9F, 0xA1, 0xA2, 0xA4, 0xA3,
  /*a ת     ש    ר     ק     ץ     צ              */
    0xA6, 0xA5, 0xA7, 0xA8, 0xA9, 0xAA,
  };
  /* clang-format on */

  int                   offset = 0;
  int                   inc    = 0;
  size_t                size   = 0;
  const unsigned char * table  = nullptr;

  switch (static_cast<unsigned char> (*c))
  {
  case 0xC3:
    offset = 128;
    table  = table_c3;
    size   = countof (table_c3);
    inc    = 1;
    break;
  case 0xC4:
    offset = 128 + 32;
    table  = table_c4;
    size   = countof (table_c4);
    inc    = 1;
    break;
  case 0xC5:
    offset = 128 + 64;
    table  = table_c5;
    size   = countof (table_c5);
    inc    = 1;
    break;
  case 0xD7:
    offset = 128 + 96;
    table  = table_d7;
    size   = countof (table_d7);
    inc    = 1;
    break;
  }

  for (size_t i = 0; i < size; ++i)
    if (static_cast<unsigned char> (*(c + inc)) == table[i])
      return offset + i;

  if (*(c + inc) < 0)
    return 1; // square

  return *(c + inc);
}

/**
 * \brief Return the character length.
 *
 * \param[in] c - The character (can be incremented).
 * \param[in] font - The font used (little or normal).
 * \returns the length.
 */
Uint8
GetCharWidth (const char *& c, Sint32 font)
{
  // clang-format off
  static const Uint8 table_width[] =
  {
     9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 10, 10,
     5,  6,  9, 13, 11, 12, 12,  6,  6,  6, 12, 12,  5,  9,  6,  9,
     8,  8,  9,  9,  8,  9,  8,  8,  9,  9,  6,  6,  8,  9, 10, 11,
    12,  8,  9,  9,  9,  8,  8,  8,  9,  4,  8,  9,  8, 10,  9,  9,
     8,  9,  8,  9, 10,  8,  9, 11,  9,  8, 10,  7, 10,  7, 13, 13,
     9,  9,  8,  8,  8,  8,  6,  8,  8,  4,  6,  8,  4, 12,  8,  8,
     8,  8,  7,  6,  7,  8,  8, 10,  8,  8,  7,  6,  6,  6, 10,  0,
     8,  9,  9,  8,  8,  8,  8,  5,  5,  8,  8,  8,  9,  8,  8,  8,
     5,  8,  9,  9,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     8,  8,  9,  8,  8,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     8,  8,  7,  6,  7,  9,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    10, 10,  9,  9, 10,  6,  8, 10, 11,  7,  9,  9, 11, 11, 10,  7,
     7, 10, 10,  9, 10, 10, 11,  9,  9, 13, 11,  0,  0,  0,  0,  0,
  };

  static const Uint8 table_width_little[] =
  {
     6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  6,  6,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5,
     3,  3,  5,  8,  5, 11,  9,  3,  4,  4,  6,  6,  3,  4,  3,  6,
     5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  3,  3,  7,  6,  7,  6,
     9,  8,  6,  7,  7,  5,  5,  8,  7,  2,  4,  7,  5, 10,  7,  8,
     6,  8,  7,  6,  6,  6,  8, 12,  7,  6,  6,  3,  5,  3,  6,  8,
     4,  6,  6,  6,  6,  6,  4,  6,  6,  2,  3,  5,  2, 10,  6,  6,
     6,  6,  3,  5,  3,  6,  6,  8,  6,  6,  5,  4,  6,  4,  7,  0,
     7,  6,  6,  6,  6,  6,  6,  3,  3,  6,  6,  6,  6,  6,  6,  6,
     3,  6,  7,  8,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     6,  5,  7,  8,  6,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     6,  5,  5,  4,  5,  6,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     8,  8,  7,  7,  8,  4,  6,  8,  9,  5,  7,  7,  9,  9,  8,  5,
     5,  8,  8,  7,  8,  8,  9,  7,  7, 11,  9,  0,  0,  0,  0,  0,
  };
  // clang-format on

  if (font == FONTLITTLE)
    return table_width_little[GetOffset (c)];
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
    rank     = GetOffset (pText);
    auto inc = rank > 127;

    if (IsRightReading ())
      pos.x += -GetCharWidth (pText, font);

    if (font != FONTLITTLE)
    {
      rank += 256 * font;
      pPixmap->DrawIcon (-1, CHTEXT, rank, pos);
    }
    else
      pPixmap->DrawIcon (-1, CHLITTLE, rank, pos);

    if (!IsRightReading ())
      pos.x += GetCharWidth (pText, font);

    if (inc)
      pText++;
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
    rank     = GetOffset (pText);
    auto inc = rank > 127;

    rank += 256 * font;
    pPixmap->DrawIcon (-1, CHTEXT, rank, pos);

    lg = GetCharWidth (pText, font);

    if (inc)
      pText++;
    pText++;
    rel += lg;
    pos.x += IsRightReading () ? -lg : lg;
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

    pDest = text;
    start.x =
      pos.x +
      (IsRightReading () ? GetTextWidth (pDest) : -GetTextWidth (pDest)) / 2;
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
    auto rank = GetOffset (pText);
    auto inc  = rank > 127;

    width += GetCharWidth (pText, font);

    if (inc)
      pText++;
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
