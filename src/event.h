/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017, Mathieu Schroeter
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

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "blupi.h"
#include "button.h"
#include "menu.h"
#include "progress.h"

class CMovie;

/////////////////////////////////////////////////////////////////////////////

typedef struct {
  Uint32       message;
  Sint32       type;
  Sint32       iconMenu[20];
  Sint32       x, y;
  const char * toolTips[16];
} Button;

typedef struct {
  Uint32 phase;
  char   backName[20];
  Sint32 bCDrom;
  Button buttons[MAXBUTTON];
} Phase;

typedef struct {
  Sint16 majRev;
  Sint16 minRev;
  Sint16 bSchool;
  Sint16 bPrivate;
  Sint16 world;
  Sint16 skill;
  Sint16 reserve1[99];
} DemoHeader;

typedef struct {
  Sint32 time;
  Uint32 message;
  Uint32 wParam; // WPARAM
  Uint32 lParam; // LPARAM
} DemoEvent;

enum class Language {
  en    = 0,
  en_US = 1,
  fr    = 2,
  de    = 3,
};

class CEvent
{
public:
  CEvent ();
  ~CEvent ();

  POINT GetMousePos ();
  void
         Create (CPixmap * pPixmap, CDecor * pDecor, CSound * pSound, CMovie * pMovie);
  void   SetFullScreen (bool bFullScreen);
  Sint32 GetWorld ();
  Sint32 GetPhysicalWorld ();
  Sint32 GetImageWorld ();
  bool   IsHelpHide ();
  bool   ChangePhase (Uint32 phase);
  void   MovieToStart ();
  Uint32 GetPhase ();
  void   TryInsert ();

  Sint32 GetButtonIndex (Sint32 button);
  Sint32 GetState (Sint32 button);
  void   SetState (Sint32 button, Sint32 state);
  bool   GetEnable (Sint32 button);
  void   SetEnable (Sint32 button, bool bEnable);
  bool   GetHide (Sint32 button);
  void   SetHide (Sint32 button, bool bHide);
  Sint32 GetMenu (Sint32 button);
  void   SetMenu (Sint32 button, Sint32 menu);

  bool         DrawButtons ();
  MouseSprites MousePosToSprite (POINT pos);
  void         MouseSprite (POINT pos);
  void         WaitMouse (bool bWait);
  void         HideMouse (bool bHide);
  POINT        GetLastMousePos ();
  bool         TreatEvent (const SDL_Event & event);
  bool         TreatEventBase (const SDL_Event & event);

  void DecorAutoShift ();

  bool StartMovie (const std::string & pFilename);
  void StopMovie ();
  bool IsMovie ();

  void Read (Sint32 message);
  void Write (Sint32 message);

  void   SetSpeed (Sint32 speed);
  Sint32 GetSpeed ();
  bool   GetPause ();
  bool   IsShift ();

  void DemoStep ();

  void IntroStep ();

  Uint8 GetWindowScale ();

public:
  static void PushUserEvent (Sint32 code, void * data = nullptr);

protected:
  void DrawTextCenter (const char * text, Sint32 x, Sint32 y, Sint32 font = 0);
  bool CreateButtons ();
  bool EventButtons (const SDL_Event & event, POINT pos);
  bool MouseOnButton (POINT pos);
  Sint32 SearchPhase (Uint32 phase);
  void   DecorShift (Sint32 dx, Sint32 dy);

  bool PlayDown (POINT pos, const SDL_Event & event);
  bool PlayMove (POINT pos);
  bool PlayUp (POINT pos);

  void SetLanguage ();
  void SetWindowSize (Uint8 prevScale, Uint8 newScale);

  void ChangeButtons (Sint32 message);

  void BuildFloor (POINT cel, Sint32 insIcon);
  void BuildWater (POINT cel, Sint32 insIcon);
  bool BuildDown (POINT pos, Uint16 mod, bool bMix = true);
  bool BuildMove (POINT pos, Uint16 mod, const SDL_Event & event);

