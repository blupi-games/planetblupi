
#include <SDL2/SDL_video.h>

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
Display::setDisplaySize (Sint32 width, Sint32 height)
{
  this->width  = width;
  this->height = height;

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
