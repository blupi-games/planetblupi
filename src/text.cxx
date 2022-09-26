/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2022, Mathieu Schroeter
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

#include <SDL_ttf.h>

#include "blupi.h"
#include "def.h"
#include "event.h"
#include "misc.h"
#include "pixmap.h"
#include "text.h"

struct TexText {
  SDL_Texture * outline;
  int           outlineW;
  int           outlineH;

  SDL_Texture * base;
  int           baseW;
  int           baseH;

  SDL_Texture * over;

  TexText (const TexText & texText)
  {
    this->outline  = texText.outline;
    this->base     = texText.base;
    this->over     = texText.over;
    this->outlineW = texText.outlineW;
    this->outlineH = texText.outlineH;
    this->baseW    = texText.baseW;
    this->baseH    = texText.baseH;
  }

  TexText (
    SDL_Texture * outline, int outlineW, int outlineH, SDL_Texture * base,
    int baseW, int baseH, SDL_Texture * over)
  {
    this->outline  = outline;
    this->base     = base;
    this->over     = over;
    this->outlineW = outlineW;
    this->outlineH = outlineH;
    this->baseW    = baseW;
    this->baseH    = baseH;
  }

  ~TexText ()
  {
    SDL_DestroyTexture (this->outline);
    SDL_DestroyTexture (this->base);
    SDL_DestroyTexture (this->over);
  }
};

class Cache
{
private:
  std::unordered_map<std::string, std::shared_ptr<struct TexText>> list;

public:
  Cache () {}

  struct TexText * Get (const std::string & text)
  {
    const auto entry = this->list.find (text);
    if (entry != this->list.end ())
      return entry->second.get ();

    return nullptr;
  }

  void
  Insert (const std::string & text, std::shared_ptr<struct TexText> texText)
  {
    this->list.insert (
      std::pair<std::string, std::shared_ptr<struct TexText>> (text, texText));
  }
};

class Font
{
  TTF_Font * font;
  SDL_Color  color;
  SDL_bool   outline;
  Cache      cache;

public:
  Font (
    const char * name, int size, SDL_Color color, SDL_bool bold,
    SDL_bool outline, SDL_bool rtl = SDL_FALSE)
  {
    this->font    = TTF_OpenFont (name, size);
    this->color   = color;
    this->outline = outline;

    TTF_SetFontHinting (this->font, TTF_HINTING_NORMAL);
    if (bold)
      TTF_SetFontStyle (this->font, TTF_STYLE_BOLD);

    if (rtl)
      TTF_SetFontDirection (this->font, TTF_DIRECTION_RTL);
  }

  ~Font () { TTF_CloseFont (this->font); }

  TTF_Font * GetFont () { return this->font; }

  void Draw (CPixmap * pPixmap, Point pos, std::string pText, Sint32 slope)
  {
    Uint32   format;
    int      access;
    SDL_Rect r0;

    const auto isRTL = IsRightReading ();
    const auto angle = isRTL ? -2.5 : 2.5;

    auto texText = this->cache.Get (pText);

    SDL_Texture * texOutline = texText ? texText->outline : nullptr;
    SDL_Texture * texBase    = texText ? texText->base : nullptr;
    SDL_Texture * texOver    = texText ? texText->over : nullptr;

    int outlineW = texText ? texText->outlineW : 0;
    int outlineH = texText ? texText->outlineH : 0;
    int baseW    = texText ? texText->baseW : 0;
    int baseH    = texText ? texText->baseH : 0;

    if (this->outline)
    {
      TTF_SetFontOutline (this->font, 1);

      if (!texOutline)
      {
        SDL_Surface * text = TTF_RenderUTF8_Solid (
          this->font, pText.c_str (), {0x00, 0x00, 0x00, 0});
        texOutline = SDL_CreateTextureFromSurface (g_renderer, text);
        SDL_FreeSurface (text);
        SDL_QueryTexture (texOutline, &format, &access, &outlineW, &outlineH);
      }

      r0.w = outlineW;
      r0.h = outlineH;
      r0.x = pos.x;
      r0.y = pos.y;

      if (isRTL)
        r0.x -= outlineW;
    }

    TTF_SetFontOutline (this->font, 0);

    if (!texBase)
    {
      SDL_Surface * text =
        TTF_RenderUTF8_Blended (this->font, pText.c_str (), this->color);
      texBase = SDL_CreateTextureFromSurface (g_renderer, text);
      SDL_FreeSurface (text);
      SDL_QueryTexture (texBase, &format, &access, &baseW, &baseH);
    }

    if (!texOver)
    {
      this->color.a = 64;
      SDL_Surface * text =
        TTF_RenderUTF8_Blended (this->font, pText.c_str (), this->color);
      texOver = SDL_CreateTextureFromSurface (g_renderer, text);
      SDL_FreeSurface (text);
      this->color.a = 0;
    }

    SDL_Rect r;
    r.w = baseW;
    r.h = baseH;
    r.x = pos.x + (isRTL ? -1 : 1);
    r.y = pos.y + 1;

    if (isRTL)
      r.x -= baseW;

    if (this->outline)
      pPixmap->Blit (-1, texOutline, r0, slope ? angle : 0, SDL_FLIP_NONE);

    pPixmap->Blit (-1, texBase, r, slope ? angle : 0, SDL_FLIP_NONE);
    pPixmap->Blit (-1, texOver, r, slope ? angle : 0, SDL_FLIP_NONE);

    if (!texText)
    {
      std::shared_ptr<struct TexText> _texText =
        std::make_shared<struct TexText> (
          texOutline, outlineW, outlineH, texBase, baseW, baseH, texOver);
      this->cache.Insert (pText, _texText);
    }
  }
};

