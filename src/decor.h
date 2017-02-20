
#pragma once

#include <unordered_map>
#include "def.h"
#include "sound.h"
#include "pixmap.h"

#define MAXENERGY       4000
#define MAXFIRE         400

#define ICON_HILI_STAT  112
#define ICON_HILI_SEL   113
#define ICON_HILI_ANY   114
#define ICON_HILI_OP    115
#define ICON_HILI_GO    117
#define ICON_HILI_BUILD 118
#define ICON_HILI_ERR   119

// Descripteur d'une cellule du décor.
typedef struct
{
    Sint16  floorChannel;
    Sint16  floorIcon;
    Sint16  objectChannel;
    Sint16  objectIcon;
    Sint16  fog;            // brouillard
    Sint16  rankMove;       // rang dans m_move
    Sint16  workBlupi;      // rang du blupi travaillant ici
    Sint16  fire;
}
Cellule;
// Cette structure doit être la plus petite possible, car
// il en existe un tableau de 100x100 = 10'000 cellules !

// Descripteur d'un blupi animé.
#define MAXBLUPI    100
#define MAXUSED     50
#define MAXLIST     10

typedef struct
{
    Sint32      bExist;         // true -> utilisé
    Sint32      bHili;          // true -> sélectionné

    Sint16  perso;          // personnage, voir (*)

    Sint16  goalAction;     // action (Sint32 terme)
    Sint16  goalPhase;      // phase (Sint32 terme)
    POINT   goalCel;        // cellule visée (Sint32 terme)
    POINT   passCel;        // cellule tranversante

    Sint16  energy;         // énergie restante

    POINT   cel;            // cellule actuelle
    POINT   destCel;        // cellule destination
    Sint16  action;         // action en cours
    Sint16  aDirect;        // direction actuelle
    Sint16  sDirect;        // direction souhaitée

    POINT   pos;            // position relative à partir de la cellule
    Sint16  posZ;           // déplacement z
    Sint16  channel;
    Sint16  lastIcon;
    Sint16  icon;
    Sint16  phase;          // phase dans l'action
    Sint16  step;           // pas global
    Sint16  interrupt;      // 0=prioritaire, 1=normal, 2=misc
    Sint16  clipLeft;

    Sint32      nbUsed;         // nb de points déjà visités
    char    nextRankUsed;
    POINT   posUsed[MAXUSED];
    char    rankUsed[MAXUSED];

    Sint16  takeChannel;    // objet transporté
    Sint16  takeIcon;

    POINT   fix;            // point fixe (cultive, pont)

    Sint16  jaugePhase;
    Sint16  jaugeMax;
    Sint16  stop;           // 1 -> devra stopper
    Sint16  bArrow;         // true -> flèche en dessus de blupi
    Sint16  bRepeat;        // true -> répète l'action
    Sint16  nLoop;          // nb de boucles pour GOAL_OTHERLOOP
    Sint16  cLoop;          // boucle en cours
    Sint16  vIcon;          // icône variable
    POINT   goalHili;       // but visé
    Sint16  bMalade;        // true -> blupi malade
    Sint16  bCache;         // true -> caché (pas dessiné)
    Sint16  vehicule;       // véhicule utilisé par blupi, voir (**)
    char    busyCount;
    char    busyDelay;
    char    clicCount;
    char    clicDelay;
    char    reserve2[2];
    Sint16  listButton[MAXLIST];
    POINT   listCel[MAXLIST];
    Sint16  listParam[MAXLIST];
    Sint16  repeatLevelHope;
    Sint16  repeatLevel;
    Sint16  reserve3[88];
}
Blupi;

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
#define MAXMOVE     100
#define MOVEICONNB  1000

typedef struct
{
    Sint32      bExist;         // true -> utilisé

    POINT   cel;            // cellule du décor
    Sint16  rankBlupi;      // blupi travaillant ici

    Sint32      bFloor;         // true -> floor, false -> object
    Sint16  channel;
    Sint16  icon;
    Sint16  maskChannel;
    Sint16  maskIcon;
    Sint16  phase;          // phase pour pMoves ou pIcon
    Sint16  rankMoves;      // *nb,dx,dy,...
    Sint16  rankIcons;      // *nb,i,i,...

    Sint16  total;          // nb total d'étapes
    Sint16  delai;          // délai entre deux pas
    Sint16  stepY;          // pas vertical *100

    Sint16  cTotal;
    Sint16  cDelai;
}
Move;


