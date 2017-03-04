
#pragma once

#include <string>
#include <memory>
#include <cstdio>

#include "blupi.h"

extern void OutputDebug (const char *pMessage);

extern POINT ConvLongToPos (LPARAM lParam);

extern void InitRandom();
extern Sint32   Random (Sint32 min, Sint32 max);

std::string GetBaseDir ();
std::string GetShareDir ();
std::string GetLocale ();
extern void AddUserPath (std::string &pFilename);

template<typename ...Args>
std::string
string_format (const std::string &format, Args ...args)
{
    size_t size = snprintf (nullptr, 0, format.c_str (), args...) + 1;
    std::unique_ptr<char[]> buf (new char[size]);
    std::snprintf (buf.get(), size, format.c_str (), args...);
    return std::string (buf.get (), buf.get () + size - 1);
}
