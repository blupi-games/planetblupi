// blupi.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include "blupi.h"
#include "def.h"
#include "resource.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "movie.h"
#include "button.h"
#include "menu.h"
#include "jauge.h"
#include "event.h"
#include "misc.h"


// Dï¿½finitions globales

#define NAME            "Blupi"
#define TITLE           "Blupi"


// Variables globales

SDL_Window *g_window;
SDL_Renderer *g_renderer;
CEvent     *g_pEvent  = nullptr;
CPixmap    *g_pPixmap = nullptr;        // pixmap principal
CSound     *g_pSound  = nullptr;        // sound principal
CMovie     *g_pMovie  = nullptr;        // movie principal
CDecor     *g_pDecor  = nullptr;
bool        g_bFullScreen = false;  // false si mode de test
Sint32          g_speedRate = 1;
Sint32          g_timerInterval = 50;   // inverval = 50ms
Sint32          g_mouseType = MOUSETYPEGRA;
bool        g_bActive = true;       // is application active ?
bool        g_bTermInit = false;    // initialisation en cours

Uint32      g_lastPhase = 999;




// Lit un numï¿½ro dï¿½cimal.

Sint32 GetNum (char *p)
{
    Sint32      n = 0;

    while (*p >= '0' && *p <= '9')
    {
        n *= 10;
        n += (*p++) - '0';
    }

    return n;
}

// Lit le fichier de configuration.

bool ReadConfig (Sint32 argc, char *argv[])
{
    FILE       *file    = nullptr;
    char        buffer[200];
    char       *pText;
    size_t      nb;

    const auto config = GetBaseDir() + "data/config.ini";

    file = fopen (config.c_str(), "rb");
    if (file == nullptr)
        return false;
    nb = fread (buffer, sizeof (char), 200 - 1, file);
    buffer[nb] = 0;
    fclose (file);

    pText = strstr (buffer, "SpeedRate=");
    if (pText != nullptr)
    {
        g_speedRate = GetNum (pText + 10);
        if (g_speedRate < 1)
            g_speedRate = 1;
        if (g_speedRate > 2)
            g_speedRate = 2;
    }

    pText = strstr (buffer, "Timer=");
    if (pText != nullptr)
    {
        g_timerInterval = GetNum (pText + 6);
        if (g_timerInterval <   10)
            g_timerInterval =   10;
        if (g_timerInterval > 1000)
            g_timerInterval = 1000;
    }

    pText = strstr (buffer, "FullScreen=");
    if (pText != nullptr)
    {
        g_bFullScreen = !!GetNum (pText + 11);
        if (g_bFullScreen != 0)
            g_bFullScreen = 1;
    }

    pText = strstr (buffer, "MouseType=");
    if (pText != nullptr)
    {
        g_mouseType = GetNum (pText + 10);
        if (g_mouseType < 1)
            g_mouseType = 1;
        if (g_mouseType > 9)
            g_mouseType = 9;
    }

    return true;
}


// Mise ï¿½ jour principale.

