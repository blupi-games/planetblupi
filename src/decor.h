/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2018, Mathieu Schroeter
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

#include "def.h"
#include "pixmap.h"
#include "sound.h"
#include <unordered_map>

#define MAXENERGY 4000
#define MAXFIRE 400

// clang-format off
#define ICON_HILI_STAT  112
#define ICON_HILI_SEL   113
#define ICON_HILI_ANY   114
#define ICON_HILI_OP    115
#define ICON_HILI_GO    117
#define ICON_HILI_BUILD 118
#define ICON_HILI_ERR   119
// clang-format on

// Descripteur d'une cellule du décor.
typedef struct {
  Sint16 floorChannel;
  Sint16 floorIcon;
  Sint16 objectChannel;
  Sint16 objectIcon;
  Sint16 fog;       // brouillard
  Sint16 rankMove;  // rang dans m_move
  Sint16 workBlupi; // rang du blupi travaillant ici
  Sint16 fire;
} Cellule;
// Cette structure doit être la plus petite possible, car
// il en existe un tableau de 100x100 = 10'000 cellules !

typedef struct : Cellule {
  bool flagged; // when a flag has been built for the iron
} CellMem;

// Descripteur d'un blupi animé.
#define MAXBLUPI 100
#define MAXUSED 50
#define MAXLIST 10

typedef struct {
  Sint32 bExist; // true -> utilisé
  Sint32 bHili;  // true -> sélectionné

  Sint16 perso; // personnage, voir (*)

  Sint16 goalAction; // action (Sint32 terme)
  Sint16 goalPhase;  // phase (Sint32 terme)
  Point  goalCel;    // cellule visée (Sint32 terme)
  Point  passCel;    // cellule tranversante

  Sint16 energy; // énergie restante

  Point  cel;     // cellule actuelle
  Point  destCel; // cellule destination
  Sint16 action;  // action en cours
  Sint16 aDirect; // direction actuelle
  Sint16 sDirect; // direction souhaitée

  Point  pos;  // position relative à partir de la cellule
  Sint16 posZ; // déplacement z
  Sint16 channel;
  Sint16 lastIcon;
  Sint16 icon;
  Sint16 phase;     // phase dans l'action
  Sint16 step;      // pas global
  Sint16 interrupt; // 0=prioritaire, 1=normal, 2=misc
  Sint16 clipLeft;

  Sint32 nbUsed; // nb de points déjà visités
  char   nextRankUsed;
  Point  posUsed[MAXUSED];
  char   rankUsed[MAXUSED];

  Sint16 takeChannel; // objet transporté
  Sint16 takeIcon;

  Point fix; // point fixe (cultive, pont)

  Sint16 jaugePhase;
  Sint16 jaugeMax;
  Sint16 stop;     // 1 -> devra stopper
  Sint16 bArrow;   // true -> flèche en dessus de blupi
  Sint16 bRepeat;  // true -> répète l'action
  Sint16 nLoop;    // nb de boucles pour GOAL_OTHERLOOP
  Sint16 cLoop;    // boucle en cours
  Sint16 vIcon;    // icône variable
  Point  goalHili; // but visé
  Sint16 bMalade;  // true -> blupi malade
  Sint16 bCache;   // true -> caché (pas dessiné)
  Sint16 vehicule; // véhicule utilisé par blupi, voir (**)
  char   busyCount;
  char   busyDelay;
  char   clicCount;
  char   clicDelay;
  char   reserve2[2];
  Sint16 listButton[MAXLIST];
  Point  listCel[MAXLIST];
  Sint16 listParam[MAXLIST];
  Sint16 repeatLevelHope;
  Sint16 repeatLevel;
  Sint16 reserve3[88];
} Blupi;

// (*)  Personnages :
//      0 -> blupi
//      1 -> araignée
//      2 -> virus
//      3 -> tracks
//      4 -> robot
//      5 -> bombe
//      6 -> détonnateur de mine (invisible)
//      7 -> électro
//      8 -> disciple (robot2)

// (**) Véhicule :
//      0 -> à pied
//      1 -> en bateau
//      2 -> en jeep
//      3 -> armure

