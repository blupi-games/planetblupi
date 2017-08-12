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

#include "blupi.h"
#include "def.h"

extern bool Action (
  Sint16 action, Sint16 direct, Sint16 & phase, Sint16 & step, Sint16 & channel,
  Sint16 & icon, POINT & pos, Sint16 & posZ, Sounds & sound);
bool   Rotate (Sint16 & icon, Sint16 direct);
Sint32 GetIconDirect (Sint16 icon);
Sint32 GetAmplitude (Sint16 action);
