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

#include <functional>

class Platform
{
private:
  static std::function<void (const SDL_Event &)> handleEvent;

private:
  static void timer (void *);

public:
  enum Type { JS, SDL };

  static Platform::Type getType ();
  static void run (std::function<void (const SDL_Event &)> handleEvent);
};
