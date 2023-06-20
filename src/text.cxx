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

#include <regex>
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
    if (this->outline)
    {
      SDL_DestroyTexture (this->outline);
      this->outline = nullptr;
    }
    if (this->base)
    {
      SDL_DestroyTexture (this->base);
      this->base = nullptr;
    }
    if (this->over)
    {
      SDL_DestroyTexture (this->over);
      this->over = nullptr;
    }
  }
};

class Cache
{
private:
  std::string                                                      locale;
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

  void Insert (
    const std::string & text, std::shared_ptr<struct TexText> texText,
    const std::string & locale)
  {
    if (locale != this->locale)
    {
      this->Clear ();
      this->locale = locale;
    }

    this->list.insert (
      std::pair<std::string, std::shared_ptr<struct TexText>> (text, texText));
  }

  void Clear () { this->list.clear (); }
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

  ~Font ()
  {
    this->cache.Clear ();
    TTF_CloseFont (this->font);
  }

  TTF_Font * GetFont () { return this->font; }

  std::string ReverseLatinWords (const std::string & text)
  {
    std::regex                  re ("([a-zA-Z0-9.,-=_@ \t()]{2,})");
    std::string                 out;
    std::string::const_iterator it = text.cbegin (), end = text.cend ();
    for (std::smatch match; std::regex_search (it, end, match, re);
         it = match[0].second)
    {
      out += match.prefix ();
      auto substr = match.str ();
      std::reverse (substr.begin (), substr.end ());
      std::replace (substr.begin (), substr.end (), '(', '$');
      std::replace (substr.begin (), substr.end (), ')', '(');
      std::replace (substr.begin (), substr.end (), '$', ')');
      if (substr.back () == ' ' && substr.at (0) != ' ')
        substr = ' ' + substr.substr (0, substr.length () - 1);
      else if (substr.back () != ' ' && substr.at (0) == ' ')
        substr = substr.substr (1) + ' ';
      out += substr;
    }
    out.append (it, end);
    return out;
  }

  void Draw (CPixmap * pPixmap, Point pos, std::string pText, Sint32 slope)
  {
    Uint32   format;
    int      access;
    SDL_Rect r0;

    const auto isRTL = IsRightReading ();
    const auto angle = isRTL ? -2.5 : 2.5;

    auto texText = this->cache.Get (pText);

    if (!texText && isRTL)
      pText = this->ReverseLatinWords (pText);

    SDL_Texture * texOutline = texText ? texText->outline : nullptr;
    SDL_Texture * texBase    = texText ? texText->base : nullptr;
    SDL_Texture * texOver    = texText ? texText->over : nullptr;

    int outlineW = texText ? texText->outlineW : 0;
    int outlineH = texText ? texText->outlineH : 0;
    int baseW    = texText ? texText->baseW : 0;
    int baseH    = texText ? texText->baseH : 0;

    const auto locale = GetLocale ();

    if (locale == "ar")
      pos.y -= 2;
    else if (locale == "he")
      pos.y -= 1;

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
      this->cache.Insert (pText, _texText, locale);
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
      arabicFont.c_str (), 13, {0xB4, 0x17, 0x12, 0}, SDL_FALSE, SDL_FALSE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicSlim->GetFont (), "Arab");
    this->arabicWhite = new Font (
      arabicFont.c_str (), 13, {0xFF, 0xFF, 0xFF, 0}, SDL_TRUE, SDL_TRUE,
      SDL_TRUE);
    TTF_SetFontScriptName (this->arabicWhite->GetFont (), "Arab");
  }

  ~Fonts () {}

  void Clear ()
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
  /*  ì     ó     Ç     Ö     Ü     õ     ã         */
    0xAC, 0xB3, 0x87, 0x96, 0x9C, 0xB5, 0xA3
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
     5,  8,  9,  9,  8,  8,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     8,  8,  9,  8,  8,  4,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     8,  8,  7,  6,  7,  9,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     9,  9,  8,  8,  9,  5,  7,  9, 10,  6,  8,  8,  9, 10,  9,  6,
     7,  9,  9,  8,  9,  9, 10,  9,  8, 12, 10,  0,  0,  0,  0,  0,
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
     3,  6,  7,  8,  6,  6,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     6,  5,  7,  8,  6,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     6,  5,  5,  4,  5,  6,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     7,  7,  5,  6,  7,  2,  4,  7,  7,  3,  5,  7,  6,  7,  7,  4,
     3,  7,  7,  7,  8,  7,  6,  7,  7,  9,  8,  0,  0,  0,  0,  0,
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
 * \param[in] slope - Text slope.
 */
void
DrawTextLegacy (
  CPixmap * pPixmap, Point pos, const char * pText, Sint32 font, Sint32 slope)
{
  Sint32       rank;
  bool         isLatin    = false;
  int          numberSize = 0;
  const char * it         = nullptr;
  int          skip       = 0;
  Sint32       start      = pos.y;
  Sint32       rel        = 0;

  auto useD7          = strchr (pText, 0xD7) != nullptr;
  auto isRightReading = useD7 && IsRightReading ();
  auto length         = strlen (pText);

  if (length >= 1 && !useD7 && IsRightReading ())
    pos.x -= GetTextWidth (pText, font);

  while (*pText != '\0' || skip)
  {
    if (isRightReading && numberSize == 0)
    {
      const auto test = [] (const char * text) -> bool {
        return *text > ' ' && *text <= '~';
      };
      it      = pText;
      isLatin = test (pText);
      if (isLatin)
      {
        while (test (pText))
          ++pText;

        numberSize = pText - it;
        skip       = numberSize - 1;
      }
    }

    if (numberSize)
    {
      pText = it + numberSize - 1;
      numberSize--;
    }

    rank = GetOffset (pText);

    if (isRightReading)
    {
      if (rank == '(')
        rank = ')';
      else if (rank == ')')
        rank = '(';
    }

    auto inc = rank > 127;
    auto lg  = GetCharWidth (pText, font);

    if (isRightReading)
      pos.x += -lg;

    rel += lg;
    if (slope)
      pos.y = start + rel / slope;

    if (font != FONTLITTLE)
    {
      rank += 256 * font;
      pPixmap->DrawIcon (-1, CHTEXT, rank, pos);
    }
    else
      pPixmap->DrawIcon (-1, CHLITTLE, rank, pos);

    if (!isRightReading)
      pos.x += lg;

    if (!numberSize && skip > 0)
    {
      pText += skip;
      skip = 0;
    }

    if (inc)
      pText++;
    pText++;
  }
}

Fonts *
GetFonts ()
{
  static Fonts fonts;
  return &fonts;
}

void
DisposeFonts ()
{
  GetFonts ()->Clear ();
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
  if (g_settingsOverload & SETTING_LEGACY)
    DrawTextLegacy (pPixmap, pos, pText, font, slope);
  else
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
GetTextWidthLegacy (const char * pText, Sint32 font)
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

Sint32
GetTextWidth (const char * pText, Sint32 font)
{
  if (g_settingsOverload & SETTING_LEGACY)
    return GetTextWidthLegacy (pText, font);
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