#define MAXLASTDRAPEAU  50

class CDecor
{
public:
    CDecor();
    ~CDecor();

    // Arrange.cpp
    void    ArrangeFloor (POINT cel);
    void    ArrangeMur (POINT cel, Sint32 &icon, Sint32 index);
    void    ArrangeBuild (POINT cel, Sint32 &channel, Sint32 &icon);
    void    ArrangeObject (POINT cel);

    bool    ArrangeFillTestFloor (POINT cel1, POINT cel2);
    bool    ArrangeFillTest (POINT pos);
    void    ArrangeFillPut (POINT pos, Sint32 channel, Sint32 icon);
    void    ArrangeFillSearch (POINT pos);
    void    ArrangeFill (POINT pos, Sint32 channel, Sint32 icon, bool bFloor);

    void    ArrangeBlupi();

    // Obstacle.cpp
    void    SearchFloor (Sint32 rank, Sint32 icon, POINT cel, Sint32 *pBits);
    void    SearchObject (Sint32 rank, Sint32 icon, POINT cel, Sint32 *pBits);
    void    AjustFloor (Sint32 rank, Sint32 icon, POINT cel, Sint32 *pBits);
    void    AjustObject (Sint32 rank, Sint32 icon, POINT cel, Sint32 *pBits);
    bool    IsFreeDirect (POINT cel, Sint32 direct, Sint32 rank);
    bool    IsFreeCelObstacle (POINT cel);
    bool    IsFreeCelFloor (POINT cel, Sint32 rank);
    bool    IsFreeCelGo (POINT cel, Sint32 rank);
    bool    IsFreeCelHili (POINT cel, Sint32 rank);
    bool    IsFreeCel (POINT cel, Sint32 rank);
    bool    IsFreeCelDepose (POINT cel, Sint32 rank);
    bool    IsFreeCelEmbarque (POINT cel, Sint32 rank, Sint32 &action,
                               POINT &limit);
    bool    IsFreeCelDebarque (POINT cel, Sint32 rank, Sint32 &action,
                               POINT &limit);
    bool    IsFreeJump (POINT cel, Sint32 direct, Sint32 rank, Sint32 &action);
    bool    IsFreeGlisse (POINT cel, Sint32 direct, Sint32 rank, Sint32 &action);
    Sint32      DirectSearch (POINT cel, POINT goal);
    void    FlushUsed (Sint32 rank);
    void    AddUsedPos (Sint32 rank, POINT pos);
    bool    IsUsedPos (Sint32 rank, POINT pos);
    bool    SearchBestBase (Sint32 rank, Sint32 &action, POINT &newCel,
                            Sint32 &direct);
    bool    SearchBestPass (Sint32 rank, Sint32 &action);
    bool    IsWorkableObject (POINT cel, Sint32 rank);
    bool    SearchOtherObject (Sint32 rank, POINT initCel, Sint32 action,
                               Sint32 distMax, Sint32 channel,
                               Sint32 firstIcon1, Sint32 lastIcon1,
                               Sint32 firstIcon2, Sint32 lastIcon2,
                               POINT &foundCel, Sint32 &foundIcon);
    bool    SearchOtherDrapeau (Sint32 rank, POINT initCel, Sint32 distMax,
                                POINT &foundCel, Sint32 &foundIcon);
    bool    SearchOtherBateau (Sint32 rank, POINT initCel, Sint32 distMax,
                               POINT &foundCel, Sint32 &foundIcon);
    bool    IsSpiderObject (Sint32 icon);
    bool    SearchSpiderObject (Sint32 rank, POINT initCel, Sint32 distMax,
                                POINT &foundCel, Sint32 &foundIcon);
    bool    IsTracksObject (Sint32 icon);
    bool    SearchTracksObject (Sint32 rank, POINT initCel, Sint32 distMax,
                                POINT &foundCel, Sint32 &foundIcon);
    bool    IsRobotObject (Sint32 icon);
    bool    SearchRobotObject (Sint32 rank, POINT initCel, Sint32 distMax,
                               POINT &foundCel, Sint32 &foundIcon,
                               Sint32 &foundAction);
    bool    IsBombeObject (Sint32 icon);
    bool    SearchBombeObject (Sint32 rank, POINT initCel, Sint32 distMax,
                               POINT &foundCel, Sint32 &foundIcon);
    bool    SearchElectroObject (Sint32 rank, POINT initCel, Sint32 distMax,
                                 POINT &foundCel, Sint32 &foundIcon);
    bool    IsUsineBuild (Sint32 rank, POINT cel);
    bool    IsUsineFree (Sint32 rank, POINT cel);
    bool    IsFireCel (POINT cel);
    bool    IsVirusCel (POINT cel);
    Sint32      IsBuildPont (POINT &cel, Sint32 &iconBuild);
    bool    IsBuildBateau (POINT cel, Sint32 &direct);
    void    InitDrapeau();
    void    AddDrapeau (POINT cel);
    void    SubDrapeau (POINT cel);
    bool    TestDrapeau (POINT cel);