void UpdateFrame (void)
{
    RECT            clip, rcRect;
    Uint32          phase;
    POINT           posMouse;
    Sint32              i, term, speed;

    posMouse = g_pEvent->GetLastMousePos();

    phase = g_pEvent->GetPhase();

    if (! (phase == g_lastPhase && phase == WM_PHASE_PLAY))
    {
        rcRect.left   = 0;
        rcRect.top    = 0;
        rcRect.right  = LXIMAGE;
        rcRect.bottom = LYIMAGE;
        g_pPixmap->DrawImage (-1, CHBACK, rcRect); // dessine le fond
    }

    if (phase == WM_PHASE_INTRO1 ||
        phase == WM_PHASE_INTRO2)
        g_pEvent->IntroStep();

    if (phase == WM_PHASE_PLAY)
    {
        clip.left   = POSDRAWX;
        clip.top    = POSDRAWY + g_pDecor->GetInfoHeight();
        clip.right  = POSDRAWX + DIMDRAWX;
        clip.bottom = POSDRAWY + DIMDRAWY;

        if (g_pEvent->IsShift())    // shift en cours ?
        {
            g_pEvent->DecorAutoShift (posMouse);
            g_pDecor->Build (clip, posMouse); // construit juste le dï¿½cor
        }
        else
        {
            if (!g_pEvent->GetPause())
            {
                speed = g_pEvent->GetSpeed() * g_speedRate;
                for (i = 0 ; i < speed ; i++)
                {
                    g_pDecor->BlupiStep (i == 0); // avance tous les blupi
                    g_pDecor->MoveStep (i == 0); // avance tous les dï¿½cors
                    g_pEvent->DemoStep();       // avance enregistrement/reproduction
                }
            }

            g_pEvent->DecorAutoShift (posMouse);
            g_pDecor->Build (clip, posMouse); // construit le dï¿½cor
            g_pDecor->NextPhase (1); // refait la carte de temps en temps
        }
    }

    if (phase == WM_PHASE_BUILD)
    {
        clip.left   = POSDRAWX;
        clip.top    = POSDRAWY;
        clip.right  = POSDRAWX + DIMDRAWX;
        clip.bottom = POSDRAWY + DIMDRAWY;
        g_pEvent->DecorAutoShift (posMouse);
        g_pDecor->Build (clip, posMouse); // construit le dï¿½cor
        g_pDecor->NextPhase (-1); // refait la carte chaque fois
    }

    if (phase == WM_PHASE_INIT)
    {
        g_pEvent->DemoStep();  // dï¿½marre ï¿½v. dï¿½mo automatique
    }

    g_pEvent->DrawButtons();

    g_lastPhase = phase;

    if (phase == WM_PHASE_H0MOVIE   ||
        phase == WM_PHASE_H1MOVIE   ||
        phase == WM_PHASE_H2MOVIE   ||
        phase == WM_PHASE_PLAYMOVIE ||
        phase == WM_PHASE_WINMOVIE)
    {
        g_pEvent->MovieToStart();  // fait dï¿½marrer un film si nï¿½cessaire
    }

    if (phase == WM_PHASE_INSERT)
        g_pEvent->TryInsert();

    if (phase == WM_PHASE_PLAY)
    {
        term = g_pDecor->IsTerminated();
        if (term == 1)
            g_pEvent->ChangePhase (WM_PHASE_LOST); // perdu
        if (term == 2)
            g_pEvent->ChangePhase (WM_PHASE_WINMOVIE);  // gagnï¿½
    }
}


void Benchmark()
{
    Sint32      i;
    POINT   pos = { 0, 0 };

    g_pPixmap->DrawIcon (-1, 2, 10, pos, 0);

    pos.x = 300;
    pos.y = 350;
    for (i = 0 ; i < 10000 ; i++)
        g_pPixmap->DrawIcon (-1, 2, i % 4, pos, 0);

    g_pPixmap->DrawIcon (-1, 2, 10, pos, 0);
    g_pSound->Play (0);
}


// Restitue le jeu aprï¿½s une activation en mode fullScreen.

bool RestoreGame()
{
    if (g_pPixmap == nullptr)
        return false;

    g_pEvent->RestoreGame();
    return true;
}

// Libï¿½re le jeu avant une dï¿½sactivation en mode fullScreen.

bool FlushGame()
{
    if (g_pPixmap == nullptr)
        return false;

    return g_pPixmap->Flush();
}


// Finished with all objects we use; release them.

static void FinishObjects (void)
{
    if (g_pMovie != nullptr)
    {
        delete g_pMovie;
        g_pMovie = nullptr;
    }

    if (g_pEvent != nullptr)
    {
        delete g_pEvent;
        g_pEvent = nullptr;
    }

    if (g_pDecor != nullptr)
    {
        delete g_pDecor;
        g_pDecor = nullptr;
    }

    if (g_pSound != nullptr)
    {
        g_pSound->StopMusic();  // stoppe la musique Midi

        delete g_pSound;
        g_pSound = nullptr;
    }

    if (g_pPixmap != nullptr)
    {
        delete g_pPixmap;
        g_pPixmap = nullptr;
    }
}

