/*
 * This file is part of the planetblupi source code
 * Copyright (C) 2018, Mathieu Schroeter
 * https://epsitec.ch; https://www.blupi.org; https://github.com/blupi-games
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

#include <SDL_video.h>

#include "blupi.h"
#include "display.h"

Display::Display ()
{
  this->width  = this->getLogicWidth ();
  this->height = this->getLogicHeight ();
}

Display &
Display::getDisplay ()
{
  static Display display;
  return display;
}

bool
Display::isWide ()
{
  return this->getWidth () > this->getLogicWidth ();
}

void
Display::readDisplaySize ()
{
  SDL_DisplayMode displayMode;
  int             res;

  if (g_window)
    res = SDL_GetWindowDisplayMode (g_window, &displayMode);
  else
    res = SDL_GetCurrentDisplayMode (0, &displayMode);

  if (res < 0)
    return;

  this->setDisplaySize (displayMode.w, displayMode.h);
}

void
Display::setDisplaySize (Sint32 w, Sint32 h)
{
  this->width  = w;
  this->height = h;

  if (this->width < this->getLogicWidth ())
    this->width = this->getLogicWidth ();
  if (this->height < this->getLogicHeight ())
    this->height = this->getLogicHeight ();
}

Sint32
Display::getWidth ()
{
  return (
    this->getLogicHeight () * this->width / this->height +
    (this->getLogicHeight () * this->width / this->height) % 2);
}

Sint32
Display::getHeight ()
{
  return this->getLogicHeight ();
}

Sint32
Display::getLogicWidth ()
{
  return 640;
}

Sint32
Display::getLogicHeight ()
{
  return 480;
}