class Fonts
{
private:
  Font * latinLittle;
  Font * latinRed;
  Font * latinSlim;
  Font * latinWhite;

  Font * hebrewLittle;
  Font * hebrewRed;
  Font * hebrewSlim;
  Font * hebrewWhite;

  Font * arabicLittle;
  Font * arabicRed;
  Font * arabicSlim;
  Font * arabicWhite;

private:
  Font * GetFont (Sint32 font)
  {
    if (GetLocale () == "he")
      switch (font)
      {
      case FONTLITTLE:
        return this->hebrewLittle;
      case FONTRED:
        return this->hebrewRed;
      case FONTSLIM:
        return this->hebrewSlim;
      case FONTWHITE:
        return this->hebrewWhite;
      }

    if (GetLocale () == "ar")
      switch (font)
      {
      case FONTLITTLE:
        return this->arabicLittle;
      case FONTRED:
        return this->arabicRed;
      case FONTSLIM:
        return this->arabicSlim;
      case FONTWHITE:
        return this->arabicWhite;
      }

    switch (font)
    {
    case FONTLITTLE:
      return this->latinLittle;
    case FONTRED:
      return this->latinRed;
    case FONTSLIM:
      return this->latinSlim;
    case FONTWHITE:
      return this->latinWhite;
    }

    return nullptr;
  }

public:
  Fonts ()
  {
    const auto latinLittleFont =
      GetBaseDir () + "fonts/ChakraPetch-Regular.ttf";
    const auto latinFont = GetBaseDir () + "fonts/NovaSlim-Regular.ttf";

    this->latinLittle = new Font (
      latinLittleFont.c_str (), 12, {0xFF, 0xFF, 0x00, 0}, SDL_FALSE, SDL_TRUE);
    this->latinRed = new Font (
      latinFont.c_str (), 13, {0xFF, 0x00, 0x00, 0}, SDL_TRUE, SDL_TRUE);
    this->latinSlim = new Font (
      latinFont.c_str (), 12, {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE);
    this->latinWhite = new Font (
      latinFont.c_str (), 13, {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE);

    const auto hebrewFont =
      GetBaseDir () + "fonts/IBMPlexSansHebrew-Regular.ttf";

    this->hebrewLittle = new Font (
      hebrewFont.c_str (), 12, {0xFF, 0xFF, 0x00, 0}, SDL_FALSE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->hebrewLittle->GetFont (), "Hebr");
    this->hebrewRed = new Font (
      hebrewFont.c_str (), 13, {0xFF, 0x00, 0x00, 0}, SDL_TRUE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->hebrewRed->GetFont (), "Hebr");
    this->hebrewSlim = new Font (
      hebrewFont.c_str (), 12, {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->hebrewSlim->GetFont (), "Hebr");
    this->hebrewWhite = new Font (
      hebrewFont.c_str (), 13, {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->hebrewWhite->GetFont (), "Hebr");

    const auto arabicFont =
      GetBaseDir () + "fonts/IBMPlexSansArabic-Regular.ttf";

    this->arabicLittle = new Font (
      arabicFont.c_str (), 12, {0xFF, 0xFF, 0x00, 0}, SDL_FALSE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicLittle->GetFont (), "Arab");
    this->arabicRed = new Font (
      arabicFont.c_str (), 13, {0xFF, 0x00, 0x00, 0}, SDL_TRUE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicRed->GetFont (), "Arab");
    this->arabicSlim = new Font (
      arabicFont.c_str (), 12, {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicSlim->GetFont (), "Arab");
    this->arabicWhite = new Font (
      arabicFont.c_str (), 13, {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicWhite->GetFont (), "Arab");
  }

  ~Fonts ()
  {
    delete this->latinLittle;
    delete this->latinRed;
    delete this->latinSlim;
    delete this->latinWhite;

    delete this->hebrewLittle;
    delete this->hebrewRed;
    delete this->hebrewSlim;
    delete this->hebrewWhite;

    delete this->arabicLittle;
    delete this->arabicRed;
    delete this->arabicSlim;
    delete this->arabicWhite;
  }

  Sint32 GetTextWidth (const char * pText, Sint32 font)
  {
    int w = 0, h = 0;
    TTF_SizeUTF8 (this->GetFont (font)->GetFont (), pText, &w, &h);
    return w;
  }

  void Draw (
    CPixmap * pPixmap, Sint32 font, Point pos, const char * pText, Sint32 slope)
  {
    this->GetFont (font)->Draw (pPixmap, pos, pText, slope);
  }
};

Fonts *
GetFonts ()
{
  static Fonts fonts;
  return &fonts;
}

/**
 * \brief Draw a text in a pixmap to a specific position.
 *
 * \param[in] pPixmap - The pixmap where it must be drawn.
 * \param[in] pos - The coordinates for the text.
 * \param[in] pText - The text.
 * \param[in] font - The font style (little or normal).
 * \param[in] slope - Text slope.
 */
void
DrawText (
  CPixmap * pPixmap, Point pos, const char * pText, Sint32 font, Sint32 slope)
{
  GetFonts ()->Draw (pPixmap, font, pos, pText, slope);
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
    itl = DIMLITTLEY - 2;
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

    DrawText (pPixmap, pos, pDest, font, pente);

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
  auto   isRightReading = IsRightReading ();

  if (font == FONTLITTLE)
    itl = DIMLITTLEY - 2;
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
      (isRightReading ? GetTextWidth (pDest) : -GetTextWidth (pDest)) / 2;
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
    itl = DIMLITTLEY - 2;
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
  return GetFonts ()->GetTextWidth (pText, font);
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