// Descripteur d'un décor animé.
#define MAXMOVE 100
#define MOVEICONNB 1000

typedef struct {
  Sint32 bExist; // true -> utilisé

  Point  cel;       // cellule du décor
  Sint16 rankBlupi; // blupi travaillant ici

  Sint32 bFloor; // true -> floor, false -> object
  Sint16 channel;
  Sint16 icon;
  Sint16 maskChannel;
  Sint16 maskIcon;
  Sint16 phase;     // phase pour pMoves ou pIcon
  Sint16 rankMoves; // *nb,dx,dy,...
  Sint16 rankIcons; // *nb,i,i,...

  Sint16 total; // nb total d'étapes
  Sint16 delai; // délai entre deux pas
  Sint16 stepY; // pas vertical *100

  Sint16 cTotal;
  Sint16 cDelai;
} Move;

#define MAXLASTDRAPEAU 50

class CDecor
{
public:
  CDecor ();
  ~CDecor ();

  // Arrange.cpp
  void ArrangeFloor (Point cel);
  void ArrangeMur (Point cel, Sint32 & icon, Sint32 index);
  void ArrangeBuild (Point cel, Sint32 & channel, Sint32 & icon);
  void ArrangeObject (Point cel);

  bool ArrangeFillTestFloor (Point cel1, Point cel2);
  bool ArrangeFillTest (Point pos);
  void ArrangeFillPut (Point pos, Sint32 channel, Sint32 icon);
  void ArrangeFillSearch (Point pos);
  void ArrangeFill (Point pos, Sint32 channel, Sint32 icon, bool bFloor);

  void ArrangeBlupi ();