void WindowProc2 (const SDL_Event &event)
{
    POINT               totalDim, iconDim;

    if (g_pEvent != nullptr &&
        g_pEvent->TreatEvent (event))
        return;

    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            if (g_bFullScreen)
            {
                RestoreGame();
                g_lastPhase = 999;
            }
            if (!g_bFullScreen && g_bTermInit)
            {
                totalDim.x = 64;
                totalDim.y = 66;
                iconDim.x = 64;
                iconDim.y = 66 / 2;
                g_pPixmap->Cache (CHHILI, "image/hili.blp", totalDim, iconDim);
            }
            SDL_SetWindowTitle (g_window, "Blupi");
            if (g_pSound != nullptr)
                g_pSound->RestartMusic();
            if (g_pMovie)
                g_pMovie->Resume();
            return;

        case SDL_WINDOWEVENT_FOCUS_LOST:
            if (g_bFullScreen)
                FlushGame();
            SDL_SetWindowTitle (g_window, "Blupi -- stop");
            if (g_pSound != nullptr)
                g_pSound->SuspendMusic();
            if (g_pMovie)
                g_pMovie->Pause();
            return;
        }
        break;
    }

    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_F5:
            g_pEvent->SetSpeed (1);
            break;
        case SDLK_F6:
            g_pEvent->SetSpeed (2);
            break;
        case SDLK_F7:
            g_pEvent->SetSpeed (4);
            break;
        case SDLK_F8:
            g_pEvent->SetSpeed (8);
            break;
        }
        break;

    case SDL_USEREVENT:
    {
        switch (event.user.code)
        {
        case WM_UPDATE:
            if (!g_pEvent->IsMovie())   // pas de film en cours ?
            {
                if (g_bActive)
                    UpdateFrame();
                g_pPixmap->Display();
            }
            break;

        case WM_MUSIC_STOP:
            if (g_pSound->IsStoppedOnDemand())
                break;

            g_pSound->RestartMusic();
            break;

        case WM_MOVIE_PLAY:
            if (!g_pMovie->Render())
                g_pEvent->StopMovie();
            break;
        }
        break;
    }
    }
}

// Erreur dans DoInit.

static bool InitFail (const char *msg, bool bDirectX)
{
    char    buffer[100];

    if (bDirectX)
        strcpy (buffer, "DirectX Init FAILED\n(while ");
    else
        strcpy (buffer, "Error (");
    strcat (buffer, msg);
    strcat (buffer, ")");

    SDL_ShowSimpleMessageBox (SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error",
                              buffer, g_window);

    FinishObjects();
    return false;
}

// Initialisation de l'application.

