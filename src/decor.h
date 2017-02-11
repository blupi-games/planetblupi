// Decor.h

#pragma once

#include <unordered_map>
#include "def.h"
#include "sound.h"
#include "pixmap.h"

/////////////////////////////////////////////////////////////////////////////


#define MAXENERGY		4000
#define MAXFIRE			400

#define ICON_HILI_STAT	112
#define ICON_HILI_SEL	113
#define ICON_HILI_ANY	114
#define ICON_HILI_OP	115
#define ICON_HILI_GO	117
#define ICON_HILI_BUILD	118
#define ICON_HILI_ERR	119

// Descripteur d'une cellule du décor.
typedef struct
{
	short	floorChannel;
	short	floorIcon;
	short	objectChannel;
	short	objectIcon;
	short	fog;			// brouillard
	short	rankMove;		// rang dans m_move
	short	workBlupi;		// rang du blupi travaillant ici
	short	fire;
}
Cellule;
// Cette structure doit être la plus petite possible, car
// il en existe un tableau de 100x100 = 10'000 cellules !

// Descripteur d'un blupi animé.
#define MAXBLUPI	100
#define MAXUSED		50
#define MAXLIST		10

typedef struct
{
	int		bExist;			// true -> utilisé
	int		bHili;			// true -> sélectionné

	short	perso;			// personnage, voir (*)

	short	goalAction;		// action (long terme)
	short	goalPhase;		// phase (long terme)
	POINT	goalCel;		// cellule visée (long terme)
	POINT	passCel;		// cellule tranversante

	short	energy;			// énergie restante

	POINT	cel;			// cellule actuelle
	POINT	destCel;		// cellule destination
	short	action;			// action en cours
	short	aDirect;		// direction actuelle
	short	sDirect;		// direction souhaitée

	POINT	pos;			// position relative à partir de la cellule
	short	posZ;			// déplacement z
	short	channel;
	short	lastIcon;
	short	icon;
	short	phase;			// phase dans l'action
	short	step;			// pas global
	short	interrupt;		// 0=prioritaire, 1=normal, 2=misc
	short	clipLeft;

	int		nbUsed;			// nb de points déjà visités
	char	nextRankUsed;
	POINT	posUsed[MAXUSED];
	char	rankUsed[MAXUSED];

	short	takeChannel;	// objet transporté
	short	takeIcon;

	POINT	fix;			// point fixe (cultive, pont)

	short	jaugePhase;
	short	jaugeMax;
	short	stop;			// 1 -> devra stopper
	short	bArrow;			// true -> flèche en dessus de blupi
	short	bRepeat;		// true -> répète l'action
	short	nLoop;			// nb de boucles pour GOAL_OTHERLOOP
	short	cLoop;			// boucle en cours
	short	vIcon;			// icône variable
	POINT	goalHili;		// but visé
	short	bMalade;		// true -> blupi malade
	short	bCache;			// true -> caché (pas dessiné)
	short	vehicule;		// véhicule utilisé par blupi, voir (**)
	char	busyCount;
	char	busyDelay;
	char	clicCount;
	char	clicDelay;
	char	reserve2[2];
	short	listButton[MAXLIST];
	POINT	listCel[MAXLIST];
	short	listParam[MAXLIST];
	short	repeatLevelHope;
	short	repeatLevel;
	short	reserve3[88];
}
Blupi;

// (*)	Personnages :
//		0 -> blupi
//		1 -> araignée
//		2 -> virus
//		3 -> tracks
//		4 -> robot
//		5 -> bombe
//		6 -> détonnateur de mine (invisible)
//		7 -> électro
//		8 -> disciple (robot2)

// (**)	Véhicule :
//		0 -> à pied
//		1 -> en bateau
//		2 -> en jeep
//		3 -> armure


// Descripteur d'un décor animé.
#define MAXMOVE		100
#define MOVEICONNB	1000

typedef struct
{
	int		bExist;			// true -> utilisé

	POINT	cel;			// cellule du décor
	short	rankBlupi;		// blupi travaillant ici

	int		bFloor;			// true -> floor, false -> object
	short	channel;
	short	icon;
	short	maskChannel;
	short	maskIcon;
	short	phase;			// phase pour pMoves ou pIcon
	short	rankMoves;		// *nb,dx,dy,...
	short	rankIcons;		// *nb,i,i,...

	short	total;			// nb total d'étapes
	short	delai;			// délai entre deux pas
	short	stepY;			// pas vertical *100

	short	cTotal;
	short	cDelai;
}
Move;