    // DecBlupi.cpp
    void    BlupiFlush();
    Sint32      BlupiCreate (POINT cel, Sint32 action, Sint32 direct,
                             Sint32 perso, Sint32 energy);
    bool    BlupiDelete (POINT cel, Sint32 perso = -1);
    void    BlupiDelete (Sint32 rank);
    void    BlupiKill (Sint32 exRank, POINT cel, Sint32 type);
    bool    BlupiIfExist (Sint32 rank);
    void    BlupiCheat (Sint32 cheat);
    void    BlupiActualise (Sint32 rank);
    void    BlupiAdaptIcon (Sint32 rank);
    void    BlupiPushFog (Sint32 rank);
    void    BlupiSound (Sint32 rank, Sint32 sound, POINT pos, bool bStop = false);
    void    BlupiInitAction (Sint32 rank, Sint32 action, Sint32 direct = -1);
    void    BlupiChangeAction (Sint32 rank, Sint32 action, Sint32 direct = -1);
    void    ListFlush (Sint32 rank);
    Sint32      ListGetParam (Sint32 rank, Sint32 button, POINT cel);
    bool    ListPut (Sint32 rank, Sint32 button, POINT cel, POINT cMem);
    void    ListRemove (Sint32 rank);
    Sint32      ListSearch (Sint32 rank, Sint32 button, POINT cel,
                            const char *&textForButton);
    bool    RepeatAdjust (Sint32 rank, Sint32 button, POINT &cel, POINT &cMem,
                          Sint32 param, Sint32 list);
    void    GoalStart (Sint32 rank, Sint32 action, POINT cel);
    bool    GoalNextPhase (Sint32 rank);
    void    SetTotalTime (Sint32 total);
    Sint32      GetTotalTime();
    void    GoalInitJauge (Sint32 rank);
    void    GoalInitPassCel (Sint32 rank);
    void    GoalAdjustCel (Sint32 rank, Sint32 &x, Sint32 &y);
    bool    GoalNextOp (Sint32 rank, Sint16 *pTable);
    void    GoalUnwork (Sint32 rank);
    void    GoalStop (Sint32 rank, bool bError = false, bool bSound = true);
    bool    BlupiIsGoalUsed (POINT cel);
    void    BlupiStartStopRayon (Sint32 rank, POINT startCel, POINT endCel);
    bool    BlupiRotate (Sint32 rank);
    bool    BlupiNextAction (Sint32 rank);
    void    BlupiNextGoal (Sint32 rank);
    void    BlupiStep (bool bFirst);
    void    BlupiGetRect (Sint32 rank, RECT &rect);
    Sint32      GetTargetBlupi (POINT pos);
    void    BlupiDeselect();
    void    BlupiDeselect (Sint32 rank);
    void    BlupiSetArrow (Sint32 rank, bool bArrow);
    void    InitOutlineRect();
    void    BlupiHiliDown (POINT pos, bool bAdd = false);
    void    BlupiHiliMove (POINT pos);
    void    BlupiHiliUp (POINT pos);
    void    BlupiDrawHili();
    Sint32      GetDefButton (POINT cel);
    bool    BlupiGoal (Sint32 rank, Sint32 button, POINT cel, POINT cMem);
    void    BlupiGoal (POINT cel, Sint32 button);
    void    BlupiDestCel (Sint32 rank);
    bool    IsTracksHere (POINT cel, bool bSkipInMove);
    bool    IsBlupiHereEx (POINT cel1, POINT cel2, Sint32 exRank, bool bSkipInMove);
    bool    IsBlupiHereEx (POINT cel, Sint32 exRank, bool bSkipInMove);
    bool    IsBlupiHere (POINT cel, bool bSkipInMove);
    bool    IsBlupiHere (POINT cel, Sint32 direct, bool bSkipInMove);
    void    GetLevelJauge (Sint32 *pLevels, Sint32 *pTypes);
    bool    IsWorkBlupi (Sint32 rank);
    void    BlupiGetButtons (POINT pos, Sint32 &nb, Sint32 *pButtons,
                             Sint32 *pErrors,
                             std::unordered_map<Sint32, const char *> &texts, Sint32 &perso);
    void    TerminatedInit();
    Sint32      IsTerminated();
    Term   *GetTerminated();

