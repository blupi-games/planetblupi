// Decor.h

#pragma once

#include "DEF.H"
#include "SOUND.H"
#include "PIXMAP.H"

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
	BOOL	bExist;			// TRUE -> utilisé
	BOOL	bHili;			// TRUE -> sélectionné

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
	short	bArrow;			// TRUE -> flèche en dessus de blupi
	short	bRepeat;		// TRUE -> répète l'action
	short	nLoop;			// nb de boucles pour GOAL_OTHERLOOP
	short	cLoop;			// boucle en cours
	short	vIcon;			// icône variable
	POINT	goalHili;		// but visé
	short	bMalade;		// TRUE -> blupi malade
	short	bCache;			// TRUE -> caché (pas dessiné)
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
	BOOL	bExist;			// TRUE -> utilisé

	POINT	cel;			// cellule du décor
	short	rankBlupi;		// blupi travaillant ici

	BOOL	bFloor;			// TRUE -> floor, FALSE -> object
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

	BOOL	ArrangeFillTestFloor(POINT cel1, POINT cel2);
	BOOL	ArrangeFillTest(POINT pos);
	void	ArrangeFillPut(POINT pos, int channel, int icon);
	void	ArrangeFillSearch(POINT pos);
	void	ArrangeFill(POINT pos, int channel, int icon, BOOL bFloor);

	void	ArrangeBlupi();

	// Obstacle.cpp
	void	SearchFloor(int rank, int icon, POINT cel, int *pBits);
	void	SearchObject(int rank, int icon, POINT cel, int *pBits);
	void	AjustFloor(int rank, int icon, POINT cel, int *pBits);
	void	AjustObject(int rank, int icon, POINT cel, int *pBits);
	BOOL	IsFreeDirect(POINT cel, int direct, int rank);
	BOOL	IsFreeCelObstacle(POINT cel);
	BOOL	IsFreeCelFloor(POINT cel, int rank);
	BOOL	IsFreeCelGo(POINT cel, int rank);
	BOOL	IsFreeCelHili(POINT cel, int rank);
	BOOL	IsFreeCel(POINT cel, int rank);
	BOOL	IsFreeCelDepose(POINT cel, int rank);
	BOOL	IsFreeCelEmbarque(POINT cel, int rank, int &action, POINT &limit);
	BOOL	IsFreeCelDebarque(POINT cel, int rank, int &action, POINT &limit);
	BOOL	IsFreeJump(POINT cel, int direct, int rank, int &action);
	BOOL	IsFreeGlisse(POINT cel, int direct, int rank, int &action);
	int		DirectSearch(POINT cel, POINT goal);
	void	FlushUsed(int rank);
	void	AddUsedPos(int rank, POINT pos);
	BOOL	IsUsedPos(int rank, POINT pos);
	BOOL	SearchBestBase(int rank, int &action, POINT &newCel, int &direct);
	BOOL	SearchBestPass(int rank, int &action);
	BOOL	IsWorkableObject(POINT cel, int rank);
	BOOL	SearchOtherObject(int rank, POINT initCel, int action,
							  int distMax, int channel,
							  int firstIcon1, int lastIcon1,
							  int firstIcon2, int lastIcon2,
							  POINT &foundCel, int &foundIcon);
	BOOL	SearchOtherDrapeau(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	BOOL	SearchOtherBateau(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon);
	BOOL	IsSpiderObject(int icon);
	BOOL	SearchSpiderObject(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	BOOL	IsTracksObject(int icon);
	BOOL	SearchTracksObject(int rank, POINT initCel, int distMax,
							   POINT &foundCel, int &foundIcon);
	BOOL	IsRobotObject(int icon);
	BOOL	SearchRobotObject(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon,
							  int &foundAction);
	BOOL	IsBombeObject(int icon);
	BOOL	SearchBombeObject(int rank, POINT initCel, int distMax,
							  POINT &foundCel, int &foundIcon);
	BOOL	SearchElectroObject(int rank, POINT initCel, int distMax,
								POINT &foundCel, int &foundIcon);
	BOOL	IsUsineBuild(int rank, POINT cel);
	BOOL	IsUsineFree(int rank, POINT cel);
	BOOL	IsFireCel(POINT cel);
	BOOL	IsVirusCel(POINT cel);
	int		IsBuildPont(POINT &cel, int &iconBuild);
	BOOL	IsBuildBateau(POINT cel, int &direct);
	void	InitDrapeau();
	void	AddDrapeau(POINT cel);
	void	SubDrapeau(POINT cel);
	BOOL	TestDrapeau(POINT cel);

	// DecBlupi.cpp
	void	BlupiFlush();
	int		BlupiCreate(POINT cel, int action, int direct,
						int perso, int energy);
	BOOL	BlupiDelete(POINT cel, int perso=-1);
	void	BlupiDelete(int rank);
	void	BlupiKill(int exRank, POINT cel, int type);
	BOOL	BlupiIfExist(int rank);
	void	BlupiCheat(int cheat);
	void	BlupiActualise(int rank);
	void	BlupiAdaptIcon(int rank);
	void	BlupiPushFog(int rank);
	void	BlupiSound(int rank, int sound, POINT pos, BOOL bStop=FALSE);
	void	BlupiInitAction(int rank, int action, int direct=-1);
	void	BlupiChangeAction(int rank, int action, int direct=-1);
	void	ListFlush(int rank);
	int		ListGetParam(int rank, int button, POINT cel);
	BOOL	ListPut(int rank, int button, POINT cel, POINT cMem);
	void	ListRemove(int rank);
	int		ListSearch(int rank, int button, POINT cel, int &textForButton);
	BOOL	RepeatAdjust(int rank, int button, POINT &cel, POINT &cMem,
						 int param, int list);
	void	GoalStart(int rank, int action, POINT cel);
	BOOL	GoalNextPhase(int rank);
	void	SetTotalTime(int total);
	int		GetTotalTime();
	void	GoalInitJauge(int rank);
	void	GoalInitPassCel(int rank);
	void	GoalAdjustCel(int rank, int &x, int &y);
	BOOL	GoalNextOp(int rank, short *pTable);
	void	GoalUnwork(int rank);
	void	GoalStop(int rank, BOOL bError=FALSE, BOOL bSound=TRUE);
	BOOL	BlupiIsGoalUsed(POINT cel);
	void	BlupiStartStopRayon(int rank, POINT startCel, POINT endCel);
	BOOL	BlupiRotate(int rank);
	BOOL	BlupiNextAction(int rank);
	void	BlupiNextGoal(int rank);
	void	BlupiStep(BOOL bFirst);
	void	BlupiGetRect(int rank, RECT &rect);
	int		GetTargetBlupi(POINT pos);
	void	BlupiDeselect();
	void	BlupiDeselect(int rank);
	void	BlupiSetArrow(int rank, BOOL bArrow);
	void	InitOutlineRect();
	void	BlupiHiliDown(POINT pos, BOOL bAdd=FALSE);
	void	BlupiHiliMove(POINT pos, BOOL bAdd=FALSE);
	void	BlupiHiliUp(POINT pos, BOOL bAdd=FALSE);
	void	BlupiDrawHili();
	int		GetDefButton(POINT cel);
	BOOL	BlupiGoal(int rank, int button, POINT cel, POINT cMem);
	void	BlupiGoal(POINT cel, int button);
	void	BlupiDestCel(int rank);
	BOOL	IsTracksHere(POINT cel, BOOL bSkipInMove);
	BOOL	IsBlupiHereEx(POINT cel1, POINT cel2, int exRank, BOOL bSkipInMove);
	BOOL	IsBlupiHereEx(POINT cel, int exRank, BOOL bSkipInMove);
	BOOL	IsBlupiHere(POINT cel, BOOL bSkipInMove);
	BOOL	IsBlupiHere(POINT cel, int direct, BOOL bSkipInMove);
	void	GetLevelJauge(int *pLevels, int *pTypes);
	BOOL	IsWorkBlupi(int rank);
	void	BlupiGetButtons(POINT pos, int &nb, int *pButtons, int *pErrors, int &perso);
	void	TerminatedInit();
	int		IsTerminated();
	Term*	GetTerminated();

	// DecMove.cpp
	void	MoveFlush();
	int		MoveMaxFire();
	void	MoveFixInit();
	BOOL	MoveCreate(POINT cel, int rankBlupi, BOOL bFloor,
					   int channel, int icon,
					   int maskChannel, int maskIcon,
					   int total, int delai, int stepY,
					   BOOL bMisc=FALSE, BOOL bNotIfExist=FALSE);
	BOOL	MoveAddMoves(POINT cel, int rankMoves);
	BOOL	MoveAddIcons(POINT cel, int rankIcons, BOOL bContinue=FALSE);
	BOOL	MoveStartFire(POINT cel);
	void	MoveProxiFire(POINT cel);
	void	MoveFire(int rank);
	void	MoveStep(BOOL bFirst);
	void	MoveFinish(POINT cel);
	void	MoveFinish(int rankBlupi);
	BOOL	MoveIsUsed(POINT cel);
	BOOL	MoveGetObject(POINT cel, int &channel, int &icon);
	BOOL	MovePutObject(POINT cel, int channel, int icon);

	// DecIO.cpp
	BOOL	Write(int rank, BOOL bUser, int world, int time, int total);
	BOOL	Read(int rank, BOOL bUser, int &world, int &time, int &total);
	BOOL	FileExist(int rank, BOOL bUser, int &world, int &time, int &total);
	void	Flush();

	// DecMap.cpp
	void	MapInitColors();
	POINT	ConvCelToMap(POINT cel);
	POINT	ConvMapToCel(POINT pos);
	BOOL	MapMove(POINT pos);
	void	MapPutCel(POINT pos);
	BOOL	GenerateMap();

	// DecStat.cpp
	void	StatisticInit();
	void	StatisticUpdate();
	int		StatisticGetBlupi();
	int		StatisticGetFire();
	void	StatisticDraw();
	void	GenerateStatictic();
	BOOL	StatisticDown(POINT pos, int fwKeys);
	BOOL	StatisticMove(POINT pos, int fwKeys);
	BOOL	StatisticUp(POINT pos, int fwKeys);
	int		StatisticDetect(POINT pos);

	// Chemin.cpp
	void	CheminMemPos(int exRank);
	BOOL	CheminTestPos(POINT pos, int &rank);
	int		CheminARebours(int rank);
	void	CheminFillTerrain(int rank);
	BOOL	CheminTestDirection(int rank, int pos, int dir,
								int &next, int &ampli,
								int &cout, int &action);
	BOOL	CheminCherche(int rank, int &action);
	BOOL	IsCheminFree(int rank, POINT dest, int button);

	// Decor.cpp
	void	SetShiftOffset(POINT offset);
	POINT	ConvCelToPos(POINT cel);
	POINT	ConvPosToCel(POINT pos, BOOL bMap=FALSE);
	POINT	ConvPosToCel2(POINT pos);

	void	Create(HWND hWnd, CSound *pSound, CPixmap *pPixmap);
	void	Init(int channel, int icon);
	void	InitAfterBuild();
	void	ResetHili();
	BOOL	LoadImages();
	void	ClearFog();
	void	ClearFire();
	void	SetBuild(BOOL bBuild);
	void	EnableFog(BOOL bEnable);
	BOOL	GetInvincible();
	void	SetInvincible(BOOL bInvincible);
	BOOL	GetSuper();
	void	SetSuper(BOOL bSuper);
	void	FlipOutline();
	BOOL	PutFloor(POINT cel, int channel, int icon);
	BOOL	PutObject(POINT cel, int channel, int icon);
	BOOL	GetFloor(POINT cel, int &channel, int &icon);
	BOOL	GetObject(POINT cel, int &channel, int &icon);
	BOOL	SetFire(POINT cel, BOOL bFire);

	void	SetCoin(POINT coin, BOOL bCenter=FALSE);
	POINT	GetCoin();
	POINT	GetHome();
	void	MemoPos(int rank, BOOL bRecord);

	void	SetTime(int time);
	int		GetTime();

	void	SetMusic(int music);
	int		GetMusic();

	void	SetSkill(int skill);
	int		GetSkill();

	void	SetRegion(int region);
	int		GetRegion();

	void	SetInfoMode(BOOL bInfo);
	BOOL	GetInfoMode();
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
	int		GetResHili(POINT posMouse);
	void	HideTooltips(BOOL bHide);

	void	UndoOpen();
	void	UndoClose();
	void	UndoCopy();
	void	UndoBack();
	BOOL	IsUndo();

	
protected:
	BOOL	GetSeeBits(POINT cel, char *pBits, int index);
	int		GetSeeIcon(char *pBits, int index);

protected:
	HWND		m_hWnd;
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
	BOOL		m_bHiliRect;
	POINT		m_p1Hili;			// coins rectangle de sélection
	POINT		m_p2Hili;
	int			m_shiftHili;
	int			m_nbBlupiHili;		// nb de blupi sélectionnés
	int			m_rankBlupiHili;	// rang blupi sélectionné
	BOOL		m_bFog;				// TRUE -> brouillard (jeu)
	BOOL		m_bBuild;			// TRUE -> construction
	BOOL		m_bInvincible;		// TRUE -> cheat code
	BOOL		m_bSuper;			// TRUE -> cheat code
	short		m_colors[100];
	int			m_time;				// temps relatif global
	int			m_timeConst;		// temps relatif global constant
	int			m_timeFlipOutline;	// temps quand basculer mode outline
	int			m_totalTime;		// temps total passé sur une partie
	int			m_phase;			// phase pour la carte
	POINT		m_celArrow;			// cellule avec flèche
	BOOL		m_bOutline;
	BOOL		m_bGroundRedraw;
	char		m_buttonExist[MAXBUTTON];
	int			m_statNb;			// nb de statistiques
	int			m_statFirst;		// première statistique visible
	int			m_bStatUp;			// flèche up statistique
	int			m_bStatDown;		// flèche down statistique
	int			m_statHili;			// statistique survolée
	BOOL		m_bStatRecalc;		// TRUE -> recalcule les statistiques
	BOOL		m_bStatRedraw;		// TRUE -> redessine les statistiques
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
	BOOL		m_bHideTooltips;	// TRUE -> menu présent
	char		m_text[50];
	POINT		m_textLastPos;
	int			m_textCount;
	int			m_skill;
	BOOL		m_bInfo;
	int			m_infoHeight;
	POINT		m_memoPos[4];

	BYTE		m_cheminWork[MAXCELX*MAXCELY];
	int			m_cheminNbPos;
	POINT		m_cheminPos[MAXBLUPI*2];
	int			m_cheminRank[MAXBLUPI*2];

	BOOL		m_bFillFloor;
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
BOOL IsValid (POINT cel);
POINT GetVector (int direct);
extern int table_multi_goal[];
extern short table_actions[];