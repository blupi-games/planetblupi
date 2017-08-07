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

#include <SDL2/SDL.h>
#include <libintl.h>

extern SDL_Window *   g_window;
extern SDL_Renderer * g_renderer;
extern bool           g_bFullScreen;

struct POINT {
  Sint32 x;
  Sint32 y;
};

struct RECT {
  Sint32 left;
  Sint32 top;
  Sint32 right;
  Sint32 bottom;
};

typedef Uint32 COLORREF;

#if defined(_WIN64)
typedef unsigned __int64 WPARAM;
typedef __int64          LPARAM;
#else
typedef Uint32 WPARAM;
typedef Sint32 LPARAM;
#endif

#define LOWORD(l) ((Uint16) (((Uint32) (l)) & 0xffff))
#define HIWORD(l) ((Uint16) ((((Uint32) (l)) >> 16) & 0xffff))

#ifdef _WIN32
#define countof(a) _countof (a)
#else /* _WIN32 */
#define countof(a) (sizeof (a) / sizeof (*a))
#endif /* !_WIN32 */

// clang-format off
#define VK_END   0x23
#define VK_LEFT  0x25
#define VK_UP    0x26
#define VK_RIGHT 0x27
#define VK_DOWN  0x28

#define EV_KEYDOWN     0x0100
#define EV_KEYUP       0x0101
#define EV_MOUSEMOVE   0x0200
#define EV_LBUTTONDOWN 0x0201
#define EV_LBUTTONUP   0x0202
#define EV_RBUTTONDOWN 0x0204
#define EV_RBUTTONUP   0x0205
// clang-format on