    // DecMove.cpp
    void    MoveFlush();
    Sint32      MoveMaxFire();
    void    MoveFixInit();
    bool    MoveCreate (POINT cel, Sint32 rankBlupi, bool bFloor,
                        Sint32 channel, Sint32 icon,
                        Sint32 maskChannel, Sint32 maskIcon,
                        Sint32 total, Sint32 delai, Sint32 stepY,
                        bool bMisc = false, bool bNotIfExist = false);
    bool    MoveAddMoves (POINT cel, Sint32 rankMoves);
    bool    MoveAddIcons (POINT cel, Sint32 rankIcons, bool bContinue = false);
    bool    MoveStartFire (POINT cel);
    void    MoveProxiFire (POINT cel);
    void    MoveFire (Sint32 rank);
    void    MoveStep (bool bFirst);
    void    MoveFinish (POINT cel);
    void    MoveFinish (Sint32 rankBlupi);
    bool    MoveIsUsed (POINT cel);
    bool    MoveGetObject (POINT cel, Sint32 &channel, Sint32 &icon);
    bool    MovePutObject (POINT cel, Sint32 channel, Sint32 icon);

    // DecIO.cpp
    bool    Write (Sint32 rank, bool bUser, Sint32 world, Sint32 time,
                   Sint32 total);
    bool    Read (Sint32 rank, bool bUser, Sint32 &world, Sint32 &time,
                  Sint32 &total);
    bool    FileExist (Sint32 rank, bool bUser, Sint32 &world, Sint32 &time,
                       Sint32 &total);
    void    Flush();

    // DecMap.cpp
    void    MapInitColors();
    POINT   ConvCelToMap (POINT cel);
    POINT   ConvMapToCel (POINT pos);
    bool    MapMove (POINT pos);
    void    MapPutCel (POINT pos);
    bool    GenerateMap();

    // DecStat.cpp
    void    StatisticInit();
    void    StatisticUpdate();
    Sint32      StatisticGetBlupi();
    Sint32      StatisticGetFire();
    void    StatisticDraw();
    void    GenerateStatictic();
    bool    StatisticDown (POINT pos);
    bool    StatisticMove (POINT pos);
    bool    StatisticUp (POINT pos);
    Sint32      StatisticDetect (POINT pos);

    // Chemin.cpp
    void    CheminMemPos (Sint32 exRank);
    bool    CheminTestPos (POINT pos, Sint32 &rank);
    Sint32      CheminARebours (Sint32 rank);
    void    CheminFillTerrain (Sint32 rank);
    bool    CheminTestDirection (Sint32 rank, Sint32 pos, Sint32 dir,
                                 Sint32 &next, Sint32 &ampli,
                                 Sint32 &cout, Sint32 &action);
    bool    CheminCherche (Sint32 rank, Sint32 &action);
    bool    IsCheminFree (Sint32 rank, POINT dest, Sint32 button);

