
#pragma once

#include "blupi.h"

extern
bool Action (Sint16 action, Sint16 direct,
             Sint16 &phase, Sint16 &step,
             Sint16 &channel, Sint16 &icon, POINT &pos, Sint16 &posZ,
             Sint16 &sound);
bool Rotate (Sint16 &icon, Sint16 direct);
Sint32 GetIconDirect (Sint16 icon);
Sint32 GetAmplitude (Sint16 action);
