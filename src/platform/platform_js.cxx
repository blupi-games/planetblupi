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

#include <emscripten.h>
#include <functional>

#include "../blupi.h"
#include "../event.h"
#include "../platform.h"

std::function<void (const SDL_Event &)> Platform::handleEvent;

Platform::Type
Platform::getType ()
{
  return JS;
}

void
Platform::run (std::function<void (const SDL_Event &)> handleEvent)
{
  Platform::handleEvent = handleEvent;
  Platform::timer (nullptr);
  emscripten_set_main_loop (
    [] () {
      SDL_Event event;
      while (SDL_PollEvent (&event))
      {
        Platform::handleEvent (event);
        if (event.type == SDL_QUIT)
          break;
      }
    },
    0, 1);
}

void
Platform::timer (void *)
{
  CEvent::PushUserEvent (EV_UPDATE);
  emscripten_async_call (Platform::timer, nullptr, g_timerInterval);
}
