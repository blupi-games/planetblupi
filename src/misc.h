// misc.h
//

#pragma once

#include "blupi.h"

extern void OutputDebug(char *pMessage);

extern POINT ConvLongToPos(LPARAM lParam);

extern void	InitRandom();
extern int	Random(int min, int max);

extern void GetCurrentDir(char *pName, size_t lg);
extern void AddUserPath(char *pFilename);
