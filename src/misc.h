
#pragma once

#include <string>
#include "blupi.h"

extern void OutputDebug (const char *pMessage);

extern POINT ConvLongToPos (LPARAM lParam);

extern void InitRandom();
extern Sint32   Random (Sint32 min, Sint32 max);

std::string GetBaseDir ();
std::string GetLocale ();
extern void AddUserPath (char *pFilename);