#define MAXLASTDRAPEAU	50

class CDecor
{
public:
	CDecor();
	~CDecor();

	// Arrange.cpp
	void	ArrangeFloor(POINT cel);
	void	ArrangeMur(POINT cel, int &icon, int index);
	void	ArrangeBuild(POINT cel, int &channel, int &icon);
	void	ArrangeObject(POINT cel);

	bool	ArrangeFillTestFloor(POINT cel1, POINT cel2);
	bool	ArrangeFillTest(POINT pos);
	void	ArrangeFillPut(POINT pos, int channel, int icon);
	void	ArrangeFillSearch(POINT pos);
	void	ArrangeFill(POINT pos, int channel, int icon, bool bFloor);

	void	ArrangeBlupi();

	// Obstacle.cpp
	void	SearchFloor(int rank, int icon, POINT cel, int *pBits);
	void	SearchObject(int rank, int icon, POINT cel, int *pBits);
	void	AjustFloor(int rank, int icon, POINT cel, int *pBits);
	void	AjustObject(int rank, int icon, POINT cel, int *pBits);
	bool	IsFreeDirect(POINT cel, int direct, int rank);
	bool	IsFreeCelObstacle(POINT cel);
	bool	IsFreeCelFloor(POINT cel, int rank);
	bool	IsFreeCelGo(POINT cel, int rank);
	bool	IsFreeCelHili(POINT cel, int rank);
	bool	IsFreeCel(POINT cel, int rank);
	bool	IsFreeCelDepose(POINT cel, int rank);
	bool	IsFreeCelEmbarque(POINT cel, int rank, int &action, POINT &limit);
	bool	IsFreeCelDebarque(POINT cel, int rank, int &action, POINT &limit);
	bool	IsFreeJump(POINT cel, int direct, int rank, int &action);
	bool	IsFreeGlisse(POINT cel, int direct, int rank, int &action);
	int		DirectSearch(POINT cel, POINT goal);
	void	FlushUsed(int rank);
	void	AddUsedPos(int rank, POINT pos);
	bool	IsUsedPos(int rank, POINT pos);
	bool	SearchBestBase(int rank, int &action, POINT &newCel, int &direct);
	bool	SearchBestPass(int rank, int &action);
	bool	IsWorkableObject(POINT cel, int rank);
	bool	SearchOtherObject(int rank, POINT initCel, int action,
							  int distMax, int channel,
							  int firstIcon1, int lastIcon1,
							  int firstIcon2, int lastIcon2,
							  POINT &foundCel, int &foundIcon);
	bool	SearchOtherDrapeau(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	bool	SearchOtherBateau(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon);
	bool	IsSpiderObject(int icon);
	bool	SearchSpiderObject(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	bool	IsTracksObject(int icon);
	bool	SearchTracksObject(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	bool	IsRobotObject(int icon);
	bool	SearchRobotObject(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon,
							  int &foundAction);
	bool	IsBombeObject(int icon);
	bool	SearchBombeObject(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon);
	bool	SearchElectroObject(int rank, POINT initCel, int distMax,
								POINT &foundCel, int &foundIcon);
	bool	IsUsineBuild(int rank, POINT cel);
	bool	IsUsineFree(int rank, POINT cel);
	bool	IsFireCel(POINT cel);
	bool	IsVirusCel(POINT cel);
	int		IsBuildPont(POINT &cel, int &iconBuild);
	bool	IsBuildBateau(POINT cel, int &direct);
	void	InitDrapeau();
	void	AddDrapeau(POINT cel);
	void	SubDrapeau(POINT cel);
	bool	TestDrapeau(POINT cel);

	// DecBlupi.cpp
	void	BlupiFlush();
	int		BlupiCreate(POINT cel, int action, int direct,
						int perso, int energy);
	bool	BlupiDelete(POINT cel, int perso=-1);
	void	BlupiDelete(int rank);
	void	BlupiKill(int exRank, POINT cel, int type);
	bool	BlupiIfExist(int rank);
	void	BlupiCheat(int cheat);
	void	BlupiActualise(int rank);
	void	BlupiAdaptIcon(int rank);
	void	BlupiPushFog(int rank);
	void	BlupiSound(int rank, int sound, POINT pos, bool bStop=false);
	void	BlupiInitAction(int rank, int action, int direct=-1);
	void	BlupiChangeAction(int rank, int action, int direct=-1);
	void	ListFlush(int rank);
	int		ListGetParam(int rank, int button, POINT cel);
	bool	ListPut(int rank, int button, POINT cel, POINT cMem);
	void	ListRemove(int rank);
	int		ListSearch(int rank, int button, POINT cel, const char *&textForButton);
	bool	RepeatAdjust(int rank, int button, POINT &cel, POINT &cMem,
						 int param, int list);
	void	GoalStart(int rank, int action, POINT cel);
	bool	GoalNextPhase(int rank);
	void	SetTotalTime(int total);
	int		GetTotalTime();
	void	GoalInitJauge(int rank);
	void	GoalInitPassCel(int rank);
	void	GoalAdjustCel(int rank, int &x, int &y);
	bool	GoalNextOp(int rank, short *pTable);
	void	GoalUnwork(int rank);
	void	GoalStop(int rank, bool bError=false, bool bSound=true);
	bool	BlupiIsGoalUsed(POINT cel);
	void	BlupiStartStopRayon(int rank, POINT startCel, POINT endCel);
	bool	BlupiRotate(int rank);
	bool	BlupiNextAction(int rank);
	void	BlupiNextGoal(int rank);
	void	BlupiStep(bool bFirst);
	void	BlupiGetRect(int rank, RECT &rect);
	int		GetTargetBlupi(POINT pos);
	void	BlupiDeselect();
	void	BlupiDeselect(int rank);
	void	BlupiSetArrow(int rank, bool bArrow);
	void	InitOutlineRect();
	void	BlupiHiliDown(POINT pos, bool bAdd=false);
	void	BlupiHiliMove(POINT pos, bool bAdd=false);
	void	BlupiHiliUp(POINT pos, bool bAdd=false);
	void	BlupiDrawHili();
	int		GetDefButton(POINT cel);
	bool	BlupiGoal(int rank, int button, POINT cel, POINT cMem);
	void	BlupiGoal(POINT cel, int button);
	void	BlupiDestCel(int rank);
	bool	IsTracksHere(POINT cel, bool bSkipInMove);
	bool	IsBlupiHereEx(POINT cel1, POINT cel2, int exRank, bool bSkipInMove);
	bool	IsBlupiHereEx(POINT cel, int exRank, bool bSkipInMove);
	bool	IsBlupiHere(POINT cel, bool bSkipInMove);
	bool	IsBlupiHere(POINT cel, int direct, bool bSkipInMove);
	void	GetLevelJauge(int *pLevels, int *pTypes);
	bool	IsWorkBlupi(int rank);
	void	BlupiGetButtons(POINT pos, int &nb, int *pButtons, int *pErrors,
							std::unordered_map<int, const char *> &texts, int &perso);
	void	TerminatedInit();
	int		IsTerminated();
	Term*	GetTerminated();

	// DecMove.cpp
	void	MoveFlush();
	int		MoveMaxFire();
	void	MoveFixInit();
	bool	MoveCreate(POINT cel, int rankBlupi, bool bFloor,
					   int channel, int icon,
					   int maskChannel, int maskIcon,
					   int total, int delai, int stepY,
					   bool bMisc=false, bool bNotIfExist=false);
	bool	MoveAddMoves(POINT cel, int rankMoves);
	bool	MoveAddIcons(POINT cel, int rankIcons, bool bContinue=false);
	bool	MoveStartFire(POINT cel);
	void	MoveProxiFire(POINT cel);
	void	MoveFire(int rank);
	void	MoveStep(bool bFirst);
	void	MoveFinish(POINT cel);
	void	MoveFinish(int rankBlupi);
	bool	MoveIsUsed(POINT cel);
	bool	MoveGetObject(POINT cel, int &channel, int &icon);
	bool	MovePutObject(POINT cel, int channel, int icon);

	// DecIO.cpp
	bool	Write(int rank, bool bUser, int world, int time, int total);
	bool	Read(int rank, bool bUser, int &world, int &time, int &total);
	bool	FileExist(int rank, bool bUser, int &world, int &time, int &total);
	void	Flush();

	// DecMap.cpp
	void	MapInitColors();
	POINT	ConvCelToMap(POINT cel);
	POINT	ConvMapToCel(POINT pos);
	bool	MapMove(POINT pos);
	void	MapPutCel(POINT pos);
	bool	GenerateMap();

	// DecStat.cpp
	void	StatisticInit();
	void	StatisticUpdate();
	int		StatisticGetBlupi();
	int		StatisticGetFire();
	void	StatisticDraw();
	void	GenerateStatictic();
	bool	StatisticDown(POINT pos);
	bool	StatisticMove(POINT pos);
	bool	StatisticUp(POINT pos);
	int		StatisticDetect(POINT pos);

	// Chemin.cpp
	void	CheminMemPos(int exRank);
	bool	CheminTestPos(POINT pos, int &rank);
	int		CheminARebours(int rank);
	void	CheminFillTerrain(int rank);
	bool	CheminTestDirection(int rank, int pos, int dir,
								int &next, int &ampli,
								int &cout, int &action);
	bool	CheminCherche(int rank, int &action);
	bool	IsCheminFree(int rank, POINT dest, int button);

	// Decor.cpp
	void	SetShiftOffset(POINT offset);
	POINT	ConvCelToPos(POINT cel);
	POINT	ConvPosToCel(POINT pos, bool bMap=false);
	POINT	ConvPosToCel2(POINT pos);

	void	Create(CSound *pSound, CPixmap *pPixmap);
	void	Init(int channel, int icon);
	void	InitAfterBuild();
	void	ResetHili();
	bool	LoadImages();
	void	ClearFog();
	void	ClearFire();
	void	SetBuild(bool bBuild);
	void	EnableFog(bool bEnable);
	bool	GetInvincible();
	void	SetInvincible(bool bInvincible);
	bool	GetSuper();
	void	SetSuper(bool bSuper);
	void	FlipOutline();
	bool	PutFloor(POINT cel, int channel, int icon);
	bool	PutObject(POINT cel, int channel, int icon);
	bool	GetFloor(POINT cel, int &channel, int &icon);
	bool	GetObject(POINT cel, int &channel, int &icon);
	bool	SetFire(POINT cel, bool bFire);

	void	SetCoin(POINT coin, bool bCenter=false);
	POINT	GetCoin();
	POINT	GetHome();
	void	MemoPos(int rank, bool bRecord);

	void	SetTime(int time);
	int		GetTime();

	void	SetMusic(int music);
	int		GetMusic();

	void	SetSkill(int skill);
	int		GetSkill();

	void	SetRegion(int region);
	int		GetRegion();

	void	SetInfoMode(bool bInfo);
	bool	GetInfoMode();
	void	SetInfoHeight(int height);
	int		GetInfoHeight();

	char*	GetButtonExist();

	void	BuildPutBlupi();
	void	BuildMoveFloor(int x, int y, POINT pos, int rank);
	void	BuildMoveObject(int x, int y, POINT pos, int rank);
	void	BuildGround(RECT clip);
	void	Build(RECT clip, POINT posMouse);
	void	NextPhase(int mode);

	int		CountFloor(int channel, int icon);
	int		CelOkForAction(POINT cel, int action, int rank,
						   int   icons[4][4],
						   POINT &celOutline1,
						   POINT &celOutline2);
	int		CelOkForAction(POINT cel, int action, int rank);
	int		GetHiliRankBlupi(int nb);
	void	CelHili(POINT pos, int action);
	void	CelHiliButton(POINT cel, int button);
	void	CelHiliRepeat(int list);
	const char *GetResHili(POINT posMouse);
	void	HideTooltips(bool bHide);

	void	UndoOpen();
	void	UndoClose();
	void	UndoCopy();
	void	UndoBack();
	bool	IsUndo();

	
protected:
	bool	GetSeeBits(POINT cel, char *pBits, int index);
	int		GetSeeIcon(char *pBits, int index);

protected:
	CSound*		m_pSound;
	CPixmap*	m_pPixmap;
	Cellule*	m_pUndoDecor;
	Cellule		m_decor[MAXCELX/2][MAXCELY/2];
	short		m_rankBlupi[MAXCELX][MAXCELY];
	Blupi		m_blupi[MAXBLUPI];
	Move		m_move[MAXMOVE];
	POINT		m_celCoin;			// cellule sup/gauche
	POINT		m_celHome;			// pour touche Home
	POINT		m_celHili;
	POINT		m_celOutline1;
	POINT		m_celOutline2;
	POINT		m_shiftOffset;
	int			m_iconHili[4][4];
	int			m_rankHili;			// rang du blupi visé
	bool		m_bHiliRect;
	POINT		m_p1Hili;			// coins rectangle de sélection
	POINT		m_p2Hili;
	int			m_shiftHili;
	int			m_nbBlupiHili;		// nb de blupi sélectionnés
	int			m_rankBlupiHili;	// rang blupi sélectionné
	bool		m_bFog;				// true -> brouillard (jeu)
	bool		m_bBuild;			// true -> construction
	bool		m_bInvincible;		// true -> cheat code
	bool		m_bSuper;			// true -> cheat code
	Uint32		m_colors[100];
	int			m_time;				// temps relatif global
	int			m_timeConst;		// temps relatif global constant
	int			m_timeFlipOutline;	// temps quand basculer mode outline
	int			m_totalTime;		// temps total passé sur une partie
	int			m_phase;			// phase pour la carte
	POINT		m_celArrow;			// cellule avec flèche
	bool		m_bOutline;
	bool		m_bGroundRedraw;
	char		m_buttonExist[MAXBUTTON];
	int			m_statNb;			// nb de statistiques
	int			m_statFirst;		// première statistique visible
	int			m_bStatUp;			// flèche up statistique
	int			m_bStatDown;		// flèche down statistique
	int			m_statHili;			// statistique survolée
	bool		m_bStatRecalc;		// true -> recalcule les statistiques
	bool		m_bStatRedraw;		// true -> redessine les statistiques
	int			m_nbStatHach;		// nb de hachures
	int			m_nbStatHachBlupi;	// hachures occupées par blupi
	int			m_nbStatHachPlanche;// hachures occupées par planches
	int			m_nbStatHachTomate;	// hachures occupées par tomates
	int			m_nbStatHachMetal;	// hachures occupées par métal
	int			m_nbStatHachRobot;	// hachures occupées par robot
	int			m_nbStatHome;		// nb de maisons
	int			m_nbStatHomeBlupi;	// maisons occupées par blupi
	int			m_nbStatRobots;		// nb d'ennemis
	Term		m_term;				// conditions pour gagner
	int			m_winCount;			// compteur avant gagné
	int			m_winLastHachBlupi;	// dernier nombre atteint
	int			m_winLastHachPlanche;// dernier nombre atteint
	int			m_winLastHachTomate;// dernier nombre atteint
	int			m_winLastHachMetal;	// dernier nombre atteint
	int			m_winLastHachRobot;	// dernier nombre atteint
	int			m_winLastHome;		// dernier nombre atteint
	int			m_winLastHomeBlupi;	// dernier nombre atteint
	int			m_winLastRobots;	// dernier nombre atteint
	int			m_music;			// numéro musique
	int			m_region;			// numéro région (*)
	int			m_lastRegion;		// numéro dernière région
	int			m_blupiHere;
	POINT		m_lastDrapeau[MAXLASTDRAPEAU];
	bool		m_bHideTooltips;	// true -> menu présent
	char		m_text[50];
	POINT		m_textLastPos;
	int			m_textCount;
	int			m_skill;
	bool		m_bInfo;
	int			m_infoHeight;
	POINT		m_memoPos[4];

	Uint8		m_cheminWork[MAXCELX*MAXCELY];
	int			m_cheminNbPos;
	POINT		m_cheminPos[MAXBLUPI*2];
	int			m_cheminRank[MAXBLUPI*2];

	bool		m_bFillFloor;
	int			m_fillSearchChannel;
	int			m_fillSearchIcon;
	int			m_fillPutChannel;
	int			m_fillPutIcon;
	char*		m_pFillMap;
};

// (*)	Régions :
//		0 -> normal
//		1 -> palmier
//		2 -> hiver
//		3 -> sapin

/////////////////////////////////////////////////////////////////////////////

POINT GetCel (int x, int y);
POINT GetCel (POINT cel, int x, int y);
bool IsValid (POINT cel);
POINT GetVector (int direct);
extern int table_multi_goal[];
extern short table_actions[];
