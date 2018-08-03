/*
 * This file is part of the planetblupi source code
 * Copyright (C) 2018, Mathieu Schroeter
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

#include <SDL_stdinc.h>

class Display
{
private:
  Sint32 width;
  Sint32 height;

private:
  Display ();

public:
  static Display & getDisplay ();

  bool   isWide ();
  void   readDisplaySize ();
  void   setDisplaySize (Sint32 w, Sint32 h);
  Sint32 getWidth ();
  Sint32 getHeight ();
  Sint32 getLogicWidth ();
  Sint32 getLogicHeight ();
};

inline Sint32
LXLOGIC ()
{
  return Display::getDisplay ().getLogicWidth ();
}

inline Sint32
LYLOGIC ()
{
  return Display::getDisplay ().getLogicHeight ();
}

inline Sint32
LXIMAGE ()
{
  return Display::getDisplay ().getWidth ();
}

inline Sint32
LYIMAGE ()
{
  return Display::getDisplay ().getHeight ();
}

inline Sint32
LXOFFSET ()
{
  return ((LXIMAGE () - LXLOGIC ()) / 2);
}