    // Decor.cpp
    void    SetShiftOffset (POINT offset);
    POINT   ConvCelToPos (POINT cel);
    POINT   ConvPosToCel (POINT pos, bool bMap = false);
    POINT   ConvPosToCel2 (POINT pos);

    void    Create (CSound *pSound, CPixmap *pPixmap);
    void    Init (Sint32 channel, Sint32 icon);
    void    InitAfterBuild();
    void    ResetHili();
    bool    LoadImages();
    void    ClearFog();
    void    ClearFire();
    void    SetBuild (bool bBuild);
    void    EnableFog (bool bEnable);
    bool    GetInvincible();
    void    SetInvincible (bool bInvincible);
    bool    GetSuper();
    void    SetSuper (bool bSuper);
    void    FlipOutline();
    bool    PutFloor (POINT cel, Sint32 channel, Sint32 icon);
    bool    PutObject (POINT cel, Sint32 channel, Sint32 icon);
    bool    GetFloor (POINT cel, Sint32 &channel, Sint32 &icon);
    bool    GetObject (POINT cel, Sint32 &channel, Sint32 &icon);
    bool    SetFire (POINT cel, bool bFire);

    void    SetCoin (POINT coin, bool bCenter = false);
    POINT   GetCoin();
    POINT   GetHome();
    void    MemoPos (Sint32 rank, bool bRecord);

    void    SetTime (Sint32 time);
    Sint32      GetTime();

    void    SetMusic (Sint32 music);
    Sint32      GetMusic();

    void    SetSkill (Sint32 skill);
    Sint32      GetSkill();

    void    SetRegion (Sint32 region);
    Sint32      GetRegion();

    void    SetInfoMode (bool bInfo);
    bool    GetInfoMode();
    void    SetInfoHeight (Sint32 height);
    Sint32      GetInfoHeight();

    char   *GetButtonExist();

    void    BuildPutBlupi();
    void    BuildMoveFloor (Sint32 x, Sint32 y, POINT pos, Sint32 rank);
    void    BuildMoveObject (Sint32 x, Sint32 y, POINT pos, Sint32 rank);
    void    BuildGround (RECT clip);
    void    Build (RECT clip, POINT posMouse);
    void    NextPhase (Sint32 mode);

    Sint32      CountFloor (Sint32 channel, Sint32 icon);
    Sint32      CelOkForAction (POINT cel, Sint32 action, Sint32 rank,
                                Sint32   icons[4][4],
                                POINT &celOutline1,
                                POINT &celOutline2);
    Sint32      CelOkForAction (POINT cel, Sint32 action, Sint32 rank);
    Sint32      GetHiliRankBlupi (Sint32 nb);
    void    CelHili (POINT pos, Sint32 action);
    void    CelHiliButton (POINT cel, Sint32 button);
    void    CelHiliRepeat (Sint32 list);
    const char *GetResHili (POINT posMouse);
    void    HideTooltips (bool bHide);

