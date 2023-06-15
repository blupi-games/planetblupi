/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2019, Mathieu Schroeter
 * http://epsitec.ch; http://www.blupi.org; http://github.com/blupi-games
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include <atomic>
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <argagg/argagg.hpp>
#ifdef USE_CURL
#include <curl/curl.h>
#endif /* USE_CURL */

#include "json/json.hpp"

#include "blupi.h"
#include "button.h"
#include "decor.h"
#include "def.h"
#include "event.h"
#include "menu.h"
#include "misc.h"
#include "movie.h"
#include "pixmap.h"
#include "platform.h"
#include "progress.h"
#include "sound.h"
#include "text.h"

// Global variables

SDL_Window *   g_window;
SDL_Renderer * g_renderer;

CEvent *          g_pEvent       = nullptr;
CPixmap *         g_pPixmap      = nullptr; // pixmap principal
CSound *          g_pSound       = nullptr; // sound principal
CMovie *          g_pMovie       = nullptr; // movie principal
CDecor *          g_pDecor       = nullptr;
std::thread *     g_updateThread = nullptr;
std::atomic<bool> g_updateAbort (false);

bool        g_bFullScreen    = false; // false si mode de test
Uint8       g_zoom           = 1;
Sint32      g_speedRate      = 1;
Sint32      g_timerInterval  = 50; // inverval = 50ms
int         g_rendererType   = 0;
bool        g_enableRecorder = false;
std::string g_playRecord;
bool        g_restoreBugs = false; // restore original < v1.9 bugs
bool        g_restoreMidi = false; // restore music playback based on midi files
bool        g_renderQuality = true; // use the best render quality with scaling
int         g_settingsOverload = 0;

bool          g_bTermInit = false; // initialisation en cours
Uint32        g_lastPhase = 999;
RestartMode   g_restart   = RestartMode::NO;
static bool   g_pause;
static Uint32 g_prevPhase = 0;

#ifdef USE_CURL
struct url_data {
  CURLcode status;
  char *   buffer;
  size_t   size;
};
#endif /* USE_CURL */

template <typename Out>
static void
split (const std::string & s, char delim, Out result)
{
  std::stringstream ss;
  ss.str (s);
  std::string item;
  while (std::getline (ss, item, delim))
    *(result++) = item;
}

static std::vector<std::string>
split (const std::string & s, char delim)
{
  std::vector<std::string> elems;
  split (s, delim, std::back_inserter (elems));
  return elems;
}

/**
 * \brief Read the config file.
 *
 * \returns true on success.
 */
static bool
ReadConfig ()
{
  const auto config = GetBaseDir () + "data/config.json";

  std::ifstream file (config, std::ifstream::in);
  if (!file)
    return false;

  nlohmann::json j;
  file >> j;

  if (
    !(g_settingsOverload & SETTING_SPEEDRATE) &&
    j.find ("speedrate") != j.end ())
  {
    g_speedRate = j["speedrate"].get<int> ();
    if (g_speedRate < 1)
      g_speedRate = 1;
    if (g_speedRate > 2)
      g_speedRate = 2;
  }

  if (
    !(g_settingsOverload & SETTING_TIMERINTERVAL) &&
    j.find ("timerinterval") != j.end ())
  {
    g_timerInterval = j["timerinterval"].get<int> ();
    if (g_timerInterval < 10)
      g_timerInterval = 10;
    if (g_timerInterval > 1000)
      g_timerInterval = 1000;
  }

  if (
    !(g_settingsOverload & SETTING_FULLSCREEN) &&
    j.find ("fullscreen") != j.end ())
    g_bFullScreen = j["fullscreen"].get<bool> ();

  if (!(g_settingsOverload & SETTING_ZOOM) && j.find ("zoom") != j.end ())
  {
    g_zoom = j["zoom"].get<Uint8> ();
    if (g_zoom != 1 && g_zoom != 2)
      g_zoom = 1;
  }

  if (
    !(g_settingsOverload & SETTING_RENDERER) && j.find ("renderer") != j.end ())
  {
    if (j["renderer"] == "software")
      g_rendererType = SDL_RENDERER_SOFTWARE;
    else if (j["renderer"] == "accelerated")
      g_rendererType = SDL_RENDERER_ACCELERATED;
  }

  if (
    !(g_settingsOverload & SETTING_DRIVER) && j.find ("driver") != j.end () &&
    (!g_rendererType || g_rendererType == SDL_RENDERER_ACCELERATED))
  {
    std::string input = j["driver"];
    if (std::regex_match (
          input, std::regex ("direct3d|direct3d11|opengl|opengles2|opengles")))
      SDL_SetHint (SDL_HINT_RENDER_DRIVER, input.c_str ());
  }

  if (
    !(g_settingsOverload & SETTING_MIDI) && j.find ("restoremidi") != j.end ())
    g_restoreMidi = j["restoremidi"].get<bool> ();

  if (
    !(g_settingsOverload & SETTING_RENDERQUALITY) &&
    j.find ("renderquality") != j.end ())
    g_renderQuality = j["renderquality"].get<bool> ();

  return true;
}