  void PrivateLibelle ();
  bool ReadLibelle (Sint32 world, bool bSchool, bool bHelp);
  bool WriteInfo ();
  bool ReadInfo ();

  void DemoRecStart ();
  void DemoRecStop ();
  bool DemoPlayStart ();
  void DemoPlayStop ();
  static void
       WinToSDLEvent (Uint32 msg, WPARAM wParam, LPARAM lParam, SDL_Event & event);
  void DemoRecEvent (Uint32 message, WPARAM wParam, LPARAM lParam);

protected:
  std::vector<Language>                    m_Languages;
  std::vector<Language>::iterator          m_Lang;
  Sint32                                   m_speed;
  Sint32                                   m_exercice;
  Sint32                                   m_mission;
  Sint32                                   m_private;
  Sint32                                   m_maxMission;
  Uint32                                   m_phase;
  Sint32                                   m_index;
  bool                                     m_bSchool;
  bool                                     m_bPrivate;
  bool                                     m_bAccessBuild;
  bool                                     m_bFullScreen;
  Uint8                                    m_WindowScale;
  CPixmap *                                m_pPixmap;
  CDecor *                                 m_pDecor;
  CSound *                                 m_pSound;
  CMovie *                                 m_pMovie;
  std::string                              m_movieToStart;
  Sint32                                   m_phaseAfterMovie;
  CButton                                  m_buttons[MAXBUTTON];
  Sint32                                   m_lastFloor[MAXBUTTON];
  Sint32                                   m_lastObject[MAXBUTTON];
  Sint32                                   m_lastHome[MAXBUTTON];
  bool                                     m_bRunMovie;
  bool                                     m_bBuildModify;
  CJauge                                   m_jauges[2];
  CMenu                                    m_menu;
  bool                                     m_bMenu;
  POINT                                    m_menuPos;
  Sint32                                   m_menuNb;
  Buttons                                  m_menuButtons[MAXBUTTON];
  Errors                                   m_menuErrors[MAXBUTTON];
  std::unordered_map<Sint32, const char *> m_menuTexts;
  Sint32                                   m_menuPerso;
  POINT                                    m_menuCel;
  POINT                                    m_oldMousePos;
  bool                                     m_bMouseDown;
  bool                                     m_bHili;
  Sint32                                   m_fileWorld[10];
  Sint32                                   m_fileTime[10];
  POINT                                    m_posToolTips;
  char                                     m_textToolTips[50];
  MouseSprites                             m_mouseSprite;
  bool                                     m_bFillMouse;
  bool                                     m_bWaitMouse;
  bool                                     m_bHideMouse;
  Sint32                                   m_rankCheat;
  Sint32                                   m_posCheat;
  bool                                     m_bMovie;
  bool                                     m_bSpeed;
  bool                                     m_bHelp;
  bool                                     m_bAllMissions;
  bool                                     m_bChangeCheat;
  Sint32                                   m_scrollSpeed;
  bool                                     m_bPause;
  bool                                     m_bShift;
  Sint32                                   m_shiftPhase;
  POINT                                    m_shiftVector;
  POINT                                    m_shiftOffset;
  char                                     m_libelle[1000];
  Sint32                                   m_tryPhase;
  Sint32                                   m_tryInsertCount;
  POINT                                    m_posInfoButton;
  POINT                                    m_posHelpButton;
  bool                                     m_bHiliInfoButton;
  bool                                     m_bHiliHelpButton;
  bool                                     m_bInfoHelp;
  bool                                     m_bDemoRec;
  bool                                     m_bDemoPlay;
  DemoEvent *                              m_pDemoBuffer;
  Sint32                                   m_demoTime;
  size_t                                   m_demoIndex;
  size_t                                   m_demoEnd;
  Sint32                                   m_demoNumber;
  Uint16                                   m_keymod;
  POINT                                    m_debugPos;
  Sint32                                   m_introTime;
};

/////////////////////////////////////////////////////////////////////////////
