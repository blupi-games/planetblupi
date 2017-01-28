// misc.h
//

#pragma once

extern void InitHInstance(HINSTANCE hInstance);
extern void OutputDebug(char *pMessage);
extern void LoadString(UINT nID, char *pBuffer, int lgBuffer);

extern POINT ConvLongToPos(LPARAM lParam);

extern void	InitRandom();
extern int	Random(int min, int max);

extern void GetCurrentDir(char *pName, size_t lg);
extern void AddCDPath(char *pFilename);
extern void AddUserPath(char *pFilename);

extern void TraceErrorDD(HRESULT hErr, char *sFile, int nLine);
extern void TraceErrorDS(HRESULT hErr, char *sFile, int nLine);

