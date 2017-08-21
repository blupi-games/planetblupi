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

#include <cstdio>
#include <memory>
#include <string>

#include "blupi.h"

extern void OutputDebug (const char * pMessage);

extern Point ConvLongToPos (LPARAM lParam);

extern void   InitRandom ();
extern Sint32 Random (Sint32 min, Sint32 max);

std::string GetBaseDir ();
std::string GetShareDir ();
std::string GetLocale ();
extern void AddUserPath (std::string & pFilename);

template <typename... Args>
std::string
string_format (const std::string & format, Args... args)
{
  size_t size = snprintf (nullptr, 0, format.c_str (), args...) + 1;
  std::unique_ptr<char[]> buf (new char[size]);
  snprintf (buf.get (), size, format.c_str (), args...);
  return std::string (buf.get (), buf.get () + size - 1);
}