/**
 * \brief Main frame update.
 */
static bool
Update (void)
{
  Rect   clip, rcRect;
  Uint32 phase;
  Point  posMouse;
  Sint32 i, term, speed;
  bool   display = true;

  posMouse = g_pEvent->GetLastMousePos ();

  phase = g_pEvent->GetPhase ();

  SDL_RenderClear (g_renderer);

  rcRect.left   = 0;
  rcRect.top    = 0;
  rcRect.right  = LXIMAGE ();
  rcRect.bottom = LYIMAGE ();
  g_pPixmap->DrawImage (-1, CHBACK, rcRect); // draw the background

  if (phase == EV_PHASE_INTRO1)
    g_pEvent->IntroStep ();

  if (phase == EV_PHASE_PLAY)
  {
    clip.left   = POSDRAWX;
    clip.top    = POSDRAWY + g_pDecor->GetInfoHeight ();
    clip.right  = POSDRAWX + DIMDRAWX;
    clip.bottom = POSDRAWY + DIMDRAWY;

    if (g_pEvent->IsShift ()) // screen shifting
      g_pEvent->DecorAutoShift ();

    if (!g_pEvent->GetPause ())
    {
      speed = g_pEvent->GetSpeed () * g_speedRate;
      for (i = 0; i < speed; i++)
      {
        g_pDecor->BlupiStep (i == 0); // move all blupi
        g_pDecor->MoveStep (i == 0);  // move the environment
        g_pEvent->DemoStep ();        // forward the recording or demo playing
      }
    }

    g_pEvent->DecorAutoShift ();
    g_pDecor->Build (clip, posMouse); // build the environment
    g_pEvent->PlayMove (posMouse);
    g_pDecor->NextPhase (1); // rebuild the map sometimes
  }

  if (phase == EV_PHASE_BUILD)
  {
    clip.left   = POSDRAWX;
    clip.top    = POSDRAWY;
    clip.right  = POSDRAWX + DIMDRAWX;
    clip.bottom = POSDRAWY + DIMDRAWY;
    g_pEvent->DecorAutoShift ();
    g_pDecor->Build (clip, posMouse); // build the environment
    g_pDecor->NextPhase (-1);         // rebuild the map sometimes
  }

  if (phase == EV_PHASE_INIT)
  {
    g_pEvent->DemoStep (); // start automatically (eventually) the demo
  }

  SDL_Event event = {0};
  event.type      = SDL_MOUSEMOTION;
  event.motion.x  = posMouse.x;
  event.motion.y  = posMouse.y;
  g_pEvent->EventButtons (event, posMouse);
  g_pEvent->MouseSprite (posMouse);

  g_pEvent->DrawButtons ();

  g_lastPhase = phase;

  if (
    phase == EV_PHASE_H0MOVIE || phase == EV_PHASE_H1MOVIE ||
    phase == EV_PHASE_H2MOVIE || phase == EV_PHASE_PLAYMOVIE ||
    phase == EV_PHASE_WINMOVIE)
  {
    display = g_pEvent->MovieToStart (); // start a movie if necessary
  }

  if (phase == EV_PHASE_INSERT)
    g_pEvent->TryInsert ();

  if (phase == EV_PHASE_PLAY)
  {
    term = g_pDecor->IsTerminated ();
    if (term == 1)
      g_pEvent->ChangePhase (EV_PHASE_LOST); // lost
    if (term == 2)
      g_pEvent->ChangePhase (EV_PHASE_WINMOVIE); // win
  }

  if (g_prevPhase != phase)
  {
    Point pos;

    pos.x = LXIMAGE () / 2;
    pos.y = LYIMAGE () / 2;

    if (phase == EV_PHASE_LOST)
    {
      if (!g_pSound->PlayImage (SOUND_LOST, pos))
        g_pSound->PlayImage (SOUND_GOAL, pos);
    }
    else if (phase == EV_PHASE_WIN || phase == EV_PHASE_LASTWIN)
    {
      if (!g_pSound->PlayImage (SOUND_WIN, pos))
        g_pSound->PlayImage (SOUND_GOAL, pos);
    }
  }

  g_prevPhase = phase;

  return display;
}

