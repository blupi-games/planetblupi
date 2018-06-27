
#pragma once

#include "SDL2/SDL_stdinc.h"

class Display
{
private:
	Sint32 width;
	Sint32 height;

private:
  Display ();

  void readDisplaySize ();

public:
  static Display & getDisplay ();

  double getRatio ();
  Sint32 getWidth ();
  Sint32 getHeight ();
  Sint32 getLogicWidth ();
  Sint32 getLogicHeight ();
};

#define LXLOGIC (Display::getDisplay ().getLogicWidth ())
#define LYLOGIC (Display::getDisplay ().getLogicHeight ())
#define LXIMAGE (Display::getDisplay ().getWidth ())
#define LYIMAGE (Display::getDisplay ().getHeight ())
#define LXOFFSET ((LXIMAGE - LXLOGIC) / 2)
