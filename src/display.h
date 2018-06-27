
#pragma once

#include "SDL2/SDL_stdinc.h"

class Display
{
private:
  Sint32 width;
  Sint32 height;

private:
  Display ();

public:
  static Display & getDisplay ();

  void   readDisplaySize ();
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
