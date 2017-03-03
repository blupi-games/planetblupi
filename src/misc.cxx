
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_mouse.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(a, b) _mkdir(a)
#else /* _WIN32 */
#include <sys/stat.h>
#endif /*! _WIN32 */

#include "misc.h"
#include "blupi.h"
#include "def.h"

// Variables globales

extern bool     g_bFullScreen;  // false si mode de test


// Affiche un message de debug.

void OutputDebug (const char *pMessage)
{
    SDL_LogDebug (SDL_LOG_CATEGORY_APPLICATION, "%s", pMessage);
}

// Conversion de la position de la souris.

POINT ConvLongToPos (LPARAM lParam)
{
    POINT   pos;

    pos.x = LOWORD (lParam); // horizontal position of cursor
    pos.y = HIWORD (lParam); // vertical position of cursor

    //  if ( !g_bFullScreen )
    //  {
    //      pos.y -= GetSystemMetrics(SM_CYCAPTION);
    //  }

    return pos;
}

static int g_seed;

/* Initialize the Microsoft pseudo-random generator */
void InitRandom()
{
    g_seed = 1;
    // srand (1);
}

/* We are not using rand from stdlib because on Linux the pseudo-generator
 * is using an other algorithm. Then the behaviour is not the same on all
 * platforms.
 * See http://stackoverflow.com/a/1280765/842097
 */
int ms_rand ()
{
  g_seed = g_seed * 0x343fd + 0x269EC3;
  return (g_seed >> 0x10) & 0x7FFF;
}

/* Returns a random number between two values (included). */
Sint32 Random (Sint32 min, Sint32 max)
{
    Sint32  n;

    n = ms_rand (); // replace rand ();
    n = min + (n % (max - min + 1));

    return (Sint32) n;
}

std::string GetLocale ()
{
    return gettext ("en");
}

// Retourne le nom de dossier en cours.

std::string GetBaseDir()
{
    return GetShareDir () + "planetblupi/";
}

std::string GetShareDir ()
{
    static std::string basePath;

    if (!basePath.size())
    {
        auto sdlBasePath = SDL_GetBasePath();
        sdlBasePath[strlen (sdlBasePath) - 1] = '\0';
        basePath = sdlBasePath;
        std::replace (basePath.begin(), basePath.end(), '\\', '/');
        basePath = basePath.substr (0, basePath.find_last_of ("//") + 1);
        SDL_free (sdlBasePath);
    }

    return basePath + "share/";
}

// Ajoute le chemin permettant de lire un fichier
// utilisateur.

void AddUserPath (char *pFilename)
{
    char                    *temp;
    char                   *pText;
    size_t                  pos;
    char                    last;

    temp = SDL_GetPrefPath ("Epsitec SA", "Planet Blupi");
    std::string path = temp;

    pText = strstr (pFilename, "/");
    if (pText != nullptr)
    {
        pos = path.size() + (pText - pFilename) + 1;
        path += pFilename;
        last = path[pos];
        path[pos] = 0;
        mkdir (path.c_str(), 755);
        path[pos] = last;
    }
    else
        path += pFilename;

    strcpy (pFilename, path.c_str());
    SDL_free (temp);
}
