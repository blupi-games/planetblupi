// misc.h
//

#pragma once

#include <string>
#include "blupi.h"

extern void OutputDebug(const char *pMessage);

extern POINT ConvLongToPos(LPARAM lParam);

extern void	InitRandom();
extern int	Random(int min, int max);

std::string GetBaseDir();
extern void AddUserPath(char *pFilename);
