
#include <SDL2/SDL_video.h>

#include "blupi.h"
#include "display.h"

#define SCRNUM 16
#define SCRDEN 9
#define SCRFACTOR SCRNUM / SCRDEN

Display::Display ()
{
  this->width  = 0;
  this->height = 0;
}

void
Display::readDisplaySize ()
{
  SDL_DisplayMode displayMode;
  SDL_GetWindowDisplayMode (g_window, &displayMode);
  this->width  = displayMode.w;
  this->height = displayMode.h;
}

Display &
Display::getDisplay ()
{
  static bool    init = false;
  static Display display;

  if (!init)
  {
    display.readDisplaySize ();
    init = true;
  }

  return display;
}

double
Display::getRatio ()
{
  return this->width / this->height;
}

Sint32
Display::getWidth ()
{
  return (
    this->getLogicHeight () * SCRFACTOR +
    (this->getLogicHeight () * SCRFACTOR) % 2);
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