static bool DoInit (Sint32 argc, char *argv[])
{
    POINT           totalDim, iconDim;
    RECT            rcRect;
    bool            bOK;

    bOK = ReadConfig (argc, argv); // lit le fichier config.ini

    auto res = SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    if (res < 0)
        return false;

    // Create a window.
    if (g_bFullScreen)
        g_window = SDL_CreateWindow (NAME, 0, 0, LXIMAGE, LYIMAGE,
                                     SDL_WINDOW_FULLSCREEN);
    else
        g_window = SDL_CreateWindow (NAME, SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, LXIMAGE, LYIMAGE, 0);

    if (!g_window)
    {
        printf (SDL_GetError());
        return false;
    }

    g_renderer = SDL_CreateRenderer (g_window, -1,
                                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE |
                                     SDL_RENDERER_PRESENTVSYNC);
    if (!g_renderer)
    {
        printf (SDL_GetError());
        SDL_DestroyWindow (g_window);
        return false;
    }

    SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    if (!bOK)    // config.ini pas correct ?
        return InitFail ("Game not correctly installed", false);

    // Crï¿½e le pixmap principal.
    g_pPixmap = new CPixmap;
    if (g_pPixmap == nullptr)
        return InitFail ("New pixmap", true);

    totalDim.x = LXIMAGE;
    totalDim.y = LYIMAGE;
    if (!g_pPixmap->Create (totalDim, g_bFullScreen, g_mouseType))
        return InitFail ("Create pixmap", true);

    OutputDebug ("Image: init\n");
    totalDim.x = LXIMAGE;
    totalDim.y = LYIMAGE;
    iconDim.x  = 0;
    iconDim.y  = 0;
#if _INTRO
    if (!g_pPixmap->Cache (CHBACK, "image/intro1.blp", totalDim, iconDim))
#else
    if (!g_pPixmap->Cache (CHBACK, "image/init.blp", totalDim, iconDim))
#endif
        return false;

    OutputDebug ("Image: init\n");
    totalDim.x = LXIMAGE;
    totalDim.y = LYIMAGE;
    iconDim.x  = 0;
    iconDim.y  = 0;
    if (!g_pPixmap->Cache (CHGROUND, "image/init.blp", totalDim, iconDim))
        return false;

    rcRect.left   = 0;
    rcRect.top    = 0;
    rcRect.right  = LXIMAGE;
    rcRect.bottom = LYIMAGE;
    g_pPixmap->DrawImage (-1, CHBACK, rcRect); // dessine le fond
    g_pPixmap->Display();

    totalDim.x = DIMCELX * 2 * 16;
    totalDim.y = DIMCELY * 2 * 6;
    iconDim.x = DIMCELX * 2;
    iconDim.y = DIMCELY * 2;
    if (!g_pPixmap->Cache (CHFLOOR, "image/floor000.blp", totalDim, iconDim))
        return InitFail ("Cache floor000.blp", true);

    totalDim.x = DIMOBJX * 16;
    totalDim.y = DIMOBJY * 8;
    iconDim.x = DIMOBJX;
    iconDim.y = DIMOBJY;
    if (!g_pPixmap->Cache (CHOBJECT, "image/obj000.blp", totalDim, iconDim))
        return InitFail ("Cache obj000.blp", true);

    if (!g_pPixmap->Cache (CHOBJECTo, "image/obj-o000.blp", totalDim, iconDim))
        return InitFail ("Cache obj-o000.blp", true);

    totalDim.x = DIMBLUPIX * 16;
    totalDim.y = DIMBLUPIY * 23;
    iconDim.x = DIMBLUPIX;
    iconDim.y = DIMBLUPIY;
    if (!g_pPixmap->Cache (CHBLUPI, "image/blupi.blp", totalDim, iconDim))
        return InitFail ("Cache blupi.blp", true);

    totalDim.x = 64;
    totalDim.y = 66;
    iconDim.x = 64;
    iconDim.y = 66 / 2;
    if (!g_pPixmap->Cache (CHHILI, "image/hili.blp", totalDim, iconDim))
        return InitFail ("Cache hili.blp", true);

    totalDim.x = DIMCELX * 2 * 3;
    totalDim.y = DIMCELY * 2 * 5;
    iconDim.x = DIMCELX * 2;
    iconDim.y = DIMCELY * 2;
    if (!g_pPixmap->Cache (CHFOG, "image/fog.blp", totalDim, iconDim))
        return InitFail ("Cache fog.blp", true);

    totalDim.x = DIMCELX * 2 * 16;
    totalDim.y = DIMCELY * 2 * 1;
    iconDim.x = DIMCELX * 2;
    iconDim.y = DIMCELY * 2;
    if (!g_pPixmap->Cache (CHMASK1, "image/mask1.blp", totalDim, iconDim))
        return InitFail ("Cache mask1.blp", true);

    totalDim.x = DIMBUTTONX * 6;
    totalDim.y = DIMBUTTONY * 21;
    iconDim.x = DIMBUTTONX;
    iconDim.y = DIMBUTTONY;
    if (!g_pPixmap->Cache (CHBUTTON, "image/button00.blp", totalDim, iconDim))
        return InitFail ("Cache button00.blp", true);

    totalDim.x = DIMJAUGEX * 1;
    totalDim.y = DIMJAUGEY * 4;
    iconDim.x = DIMJAUGEX;
    iconDim.y = DIMJAUGEY;
    if (!g_pPixmap->Cache (CHJAUGE, "image/jauge.blp", totalDim, iconDim))
        return InitFail ("Cache jauge.blp", true);

    totalDim.x = DIMTEXTX * 16;
    totalDim.y = DIMTEXTY * 8 * 3;
    iconDim.x = DIMTEXTX;
    iconDim.y = DIMTEXTY;
    if (!g_pPixmap->Cache (CHTEXT, "image/text.blp", totalDim, iconDim))
        return InitFail ("Cache text.blp", true);

    totalDim.x = DIMLITTLEX * 16;
    totalDim.y = DIMLITTLEY * 8;
    iconDim.x = DIMLITTLEX;
    iconDim.y = DIMLITTLEY;
    if (!g_pPixmap->Cache (CHLITTLE, "image/little.blp", totalDim, iconDim))
        return InitFail ("Cache little.blp", true);

    totalDim.x = 426;
    totalDim.y = 52;
    iconDim.x = 426;
    iconDim.y = 52;
    if (!g_pPixmap->Cache (CHBIGNUM, "image/bignum.blp", totalDim, iconDim))
        return InitFail ("Cache bignum.blp", true);

    // Load all cursors
    g_pPixmap->LoadCursors();
    g_pPixmap->ChangeSprite (SPRITE_WAIT); // met le sablier maison

    // Crï¿½e le gestionnaire de son.
    g_pSound = new CSound;
    if (g_pSound == nullptr)
        return InitFail ("New sound", true);

    g_pSound->Create();
    g_pSound->CacheAll();
    g_pSound->SetState (true);

    // Crï¿½e le gestionnaire de films.
    g_pMovie = new CMovie;
    if (g_pMovie == nullptr)
        return InitFail ("New movie", false);

    g_pMovie->Create();

    // Crï¿½e le gestionnaire de dï¿½cors.
    g_pDecor = new CDecor;
    if (g_pDecor == nullptr)
        return InitFail ("New decor", false);

    g_pDecor->Create (g_pSound, g_pPixmap);
    g_pDecor->MapInitColors();

    // Crï¿½e le gestionnaire d'ï¿½vï¿½nements.
    g_pEvent = new CEvent;
    if (g_pEvent == nullptr)
        return InitFail ("New event", false);

    g_pEvent->Create (g_pPixmap, g_pDecor, g_pSound, g_pMovie);
    g_pEvent->SetFullScreen (g_bFullScreen);
    g_pEvent->SetMouseType (g_mouseType);
#if _INTRO
    g_pEvent->ChangePhase (WM_PHASE_INTRO1);
#else
    g_pEvent->ChangePhase (WM_PHASE_TESTCD);
#endif

    g_bTermInit = true;  // initialisation terminï¿½e
    return true;
}


// Programme principal.

Sint32 main (Sint32 argc, char *argv[])
{
    if (!DoInit (argc, argv))
        return -1;

    SDL_TimerID updateTimer = SDL_AddTimer (g_timerInterval, [] (Uint32 interval,
                                            void *param) -> Uint32
    {
        CEvent::PushUserEvent (WM_UPDATE);
        return interval;
    }, nullptr);

    SDL_Event event;
    while (SDL_WaitEvent (&event))
    {
        WindowProc2 (event);

        if (event.type == SDL_QUIT)
            break;
    }

    if (g_window)
        SDL_DestroyWindow (g_window);

    SDL_RemoveTimer (updateTimer);
    FinishObjects();
    SDL_Quit();
    return 0;
}
