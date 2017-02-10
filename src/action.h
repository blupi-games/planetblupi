// Action.h
//

#pragma once

#include "blupi.h"

extern
bool Action(short action, short direct,
			short &phase, short &step,
			short &channel, short &icon, POINT &pos, short &posZ,
			short &sound);

extern
bool Rotate(short &icon, short direct);

extern
int GetIconDirect(short icon);

extern
int GetAmplitude(short action);