/**
 * \brief Finished with all objects we use; release them.
 */
static void
FinishObjects (void)
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
    g_pSound->StopMusic ();

    delete g_pSound;
    g_pSound = nullptr;
  }

  if (g_pPixmap != nullptr)
  {
    delete g_pPixmap;
    g_pPixmap = nullptr;
  }
}

static void
HandleEvent (const SDL_Event & event)
{
  if (!g_pause && g_pEvent != nullptr && g_pEvent->TreatEvent (event))
    return;

  switch (event.type)
  {
  case SDL_WINDOWEVENT: {
#ifndef DEBUG
    Point totalDim, iconDim;

    switch (event.window.event)
    {
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      g_pause = false;

      if (g_bFullScreen)
        g_lastPhase = 999;

      if (!g_bFullScreen && g_bTermInit)
      {
        totalDim.x = 64;
        totalDim.y = 66;
        iconDim.x  = 64;
        iconDim.y  = 66 / 2;
        g_pPixmap->Cache (CHHILI, "hili.png", totalDim, iconDim);
      }
      SDL_SetWindowTitle (g_window, gettext ("Planet Blupi"));
      if (g_pSound != nullptr)
        g_pSound->RestartMusic ();
      if (g_pMovie)
        g_pMovie->Resume ();
      return;

    case SDL_WINDOWEVENT_FOCUS_LOST:
      g_pause = true;

      SDL_SetWindowTitle (g_window, gettext ("Planet Blupi -- stop"));
      if (g_pSound != nullptr)
        g_pSound->SuspendMusic ();
      if (g_pMovie)
        g_pMovie->Pause ();
      return;
    }
#endif /* !DEBUG */
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

  case SDL_RENDER_DEVICE_RESET:
  case SDL_RENDER_TARGETS_RESET:
    g_pDecor->InvalidateGrounds ();
    g_pPixmap->ReloadTargetTextures ();
    g_pEvent->LoadBackground ();
    break;

  case SDL_USEREVENT: {
    switch (event.user.code)
    {
    case EV_UPDATE:
      if (!g_pEvent->IsMovie ()) // pas de film en cours ?
      {
        bool display = true;

        if (!g_pause)
          display = Update ();

        if (!g_pEvent->IsMovie () && display)
          g_pPixmap->Display ();
      }
      break;

    case EV_WARPMOUSE: {
      const SDL_Point * coord = static_cast<SDL_Point *> (event.user.data1);

      Sint32 x = coord->x, y = coord->y;
      g_pPixmap->FromGameToDisplay (x, y);
      SDL_WarpMouseInWindow (g_window, x, y);
      delete coord;
      break;
    }

    case EV_CHECKUPDATE: {
      std::string * data = static_cast<std::string *> (event.user.data1);

      using json    = nlohmann::json;
      json jsonData = json::parse (*data);

      /* Check if the remote version is newer */
      std::vector<std::string>  list = split (jsonData["version"], '.');
      std::vector<unsigned int> version (3);
      std::transform (
        list.begin (), list.end (), version.begin (),
        [] (const std::string & s) -> unsigned int { return std::stoi (s); });

      if (
        (!!PB_VERSION_EXTRA[0] &&
         PB_VERSION_INT (version[0], version[1], version[2]) >=
           PLANETBLUPI_VERSION_INT) ||
        PB_VERSION_INT (version[0], version[1], version[2]) >
          PLANETBLUPI_VERSION_INT)
        g_pEvent->SetUpdateVersion (jsonData["version"]);

      delete data;
    }

    case EV_MUSIC_STOP:
      if (g_pSound->IsStoppedOnDemand ())
        break;

      g_pSound->RestartMusic ();
      break;

    case EV_MOVIE_PLAY:
      if (!g_pMovie->Render ())
        g_pEvent->StopMovie ();
      break;
    }
    break;
  }
  }
}

// Error with DoInit function.

static void
InitFail (const char * msg)
{
  char buffer[100];

  strcpy (buffer, "Error (");
  strcat (buffer, msg);
  strcat (buffer, ")");

  SDL_ShowSimpleMessageBox (
    SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", buffer, g_window);

  FinishObjects ();
}

#ifdef USE_CURL
static size_t
updateCallback (void * ptr, size_t size, size_t nmemb, void * data)
{
  size_t     realsize = size * nmemb;
  url_data * mem      = static_cast<url_data *> (data);

  mem->buffer =
    static_cast<char *> (realloc (mem->buffer, mem->size + realsize + 1));
  if (mem->buffer)
  {
    memcpy (&(mem->buffer[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0;
  }

  return realsize;
}

static int
progressCallback (
  void * userData, double dltotal, double dlnow, double ultotal, double ulnow)
{
  return g_updateAbort ? 1 : 0;
}
#endif /* USE_CURL */

#ifdef USE_CURL
static void
CheckForUpdates ()
{
  url_data chunk;

  chunk.buffer = nullptr; /* we expect realloc(NULL, size) to work */
  chunk.size   = 0;       /* no data at this point */
  chunk.status = CURLE_FAILED_INIT;

  CURL * curl = curl_easy_init ();
  if (!curl)
    return;

  curl_easy_setopt (curl, CURLOPT_NOSIGNAL, 1);
  curl_easy_setopt (curl, CURLOPT_FAILONERROR, 1);
  curl_easy_setopt (curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt (curl, CURLOPT_TIMEOUT, 20);
  curl_easy_setopt (curl, CURLOPT_CONNECTTIMEOUT, 5);

  curl_easy_setopt (curl, CURLOPT_WRITEFUNCTION, updateCallback);

  curl_easy_setopt (curl, CURLOPT_URL, "http://blupi.org/update/planet.json");
  curl_easy_setopt (curl, CURLOPT_WRITEDATA, (void *) &chunk);

  curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0);
  curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, nullptr);
  curl_easy_setopt (curl, CURLOPT_PROGRESSFUNCTION, progressCallback);

  chunk.status = curl_easy_perform (curl);

  if (chunk.status)
  {
    const char * err = curl_easy_strerror (chunk.status);
    SDL_LogError (
      SDL_LOG_CATEGORY_APPLICATION, "Check for updates, error: %s", err);
  }
  else
  {
    std::string * res = new std::string (chunk.buffer, chunk.size);
    CEvent::PushUserEvent (EV_CHECKUPDATE, res);
  }

  if (chunk.buffer)
    free (chunk.buffer);

  curl_easy_cleanup (curl);
}
#endif /* USE_CURL */

static int
parseArgs (int argc, char * argv[], bool & exit)
{
  argagg::parser argparser{
    {{"help", {"-h", "--help"}, "print this help message and exit", 0},
     {"version", {"-V", "--version"}, "print version and exit", 0},
     {"speedrate",
      {"-s", "--speed-rate"},
      "change the speed rate [1;2] (default: 1)",
      1},
     {"timerinterval",
      {"-t", "--timer-interval"},
      "set the timer interval (refresh)",
      1},
     {"fullscreen",
      {"-f", "--fullscreen"},
      "load in fullscreen [on;off] (default: on)",
      1},
     {"zoom",
      {"-z", "--zoom"},
      "change the window scale (only if fullscreen is off) [1;2] (default: 1)",
      1},
     {"legacy",
      {"-l", "--legacy"},
      "start the game in legacy display mode (640x480)",
      0},
     {"renderer",
      {"-r", "--renderer"},
      "set a renderer [auto;software;accelerated] (default: auto)",
      1},
     {"driver",
      {"-d", "--driver"},
      "set a driver [auto;direct3d;direct3d11;opengl;opengles2;opengles] "
      "(default: auto, ignored with "
      "software renderer)",
      1},
     {"enablerecorder",
      {"-c", "--enable-recorder"},
      "enable the recorder feature (F3/F4)",
      0},
     {"playrecord",
      {"-p", "--play-record"},
      "play a record generated by F3 (--enable-recorder)",
      1},
     {"restorebugs",
      {"-b", "--restore-bugs"},
      "restore funny original bugs of older versions < v1.9",
      0},
     {"restoremidi",
      {"-m", "--restore-midi"},
      "restore playback based on MIDI music instead of OGG",
      0},
     {"renderquality",
      {"-q", "--render-quality"},
      "enable anti-aliasing [on;off] (default: on, ignored if "
      "windowed)",
      1}}};

  argagg::parser_results args;
  try
  {
    args = argparser.parse (argc, argv);
  }
  catch (const std::exception & e)
  {
    std::cerr << e.what () << std::endl;
    exit = true;
    return EXIT_FAILURE;
  }

  if (args["help"])
  {
    std::cerr << "Usage: planetblupi [options]" << std::endl << argparser;
    exit = true;
    return EXIT_SUCCESS;
  }

  if (args["version"])
  {
    std::cerr << PLANETBLUPI_VERSION_STR << std::endl;
    exit = true;
    return EXIT_SUCCESS;
  }

  if (args["speedrate"])
  {
    g_speedRate = args["speedrate"];
    g_settingsOverload |= SETTING_SPEEDRATE;
  }

  if (args["timerinterval"])
  {
    g_timerInterval = args["timerinterval"];
    g_settingsOverload |= SETTING_TIMERINTERVAL;
  }

  if (args["fullscreen"])
  {
    g_bFullScreen =
      args["fullscreen"].as<std::string> () != std::string ("off");
    g_settingsOverload |= SETTING_FULLSCREEN;
  }

  if (args["zoom"])
  {
    g_zoom = args["zoom"];
    if (g_zoom != 1 && g_zoom != 2)
      g_zoom = 1;
    g_settingsOverload |= SETTING_ZOOM;
  }

  if (args["legacy"])
  {
    Display::getDisplay ().setDisplaySize (LXLOGIC (), LYLOGIC ());
    g_settingsOverload |= SETTING_LEGACY;
  }

  if (args["renderer"])
  {
    if (args["renderer"].as<std::string> () == "auto")
      g_rendererType = 0;
    else if (args["renderer"].as<std::string> () == "software")
      g_rendererType = SDL_RENDERER_SOFTWARE;
    else if (args["renderer"].as<std::string> () == "accelerated")
      g_rendererType = SDL_RENDERER_ACCELERATED;
    g_settingsOverload |= SETTING_RENDERER;
  }

  if (
    args["driver"] &&
    (!g_rendererType || g_rendererType == SDL_RENDERER_ACCELERATED))
  {
    std::string input = args["driver"].as<std::string> ();
    if (std::regex_match (
          input, std::regex ("direct3d|direct3d11|opengl|opengles2|opengles")))
      SDL_SetHint (SDL_HINT_RENDER_DRIVER, input.c_str ());
    g_settingsOverload |= SETTING_DRIVER;
  }

  if (args["enablerecorder"])
    g_enableRecorder = true;

  if (args["playrecord"])
    g_playRecord = args["playrecord"].as<std::string> ();

  if (args["restorebugs"])
    g_restoreBugs = true;

  if (args["restoremidi"])
  {
    g_restoreMidi = true;
    g_settingsOverload |= SETTING_MIDI;
  }

  if (args["renderquality"])
  {
    g_renderQuality =
      args["renderquality"].as<std::string> () != std::string ("off");
    g_settingsOverload |= SETTING_RENDERQUALITY;
  }

  return EXIT_SUCCESS;
}

// Main initialization function.

static int
DoInit (int argc, char * argv[], bool & exit)
{
  int rc = parseArgs (argc, argv, exit);
  if (exit)
    return rc;

  Point totalDim, iconDim;
  Rect  rcRect;
  bool  bOK;

  bOK = ReadConfig (); // lit le fichier config.json

  if (!bOK) // Something wrong with config.json file?
  {
    InitFail ("Game not correctly installed");
    return EXIT_FAILURE;
  }

#ifdef __LINUX__
  if (!getenv ("ALSA_CONFIG_DIR"))
  {
    static char env[256];
    snprintf (env, sizeof (env), "ALSA_CONFIG_DIR=/usr/share/alsa");
    putenv (env);
  }
#endif /* __LINUX__ */

#ifdef _WIN32
  /* Fix laggy sounds on Windows by not using winmm driver. */
  SDL_setenv ("SDL_AUDIODRIVER", "directsound", true);
#endif /* _WIN32 */

  auto res = SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
  if (res < 0)
  {
    SDL_Log ("Unable to initialize SDL: %s", SDL_GetError ());
    return EXIT_FAILURE;
  }

  if (TTF_Init () < 0)
  {
    SDL_Log ("Couldn't initialize TTF: %s", SDL_GetError ());
    return EXIT_FAILURE;
  }

  GetFonts ();

  if (!(g_settingsOverload & SETTING_LEGACY))
    Display::getDisplay ().readDisplaySize ();

  // Create a window.
  g_window = SDL_CreateWindow (
    gettext ("Planet Blupi"), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    LXIMAGE (), LYIMAGE (), 0);

  if (!g_window)
  {
    printf ("%s", SDL_GetError ());
    return EXIT_FAILURE;
  }

#if 0
  auto icon = IMG_Load ((GetShareDir () + "icons/hicolor/256x256/apps/blupi.png").c_str ());
  SDL_SetWindowIcon (g_window, icon);
  SDL_FreeSurface (icon);
#endif /* 0 */

  g_renderer = SDL_CreateRenderer (
    g_window, -1, g_rendererType | SDL_RENDERER_TARGETTEXTURE);
  if (!g_renderer)
  {
    printf ("%s", SDL_GetError ());
    SDL_DestroyWindow (g_window);
    return EXIT_FAILURE;
  }

  SDL_RenderSetLogicalSize (g_renderer, LXIMAGE (), LYIMAGE ());

  const auto renders = SDL_GetNumRenderDrivers ();
  for (int i = 0; i < renders; ++i)
  {
    SDL_RendererInfo info = {0};
    if (SDL_GetRenderDriverInfo (i, &info))
    {
      SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: failed\n", i);
      continue;
    }

    SDL_LogInfo (
      SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: name=%s", i, info.name);
    SDL_LogInfo (
      SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: flags=%u", i, info.flags);
    SDL_LogInfo (
      SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: num_texture_formats=%u", i,
      info.num_texture_formats);
    SDL_LogInfo (
      SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: max_texture_width=%u", i,
      info.max_texture_width);
    SDL_LogInfo (
      SDL_LOG_CATEGORY_APPLICATION, "renderer[%d]: max_texture_height=%u", i,
      info.max_texture_height);
  }

  // Create the event manager.
  g_pEvent = new CEvent;
  if (g_pEvent == nullptr)
  {
    InitFail ("New event");
    return EXIT_FAILURE;
  }

  // Create the main pixmap.
  g_pPixmap = new CPixmap (g_pEvent);
  if (g_pPixmap == nullptr)
  {
    InitFail ("New pixmap");
    return EXIT_FAILURE;
  }

  totalDim.x = LXIMAGE ();
  totalDim.y = LYIMAGE ();
  if (!g_pPixmap->Create (totalDim))
  {
    InitFail ("Create pixmap");
    return EXIT_FAILURE;
  }

  OutputDebug ("Image: init\n");
  totalDim.x = LXLOGIC ();
  totalDim.y = LYLOGIC ();
  iconDim.x  = 0;
  iconDim.y  = 0;
#if _INTRO
  if (!g_pPixmap->Cache (CHBACK, "intro1.png", totalDim, iconDim))
#else
  if (!g_pPixmap->Cache (CHBACK, "init.png", totalDim, iconDim))
#endif
    return EXIT_FAILURE;

  OutputDebug ("Image: init\n");
  totalDim.x = LXIMAGE ();
  totalDim.y = LYIMAGE ();
  if (!g_pPixmap->Cache (CHGROUND, totalDim))
    return EXIT_FAILURE;

  rcRect.left   = 0;
  rcRect.top    = 0;
  rcRect.right  = LXIMAGE ();
  rcRect.bottom = LYIMAGE ();
  g_pPixmap->DrawImage (-1, CHBACK, rcRect); // dessine le fond
  g_pPixmap->Display ();

  totalDim.x = DIMCELX * 2 * 16;
  totalDim.y = DIMCELY * 2 * 6;
  iconDim.x  = DIMCELX * 2;
  iconDim.y  = DIMCELY * 2;
  if (!g_pPixmap->Cache (CHFLOOR, "floor000.png", totalDim, iconDim))
  {
    InitFail ("Cache floor000.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMOBJX * 16;
  totalDim.y = DIMOBJY * 8;
  iconDim.x  = DIMOBJX;
  iconDim.y  = DIMOBJY;
  if (!g_pPixmap->Cache (CHOBJECT, "obj000.png", totalDim, iconDim))
  {
    InitFail ("Cache obj000.png");
    return EXIT_FAILURE;
  }

  if (!g_pPixmap->Cache (CHOBJECTo, "obj-o000.png", totalDim, iconDim))
  {
    InitFail ("Cache obj-o000.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMBLUPIX * 16;
  totalDim.y = DIMBLUPIY * 23;
  iconDim.x  = DIMBLUPIX;
  iconDim.y  = DIMBLUPIY;
  if (!g_pPixmap->Cache (CHBLUPI, "blupi.png", totalDim, iconDim))
  {
    InitFail ("Cache blupi.png");
    return EXIT_FAILURE;
  }

  totalDim.x = 64;
  totalDim.y = 66;
  iconDim.x  = 64;
  iconDim.y  = 66 / 2;
  if (!g_pPixmap->Cache (CHHILI, "hili.png", totalDim, iconDim))
  {
    InitFail ("Cache hili.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMCELX * 2 * 3;
  totalDim.y = DIMCELY * 2 * 5;
  iconDim.x  = DIMCELX * 2;
  iconDim.y  = DIMCELY * 2;
  if (!g_pPixmap->Cache (CHFOG, "fog.png", totalDim, iconDim))
  {
    InitFail ("Cache fog.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMCELX * 2 * 16;
  totalDim.y = DIMCELY * 2 * 1;
  iconDim.x  = DIMCELX * 2;
  iconDim.y  = DIMCELY * 2;
  if (!g_pPixmap->Cache (CHMASK1, "mask1.png", totalDim, iconDim))
  {
    InitFail ("Cache mask1.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMCELX * 2 * 16;
  totalDim.y = DIMCELY * 2 * 1;
  iconDim.x  = DIMCELX * 2;
  iconDim.y  = DIMCELY * 2;
  if (!g_pPixmap->Cache (CHMASK2, "mask2.png", totalDim, iconDim))
  {
    InitFail ("Cache mask2.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMBUTTONX * 6;
  totalDim.y = DIMBUTTONY * 21;
  iconDim.x  = DIMBUTTONX;
  iconDim.y  = DIMBUTTONY;
  if (!g_pPixmap->Cache (CHBUTTON, "button00.png", totalDim, iconDim))
  {
    InitFail ("Cache button00.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMJAUGEX * 1;
  totalDim.y = DIMJAUGEY * 4;
  iconDim.x  = DIMJAUGEX;
  iconDim.y  = DIMJAUGEY;
  if (!g_pPixmap->Cache (CHJAUGE, "jauge.png", totalDim, iconDim))
  {
    InitFail ("Cache jauge.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMTEXTX * 16;
  totalDim.y = DIMTEXTY * 16 * 3;
  iconDim.x  = DIMTEXTX;
  iconDim.y  = DIMTEXTY;
  if (!g_pPixmap->Cache (CHTEXT, "text.png", totalDim, iconDim))
  {
    InitFail ("Cache text.png");
    return EXIT_FAILURE;
  }

  totalDim.x = DIMLITTLEX * 16;
  totalDim.y = DIMLITTLEY * 19;
  iconDim.x  = DIMLITTLEX;
  iconDim.y  = DIMLITTLEY;
  if (!g_pPixmap->Cache (CHLITTLE, "little.png", totalDim, iconDim))
  {
    InitFail ("Cache little.png");
    return EXIT_FAILURE;
  }

  totalDim.x = 426;
  totalDim.y = 52;
  iconDim.x  = 426;
  iconDim.y  = 52;
  if (!g_pPixmap->Cache (CHBIGNUM, "bignum.png", totalDim, iconDim))
  {
    InitFail ("Cache bignum.png");
    return EXIT_FAILURE;
  }

  // Create the sound manager.
  g_pSound = new CSound;
  if (g_pSound == nullptr)
  {
    InitFail ("New sound");
    return EXIT_FAILURE;
  }

  g_pSound->Create ();
  g_pSound->CacheAll ();
  g_pSound->SetState (true);

  // Create the movie manager.
  g_pMovie = new CMovie (g_pPixmap);
  if (g_pMovie == nullptr)
  {
    InitFail ("New movie");
    return EXIT_FAILURE;
  }

  g_pMovie->Create ();

  // Create the decor manager.
  g_pDecor = new CDecor;
  if (g_pDecor == nullptr)
  {
    InitFail ("New decor");
    return EXIT_FAILURE;
  }

  g_pDecor->Create (g_pSound, g_pPixmap);
  g_pDecor->MapInitColors ();

  const bool zoom = g_zoom;

  const bool renderQuality = g_renderQuality;
  g_pEvent->Create (g_pPixmap, g_pDecor, g_pSound, g_pMovie);
  if (renderQuality != g_renderQuality)
    g_pPixmap->CreateMainTexture ();

  // Load all cursors
  g_pPixmap->LoadCursors ();
  g_pPixmap->ChangeSprite (SPRITE_WAIT);

#ifdef USE_CURL
  g_updateThread = new std::thread (CheckForUpdates);
#endif /* USE_CURL */

  if (zoom != g_zoom)
    g_pEvent->SetWindowSize (g_zoom);
  g_pEvent->SetFullScreen (g_bFullScreen);
  g_pEvent->ChangePhase (EV_PHASE_INTRO1);

  g_bTermInit = true;
  return EXIT_SUCCESS;
}

static void
initGettext ()
{
  setlocale (LC_ALL, "");
  textdomain ("planetblupi");
  bindtextdomain ("planetblupi", (GetShareDir () + "locale").c_str ());
  bind_textdomain_codeset ("planetblupi", "UTF-8");
}

int
main (int argc, char * argv[])
{
  initGettext ();

  int  res  = 0;
  bool exit = false;
  if ((res = DoInit (argc, argv, exit)) || exit)
    return res;

  Platform::run (HandleEvent);

  DisposeFonts ();
  FinishObjects ();

  if (g_renderer)
    SDL_DestroyRenderer (g_renderer);

  if (g_window)
    SDL_DestroyWindow (g_window);

  SDL_Quit ();

  if (g_updateThread)
  {
    g_updateAbort = true;
    g_updateThread->join ();
    delete (g_updateThread);
  }

  /* Restart the game when the fullscreen mode (ratio) has changed. */
  if (g_restart != RestartMode::NO)
  {
    std::vector<const char *> _argv;
    std::string               argv0;
    std::string               bin = basename (argv[0]);

    if (getenv ("APPIMAGE"))
      argv0 = getenv ("APPIMAGE");
    else
      argv0 = GetBinDir () + bin;

    _argv.push_back (bin.c_str ());
    if (g_restart == RestartMode::LEGACY)
    {
      _argv.push_back ("--legacy");
      _argv.push_back ("--fullscreen");
      _argv.push_back ("on");
      _argv.push_back ("--zoom");
      _argv.push_back ("2");
    }
    _argv.push_back (nullptr);

    SDL_Log ("Reload the game from %s", argv0.c_str ());
    execv (argv0.c_str (), const_cast<char **> (&_argv[0]));
  }

  return 0;
}