    void    UndoOpen();
    void    UndoClose();
    void    UndoCopy();
    void    UndoBack();
    bool    IsUndo();


protected:
    bool    GetSeeBits (POINT cel, char *pBits, Sint32 index);
    Sint32      GetSeeIcon (char *pBits, Sint32 index);

protected:
    CSound     *m_pSound;
    CPixmap    *m_pPixmap;
    Cellule    *m_pUndoDecor;
    Cellule     m_decor[MAXCELX / 2][MAXCELY / 2];
    Sint16      m_rankBlupi[MAXCELX][MAXCELY];
    Blupi       m_blupi[MAXBLUPI];
    Move        m_move[MAXMOVE];
    POINT       m_celCoin;          // cellule sup/gauche
    POINT       m_celHome;          // pour touche Home
    POINT       m_celHili;
    POINT       m_celOutline1;
    POINT       m_celOutline2;
    POINT       m_shiftOffset;
    Sint32          m_iconHili[4][4];
    Sint32          m_rankHili;         // rang du blupi visé
    bool        m_bHiliRect;
    POINT       m_p1Hili;           // coins rectangle de sélection
    POINT       m_p2Hili;
    Sint32          m_shiftHili;
    Sint32          m_nbBlupiHili;      // nb de blupi sélectionnés
    Sint32          m_rankBlupiHili;    // rang blupi sélectionné
    bool        m_bFog;             // true -> brouillard (jeu)
    bool        m_bBuild;           // true -> construction
    bool        m_bInvincible;      // true -> cheat code
    bool        m_bSuper;           // true -> cheat code
    Uint32      m_colors[100];
    Sint32          m_time;             // temps relatif global
    Sint32          m_timeConst;        // temps relatif global constant
    Sint32          m_timeFlipOutline;  // temps quand basculer mode outline
    Sint32          m_totalTime;        // temps total passé sur une partie
    Sint32          m_phase;            // phase pour la carte
    POINT       m_celArrow;         // cellule avec flèche
    bool        m_bOutline;
    bool        m_bGroundRedraw;
    char        m_buttonExist[MAXBUTTON];
    Sint32          m_statNb;           // nb de statistiques
    Sint32          m_statFirst;        // première statistique visible
    Sint32          m_bStatUp;          // flèche up statistique
    Sint32          m_bStatDown;        // flèche down statistique
    Sint32          m_statHili;         // statistique survolée
    bool        m_bStatRecalc;      // true -> recalcule les statistiques
    Sint32          m_nbStatHach;       // nb de hachures
    Sint32          m_nbStatHachBlupi;  // hachures occupées par blupi
    Sint32          m_nbStatHachPlanche;// hachures occupées par planches
    Sint32          m_nbStatHachTomate; // hachures occupées par tomates
    Sint32          m_nbStatHachMetal;  // hachures occupées par métal
    Sint32          m_nbStatHachRobot;  // hachures occupées par robot
    Sint32          m_nbStatHome;       // nb de maisons
    Sint32          m_nbStatHomeBlupi;  // maisons occupées par blupi
    Sint32          m_nbStatRobots;     // nb d'ennemis
    Term        m_term;             // conditions pour gagner
    Sint32          m_winCount;         // compteur avant gagné
    Sint32          m_winLastHachBlupi; // dernier nombre atteint
    Sint32          m_winLastHachPlanche;// dernier nombre atteint
    Sint32          m_winLastHachTomate;// dernier nombre atteint
    Sint32          m_winLastHachMetal; // dernier nombre atteint
    Sint32          m_winLastHachRobot; // dernier nombre atteint
    Sint32          m_winLastHome;      // dernier nombre atteint
    Sint32          m_winLastHomeBlupi; // dernier nombre atteint
    Sint32          m_winLastRobots;    // dernier nombre atteint
    Sint32          m_music;            // numéro musique
    Sint32          m_region;           // numéro région (*)
    Sint32          m_lastRegion;       // numéro dernière région
    Sint32          m_blupiHere;
    POINT       m_lastDrapeau[MAXLASTDRAPEAU];
    bool        m_bHideTooltips;    // true -> menu présent
    char        m_text[50];
    POINT       m_textLastPos;
    Sint32          m_textCount;
    Sint32          m_skill;
    bool        m_bInfo;
    Sint32          m_infoHeight;
    POINT       m_memoPos[4];

    Uint8       m_cheminWork[MAXCELX * MAXCELY];
    Sint32          m_cheminNbPos;
    POINT       m_cheminPos[MAXBLUPI * 2];
    Sint32          m_cheminRank[MAXBLUPI * 2];

    bool        m_bFillFloor;
    Sint32          m_fillSearchChannel;
    Sint32          m_fillSearchIcon;
    Sint32          m_fillPutChannel;
    Sint32          m_fillPutIcon;
    char       *m_pFillMap;

    SDL_Surface *m_SurfaceMap;
};

// (*)  Régions :
//      0 -> normal
//      1 -> palmier
//      2 -> hiver
//      3 -> sapin

/////////////////////////////////////////////////////////////////////////////

POINT GetCel (Sint32 x, Sint32 y);
POINT GetCel (POINT cel, Sint32 x, Sint32 y);
bool IsValid (POINT cel);
POINT GetVector (Sint32 direct);
extern Sint32 table_multi_goal[];
extern const Sint16 table_actions[];
