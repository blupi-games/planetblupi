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

#define FONTWHITE   0
#define FONTRED     1
#define FONTSLIM    2
#define FONTLITTLE 10

void DrawText (CPixmap *pPixmap, POINT pos, const char *pText, Sint32 font = 0);
void DrawTextPente (CPixmap *pPixmap, POINT pos, const char *pText,
                    Sint32 pente, Sint32 font = 0);
void DrawTextRect (CPixmap *pPixmap, POINT pos, char *pText,
                   Sint32 pente, Sint32 font = 0, Sint32 part = -1);
void DrawTextCenter (CPixmap *pPixmap, POINT pos, const char *pText,
                     Sint32 font = 0);
Sint32 GetTextHeight (char *pText, Sint32 font = 0, Sint32 part = -1);
Sint32 GetTextWidth (const char *pText, Sint32 font = 0);
void DrawBignum (CPixmap *pPixmap, POINT pos, Sint32 num);
Sint32 GetBignumWidth (Sint32 num);