  // Obstacle.cpp
  void SearchFloor (Sint32 rank, Sint32 icon, Point cel, Sint32 * pBits);
  void SearchObject (Sint32 rank, Sint32 icon, Point cel, Sint32 * pBits);
  void AjustFloor (Sint32 rank, Sint32 icon, Point cel, Sint32 * pBits);
  void AjustObject (Sint32 rank, Sint32 icon, Point cel, Sint32 * pBits);
  bool IsFreeDirect (Point cel, Sint32 direct, Sint32 rank);
  bool IsFreeCelObstacle (Point cel);
  bool IsFreeCelFloor (Point cel, Sint32 rank);
  bool IsFreeCelGo (Point cel, Sint32 rank);
  bool IsFreeCelHili (Point cel, Sint32 rank);
  bool IsFreeCel (Point cel, Sint32 rank);
  bool IsFreeCelDepose (Point cel, Sint32 rank);
  bool
  IsFreeCelEmbarque (Point cel, Sint32 rank, Sint32 & action, Point & limit);
  bool
         IsFreeCelDebarque (Point cel, Sint32 rank, Sint32 & action, Point & limit);
  bool   IsFreeJump (Point cel, Sint32 direct, Sint32 rank, Sint32 & action);
  bool   IsFreeGlisse (Point cel, Sint32 direct, Sint32 rank, Sint32 & action);
  Sint32 DirectSearch (Point cel, Point goal);
  void   FlushUsed (Sint32 rank);
  void   AddUsedPos (Sint32 rank, Point pos);
  bool   IsUsedPos (Sint32 rank, Point pos);
  bool   SearchBestBase (
      Sint32 rank, Sint32 & action, Point & newCel, Sint32 & direct);
  bool SearchBestPass (Sint32 rank, Sint32 & action);
  bool IsWorkableObject (Point cel, Sint32 rank);
  bool SearchOtherObject (
    Sint32 rank, Point initCel, Sint32 action, Sint32 distMax, Sint32 channel,
    Sint32 firstIcon1, Sint32 lastIcon1, Sint32 firstIcon2, Sint32 lastIcon2,
    Point & foundCel, Sint32 & foundIcon);
  bool SearchOtherDrapeau (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool SearchOtherBateau (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool IsSpiderObject (Sint32 icon);
  bool SearchSpiderObject (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool IsTracksObject (Sint32 icon);
  bool SearchTracksObject (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool IsRobotObject (Sint32 icon);
  bool SearchRobotObject (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon, Sint32 & foundAction);
  bool IsBombeObject (Sint32 icon);
  bool SearchBombeObject (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool SearchElectroObject (
    Sint32 rank, Point initCel, Sint32 distMax, Point & foundCel,
    Sint32 & foundIcon);
  bool   IsUsineBuild (Sint32 rank, Point cel);
  bool   IsUsineFree (Sint32 rank, Point cel);
  bool   IsFireCel (Point cel);
  bool   IsVirusCel (Point cel);
  Errors IsBuildPont (Point & cel, Sint32 & iconBuild);
  bool   IsBuildBateau (Point cel, Sint32 & direct);
  void   InitDrapeau ();
  void   AddDrapeau (Point cel);
  void   SubDrapeau (Point cel);
  bool   TestDrapeau (Point cel);

  // DecBlupi.cpp
  void   BlupiFlush ();
  Sint32 BlupiCreate (
    Point cel, Sint32 action, Sint32 direct, Sint32 perso, Sint32 energy);
  bool   BlupiDelete (Point cel, Sint32 perso = -1);
  void   BlupiDelete (Sint32 rank);
  void   BlupiKill (Sint32 exRank, Point cel, Sint32 type);
  bool   BlupiIfExist (Sint32 rank);
  void   BlupiCheat (Sint32 cheat);
  void   BlupiActualise (Sint32 rank);
  void   BlupiAdaptIcon (Sint32 rank);
  void   BlupiPushFog (Sint32 rank);
  void   BlupiSound (Sint32 rank, Sounds sound, Point pos, bool bStop = false);
  void   BlupiInitAction (Sint32 rank, Sint32 action, Sint32 direct = -1);
  void   BlupiChangeAction (Sint32 rank, Sint32 action, Sint32 direct = -1);
  void   ListFlush (Sint32 rank);
  Sint32 ListGetParam (Sint32 rank, Buttons button, Point cel);
  bool   ListPut (Sint32 rank, Buttons button, Point cel, Point cMem);
  void   ListRemove (Sint32 rank);
  Sint32 ListSearch (
    Sint32 rank, Buttons button, Point cel, const char *& textForButton);
  bool RepeatAdjust (
    Sint32 rank, Sint32 button, Point & cel, Point & cMem, Sint32 param,
    Sint32 list);
  void    GoalStart (Sint32 rank, Sint32 action, Point cel);
  bool    GoalNextPhase (Sint32 rank);
  void    SetTotalTime (Sint32 total);
  Sint32  GetTotalTime ();
  void    GoalInitJauge (Sint32 rank);
  void    GoalInitPassCel (Sint32 rank);
  void    GoalAdjustCel (Sint32 rank, Sint32 & x, Sint32 & y);
  bool    GoalNextOp (Sint32 rank, Sint16 * pTable);
  void    GoalUnwork (Sint32 rank);
  void    GoalStop (Sint32 rank, bool bError = false, bool bSound = true);
  bool    BlupiIsGoalUsed (Point cel);
  void    BlupiStartStopRayon (Sint32 rank, Point startCel, Point endCel);
  bool    BlupiRotate (Sint32 rank);
  bool    BlupiNextAction (Sint32 rank);
  void    BlupiNextGoal (Sint32 rank);
  void    BlupiStep (bool bFirst);
  void    BlupiGetRect (Sint32 rank, Rect & rect);
  Sint32  GetTargetBlupi (Point pos);
  void    BlupiDeselect ();
  void    BlupiDeselect (Sint32 rank);
  void    BlupiSetArrow (Sint32 rank, bool bArrow);
  void    InitOutlineRect ();
  void    BlupiHiliDown (Point pos, bool bAdd = false);
  void    BlupiHiliMove (Point pos);
  void    BlupiHiliUp (Point pos);
  void    BlupiDrawHili ();
  Buttons GetDefButton (Point cel);
  bool    BlupiGoal (Sint32 rank, Buttons button, Point cel, Point cMem);
  void    BlupiGoal (Point cel, Buttons button);
  void    BlupiDestCel (Sint32 rank);
  bool    IsTracksHere (Point cel, bool bSkipInMove);
  bool IsBlupiHereEx (Point cel1, Point cel2, Sint32 exRank, bool bSkipInMove);
  bool IsBlupiHereEx (Point cel, Sint32 exRank, bool bSkipInMove);
  bool IsBlupiHere (Point cel, bool bSkipInMove);
  bool IsBlupiHere (Point cel, Sint32 direct, bool bSkipInMove);
  void GetLevelJauge (Sint32 * pLevels, Sint32 * pTypes);
  bool IsWorkBlupi (Sint32 rank);
  void BlupiGetButtons (
    Point pos, Sint32 & nb, Buttons * pButtons, Errors * pErrors,
    std::unordered_map<Sint32, const char *> & texts, Sint32 & perso);
  void   TerminatedInit ();
  Sint32 IsTerminated ();
  Term * GetTerminated ();

  // DecMove.cpp
  bool   CanBurn (Point cel);
  void   MoveFlush ();
  Sint32 MoveMaxFire ();
  void   MoveFixInit ();
  bool   MoveCreate (
      Point cel, Sint32 rankBlupi, bool bFloor, Sint32 channel, Sint32 icon,
      Sint32 maskChannel, Sint32 maskIcon, Sint32 total, Sint32 delai,
      Sint32 stepY, bool bMisc = false, bool bNotIfExist = false);
  bool MoveAddMoves (Point cel, Sint32 rankMoves);
  bool MoveAddIcons (Point cel, Sint32 rankIcons, bool bContinue = false);
  bool MoveStartFire (Point cel);
  void MoveProxiFire (Point cel);
  void MoveFire (Sint32 rank);
  void MoveStep (bool bFirst);
  void MoveFinish (Point cel);
  void MoveFinish (Sint32 rankBlupi);
  bool MoveIsUsed (Point cel);
  bool MoveGetObject (Point cel, Sint32 & channel, Sint32 & icon);
  bool MovePutObject (Point cel, Sint32 channel, Sint32 icon);

  // DecIO.cpp
  bool Write (Sint32 rank, bool bUser, Sint32 world, Sint32 time, Sint32 total);
  bool
       Read (Sint32 rank, bool bUser, Sint32 & world, Sint32 & time, Sint32 & total);
  bool FileExist (
    Sint32 rank, bool bUser, Sint32 & world, Sint32 & time, Sint32 & total);
  void Flush ();

  // DecMap.cpp
  void  MapInitColors ();
  Point ConvCelToMap (Point cel);
  Point ConvMapToCel (Point pos);
  bool  MapMove (Point pos);
  void  MapPutCel (Point pos);
  bool  GenerateMap ();

  // DecStat.cpp
  void   StatisticInit ();
  void   StatisticUpdate ();
  Sint32 StatisticGetBlupi ();
  Sint32 StatisticGetFire ();
  void   StatisticDraw ();
  void   GenerateStatictic ();
  bool   StatisticDown (Point pos);
  bool   StatisticMove (Point pos, bool & disable);
  bool   StatisticUp (Point pos);
  Sint32 StatisticDetect (Point pos, bool & disable);

  // Chemin.cpp
  void   CheminMemPos (Sint32 exRank);
  bool   CheminTestPos (Point pos, Sint32 & rank);
  Sint32 CheminARebours (Sint32 rank);
  void   CheminFillTerrain (Sint32 rank);
  bool   CheminTestDirection (
      Sint32 rank, Sint32 pos, Sint32 dir, Sint32 & next, Sint32 & ampli,
      Sint32 & cout, Sint32 & action);
  bool CheminCherche (Sint32 rank, Sint32 & action);
  bool IsCheminFree (Sint32 rank, Point dest, Sint32 button);

  // Decor.cpp
  void  FixShifting (Sint32 & nbx, Sint32 & nby, Point & iCel, Point & iPos);
  void  SetShiftOffset (Point offset);
  Point ConvCelToPos (Point cel);
  Point ConvPosToCel (Point pos, bool bMap = false);
  Point ConvPosToCel2 (Point pos);

  void Create (CSound * pSound, CPixmap * pPixmap);
  void Init (Sint32 channel, Sint32 icon);
  void InitAfterBuild ();
  void ResetHili ();
  bool LoadImages ();
  void ClearFog ();
  void ClearFire ();
  void SetBuild (bool bBuild);
  void EnableFog (bool bEnable);
  bool GetInvincible ();
  void SetInvincible (bool bInvincible);
  bool GetSuper ();
  void SetSuper (bool bSuper);
  void FlipOutline ();
  bool PutFloor (Point cel, Sint32 channel, Sint32 icon);
  bool PutObject (Point cel, Sint32 channel, Sint32 icon);
  bool GetFloor (Point cel, Sint32 & channel, Sint32 & icon);
  bool GetObject (Point cel, Sint32 & channel, Sint32 & icon);
  bool SetFire (Point cel, bool bFire);

  void  SetCorner (Point corner, bool bCenter = false);
  Point GetCorner ();
  Point GetHome ();
  void  MemoPos (Sint32 rank, bool bRecord);

  void   SetTime (Sint32 time);
  Sint32 GetTime ();

  void   SetMusic (Sint32 music);
  Sint32 GetMusic ();

  void   SetSkill (Sint32 skill);
  Sint32 GetSkill ();

  void   SetRegion (Sint32 region);
  Sint32 GetRegion ();

  void   SetInfoMode (bool bInfo);
  bool   GetInfoMode ();
  void   SetInfoHeight (Sint32 height);
  Sint32 GetInfoHeight ();

  char * GetButtonExist ();

  void BuildPutBlupi ();
  void BuildMoveFloor (Sint32 x, Sint32 y, Point pos, Sint32 rank);
  void BuildMoveObject (Sint32 x, Sint32 y, Point pos, Sint32 rank);
  void BuildGround (Rect clip);
  void Build (Rect clip, Point posMouse);
  void NextPhase (Sint32 mode);

  Sint32 CountFloor (Sint32 channel, Sint32 icon);
  Errors CelOkForAction (
    Point cel, Sint32 action, Sint32 rank, Sint32 icons[4][4],
    Point & celOutline1, Point & celOutline2);
  Errors       CelOkForAction (Point cel, Sint32 action, Sint32 rank);
  Sint32       GetHiliRankBlupi (Sint32 nb);
  void         CelHili (Point pos, Sint32 action);
  void         CelHiliButton (Point cel, Sint32 button);
  void         CelHiliRepeat (Sint32 list);
  const char * GetResHili (Point posMouse);
  void         HideTooltips (bool bHide);

  void UndoOpen ();
  void UndoClose ();
  void UndoCopy ();
  void UndoBack ();
  bool IsUndo ();

protected:
  bool   GetSeeBits (Point cel, char * pBits, Sint32 index);
  Sint32 GetSeeIcon (char * pBits, Sint32 index);

protected:
  CSound *  m_pSound;
  CPixmap * m_pPixmap;
  Cellule * m_pUndoDecor;
  Cellule   m_decor[MAXCELX / 2][MAXCELY / 2];
  CellMem   m_decorMem[MAXCELX / 2][MAXCELY / 2];
  Sint16    m_rankBlupi[MAXCELX][MAXCELY];
  Blupi     m_blupi[MAXBLUPI];
  Move      m_move[MAXMOVE];
  Point     m_celCorner; // cellule sup/gauche
  Point     m_celHome;   // pour touche Home
  Point     m_celHili;
  Point     m_celOutline1;
  Point     m_celOutline2;
  Point     m_shiftOffset;
  Sint32    m_iconHili[4][4];
  Sint32    m_rankHili; // rang du blupi visé
  bool      m_bHiliRect;
  Point     m_p1Hili; // coins rectangle de sélection
  Point     m_p2Hili;
  Sint32    m_shiftHili;
  Sint32    m_nbBlupiHili;   // nb de blupi sélectionnés
  Sint32    m_rankBlupiHili; // rang blupi sélectionné
  bool      m_bFog;          // true -> brouillard (jeu)
  bool      m_bBuild;        // true -> construction
  bool      m_bInvincible;   // true -> cheat code
  bool      m_bSuper;        // true -> cheat code
  Uint32    m_colors[100];
  Sint32    m_time;            // temps relatif global
  Sint32    m_timeConst;       // temps relatif global constant
  Sint32    m_timeFlipOutline; // temps quand basculer mode outline
  Sint32    m_totalTime;       // temps total passé sur une partie
  Sint32    m_phase;           // phase pour la carte
  Point     m_celArrow;        // cellule avec flèche
  bool      m_bOutline;
  bool      m_bGroundRedraw;
  char      m_buttonExist[MAXBUTTON];
  Sint32    m_statNb;             // nb de statistiques
  Sint32    m_statFirst;          // première statistique visible
  Sint32    m_bStatUp;            // flèche up statistique
  Sint32    m_bStatDown;          // flèche down statistique
  Sint32    m_statHili;           // statistique survolée
  bool      m_bStatRecalc;        // true -> recalcule les statistiques
  Sint32    m_nbStatHach;         // nb de hachures
  Sint32    m_nbStatHachBlupi;    // hachures occupées par blupi
  Sint32    m_nbStatHachPlanche;  // hachures occupées par planches
  Sint32    m_nbStatHachTomate;   // hachures occupées par tomates
  Sint32    m_nbStatHachMetal;    // hachures occupées par métal
  Sint32    m_nbStatHachRobot;    // hachures occupées par robot
  Sint32    m_nbStatHome;         // nb de maisons
  Sint32    m_nbStatHomeBlupi;    // maisons occupées par blupi
  Sint32    m_nbStatRobots;       // nb d'ennemis
  Term      m_term;               // conditions pour gagner
  Sint32    m_winCount;           // compteur avant gagné
  Sint32    m_winLastHachBlupi;   // dernier nombre atteint
  Sint32    m_winLastHachPlanche; // dernier nombre atteint
  Sint32    m_winLastHachTomate;  // dernier nombre atteint
  Sint32    m_winLastHachMetal;   // dernier nombre atteint
  Sint32    m_winLastHachRobot;   // dernier nombre atteint
  Sint32    m_winLastHome;        // dernier nombre atteint
  Sint32    m_winLastHomeBlupi;   // dernier nombre atteint
  Sint32    m_winLastRobots;      // dernier nombre atteint
  Sint32    m_music;              // numéro musique
  Sint32    m_region;             // numéro région (*)
  Sint32    m_lastRegion;         // numéro dernière région
  Sint32    m_blupiHere;
  Point     m_lastDrapeau[MAXLASTDRAPEAU];
  bool      m_bHideTooltips; // true -> menu présent
  char      m_text[50];
  Point     m_textLastPos;
  Sint32    m_textCount;
  Sint32    m_skill;
  bool      m_bInfo;
  Sint32    m_infoHeight;
  Point     m_memoPos[4];

  Uint8  m_cheminWork[MAXCELX * MAXCELY];
  Sint32 m_cheminNbPos;
  Point  m_cheminPos[MAXBLUPI * 2];
  Sint32 m_cheminRank[MAXBLUPI * 2];

  bool   m_bFillFloor;
  Sint32 m_fillSearchChannel;
  Sint32 m_fillSearchIcon;
  Sint32 m_fillPutChannel;
  Sint32 m_fillPutIcon;
  char * m_pFillMap;

  SDL_Surface * m_SurfaceMap;
};

// (*)  Régions :
//      0 -> normal
//      1 -> palmier
//      2 -> hiver
//      3 -> sapin

/////////////////////////////////////////////////////////////////////////////

Point GetCel (Sint32 x, Sint32 y);
Point GetCel (Point cel, Sint32 x, Sint32 y);
bool  IsValid (Point cel);
Point GetVector (Sint32 direct);

extern Sint32       table_multi_goal[];
extern const Sint16 table_actions[];
