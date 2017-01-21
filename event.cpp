// Event.cpp
//

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
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
#include "action.h"
#include "text.h"
#include "misc.h"


#define DEF_TIME_HELP	10000		// ~10 minutes
#define DEF_TIME_DEMO	1000		// ~1 minute
#define MAXDEMO			2000


typedef struct
{
	short			majRev;
	short			minRev;
	short			reserve1[9];
	short			exercice;		// exercice en cours (0..n)
	short			mission;		// mission en cours (0..n)
	short			speed;
	short			bMovie;
	short			maxMission;		// dernière mission effectuée (0..n)
	short			scrollSpeed;
	short			audioVolume;
	short			midiVolume;
	short			bAccessBuild;
	short			prive;
	short			skill;
	short			reserve2[93];
}
DescInfo;



// Toutes les premières lettres doivent
// être différentes !

static char cheat_code[9][20] =
{
	"VISION",		// 0
	"POWER",		// 1
	"LONESOME",		// 2
	"ALLMISSIONS",	// 3
	"QUICK",		// 4
	"HELPME",		// 5
	"INVINCIBLE",	// 6
	"SUPERBLUPI",	// 7
	"CONSTRUIRE",	// 8 (CPOTUSVJSF)
};



/////////////////////////////////////////////////////////////////////////////


static Phase table[] =
{
	{
		WM_PHASE_TESTCD,
		"image\\init.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_INTRO1,
		"image\\intro1.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_INTRO2,
		"image\\intro2.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_INIT,
		"image\\init.blp",
		FALSE,
		{
			{
				WM_PHASE_DEMO,
				0, {1,108},
				16, 424-60-42*3-18,
				{1,TX_BUTTON_DEMO},
			},
			{
				WM_PHASE_SCHOOL,
				0, {1,79},
				16, 424-60-42*2,
				{1,TX_BUTTON_APPRENDRE},
			},
			{
				WM_PHASE_MISSION,
				0, {1,80},
				16, 424-60-42*1,
				{1,TX_BUTTON_JOUER},
			},
#if !_DEMO
			{
				WM_PHASE_PRIVATE,
				0, {1,49},
				16, 424-60-42*0,
				{1,TX_BUTTON_PRIVE},
			},
#endif
			{
				WM_PHASE_BYE,
//?				WM_CLOSE,
				0, {1,36},
				16, 424,
				{1,TX_BUTTON_QUITTER},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_HISTORY0,
		"image\\history0.blp",
		TRUE,
		{
			{
				WM_PHASE_INIT,
				0, {1,50},
				42+42*0, 433,
				{1,TX_BUTTON_PREVH},
			},
			{
				WM_PHASE_H1MOVIE,
				0, {1,51},
				558-42*0, 433,
				{1,TX_BUTTON_NEXTH},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_HISTORY1,
		"image\\history1.blp",
		TRUE,
		{
			{
				WM_PHASE_HISTORY0,
				0, {1,50},
				42+42*0, 433,
				{1,TX_BUTTON_PREVH},
			},
			{
				WM_PHASE_H2MOVIE,
				0, {1,51},
				558-42*0, 433,
				{1,TX_BUTTON_NEXTH},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_INFO,
		"image\\info%.3d.blp",
		FALSE,
		{
			{
				WM_PREV,
				0, {1,50},
				558-42*2, 433,
				{1,TX_BUTTON_PREVP},
			},
			{
				WM_PHASE_PLAYMOVIE,
				0, {1,48},
				558-42*1, 433,
				{1,TX_BUTTON_PLAYP},
			},
			{
				WM_NEXT,
				0, {1,51},
				558-42*0, 433,
				{1,TX_BUTTON_NEXTP},
			},
			{
				WM_PHASE_READ,
				0, {1,52},
				42+42*4, 433,
				{1,TX_BUTTON_READP},
			},
			{
				WM_PHASE_SETUP,
				0, {1,47},
				42+42*7, 433,
				{1,TX_BUTTON_SETUP},
			},
#if !_DEMO
			{
				WM_PHASE_BUILD,
				0, {1,49},
				42+42*8, 433,
				{1,TX_BUTTON_BUILDP},
			},
			{
				WM_PHASE_SKILL1,
				0, {1,94},
				150, 230,
				{1,TX_BUTTON_SKILL},
			},
			{
				WM_PHASE_SKILL2,
				0, {1,95},
				150, 230+42,
				{1,TX_BUTTON_SKILL},
			},
#endif
			{
				WM_PHASE_INIT,
				0, {1,40},
				42+42*0, 433,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_PLAY,
		"image\\play.blp",
		FALSE,
		{
			{
				WM_PHASE_STOP,
				0, {1,40},
				10+42*0, 422,
				0,
			},
			{
				WM_PHASE_SETUPp,
				0, {1,47},
				10+42*1, 422,
				0,
			},
			{
				WM_PHASE_WRITEp,
				0, {1,53},
				10+42*2, 422,
				0,
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_STOP,
		"image\\stop%.3d.blp",
		FALSE,
		{
			{
				WM_PHASE_PLAY,
				0, {1,77},
				558-42*1, 433,
				{1,TX_BUTTON_CONTP},
			},
			{
				WM_PHASE_READ,
				0, {1,52},
				42+42*4, 433,
				{1,TX_BUTTON_READP},
			},
			{
				WM_PHASE_WRITE,
				0, {1,53},
				42+42*5, 433,
				{1,TX_BUTTON_WRITEP},
			},
			{
				WM_PHASE_SETUP,
				0, {1,47},
				42+42*7, 433,
				{1,TX_BUTTON_SETUP},
			},
			{
				WM_PHASE_INFO,
				0, {1,78},
				42+42*0, 433,
				{1,TX_BUTTON_CANCELP},
			},
#if !_DEMO
			{
				WM_PHASE_HELP,
				0, {1,86},
				42+42*9, 433,
				{1,TX_BUTTON_HELP},
			},
#endif
			{
				0
			},
		},
	},

	{
		WM_PHASE_HELP,
		"image\\help.blp",
		TRUE,
		{
			{
				WM_PHASE_PLAY,
				0, {1,77},
				558-42*1, 433,
				{1,TX_BUTTON_CONTP},
			},
			{
				WM_PHASE_READ,
				0, {1,52},
				42+42*4, 433,
				{1,TX_BUTTON_READP},
			},
			{
				WM_PHASE_WRITE,
				0, {1,53},
				42+42*5, 433,
				{1,TX_BUTTON_WRITEP},
			},
			{
				WM_PHASE_SETUP,
				0, {1,47},
				42+42*7, 433,
				{1,TX_BUTTON_SETUP},
			},
			{
				WM_PHASE_STOP,
				0, {1,50},
				42+42*0, 433,
				{1,TX_BUTTON_PREVH},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_SETUP,
		"image\\setup.blp",
		FALSE,
		{
			{
				WM_BUTTON1,
				0, {1,50},
				54, 330,
				{1,TX_BUTTON_SETUP1},
			},
			{
				WM_BUTTON2,
				0, {1,51},
				54+40, 330,
				{1,TX_BUTTON_SETUP2},
			},
			{
				WM_BUTTON3,
				0, {1,50},
				284, 330,
				{1,TX_BUTTON_SETUP3},
			},
			{
				WM_BUTTON4,
				0, {1,51},
				284+40, 330,
				{1,TX_BUTTON_SETUP4},
			},
			{
				WM_BUTTON5,
				0, {1,50},
				399, 330,
				{1,TX_BUTTON_SETUP5},
			},
			{
				WM_BUTTON6,
				0, {1,51},
				399+40, 330,
				{1,TX_BUTTON_SETUP6},
			},
#if !_EGAMES
			{
				WM_BUTTON7,
				0, {1,50},
				514, 330,
				{1,TX_BUTTON_SETUP7},
			},
			{
				WM_BUTTON8,
				0, {1,51},
				514+40, 330,
				{1,TX_BUTTON_SETUP8},
			},
#endif
			{
				WM_BUTTON9,
				0, {1,50},
				169, 330,
				{1,TX_BUTTON_SETUP9},
			},
			{
				WM_BUTTON10,
				0, {1,51},
				169+40, 330,
				{1,TX_BUTTON_SETUP10},
			},
			{
				WM_PHASE_STOP,
				0, {1,40},
				11, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_SETUPp,
		"image\\setup.blp",
		FALSE,
		{
			{
				WM_BUTTON1,
				0, {1,50},
				54, 330,
				{1,TX_BUTTON_SETUP1},
			},
			{
				WM_BUTTON2,
				0, {1,51},
				54+40, 330,
				{1,TX_BUTTON_SETUP2},
			},
			{
				WM_BUTTON3,
				0, {1,50},
				284, 330,
				{1,TX_BUTTON_SETUP3},
			},
			{
				WM_BUTTON4,
				0, {1,51},
				284+40, 330,
				{1,TX_BUTTON_SETUP4},
			},
			{
				WM_BUTTON5,
				0, {1,50},
				399, 330,
				{1,TX_BUTTON_SETUP5},
			},
			{
				WM_BUTTON6,
				0, {1,51},
				399+40, 330,
				{1,TX_BUTTON_SETUP6},
			},
#if !_EGAMES
			{
				WM_BUTTON7,
				0, {1,50},
				514, 330,
				{1,TX_BUTTON_SETUP7},
			},
			{
				WM_BUTTON8,
				0, {1,51},
				514+40, 330,
				{1,TX_BUTTON_SETUP8},
			},
#endif
			{
				WM_BUTTON9,
				0, {1,50},
				169, 330,
				{1,TX_BUTTON_SETUP9},
			},
			{
				WM_BUTTON10,
				0, {1,51},
				169+40, 330,
				{1,TX_BUTTON_SETUP10},
			},
			{
				WM_PHASE_PLAY,
				0, {1,77},
				11, 424,
				{1,TX_BUTTON_CONTP},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_READ,
		"image\\read.blp",
		FALSE,
		{
			{
				WM_READ0,
				0, {0},
				420, 30+42*0,
				0,
			},
			{
				WM_READ1,
				0, {0},
				420, 30+42*1,
				0,
			},
			{
				WM_READ2,
				0, {0},
				420, 30+42*2,
				0,
			},
			{
				WM_READ3,
				0, {0},
				420, 30+42*3,
				0,
			},
			{
				WM_READ4,
				0, {0},
				420, 30+42*4,
				0,
			},
			{
				WM_READ5,
				0, {0},
				420, 30+42*5,
				0,
			},
			{
				WM_READ6,
				0, {0},
				420, 30+42*6,
				0,
			},
			{
				WM_READ7,
				0, {0},
				420, 30+42*7,
				0,
			},
			{
				WM_READ8,
				0, {0},
				420, 30+42*8,
				0,
			},
			{
				WM_READ9,
				0, {0},
				420, 30+42*9,
				0,
			},
			{
				WM_PHASE_STOP,
				0, {1,40},
				16, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_WRITE,
		"image\\write.blp",
		FALSE,
		{
			{
				WM_WRITE0,
				0, {0},
				420, 30+42*0,
				0,
			},
			{
				WM_WRITE1,
				0, {0},
				420, 30+42*1,
				0,
			},
			{
				WM_WRITE2,
				0, {0},
				420, 30+42*2,
				0,
			},
			{
				WM_WRITE3,
				0, {0},
				420, 30+42*3,
				0,
			},
			{
				WM_WRITE4,
				0, {0},
				420, 30+42*4,
				0,
			},
			{
				WM_WRITE5,
				0, {0},
				420, 30+42*5,
				0,
			},
			{
				WM_WRITE6,
				0, {0},
				420, 30+42*6,
				0,
			},
			{
				WM_WRITE7,
				0, {0},
				420, 30+42*7,
				0,
			},
			{
				WM_WRITE8,
				0, {0},
				420, 30+42*8,
				0,
			},
			{
				WM_WRITE9,
				0, {0},
				420, 30+42*9,
				0,
			},
			{
				WM_PHASE_STOP,
				0, {1,40},
				16, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_WRITEp,
		"image\\write.blp",
		FALSE,
		{
			{
				WM_WRITE0,
				0, {0},
				420, 30+42*0,
				0,
			},
			{
				WM_WRITE1,
				0, {0},
				420, 30+42*1,
				0,
			},
			{
				WM_WRITE2,
				0, {0},
				420, 30+42*2,
				0,
			},
			{
				WM_WRITE3,
				0, {0},
				420, 30+42*3,
				0,
			},
			{
				WM_WRITE4,
				0, {0},
				420, 30+42*4,
				0,
			},
			{
				WM_WRITE5,
				0, {0},
				420, 30+42*5,
				0,
			},
			{
				WM_WRITE6,
				0, {0},
				420, 30+42*6,
				0,
			},
			{
				WM_WRITE7,
				0, {0},
				420, 30+42*7,
				0,
			},
			{
				WM_WRITE8,
				0, {0},
				420, 30+42*8,
				0,
			},
			{
				WM_WRITE9,
				0, {0},
				420, 30+42*9,
				0,
			},
			{
				WM_PHASE_PLAY,
				0, {1,77},
				16, 424,
				{1,TX_BUTTON_CONTP},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_LOST,
		"image\\lost.blp",
		TRUE,
		{
			{
				WM_PHASE_INFO,
				0, {1,50},
				9, 431,
				{1,TX_BUTTON_REPEAT},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_WIN,
		"image\\win.blp",
		TRUE,
		{
			{
				WM_NEXT,
				0, {1,51},
				9, 431,
				{1,TX_BUTTON_NEXTP},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_LASTWIN,
		"image\\last%.3d.blp",
		TRUE,
		{
			{
				WM_PHASE_INIT,
				0, {1,51},
				9, 431,
				{1,TX_BUTTON_NEXTP},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_BUILD,
		"image\\build.blp",
		TRUE,
		{
			{
				WM_DECOR1,		// pose des sols
				0, {6, 0,1,2,3,4,25},
				11+42*2, 190+42*0,
				{6,TX_OBJ_HERBE,TX_OBJ_MOUSSE,TX_OBJ_TERRE,
				   TX_OBJ_EAU,TX_OBJ_DALLESPEC,TX_OBJ_COUVTELE},
			},
			{
				WM_DECOR2,		// pose des plantes
				0, {4, 6,7,8,11},
				11+42*2, 190+42*1,
				{4,TX_OBJ_DELOBJ,TX_OBJ_PLANTE,
				   TX_OBJ_ARBRE,TX_OBJ_FLEUR1},
			},
			{
				WM_DECOR3,		// pose des batiments
				0, {11, 18,81,33,61,82,93,20,21,22,57,58},
				11+42*2, 190+42*2,
				{11,TX_OBJ_DELOBJ,TX_OBJ_BATIMENT,TX_OBJ_TOUR,
					TX_OBJ_MINEFER,TX_OBJ_BATENNEMIS,TX_OBJ_BARENNEMIS,
					TX_OBJ_MURPAL,TX_OBJ_ROC,TX_OBJ_OBJET,
					TX_OBJ_ARME,TX_OBJ_VEHICULE},
			},
			{
				WM_DECOR4,		// pose des blupi
				0, {10, 12,13,14,85,15,16,17,38,75,56},
				11+42*2, 190+42*3,
				{10,TX_OBJ_DELPERSO,TX_OBJ_BLUPIf,TX_OBJ_BLUPI,
					TX_OBJ_DISCIPLE,TX_OBJ_ARAIGNEE,TX_OBJ_VIRUS,
					TX_OBJ_TRACKS,TX_OBJ_BOMBE,TX_OBJ_ELECTRO,
					TX_OBJ_ROBOT},
			},
			{
				WM_DECOR5,		// pose les catastrophes
				0, {2, 36,37},
				11+42*2, 190+42*4,
				{2,TX_OBJ_DELFEU,TX_OBJ_STARTFEU},
			},
			{
				WM_PHASE_REGION,
				0, {1,5},
				11+42*0, 190+42*1,
				{1,TX_BUTTON_REGION},
			},
			{
				WM_PHASE_MUSIC,
				0, {1,44},
				11+42*0, 190+42*2,
				{1,TX_BUTTON_MUSIC},
			},
			{
				WM_PHASE_BUTTON,
				0, {1,46},
				11+42*0, 190+42*3,
				{1,TX_BUTTON_BUTTON},
			},
			{
				WM_PHASE_TERM,
				0, {1,45},
				11+42*0, 190+42*4,
				{1,TX_BUTTON_CTERM},
			},
			{
				WM_PHASE_INFO,
				0, {1,40},
				11+42*0, 424,
				{1,TX_BUTTON_TERMC},
			},
			{
				WM_PHASE_UNDO,
				0, {1,87},
				11+42*2, 424,
				{1,TX_BUTTON_UNDO},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_BUTTON,
		"image\\button.blp",
		TRUE,
		{
			{
				WM_BUTTON1,		// stop
				0, {1,40},
				170+42*0, 30+52*0,
				{1,TX_ACTION_STOP},
			},
			{
				WM_BUTTON0,		// go
				0, {1,24},
				170+42*1, 30+52*0,
				{1,TX_ACTION_GO},
			},
			{
				WM_BUTTON3,		// carry
				0, {1,30},
				170+42*3, 30+52*0,
				{1,TX_ACTION_CARRY},
			},
			{
				WM_BUTTON4,		// depose
				0, {1,31},
				170+42*4, 30+52*0,
				{1,TX_ACTION_DEPOSE},
			},
			{
				WM_BUTTON32,	// répète
				0, {1,100},
				170+42*6, 30+52*0,
				{1,TX_ACTION_REPEAT},
			},

			{
				WM_BUTTON5,		// abat
				0, {1,22},
				170+42*0, 30+52*1,
				{1,TX_ACTION_ABAT},
			},
			{
				WM_BUTTON16,	// abat n
				0, {1,42},
				170+42*1, 30+52*1,
				{1,TX_ACTION_ABATn},
			},
			{
				WM_BUTTON6,		// roc
				0, {1,27},
				170+42*3, 30+52*1,
				{1,TX_ACTION_ROC},
			},
			{
				WM_BUTTON17,	// roc n
				0, {1,43},
				170+42*4, 30+52*1,
				{1,TX_ACTION_ROCn},
			},
			{
				WM_BUTTON22,	// fleurs
				0, {1,54},
				170+42*6, 30+52*1,
				{1,TX_ACTION_FLEUR},
			},
			{
				WM_BUTTON23,	// fleurs n
				0, {1,55},
				170+42*7, 30+52*1,
				{1,TX_ACTION_FLEURn},
			},

			{
				WM_BUTTON9,		// build2 (couveuse)
				0, {1,25},
				170+42*0, 30+52*2,
				{1,TX_ACTION_BUILD2},
			},
			{
				WM_BUTTON15,	// palis
				0, {1,26},
				170+42*1, 30+52*2,
				{1,TX_ACTION_PALIS},
			},
			{
				WM_BUTTON18,	// pont
				0, {1,23},
				170+42*2, 30+52*2,
				{1,TX_ACTION_PONT},
			},
			{
				WM_BUTTON25,	// bateau
				0, {1,58},
				170+42*3, 30+52*2,
				{1,TX_ACTION_BATEAU},
			},
			{
				WM_BUTTON13,	// build6 (téléporteur)
				0, {1,101},
				170+42*4, 30+52*2,
				{1,TX_ACTION_BUILD6},
			},
			{
				WM_BUTTON14,	// mur
				0, {1,20},
				170+42*6, 30+52*2,
				{1,TX_ACTION_MUR},
			},
			{
				WM_BUTTON19,	// tour
				0, {1,33},
				170+42*7, 30+52*2,
				{1,TX_ACTION_TOUR},
			},

			{
				WM_BUTTON8,		// build1 (cabane)
				0, {1,19},
				170+42*0, 30+52*3,
				{1,TX_ACTION_BUILD1},
			},
			{
				WM_BUTTON7,		// cultive
				0, {1,28},
				170+42*1, 30+52*3,
				{1,TX_ACTION_CULTIVE},
			},
			{
				WM_BUTTON2,		// mange
				0, {1,32},
				170+42*2, 30+52*3,
				{1,TX_ACTION_MANGE},
			},

			{
				WM_BUTTON10,	// build3 (laboratoire)
				0, {1,35},
				170+42*0, 30+52*4,
				{1,TX_ACTION_BUILD3},
			},
			{
				WM_BUTTON21,	// laboratoire
				0, {1,39},
				170+42*1, 30+52*4,
				{1,TX_ACTION_LABO},
			},
			{
				WM_BUTTON20,	// boit
				0, {1,34},
				170+42*2, 30+52*4,
				{1,TX_ACTION_BOIT},
			},
			{
				WM_BUTTON24,	// dynamite
				0, {1,41},
				170+42*3, 30+52*4,
				{1,TX_ACTION_DYNAMITE},
			},

			{
				WM_BUTTON27,	// drapeau
				0, {1,64},
				170+42*0, 30+52*5,
				{1,TX_ACTION_DRAPEAU},
			},
			{
				WM_BUTTON11,	// build4 (mine)
				0, {1,61},
				170+42*1, 30+52*5,
				{1,TX_ACTION_BUILD4},
			},
			{
				WM_BUTTON28,	// extrait
				0, {1,62},
				170+42*2, 30+52*5,
				{1,TX_ACTION_EXTRAIT},
			},
			{
				WM_BUTTON12,	// build5 (usine)
				0, {1,59},
				170+42*4, 30+52*5,
				{1,TX_ACTION_BUILD5},
			},
			{
				WM_BUTTON29,	// fabrique jeep
				0, {1,65},
				170+42*5, 30+52*5,
				{1,TX_ACTION_FABJEEP},
			},
			{
				WM_BUTTON30,	// fabrique mine
				0, {1,63},
				170+42*6, 30+52*5,
				{1,TX_ACTION_FABMINE},
			},
			{
				WM_BUTTON34,	// fabrique armure
				0, {1,106},
				170+42*7, 30+52*5,
				{1,TX_ACTION_FABARMURE},
			},
			{
				WM_BUTTON31,	// fabrique disciple
				0, {1,83},
				170+42*8, 30+52*5,
				{1,TX_ACTION_FABDISC},
			},

//			{
//				WM_BUTTON13,	// build6
//				0, {0},
//				170+42*2, 30+52*5,
//				{1,TX_ACTION_BUILD6},
//			},
			{
				WM_PHASE_BUILD,
				0, {1,50},
				11, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_TERM,
		"image\\term.blp",
		TRUE,
		{
			{
				WM_BUTTON8,		// home blupi
				0, {1,81},
				170+42*0, 30+42*0,
				{1,TX_BUTTON_TERMMBLUPI},
			},
			{
				WM_BUTTON9,		// kill robots
				0, {1,57},
				170+42*1, 30+42*0,
				{1,TX_BUTTON_TERMKILL},
			},
			{
				WM_BUTTON3,		// stop fire
				0, {1,37},
				170+42*2, 30+42*0,
				{1,TX_BUTTON_TERMFIRE},
			},
			{
				WM_BUTTON1,		// hach blupi
				0, {1,14},
				170+42*0, 30+42*2,
				{1,TX_BUTTON_TERMHBLUPI},
			},
			{
				WM_BUTTON2,		// hach planche
				0, {1,22},
				170+42*1, 30+42*2,
				{1,TX_BUTTON_TERMHPLANCHE},
			},
			{
				WM_BUTTON10,	// hach tomate
				0, {1,28},
				170+42*2, 30+42*2,
				{1,TX_BUTTON_TERMHTOMATE},
			},
			{
				WM_BUTTON11,	// hach métal
				0, {1,84},
				170+42*3, 30+42*2,
				{1,TX_BUTTON_TERMHMETAL},
			},
			{
				WM_BUTTON12,	// hach robot
				0, {1,94},
				170+42*4, 30+42*2,
				{1,TX_BUTTON_TERMHROBOT},
			},
			{
				WM_BUTTON4,		// - min blupi
				0, {1,50},
				170+42*0, 30+42*4,
				{1,TX_BUTTON_TERMDEC},
			},
			{
				WM_BUTTON5,		// + min blupi
				0, {1,51},
				170+42*1, 30+42*4,
				{1,TX_BUTTON_TERMINC},
			},
			{
				WM_BUTTON6,		// - max blupi
				0, {1,50},
				170+42*0, 30+42*5,
				{1,TX_BUTTON_TERMDEC},
			},
			{
				WM_BUTTON7,		// + max blupi
				0, {1,51},
				170+42*1, 30+42*5,
				{1,TX_BUTTON_TERMINC},
			},
			{
				WM_PHASE_BUILD,
				0, {1,50},
				11, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_MUSIC,
		"image\\music.blp",
		TRUE,
		{
			{
				WM_BUTTON1,
				0, {1,40},
				170+42*0, 30+42*0,
				{1,TX_BUTTON_MUSIC1},
			},
			{
				WM_BUTTON2,
				0, {1,44},
				170+42*0, 30+42*1,
				{1,TX_BUTTON_MUSIC2},
			},
			{
				WM_BUTTON3,
				0, {1,44},
				170+42*0, 30+42*2,
				{1,TX_BUTTON_MUSIC3},
			},
			{
				WM_BUTTON4,
				0, {1,44},
				170+42*0, 30+42*3,
				{1,TX_BUTTON_MUSIC4},
			},
			{
				WM_BUTTON5,
				0, {1,44},
				170+42*0, 30+42*4,
				{1,TX_BUTTON_MUSIC5},
			},
			{
				WM_BUTTON6,
				0, {1,44},
				170+42*0, 30+42*5,
				{1,TX_BUTTON_MUSIC6},
			},
			{
				WM_BUTTON7,
				0, {1,44},
				170+42*0, 30+42*6,
				{1,TX_BUTTON_MUSIC7},
			},
			{
				WM_BUTTON8,
				0, {1,44},
				170+42*0, 30+42*7,
				{1,TX_BUTTON_MUSIC8},
			},
			{
				WM_BUTTON9,
				0, {1,44},
				170+42*0, 30+42*8,
				{1,TX_BUTTON_MUSIC9},
			},
			{
				WM_BUTTON10,
				0, {1,44},
				170+42*0, 30+42*9,
				{1,TX_BUTTON_MUSIC10},
			},
			{
				WM_PHASE_BUILD,
				0, {1,50},
				11, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_REGION,
		"image\\region.blp",
		TRUE,
		{
			{
				WM_BUTTON1,		// normal
				0, {0},
				220, 60,
				{1,TX_BUTTON_REGION1},
			},
			{
				WM_BUTTON4,		// sapins
				0, {0},
				220, 170,
				{1,TX_BUTTON_REGION4},
			},
			{
				WM_BUTTON2,		// palmiers
				0, {0},
				220, 280,
				{1,TX_BUTTON_REGION2},
			},
			{
				WM_BUTTON3,		// hivers
				0, {0},
				220, 390,
				{1,TX_BUTTON_REGION3},
			},
			{
				WM_PHASE_BUILD,
				0, {1,50},
				11, 424,
				{1,TX_BUTTON_TERM},
			},
			{
				0
			},
		},
	},

	{
		WM_PHASE_PLAYMOVIE,
		"image\\movie.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_WINMOVIE,
		"image\\movie.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_H0MOVIE,
		"image\\movie.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_H1MOVIE,
		"image\\movie.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_H2MOVIE,
		"image\\movie.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_BYE,
		"image\\bye.blp",
		FALSE,
		{
			{
				0
			},
		},
	},

	{
		WM_PHASE_INSERT,
		"image\\insert.blp",
		FALSE,
		{
//?			{
//?				WM_PHASE_INFO,
//?				0, {1,48},
//?				16, 424-42,
//?				{1,TX_BUTTON_PLAYP},
//?			},
			{
				WM_PHASE_INIT,
				0, {1,40},
				16, 424,
				{1,TX_BUTTON_QUITTER},
			},
			{
				0
			},
		},
	},

	{
		0
	}
};


/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CEvent::CEvent()
{
	int		i;

	m_bFullScreen   = TRUE;
	m_mouseType     = MOUSETYPEGRA;
	m_exercice      = 0;
	m_mission       = 0;
	m_private       = 0;
	m_maxMission    = 0;
	m_phase         = 0;
	m_index         = -1;
	m_bSchool       = FALSE;
	m_bPrivate      = FALSE;
	m_bAccessBuild  = FALSE;
	m_bRunMovie     = FALSE;
	m_bBuildModify  = FALSE;
	m_bMousePress   = FALSE;
	m_bMouseDown    = FALSE;
	m_oldMousePos.x = 0;
	m_oldMousePos.y = 0;
	m_mouseSprite   = 0;
	m_bFillMouse    = FALSE;
	m_bWaitMouse    = FALSE;
	m_bHideMouse    = FALSE;
	m_bShowMouse    = FALSE;
	m_rankCheat     = -1;
	m_posCheat      = 0;
	m_speed         = 1;
	m_bMovie        = TRUE;
	m_bSpeed        = FALSE;
	m_bHelp         = FALSE;
	m_bAllMissions  = FALSE;
	m_bChangeCheat  = FALSE;
	m_scrollSpeed   = 1;
	m_bPause        = FALSE;
	m_bShift        = FALSE;
	m_shiftPhase    = 0;
	m_movieToStart[0] = 0;
	m_bInfoHelp     = FALSE;
	m_bDemoRec      = FALSE;
	m_bDemoPlay     = FALSE;
	m_pDemoBuffer   = NULL;
	m_demoTime      = 0;
	m_bCtrlDown     = FALSE;

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		m_lastFloor[i] = 0;
	}

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		m_lastObject[i] = 0;
	}

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		m_lastHome[i] = 0;
	}
}

// Destructeur.

CEvent::~CEvent()
{
	WriteInfo();  // lit le fichier "info.blp"
}


// Retourne la position de la souris.

POINT CEvent::GetMousePos()
{
	POINT		pos;

	GetCursorPos(&pos);
	ScreenToClient(m_hWnd, &pos);

	return pos;
}


// Initialise le mode full screen ou non.

void CEvent::SetFullScreen(BOOL bFullScreen)
{
	m_bFullScreen = bFullScreen;
}

// Initialise le type de souris.

void CEvent::SetMouseType(int mouseType)
{
	m_mouseType = mouseType;
}

// Crée le gestionnaire d'événements.

void CEvent::Create(HWND hWnd, CPixmap *pPixmap, CDecor *pDecor,
					CSound *pSound, CMovie *pMovie)
{
	POINT	pos;

	m_hWnd    = hWnd;
	m_pPixmap = pPixmap;
	m_pDecor  = pDecor;
	m_pSound  = pSound;
	m_pMovie  = pMovie;

	ReadInfo();  // lit le fichier "info.blp"

	pos.x = 10;
	pos.y = 158;
	m_jauges[0].Create(m_hWnd, m_pPixmap, m_pSound, pos, 1, TRUE);

	pos.y += DIMJAUGEY+2;
	m_jauges[1].Create(m_hWnd, m_pPixmap, m_pSound, pos, 3, TRUE);
}


// Retourne l'index d'un bouton.

int CEvent::GetButtonIndex(int button)
{
	int			i=0;

	while ( table[m_index].buttons[i].message != 0 )
	{
		if ( (UINT)button == table[m_index].buttons[i].message )
		{
			return i;
		}
		i ++;
	}

	return -1;
}

int CEvent::GetState(int button)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return 0;

	return m_buttons[index].GetState();
}

void CEvent::SetState(int button, int state)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return;

	m_buttons[index].SetState(state);
}

BOOL CEvent::GetEnable(int button)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return 0;

	return m_buttons[index].GetEnable();
}

void CEvent::SetEnable(int button, BOOL bEnable)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return;

	m_buttons[index].SetEnable(bEnable);
}

BOOL CEvent::GetHide(int button)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return 0;

	return m_buttons[index].GetHide();
}

void CEvent::SetHide(int button, BOOL bHide)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return;

	m_buttons[index].SetHide(bHide);
}

int CEvent::GetMenu(int button)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return 0;

	return m_buttons[index].GetMenu();
}

void CEvent::SetMenu(int button, int menu)
{
	int		index;

	index = GetButtonIndex(button);
	if ( index < 0 )  return;

	m_buttons[index].SetMenu(menu);
}



// Restitue le jeu après une activation en mode fullScreen.

void CEvent::RestoreGame()
{
	int		i;

	if ( m_phase == WM_PHASE_PLAY )
	{
		i = 0;
		while ( table[m_index].buttons[i].message != 0 )
		{
			m_buttons[i].Redraw();  // faudra redessiner
			i ++;
		}

		for ( i=0 ; i<2 ; i++ )
		{
			m_jauges[i].Redraw();
		}
	}

	HideMouse(FALSE);
	WaitMouse(TRUE);
	WaitMouse(FALSE);  // force le changement de sprite !
}


// Crée tous les boutons nécessaires à la phase en cours.

BOOL CEvent::CreateButtons()
{
	int			i=0, message;
	POINT		pos;
	BOOL		bMinimizeRedraw = FALSE;

	if ( m_phase == WM_PHASE_PLAY )
	{
		bMinimizeRedraw = TRUE;
	}

	while ( table[m_index].buttons[i].message != 0 )
	{
		pos.x = table[m_index].buttons[i].x;
		pos.y = table[m_index].buttons[i].y;
		message = table[m_index].buttons[i].message;

		if ( m_bPrivate )
		{
			if ( message == WM_PHASE_SKILL1 )
			{
				pos.x = 117;
				pos.y = 115;
			}
			if ( message == WM_PHASE_SKILL2 )
			{
				pos.x = 117;
				pos.y = 115+42;
			}
		}

		m_buttons[i].Create(m_hWnd, m_pPixmap, m_pSound, pos,
							table[m_index].buttons[i].type,
							bMinimizeRedraw,
							table[m_index].buttons[i].iconMenu+1,
							table[m_index].buttons[i].iconMenu[0],
							table[m_index].buttons[i].toolTips+1,
							table[m_index].buttons[i].toolTips[0],
							m_pDecor->GetRegion(),
							message);
		i ++;
	}

	return TRUE;

}

// Ajoute un cheat-code dans un buffer.

void AddCheatCode(char *pDst, char *pSrc)
{
	int		i, j;

	if ( pDst[0] != 0 )  strcat(pDst, " / ");

	i = 0;
	j = strlen(pDst);
	while ( pSrc[i] != 0 )
	{
		pDst[j++] = tolower(pSrc[i++]);
	}
	pDst[j] = 0;
}

// Dessine un texte multi-lignes centré.

void CEvent::DrawTextCenter(int res, int x, int y, int font)
{
	char	text[100];
	POINT	pos;

	LoadString(res, text, 100);
	pos.x = x;
	pos.y = y;
	::DrawTextCenter(m_pPixmap, pos, text, font);
}

// Dessine tous les boutons de la phase en cours.

BOOL CEvent::DrawButtons()
{
	int			i;
	int			levels[2];
	int			types[2];
	int			world, time, lg, button, volume, pente, icon;
	char		res[100];
	char		text[100];
	POINT		pos;
	RECT		rect;
	BOOL		bEnable;

//?
#if 0
	pos.x = 1;
	pos.y = 1;
	rect.left   = pos.x;
	rect.right  = pos.x+50;
	rect.top    = pos.y;
	rect.bottom = pos.y+DIMLITTLEY;
	m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond
	time = m_pDecor->GetTotalTime();
	sprintf(text, "time=%d", time);
	DrawText(m_pPixmap, pos, text, FONTLITTLE);
#endif
//?

	if ( (m_phase == WM_PHASE_PLAY && m_bChangeCheat) ||
		 (m_phase != WM_PHASE_PLAY   &&
		  m_phase != WM_PHASE_INSERT &&
		  m_phase != WM_PHASE_INTRO1 &&
		  m_phase != WM_PHASE_INTRO2 &&
		  m_phase != WM_PHASE_BYE    ) )
	{
		m_bChangeCheat = FALSE;

		text[0] = 0;
		if ( m_bAllMissions )
		{
			AddCheatCode(text, cheat_code[3]);
		}
		if ( m_bSpeed )
		{
			AddCheatCode(text, cheat_code[4]);
		}
		if ( m_bHelp )
		{
			AddCheatCode(text, cheat_code[5]);
		}
		if ( m_pDecor->GetInvincible() )
		{
			AddCheatCode(text, cheat_code[6]);
		}
		if ( m_pDecor->GetSuper() )
		{
			AddCheatCode(text, cheat_code[7]);
		}
		pos.x = 2;
		pos.y = 2;
		rect.left   = pos.x;
		rect.right  = pos.x+300;
		rect.top    = pos.y;
		rect.bottom = pos.y+DIMLITTLEY;
		m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond
		DrawText(m_pPixmap, pos, text, FONTLITTLE);
	}

	if ( m_phase == WM_PHASE_INIT )
	{
		pos.x = 580;
		pos.y = 465;
#if _EGAMES
		DrawText(m_pPixmap, pos, "Version 1.0", FONTLITTLE);
#else
		DrawText(m_pPixmap, pos, "Version 1.7", FONTLITTLE);
#endif
#if _DEMO
		pos.x = 580;
		pos.y = 465-20;
		DrawText(m_pPixmap, pos, "D E M O", FONTRED);
#endif
#if _SE
		pos.x = 580;
		pos.y = 465-20;
		DrawText(m_pPixmap, pos, "S E", FONTRED);
#endif
	}

	if ( m_phase == WM_PHASE_SETUP  ||
		 m_phase == WM_PHASE_SETUPp )
	{
		bEnable = TRUE;
		if ( m_speed == 1 )  bEnable = FALSE;
		SetEnable(WM_BUTTON1, bEnable);
		bEnable = TRUE;
		if ( m_speed >= (m_bSpeed?8:2) )  bEnable = FALSE;
		SetEnable(WM_BUTTON2, bEnable);

		volume = m_pSound->GetAudioVolume();
		bEnable = TRUE;
		if ( volume == 0 ||
			 !m_pSound->GetEnable() )  bEnable = FALSE;
		SetEnable(WM_BUTTON3, bEnable);
		bEnable = TRUE;
		if ( volume >= MAXVOLUME ||
			 !m_pSound->GetEnable() )  bEnable = FALSE;
		SetEnable(WM_BUTTON4, bEnable);

		volume = m_pSound->GetMidiVolume();
		bEnable = TRUE;
		if ( volume == 0 ||
			 !m_pSound->GetEnable() )  bEnable = FALSE;
		SetEnable(WM_BUTTON5, bEnable);
		bEnable = TRUE;
		if ( volume >= MAXVOLUME ||
			 !m_pSound->GetEnable() )  bEnable = FALSE;
		SetEnable(WM_BUTTON6, bEnable);

		if ( m_pMovie->GetEnable() )
		{
			SetEnable(WM_BUTTON7,  m_bMovie);
			SetEnable(WM_BUTTON8, !m_bMovie);
		}
		else
		{
			SetEnable(WM_BUTTON7, FALSE);
			SetEnable(WM_BUTTON8, FALSE);
		}

		bEnable = TRUE;
		if ( m_scrollSpeed == 0 )  bEnable = FALSE;
		SetEnable(WM_BUTTON9, bEnable);
		bEnable = TRUE;
		if ( m_scrollSpeed >= 3 )  bEnable = FALSE;
		SetEnable(WM_BUTTON10, bEnable);
	}

	// Dessine les boutons.
	i = 0;
	while ( table[m_index].buttons[i].message != 0 )
	{
		m_buttons[i].Draw();
		i ++;
	}

	if ( m_phase == WM_PHASE_PLAY )
	{
		// Dessine les jauges.
		m_pDecor->GetLevelJauge(levels, types);

		for ( i=0 ; i<2 ; i++ )
		{
			if ( levels[i] < 0 )
			{
				m_jauges[i].SetHide(TRUE);
			}
			else
			{
				m_jauges[i].SetHide(FALSE);
				m_jauges[i].SetLevel(levels[i]);
				m_jauges[i].SetType(types[i]);
			}
			m_jauges[i].Draw();
		}

		// Dessine le menu.
		if ( m_menu.IsExist() )
		{
			m_pDecor->BlupiGetButtons(m_menuPos, m_menuNb,
									  m_menuButtons, m_menuErrors,
									  m_menuPerso);
			m_menu.Update(m_menuNb, m_menuButtons, m_menuErrors);

			button = m_menu.GetSel();
			m_pDecor->CelHiliButton(m_menuCel, button);
		}
		m_menu.Draw();

		// Dessine la pause.
		if ( m_bPause )
		{
			DrawTextCenter(TX_PAUSE, (10+134)/2, 20);
		}
		else
		{
			if ( m_bDemoRec )  // démo en enregistrement ?
			{
				DrawTextCenter(TX_DEMOREC, (10+134)/2, 20, FONTRED);
			}
			if ( m_bDemoPlay )  // démo en restitution ?
			{
				DrawTextCenter(TX_DEMOPLAY, (10+134)/2, 20, FONTRED);
			}
		}

		// Dessine la rose des vents.
		if ( !m_bPause && !m_bDemoRec && !m_bDemoPlay )
		{
			DrawTextCenter(TX_DIRECT_N, (10+134)/2, 17);
			DrawTextCenter(TX_DIRECT_S, (10+134)/2, 126);
			DrawTextCenter(TX_DIRECT_O, 14, 70);
			DrawTextCenter(TX_DIRECT_E, 129, 70);
		}

		// Dessine la vitesse.
		pos.x = 64;
		pos.y = LYIMAGE-15;
		rect.left   = pos.x;
		rect.right  = pos.x+20;
		rect.top    = pos.y;
		rect.bottom = pos.y+15;
		m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond
		if ( m_speed > 1 )
		{
			sprintf(res, "x%d", m_speed);
			DrawText(m_pPixmap, pos, res);
		}

		// Dessine le bouton pour les infos.
		if ( m_pDecor->GetInfoMode() )  // infos visibles ?
		{
			lg = m_pDecor->GetInfoHeight();

			pos.x = POSDRAWX;
			pos.y = 0;
			rect.left   = POSDRAWX;
			rect.right  = POSDRAWX+DIMDRAWX;
			rect.top    = 0;
			rect.bottom = lg;
			m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);

			pos.x = POSDRAWX;
			pos.y = lg;
			rect.left   = POSDRAWX;
			rect.right  = POSDRAWX+DIMDRAWX;
			rect.top    = 0;
			rect.bottom = POSDRAWY;
			m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);

			pos.x = POSDRAWX+20;
			pos.y = POSDRAWY+4;
			DrawTextRect(m_pPixmap, pos, m_libelle, 0, FONTLITTLE, 1);
			pos.x = POSDRAWX+DIMDRAWX/2+20;
			DrawTextRect(m_pPixmap, pos, m_libelle, 0, FONTLITTLE, 2);

			pos.x = POSDRAWX+DIMDRAWX/2-DIMBUTTONX/2;
			pos.y = lg-14;
			m_pPixmap->DrawIcon(-1, CHBUTTON, m_bHiliInfoButton?73:72, pos);

			if ( IsHelpHide() )  // bouton pour aide ?
			{
				m_posHelpButton.x = -1;
			}
			else
			{
				m_posHelpButton.x = POSDRAWX+DIMDRAWX-DIMBUTTONX-2;
				m_posHelpButton.y = lg-DIMBUTTONY-2;
				m_pPixmap->DrawIcon(-1, CHBUTTON, m_bHiliHelpButton?2:0, m_posHelpButton);
				if ( m_bInfoHelp )  icon = 86;  // livre
				else                icon = 92;  // aide
				m_pPixmap->DrawIcon(-1, CHBUTTON, icon, m_posHelpButton);
			}
		}
		else		// infos cachées ?
		{
			pos.x = POSDRAWX+DIMDRAWX/2-DIMBUTTONX/2;
			pos.y = -12;
			m_pPixmap->DrawIcon(-1, CHBUTTON, m_bHiliInfoButton?75:74, pos);
		}
		m_posInfoButton = pos;
	}

	// Dessine les noms des fichiers.
	if ( m_phase == WM_PHASE_READ   ||
		 m_phase == WM_PHASE_WRITE  ||
		 m_phase == WM_PHASE_WRITEp )
	{
		if ( m_phase == WM_PHASE_READ )
		{
			LoadString(TX_BUTTON_READP, res, 50);
		}
		else
		{
			LoadString(TX_BUTTON_WRITEP, res, 50);
		}
		pos.x = 420;
		pos.y = 8;
		DrawText(m_pPixmap, pos, res);

		for ( i=0 ; i<10 ; i++ )
		{
			world = m_fileWorld[i];
			time  = m_fileTime[i];

			sprintf(text, "%d", i+1);
			lg = GetTextWidth(text);
			pos.x = (420+460)/2-lg/2;
			pos.y = 30+12+42*i;
			DrawText(m_pPixmap, pos, text, FONTSLIM);

			pos.x = 420+50;

			if ( world >= 0 )
			{
				if ( world >= 200 )
				{
					LoadString(TX_IONAMEPR, res, 50);
					sprintf(text, res, (world-200)+1, time/100);
				}
				else if ( world >= 100 )
				{
					LoadString(TX_IONAMEMI, res, 50);
					sprintf(text, res, (world-100)+1, time/100);
				}
				else
				{
					LoadString(TX_IONAMEEX, res, 50);
					sprintf(text, res, world+1, time/100);
				}
				DrawText(m_pPixmap, pos, text);  // partie x, temps t
			}
			else
			{
				LoadString(TX_IOFREE, res, 50);
				DrawText(m_pPixmap, pos, res, FONTRED);  // libre
			}
		}
	}

	// Dessine les réglages pour la fin de la partie.
	if ( m_phase == WM_PHASE_TERM )
	{
		Term*	pTerm = m_pDecor->GetTerminated();

		DrawTextCenter(TX_TERM, (10+134)/2, 20);

		pos.x = 170+42*2+4;
		pos.y = 30+12+42*4;
		LoadString(TX_TERMMIN, res, 50);
		sprintf(text, res, pTerm->nbMinBlupi);
		DrawText(m_pPixmap, pos, text);

		pos.x = 170+42*2+4;
		pos.y = 30+12+42*5;
		LoadString(TX_TERMMAX, res, 50);
		sprintf(text, res, pTerm->nbMaxBlupi);
		DrawText(m_pPixmap, pos, text);
	}

	// Dessine les textes pour les choix des boutons.
	if ( m_phase == WM_PHASE_BUTTON )
	{
		DrawTextCenter(TX_BUTTON, (10+134)/2, 20);
	}

	// Dessine les textes pour le choix des musiques.
	if ( m_phase == WM_PHASE_MUSIC )
	{
		DrawTextCenter(TX_MUSIC, (10+134)/2, 20);
	}

	// Dessine les textes pour le choix de la région.
	if ( m_phase == WM_PHASE_REGION )
	{
		DrawTextCenter(TX_REGION, (10+134)/2, 20);
	}

	// Ajoute "Mission numéro".
	if ( m_phase == WM_PHASE_INFO )
	{
		if ( m_bSchool  )  LoadString(TX_SCHOOL,  res, 50);
		else               LoadString(TX_MISSION, res, 50);
		if ( m_bPrivate )  LoadString(TX_PRIVATE, res, 50);

		lg = GetTextWidth(res);
		pos.x = (140+270)/2-lg/2;
		pos.y = 70;
		if ( m_bSchool  )  pos.x -= 40;
		if ( m_bPrivate )  pos.x -= 100;
		if ( m_bPrivate )  pos.y += 14;
		DrawText(m_pPixmap, pos, res, FONTSLIM);
	}

	// Ajoute le texte "Partie interrompue".
	if ( m_phase == WM_PHASE_STOP )
	{
		LoadString(TX_PAUSE, res, 50);
		lg = GetTextWidth(res);
		pos.x = (140+270)/2-lg/2;
		pos.y = 70;
		if ( m_bSchool  )  pos.x -= 40;
		if ( m_bPrivate )  pos.x -= 100;
		if ( m_bPrivate )  pos.y += 14;
		DrawText(m_pPixmap, pos, res, FONTRED);
	}

	// Ajoute le texte "Informations complémentaires".
	if ( m_phase == WM_PHASE_HELP )
	{
		LoadString(TX_HELP, res, 50);
		lg = GetTextWidth(res);
		pos.x = (140+270)/2-lg/2;
		pos.y = 70;
		if ( m_bSchool  )  pos.x -= 40;
		if ( m_bPrivate )  pos.x -= 100;
		if ( m_bPrivate )  pos.y += 14;
		DrawText(m_pPixmap, pos, res, FONTRED);
	}

	// Ajoute le numéro du monde.
	if ( m_phase == WM_PHASE_INFO ||
		 m_phase == WM_PHASE_STOP ||
		 m_phase == WM_PHASE_HELP )
	{
		if ( m_bSchool  )  world = m_exercice;
		else               world = m_mission;
		if ( m_bPrivate )  world = m_private;

		lg = GetBignumWidth(world+1);
		pos.x = (140+270)/2-lg/2;
		pos.y = 100;
		if ( m_bSchool  )  pos.x -= 40;
		if ( m_bPrivate )  pos.x -= 135;
		if ( m_bPrivate )  pos.y = 115;
		DrawBignum(m_pPixmap, pos, world+1);
	}

#if !_DEMO
	// Affiche facile/difficile.
	if ( m_phase == WM_PHASE_INFO )
	{
		if ( !m_bSchool )
		{
			if ( m_pDecor->GetSkill() == 0 )
			{
				LoadString(TX_SKILL1, res, 50);
				if ( m_bPrivate )
				{
					pos.x = 117+50;
					pos.y = 115+13;
				}
				else
				{
					pos.x = 150+50;
					pos.y = 230+13;
				}
				DrawText(m_pPixmap, pos, res, FONTSLIM);
			}

			if ( m_pDecor->GetSkill() == 1 )
			{
				LoadString(TX_SKILL2, res, 50);
				if ( m_bPrivate )
				{
					pos.x = 117+50;
					pos.y = 115+42+13;
				}
				else
				{
					pos.x = 150+50;
					pos.y = 230+42+13;
				}
				DrawText(m_pPixmap, pos, res, FONTSLIM);
			}
		}
	}
#endif

	// Affiche le libellé de l'énigme.
	if ( m_phase == WM_PHASE_INFO     ||
		 m_phase == WM_PHASE_STOP     ||
		 m_phase == WM_PHASE_HELP     ||
		 m_phase == WM_PHASE_HISTORY0 ||
		 m_phase == WM_PHASE_HISTORY1 )
	{
		pos.x = 355;
		pos.y = 70;
		if ( m_bSchool )  pos.x -= 20;
		if ( m_bPrivate )
		{
			pos.x = 460;
			pos.y = 260;
		}
		if ( m_bSchool || m_bPrivate )  pente = 0;
		else                            pente = 19;
		DrawTextRect(m_pPixmap, pos, m_libelle, pente, FONTSLIM);
	}

	// Affiche le texte lorsque c'est raté.
	if ( m_phase == WM_PHASE_LOST )
	{
		LoadString(TX_LOST1+GetWorld()%5, res, 50);
		pos.x = 60;
		pos.y = 443;
		DrawText(m_pPixmap, pos, res);
	}

	// Affiche le texte lorsque c'est réussi.
	if ( m_phase == WM_PHASE_WIN )
	{
		LoadString(TX_WIN1+GetWorld()%5, res, 50);
		pos.x = 60;
		pos.y = 443;
		DrawText(m_pPixmap, pos, res);
	}

	// Affiche le texte lorsque c'est fini.
	if ( m_phase == WM_PHASE_LASTWIN )
	{
		if ( m_bSchool  )  LoadString(TX_LASTWIN1, res, 50);
		else               LoadString(TX_LASTWIN2, res, 50);
		if ( m_bPrivate )  LoadString(TX_LASTWIN3, res, 50);
		pos.x = 60;
		pos.y = 443;
		DrawText(m_pPixmap, pos, res);
	}

	// Dessine les réglages.
	if ( m_phase == WM_PHASE_SETUP  ||
		 m_phase == WM_PHASE_SETUPp )
	{
		DrawTextCenter(TX_INFO_SETUP1,  54+40, 80);
		DrawTextCenter(TX_INFO_SETUP5, 169+40, 80);
		DrawTextCenter(TX_INFO_SETUP2, 284+40, 80);
		DrawTextCenter(TX_INFO_SETUP3, 399+40, 80);
#if !_EGAMES
		DrawTextCenter(TX_INFO_SETUP4, 514+40, 80);
#endif

		sprintf(res, "x%d", m_speed);
		lg = GetTextWidth(res);
		pos.x = (54+40)-lg/2;
		pos.y = 330-20;
		DrawText(m_pPixmap, pos, res);

		sprintf(res, "%d", m_pSound->GetAudioVolume());
		lg = GetTextWidth(res);
		pos.x = (284+40)-lg/2;
		pos.y = 330-20;
		DrawText(m_pPixmap, pos, res);

		sprintf(res, "%d", m_pSound->GetMidiVolume());
		lg = GetTextWidth(res);
		pos.x = (399+40)-lg/2;
		pos.y = 330-20;
		DrawText(m_pPixmap, pos, res);

#if !_EGAMES
		if ( m_pMovie->GetEnable() )
		{
			if ( m_bMovie )  LoadString(TX_OUI, res, 50);
			else             LoadString(TX_NON, res, 50);
		}
		else
		{
			LoadString(TX_NON, res, 50);
		}
		lg = GetTextWidth(res);
		pos.x = (514+40)-lg/2;
		pos.y = 330-20;
		DrawText(m_pPixmap, pos, res);
#endif

		if ( m_scrollSpeed == 0 )  LoadString(TX_INFO_NOSCROLL, res, 50);
		else                       sprintf(res, "%d", m_scrollSpeed);
		lg = GetTextWidth(res);
		pos.x = (169+40)-lg/2;
		pos.y = 330-20;
		DrawText(m_pPixmap, pos, res);
	}

	// Affiche le texte de fin de la version demo.
	if ( m_phase == WM_PHASE_BYE )
	{
#if _DEMO
		LoadString(TX_DEMO_END1, res, 100);
#else
		LoadString(TX_FULL_END1, res, 100);
#endif
		lg = GetTextWidth(res);
		pos.x = LXIMAGE/2-lg/2;
		pos.y = 20;
		DrawText(m_pPixmap, pos, res);
#if _DEMO
		LoadString(TX_DEMO_END2, res, 100);
#else
		LoadString(TX_FULL_END2, res, 100);
#endif
		lg = GetTextWidth(res);
		pos.x = LXIMAGE/2-lg/2;
		pos.y = 40;
		DrawText(m_pPixmap, pos, res);

#if _DEMO
		LoadString(TX_DEMO_END3, res, 100);
#else
		LoadString(TX_FULL_END3, res, 100);
#endif
		lg = GetTextWidth(res);
		pos.x = LXIMAGE/2-lg/2;
		pos.y = 430;
		DrawText(m_pPixmap, pos, res);
#if _DEMO
		LoadString(TX_DEMO_END4, res, 100);
#else
		LoadString(TX_FULL_END4, res, 100);
#endif
		lg = GetTextWidth(res);
		pos.x = LXIMAGE/2-lg/2;
		pos.y = 450;
		DrawText(m_pPixmap, pos, res);
	}

	// Affiche le texte lorsqu'il faut insérer le CD-Rom.
	if ( m_phase == WM_PHASE_INSERT )
	{
		DrawTextCenter(TX_INSERT, LXIMAGE/2, 20);
	}

	if ( m_phase == WM_PHASE_BUILD )
	{
		SetEnable(WM_PHASE_UNDO, m_pDecor->IsUndo());
	}

	// Dessine les tool tips (info bulle).
	if ( m_textToolTips[0] != 0 )
	{
		DrawText(m_pPixmap, m_posToolTips, m_textToolTips);
	}

	return TRUE;
}

// Retourne le lutin à utiliser à une position donnée.

int CEvent::MousePosToSprite(POINT pos)
{
	int		sprite;
	BOOL	bUp=FALSE, bDown=FALSE, bLeft=FALSE, bRight=FALSE;

	sprite = SPRITE_POINTER;

	if ( m_phase == WM_PHASE_PLAY  ||
		 m_phase == WM_PHASE_BUILD )
	{
		if ( pos.x >= POSMAPX && pos.x <= POSMAPX+DIMMAPX &&
			 pos.y >= POSMAPY && pos.y <= POSMAPY+DIMMAPY )
		{
			sprite = SPRITE_MAP;
		}

		if ( m_bFullScreen &&
			 !m_bDemoRec   &&
			 !m_bDemoPlay  &&
			 m_scrollSpeed != 0 )
		{
			if ( pos.x <= 5 && pos.x >= -2 )
			{
				bLeft = TRUE;
			}

			if ( pos.x >= LXIMAGE-5 && pos.x <= LXIMAGE+2 )
			{
				bRight = TRUE;
			}

			if ( pos.y <= 5 && pos.y >= -2 )
			{
				bUp = TRUE;
			}

			if ( pos.y >= LYIMAGE-5 && pos.y <= LYIMAGE+2 )
			{
				bDown = TRUE;
			}

			if ( bLeft           )  sprite = SPRITE_ARROWL;
			if ( bRight          )  sprite = SPRITE_ARROWR;
			if ( bUp             )  sprite = SPRITE_ARROWU;
			if ( bDown           )  sprite = SPRITE_ARROWD;
			if ( bLeft  && bUp   )  sprite = SPRITE_ARROWUL;
			if ( bLeft  && bDown )  sprite = SPRITE_ARROWDL;
			if ( bRight && bUp   )  sprite = SPRITE_ARROWUR;
			if ( bRight && bDown )  sprite = SPRITE_ARROWDR;
		}
	}
	else if ( m_phase == WM_PHASE_INTRO1 ||
			  m_phase == WM_PHASE_INTRO2 )
	{
		sprite = SPRITE_POINTER;
	}
	else if ( m_phase == WM_PHASE_BYE )
	{
		sprite = SPRITE_POINTER;
	}
	else
	{
		if ( !MouseOnButton(pos) )
		{
			sprite = SPRITE_ARROW;
		}
	}

	if ( m_bFillMouse &&  // bidon présent ?
		 pos.x >= POSDRAWX && pos.x <= POSDRAWX+DIMDRAWX &&
		 pos.y >= POSDRAWY && pos.y <= POSDRAWY+DIMDRAWY )
	{
		sprite = SPRITE_FILL;
	}
	if ( m_bWaitMouse )  // sablier présent ?
	{
		sprite = SPRITE_WAIT;
	}
	if ( m_bHideMouse )  // souris cachée ?
	{
		sprite = SPRITE_EMPTY;
	}

	return sprite;
}

// Gère le lutin de la souris.

void CEvent::MouseSprite(POINT pos)
{
	m_mouseSprite = MousePosToSprite(pos);

	m_pPixmap->SetMousePosSprite(pos, m_mouseSprite, m_bDemoPlay);
	ChangeSprite(m_mouseSprite);
}

// Met ou enlève le sablier de la souris.

void CEvent::WaitMouse(BOOL bWait)
{
	m_bWaitMouse = bWait;

	if ( bWait )
	{
		m_mouseSprite = SPRITE_WAIT;
	}
	else
	{
		m_mouseSprite = MousePosToSprite(GetMousePos());
	}
	m_pPixmap->SetMouseSprite(m_mouseSprite, m_bDemoPlay);
	ChangeSprite(m_mouseSprite);
}

// Cache ou montre la souris.

void CEvent::HideMouse(BOOL bHide)
{
	m_bHideMouse = bHide;

	if ( bHide )
	{
		m_mouseSprite = SPRITE_EMPTY;
	}
	else
	{
		m_mouseSprite = MousePosToSprite(GetMousePos());
	}
	m_pPixmap->SetMouseSprite(m_mouseSprite, m_bDemoPlay);
	ChangeSprite(m_mouseSprite);
}

// Traite les événements pour tous les boutons.

BOOL CEvent::EventButtons(UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT		pos, test;
	int			i, lg, oldx, sound, res;

	pos = ConvLongToPos(lParam);

	// Cherche le tool tips à utiliser pour la souris.
	m_textToolTips[0] = 0;
	oldx = m_posToolTips.x;
	m_posToolTips.x = -1;
	if ( m_phase == WM_PHASE_PLAY )
	{
		for ( i=0 ; i<2 ; i++ )
		{
			if ( !m_jauges[i].GetHide() )
			{
				test = m_jauges[i].GetPos();
				if ( pos.x >= test.x           &&
					 pos.x <= test.x+DIMJAUGEX &&
					 pos.y >= test.y           &&
					 pos.y <= test.y+DIMJAUGEY )
				{
					LoadString(TX_JAUGE1+i, m_textToolTips, 50);
					lg = GetTextWidth(m_textToolTips);
					test.x += (DIMJAUGEX-lg)/2;
					test.y += 4;
					m_posToolTips = test;
					break;
				}
			}
		}
		if ( oldx != m_posToolTips.x )
		{
			for ( i=0 ; i<2 ; i++ )
			{
				m_jauges[i].SetRedraw();
			}
		}
	}
	else
	{
		i = 0;
		while ( table[m_index].buttons[i].message != 0 )
		{
			res = m_buttons[i].GetToolTips(pos);
			if ( res != -1 )
			{
				LoadString(res, m_textToolTips, 50);
				lg = GetTextWidth(m_textToolTips);
				pos.x += 10;
				pos.y += 20;
				if ( pos.x > LXIMAGE-lg )  pos.x = LXIMAGE-lg;
				if ( pos.y > LYIMAGE-14 )  pos.y = LYIMAGE-14;
				m_posToolTips = pos;
				break;
			}
			i ++;
		}
	}

	if ( m_phase == WM_PHASE_PLAY )
	{
		m_bHiliInfoButton = FALSE;
		if ( pos.x > m_posInfoButton.x+6            &&
			 pos.x < m_posInfoButton.x+DIMBUTTONX-6 &&
			 pos.y > m_posInfoButton.y+8            &&
			 pos.y < m_posInfoButton.y+DIMBUTTONY-8 )
		{
			m_bHiliInfoButton = TRUE;

			if ( message == WM_LBUTTONDOWN ||
				 message == WM_RBUTTONDOWN )
			{
				if ( m_pDecor->GetInfoMode() )  sound = SOUND_CLOSE;
				else                            sound = SOUND_OPEN;
				m_pSound->PlayImage(sound, pos);
			}
			if ( message == WM_LBUTTONUP ||
				 message == WM_RBUTTONUP )
			{
				// Montre ou cache les infos tout en haut.
				m_pDecor->SetInfoMode(!m_pDecor->GetInfoMode());
			}
		}

		m_bHiliHelpButton = FALSE;
		if ( m_posHelpButton.x != -1              &&
			 pos.x > m_posHelpButton.x            &&
			 pos.x < m_posHelpButton.x+DIMBUTTONX &&
			 pos.y > m_posHelpButton.y            &&
			 pos.y < m_posHelpButton.y+DIMBUTTONY )
		{
			m_bHiliHelpButton = TRUE;

			if ( message == WM_LBUTTONDOWN ||
				 message == WM_RBUTTONDOWN )
			{
				m_pSound->PlayImage(SOUND_CLICK, pos);
			}
			if ( message == WM_LBUTTONUP ||
				 message == WM_RBUTTONUP )
			{
				// Inverse le mode aide dans les infos.
				m_bInfoHelp = !m_bInfoHelp;

				if ( m_bInfoHelp )
				{
					ReadLibelle(GetWorld(), FALSE, TRUE);
				}
				else
				{
					ReadLibelle(GetWorld()+2, FALSE, FALSE);
				}
			}
		}
	}

	if ( m_phase == WM_PHASE_BUILD )
	{
		if ( message == WM_LBUTTONDOWN ||
			 message == WM_RBUTTONDOWN )
		{
			m_pDecor->HideTooltips(TRUE);  // plus de tooltips pour décor
		}
		if ( message == WM_LBUTTONUP ||
			 message == WM_RBUTTONUP )
		{
			m_pDecor->HideTooltips(FALSE);
		}
	}


	i = 0;
	while ( table[m_index].buttons[i].message != 0 )
	{
		if ( m_buttons[i].TreatEvent(message, wParam, lParam) )  return TRUE;
		i ++;
	}

	if ( m_phase == WM_PHASE_PLAY )
	{
		if ( m_menu.TreatEvent(message, wParam, lParam) )  return TRUE;
	}

	return FALSE;
}


// Indique si la souris est sur un bouton.

BOOL CEvent::MouseOnButton(POINT pos)
{
	int		i;

	i = 0;
	while ( table[m_index].buttons[i].message != 0 )
	{
		if ( m_buttons[i].MouseOnButton(pos) )  return TRUE;
		i ++;
	}

	return FALSE;
}


// Retourne l'index dans table pour une phase donnée.

int CEvent::SearchPhase(UINT phase)
{
	int		i=0;

	while ( table[i].phase != 0 )
	{
		if ( table[i].phase == phase )  return i;
		i ++;
	}

	return -1;
}

// Donne le numéro du monde.

int CEvent::GetWorld()
{
	if ( m_bPrivate )  return m_private;
	if ( m_bSchool  )  return m_exercice;
	else               return m_mission;
}

// Donne le numéro physique du monde.

int CEvent::GetPhysicalWorld()
{
	if ( m_bPrivate )  return m_private+200;
	if ( m_bSchool  )  return m_exercice;
	else               return m_mission+100;
}

int CEvent::GetImageWorld()
{
	if ( m_bPrivate )  return 2;
	if ( m_bSchool  )  return 0;
	else               return 1;
}

// Indique si l'aide est disponible.

BOOL CEvent::IsHelpHide()
{
	BOOL	bHide = TRUE;

#if !_DEMO
	if ( m_bHelp || m_pDecor->GetTotalTime() > DEF_TIME_HELP )
	{
		bHide = FALSE;
	}
	if ( m_bSchool || m_bPrivate )
	{
		bHide = TRUE;  // pas d'aide pour les exercices
	}
#endif
	return bHide;
}

// Change de phase.

BOOL CEvent::ChangePhase(UINT phase)
{
	int		index, world, time, total, music, i, max;
	POINT	totalDim, iconDim;
	char	filename[MAX_PATH];
	char*	pButtonExist;
	BOOL	bEnable, bHide;
	Term*	pTerm;

	if ( m_mouseType == MOUSETYPEGRA && m_bFullScreen )
	{
		ShowCursor(FALSE);  // cache la vilaine souris Windows
		m_bShowMouse = FALSE;
	}

	if ( phase != WM_PHASE_SETUPp &&
		 phase != WM_PHASE_WRITEp &&
		 phase != WM_PHASE_PLAY   )
	{
		m_pSound->StopMusic();
	}
	if ( phase == WM_PHASE_SETUPp && m_bPause )
	{
		m_pSound->StopMusic();
	}

	m_textToolTips[0] = 0;
	m_posToolTips.x = -1;
	m_bPause = FALSE;
	m_bCtrlDown = FALSE;
	m_bMouseDown = FALSE;
	m_debugPos.x = 0;
	m_debugPos.y = 0;

	m_pDecor->SetInfoMode(FALSE);
	m_bInfoHelp = FALSE;
	m_bHiliInfoButton = FALSE;
	m_bHiliHelpButton = FALSE;

	if ( phase == WM_PHASE_INTRO1 ||
		 phase == WM_PHASE_INTRO2 )
	{
		m_introTime = 0;
	}

	if ( phase == WM_PHASE_INIT )
	{
		m_demoTime = 0;
	}
	if ( phase == WM_PHASE_PLAY &&
		 !m_bDemoPlay &&
		 GetPhysicalWorld() >= 150 &&  // mission spéciale démo ?
		 GetPhysicalWorld() <  200 )
	{
		DemoRecStart();  // début enregistrement
	}
	if ( phase != WM_PHASE_PLAY )
	{
		DemoRecStop();  // stoppe l'enregistrement d'une démo
	}

	m_pDecor->UndoClose();  // libère le buffer undo

	index = SearchPhase(phase);
	if ( index < 0 )  return FALSE;

	HideMouse(FALSE);  // montre la souris
	WaitMouse(TRUE);   // met le sablier

	if ( m_bBuildModify )
	{
		m_pDecor->InitAfterBuild();
		m_bBuildModify = FALSE;
	}

	if ( m_phase == WM_PHASE_BUILD &&
		   phase == WM_PHASE_INFO  )  // quitte construction ?
	{
		m_pDecor->Write(GetPhysicalWorld(), FALSE,
						GetPhysicalWorld(), 0, 0);  // écrit le monde
	}

	m_phase = phase;  // change de phase
	m_index = index;

	strcpy(filename, table[m_index].backName);
	if ( strstr(filename, "%.3d") != NULL )  // "%.3d" dans le nom ?
	{
		sprintf(filename, table[m_index].backName, GetImageWorld());
	}
	if ( table[m_index].bCDrom )  // sur le CD-rom ?
	{
		AddCDPath(filename);
	}
	totalDim.x = LXIMAGE;
	totalDim.y = LYIMAGE;
	iconDim.x  = 0;
	iconDim.y  = 0;
	if ( !m_pPixmap->Cache(CHBACK, filename, totalDim, iconDim, FALSE) )
	{
		WaitMouse(FALSE);  // enlève le sablier
		m_tryInsertCount = 40;
		m_tryPhase = m_phase;
		return ChangePhase(WM_PHASE_INSERT);  // insérez le CD-Rom ...
	}

	if ( m_phase == WM_PHASE_READ   ||
		 m_phase == WM_PHASE_WRITE  ||
		 m_phase == WM_PHASE_WRITEp )
	{
		for ( i=0 ; i<10 ; i++ )
		{
			if ( m_pDecor->FileExist(i, TRUE, world, time, total) )
			{
				m_fileWorld[i] = world;
				m_fileTime[i]  = time;
			}
			else
			{
				m_fileWorld[i] = -1;
			}
		}
	}

	if ( m_phase == WM_PHASE_INFO     ||
		 m_phase == WM_PHASE_HISTORY0 ||
		 m_phase == WM_PHASE_HISTORY1 )
	{
		if ( !m_pDecor->Read(GetPhysicalWorld(), FALSE,
							 world, time, total) &&  // lit le monde
			 !m_bAccessBuild &&
			 !m_bPrivate )
		{
			m_tryInsertCount = 40;
			m_tryPhase = m_phase;
			return ChangePhase(WM_PHASE_INSERT);  // insérez le CD-Rom ...
		}
		m_pDecor->SetTime(0);
		m_pDecor->SetTotalTime(0);
		m_pDecor->SetInvincible(FALSE);
		m_pDecor->SetSuper(FALSE);
	}

	if ( m_phase == WM_PHASE_INFO     ||
		 m_phase == WM_PHASE_STOP     ||
		 m_phase == WM_PHASE_HELP     ||
		 m_phase == WM_PHASE_HISTORY0 ||
		 m_phase == WM_PHASE_HISTORY1 )
	{
		if ( m_bPrivate )
		{
			PrivateLibelle();
		}
		else if ( m_phase == WM_PHASE_INFO ||
				  m_phase == WM_PHASE_STOP )
		{
			if ( m_bSchool )
			{
				ReadLibelle(GetWorld(), m_bSchool, FALSE);
			}
			else
			{
				ReadLibelle(GetWorld()+2, m_bSchool, FALSE);
			}
		}
		else if ( m_phase == WM_PHASE_HELP )
		{
			ReadLibelle(GetWorld(), FALSE, TRUE);
		}
		else
		{
			if ( m_phase == WM_PHASE_HISTORY0 )  world = 0;
			else                                 world = 1;
			ReadLibelle(world, FALSE, FALSE);
		}
	}

	if ( m_phase == WM_PHASE_TESTCD )
	{
		if ( m_pDecor->Read(0, FALSE, world, time, total) )  // lit un monde
		{
			return ChangePhase(WM_PHASE_INIT);  // ok
		}
		else
		{
			m_tryInsertCount = 40;
			m_tryPhase = m_phase;
			return ChangePhase(WM_PHASE_INSERT);  // insérez le CD-Rom ...
		}
	}

	m_jauges[0].SetHide(TRUE);  // cache les jauges
	m_jauges[1].SetHide(TRUE);
	CreateButtons();  // crée les boutons selon la phase
	m_bMenu = FALSE;
	m_pDecor->HideTooltips(FALSE);
	m_menu.Delete();
	m_pDecor->BlupiSetArrow(0, FALSE);  // enlève toutes les flèches
	m_pDecor->ResetHili();  // enlève les mises en évidence

	if ( m_phase == WM_PHASE_PLAY )
	{
		m_pDecor->LoadImages();
		m_pDecor->SetBuild(FALSE);
		m_pDecor->EnableFog(TRUE);
		m_pDecor->NextPhase(0);  // refait la carte tout de suite
		m_pDecor->StatisticInit();
		m_pDecor->TerminatedInit();
		m_bChangeCheat = TRUE;  // affiche les cheat-codes
	}

	if ( m_phase == WM_PHASE_BUILD )
	{
		m_bBuildModify = TRUE;
		SetState(WM_DECOR1, 1);
		SetMenu(WM_DECOR1, 0);  // herbe
		SetMenu(WM_DECOR2, 2);  // arbre
		SetMenu(WM_DECOR3, 1);  // maison
		SetMenu(WM_DECOR4, 2);  // blupi fort
		SetMenu(WM_DECOR5, 1);  // feu
		m_pDecor->LoadImages();
		m_pDecor->SetBuild(TRUE);
		m_pDecor->EnableFog(FALSE);
		m_pDecor->BlupiDeselect();
		m_pDecor->NextPhase(0);  // refait la carte tout de suite
	}

	if ( m_phase == WM_PHASE_INFO )
	{
		bEnable = TRUE;
		if ( GetWorld() == 0 )
		{
			bEnable = FALSE;
		}
		SetEnable(WM_PREV, bEnable);

		bEnable = TRUE;
		if ( m_bAllMissions )  max = 99;
		else                   max = m_maxMission;
		if ( !m_bSchool &&
			 GetWorld() >= max )
		{
			bEnable = FALSE;
		}
		if  ( !m_pDecor->FileExist(GetPhysicalWorld()+1, FALSE,
								   world, time, total) )
		{
			bEnable = FALSE;
		}
#if !_DEMO
		if ( m_bAccessBuild ||
			 m_pDecor->GetTotalTime() > DEF_TIME_HELP*6 )
		{
			bEnable = TRUE;
		}
#endif
		if ( GetWorld() >= 99 )
		{
			bEnable = FALSE;
		}
#if _DEMO
		if ( GetWorld() >= 4-1 )
		{
			bEnable = FALSE;
		}
#endif
		if ( m_bPrivate )
		{
			bEnable = GetWorld()<20-1;
		}
		SetEnable(WM_NEXT, bEnable);

		bHide = TRUE;
		if ( m_bAccessBuild || m_bPrivate )
		{
			bHide = FALSE;
		}
		SetHide(WM_PHASE_BUILD, bHide);

#if !_DEMO
		if ( m_bSchool )
		{
			SetHide(WM_PHASE_SKILL1, TRUE);
			SetHide(WM_PHASE_SKILL2, TRUE);
		}
		else
		{
			SetState(WM_PHASE_SKILL1, m_pDecor->GetSkill()==0?1:0);
			SetState(WM_PHASE_SKILL2, m_pDecor->GetSkill()==1?1:0);
		}
#endif
	}

	if ( m_phase == WM_PHASE_STOP )
	{
		SetHide(WM_PHASE_HELP, IsHelpHide());
	}

	if ( m_phase == WM_PHASE_READ )
	{
		for ( i=0 ; i<10 ; i++ )
		{
			if ( m_fileWorld[i] == -1 )
			{
				SetEnable(WM_READ0+i, FALSE);
			}
		}
	}

	if ( m_phase == WM_PHASE_BUTTON )
	{
		pButtonExist = m_pDecor->GetButtonExist();

		for ( i=0 ; i<MAXBUTTON ; i++ )
		{
			SetState(WM_BUTTON0+i, pButtonExist[i]);
		}
	}

	if ( m_phase == WM_PHASE_TERM )
	{
		pTerm = m_pDecor->GetTerminated();

		SetState(WM_BUTTON1,  pTerm->bHachBlupi?1:0);
		SetState(WM_BUTTON2,  pTerm->bHachPlanche?1:0);
		SetState(WM_BUTTON3,  pTerm->bStopFire?1:0);
		SetState(WM_BUTTON8,  pTerm->bHomeBlupi?1:0);
		SetState(WM_BUTTON9,  pTerm->bKillRobots?1:0);
		SetState(WM_BUTTON10, pTerm->bHachTomate?1:0);
		SetState(WM_BUTTON11, pTerm->bHachMetal?1:0);
		SetState(WM_BUTTON12, pTerm->bHachRobot?1:0);
	}

	if ( m_phase == WM_PHASE_MUSIC )
	{
		music = m_pDecor->GetMusic();

		for ( i=0 ; i<10 ; i++ )
		{
			SetState(WM_BUTTON1+i, music==i?1:0);
		}
	}

	if ( m_phase == WM_PHASE_REGION )
	{
		music = m_pDecor->GetRegion();

		for ( i=0 ; i<4 ; i++ )
		{
			SetState(WM_BUTTON1+i, music==i?1:0);
		}
	}

	if ( m_phase == WM_PHASE_PLAY  ||
		 m_phase == WM_PHASE_MUSIC )
	{
		if ( m_pSound->IsPlayingMusic() )
		{
			m_pSound->AdaptVolumeMusic();  // adapte le volume
		}
		else
		{
			music = m_pDecor->GetMusic();
			if ( music > 0 )
			{
				sprintf(filename, "sound\\music%.3d.blp", music-1);
				m_pSound->PlayMusic(m_hWnd, filename);
			}
		}
	}

	if ( phase == WM_PHASE_H0MOVIE )
	{
		strcpy(m_movieToStart, "movie\\history0.avi");
		AddCDPath(m_movieToStart);
		m_phaseAfterMovie = WM_PHASE_HISTORY0;
	}

	if ( phase == WM_PHASE_H1MOVIE )
	{
		strcpy(m_movieToStart, "movie\\history1.avi");
		AddCDPath(m_movieToStart);
		m_phaseAfterMovie = WM_PHASE_HISTORY1;
	}

	if ( phase == WM_PHASE_H2MOVIE )
	{
		strcpy(m_movieToStart, "movie\\history2.avi");
		AddCDPath(m_movieToStart);
		m_phaseAfterMovie = WM_PHASE_INFO;
	}

	if ( phase == WM_PHASE_PLAYMOVIE )
	{
		sprintf(m_movieToStart, "movie\\play%.3d.avi", GetPhysicalWorld());
		AddCDPath(m_movieToStart);
		m_phaseAfterMovie = WM_PHASE_PLAY;
	}

	if ( phase == WM_PHASE_WINMOVIE )
	{
		sprintf(m_movieToStart, "movie\\win%.3d.avi", GetPhysicalWorld());
		AddCDPath(m_movieToStart);
		m_phaseAfterMovie = WM_PHASE_WIN;

		if ( !m_bPrivate &&
			  m_pDecor->FileExist(GetPhysicalWorld(),
								  FALSE, world, time, total) &&
			 !m_pDecor->FileExist(GetPhysicalWorld()+1,
								  FALSE, world, time, total) )
		{
			m_phaseAfterMovie = WM_PHASE_LASTWIN;
		}
	}

	WaitMouse(FALSE);  // enlève le sablier
	return TRUE;
}

// Retourne la phase en cours.

UINT CEvent::GetPhase()
{
	return m_phase;
}

// Essaye de lire le CD-Rom.

void CEvent::TryInsert()
{
	if ( m_tryInsertCount == 0 )
	{
		ChangePhase(m_tryPhase);
	}
	else
	{
		m_tryInsertCount --;
	}
}

// Fait démarrer un film si nécessaire.

void CEvent::MovieToStart()
{
	if ( m_movieToStart[0] != 0 )  // y a-t-il un film à démarrer ?
	{
		HideMouse(TRUE);  // cache la souris

		if ( StartMovie(m_movieToStart) )
		{
			m_phase = m_phaseAfterMovie;  // prochaine phase normale
		}
		else
		{
			ChangePhase(m_phaseAfterMovie);
		}

		m_movieToStart[0] = 0;
	}
}


// Décale le décor.

void CEvent::DecorShift(int dx, int dy)
{
	POINT		coin;

	if ( m_phase != WM_PHASE_PLAY  &&
		 m_phase != WM_PHASE_BUILD )  return;

	coin = m_pDecor->GetCoin();

	coin.x += dx;
	coin.y += dy;

	m_pDecor->SetCoin(coin);
//?	m_pDecor->NextPhase(0);  // faudra refaire la carte tout de suite
}

// Décale le décor lorsque la souris touche un bord.

void CEvent::DecorAutoShift(POINT pos)
{
	int			max;
	POINT		offset;

	m_bShift = FALSE;

	if ( !m_bFullScreen ||
		 m_bDemoRec     ||
		 m_bDemoPlay    ||
		 m_scrollSpeed == 0 )  return;

	max = 4-m_scrollSpeed;  // max <- 3..1

//?	if ( m_bMousePress &&
//?		 (m_phase == WM_PHASE_PLAY  ||
//?		  m_phase == WM_PHASE_BUILD) )
	if ( m_phase == WM_PHASE_PLAY  ||
		 m_phase == WM_PHASE_BUILD )
	{
		if ( m_shiftPhase == 0 )  // début du shift ?
		{
			m_shiftOffset.x = 0;
			m_shiftOffset.y = 0;
			m_shiftVector.x = 0;
			m_shiftVector.y = 0;

			if ( m_mouseSprite == SPRITE_ARROWL )
			{
				m_shiftOffset.x = +2;
				m_shiftOffset.y = 0;
				m_shiftVector.x = -1;
				m_shiftVector.y = +1;
			}

			if ( m_mouseSprite == SPRITE_ARROWR )
			{
				m_shiftOffset.x = -2;
				m_shiftOffset.y = 0;
				m_shiftVector.x = +1;
				m_shiftVector.y = -1;
			}

			if ( m_mouseSprite == SPRITE_ARROWU )
			{
				m_shiftOffset.x = 0;
				m_shiftOffset.y = +2;
				m_shiftVector.x = -1;
				m_shiftVector.y = -1;
			}

			if ( m_mouseSprite == SPRITE_ARROWD )
			{
				m_shiftOffset.x = 0;
				m_shiftOffset.y = -2;
				m_shiftVector.x = +1;
				m_shiftVector.y = +1;
			}

			if ( m_mouseSprite == SPRITE_ARROWUL )
			{
				m_shiftOffset.x = +1;
				m_shiftOffset.y = +1;
				m_shiftVector.x = -1;
				m_shiftVector.y = 0;
			}

			if ( m_mouseSprite == SPRITE_ARROWUR )
			{
				m_shiftOffset.x = -1;
				m_shiftOffset.y = +1;
				m_shiftVector.x = 0;
				m_shiftVector.y = -1;
			}

			if ( m_mouseSprite == SPRITE_ARROWDL )
			{
				m_shiftOffset.x = +1;
				m_shiftOffset.y = -1;
				m_shiftVector.x = 0;
				m_shiftVector.y = +1;
			}

			if ( m_mouseSprite == SPRITE_ARROWDR )
			{
				m_shiftOffset.x = -1;
				m_shiftOffset.y = -1;
				m_shiftVector.x = +1;
				m_shiftVector.y = 0;
			}

			if ( m_shiftVector.x != 0 ||
				 m_shiftVector.y != 0 )
			{
				m_shiftPhase = max;
			}
		}

		if ( m_shiftPhase > 0 )
		{
			m_bShift = TRUE;
			m_shiftPhase --;

			offset.x = m_shiftOffset.x*(max-m_shiftPhase)*(DIMCELX/2/max);
			offset.y = m_shiftOffset.y*(max-m_shiftPhase)*(DIMCELY/2/max);
			m_pDecor->SetShiftOffset(offset);

			if ( m_shiftPhase == 0 )  // dernière phase ?
			{
				offset.x = 0;
				offset.y = 0;
				m_pDecor->SetShiftOffset(offset);
				DecorShift(m_shiftVector.x, m_shiftVector.y);
			}
		}
	}
}

// Indique su un shift est en cours.

BOOL CEvent::IsShift()
{
	return m_bShift;
}


// Modifie le décor lorsque le bouton de la souris est pressé.

BOOL CEvent::PlayDown(POINT pos, int fwKeys)
{
	BOOL	bDecor = FALSE;
	BOOL	bMap   = FALSE;
	int		rank, button, h;
	POINT	cel;

	m_pDecor->BlupiSetArrow(0, FALSE);  // enlève toutes les flèches

	m_bMouseDown = FALSE;

	if ( m_bMenu )
	{
		m_menu.Message();
		m_bMenu = FALSE;
		m_pDecor->HideTooltips(FALSE);
		m_menu.Delete();
		return TRUE;
	}

	m_pDecor->StatisticDown(pos, fwKeys);

	if ( pos.x >= POSMAPX && pos.x <= POSMAPX+DIMMAPX &&
		 pos.y >= POSMAPY && pos.y <= POSMAPY+DIMMAPY )
	{
		bMap = TRUE;
	}
	h = m_pDecor->GetInfoHeight()+POSDRAWY;
	if ( pos.x >= POSDRAWX && pos.x <= POSDRAWX+DIMDRAWX &&
		 pos.y >= h        && pos.y <= h       +DIMDRAWY )
	{
		bDecor = TRUE;
	}

	if ( !bDecor && !bMap )  return FALSE;

	cel = m_pDecor->ConvPosToCel(pos, TRUE);
	if ( fwKeys&MK_RBUTTON )
	{
		if ( bMap )
		{
			button = BUTTON_GO;
		}
		else
		{
			button = m_pDecor->GetDefButton(cel);
		}
		m_pDecor->BlupiGoal(cel, button);
		return TRUE;
	}

	if ( bMap )
	{
		m_pDecor->SetCoin(cel, TRUE);
		m_pDecor->NextPhase(0);  // faudra refaire la carte tout de suite
		return TRUE;
	}

	rank = m_pDecor->GetTargetBlupi(pos);
	if ( rank >= 0 &&
		 !m_pDecor->IsWorkBlupi(rank) )
	{
		m_bHili = TRUE;
		m_bMouseDown = TRUE;
		m_pDecor->BlupiHiliDown(pos, fwKeys&MK_SHIFT);
	}
	else
	{
		m_bHili = FALSE;
		m_bMouseDown = TRUE;
	}

	return TRUE;
}

// Modifie le décor lorsque la souris est déplacée.

BOOL CEvent::PlayMove(POINT pos, int fwKeys)
{
	if ( m_bMenu )
	{
		if ( !m_menu.IsExist() )
		{
			m_bMenu = FALSE;
			m_pDecor->HideTooltips(FALSE);
			m_menu.Delete();
		}
		return TRUE;
	}

	m_pDecor->StatisticMove(pos, fwKeys);

	if ( m_bMouseDown )  // bouton souris pressé ?
	{
		if ( m_bHili )
		{
			m_pDecor->BlupiHiliMove(pos, fwKeys&MK_SHIFT);
		}
		else
		{
			m_pDecor->CelHili(pos, 0);
		}
	}
	else
	{
		m_pDecor->CelHili(pos, 0);
	}

	return TRUE;
}

// Modifie le décor lorsque le bouton de la souris est relâché.

BOOL CEvent::PlayUp(POINT pos, int fwKeys)
{
	static int table_sound_boing[3] =
	{
		SOUND_BOING1,
		SOUND_BOING2,
		SOUND_BOING3,
	};

	m_pDecor->StatisticUp(pos, fwKeys);

	if ( m_bMouseDown )  // bouton souris pressé ?
	{
		if ( m_bHili )
		{
			m_pDecor->BlupiHiliUp(pos, fwKeys&MK_SHIFT);
		}
		else
		{
			m_pDecor->BlupiGetButtons(pos, m_menuNb, m_menuButtons,
									  m_menuErrors, m_menuPerso);
			if ( m_menuNb == 0 )
			{
				m_pDecor->BlupiSound(-1, table_sound_boing[Random(0,2)], pos);
			}
			else
			{
				m_menuCel = m_pDecor->ConvPosToCel(pos);
				m_menuPos = pos;
				m_menu.Create(m_hWnd, m_pPixmap, m_pSound,
							  pos, m_menuNb, m_menuButtons, m_menuErrors,
							  m_menuPerso);
				m_bMenu = TRUE;
				m_pDecor->HideTooltips(TRUE);  // plus de tooltips pour décor
			}
		}
	}

	m_bMouseDown = FALSE;

	return TRUE;
}


// Clic dans un bouton.
// Message = WM_BUTTON0..WM_BUTTON39

void CEvent::ChangeButtons(int message)
{
	int		button, state, volume, max;
	char*	pButtonExist;
	Term*	pTerm;

	if ( m_phase == WM_PHASE_PLAY )
	{
		button = m_menuButtons[message-WM_BUTTON0];
		m_pDecor->BlupiGoal(m_menuCel, button);
	}

	if ( m_phase == WM_PHASE_BUTTON )
	{
		pButtonExist = m_pDecor->GetButtonExist();

		state = GetState(message);
		if ( state == 0 )  state = 1;
		else               state = 0;
		SetState(message, state);  // pressé <-> relâché

		pButtonExist[message-WM_BUTTON0] = state;
		pButtonExist[BUTTON_DJEEP]   = TRUE;
		pButtonExist[BUTTON_DARMURE] = TRUE;
	}

	if ( m_phase == WM_PHASE_TERM )
	{
		pTerm = m_pDecor->GetTerminated();

		if ( message == WM_BUTTON1  ||
			 message == WM_BUTTON2  ||
			 message == WM_BUTTON3  ||
			 message == WM_BUTTON8  ||
			 message == WM_BUTTON9  ||
			 message == WM_BUTTON10 ||
			 message == WM_BUTTON11 ||
			 message == WM_BUTTON12 )
		{
			state = GetState(message);
			if ( state == 0 )  state = 1;
			else               state = 0;
			SetState(message, state);  // pressé <-> relâché

			if ( message == WM_BUTTON1  )  pTerm->bHachBlupi   = state;
			if ( message == WM_BUTTON2  )  pTerm->bHachPlanche = state;
			if ( message == WM_BUTTON3  )  pTerm->bStopFire    = state;
			if ( message == WM_BUTTON8  )  pTerm->bHomeBlupi   = state;
			if ( message == WM_BUTTON9  )  pTerm->bKillRobots  = state;
			if ( message == WM_BUTTON10 )  pTerm->bHachTomate  = state;
			if ( message == WM_BUTTON11 )  pTerm->bHachMetal   = state;
			if ( message == WM_BUTTON12 )  pTerm->bHachRobot   = state;
		}

		if ( message == WM_BUTTON4 )
		{
			if ( pTerm->nbMinBlupi > 1 )  pTerm->nbMinBlupi --;
		}
		if ( message == WM_BUTTON5 )
		{
			if ( pTerm->nbMinBlupi < 100 )  pTerm->nbMinBlupi ++;
		}

		if ( message == WM_BUTTON6 )
		{
			if ( pTerm->nbMaxBlupi > 1 )  pTerm->nbMaxBlupi --;
		}
		if ( message == WM_BUTTON7 )
		{
			if ( pTerm->nbMaxBlupi < 100 )  pTerm->nbMaxBlupi ++;
		}
	}

	if ( m_phase == WM_PHASE_MUSIC )
	{
		m_pDecor->SetMusic(message-WM_BUTTON1);
		ChangePhase(m_phase);
	}

	if ( m_phase == WM_PHASE_REGION )
	{
		m_pDecor->SetRegion(message-WM_BUTTON1);
		ChangePhase(WM_PHASE_BUILD);
	}

	if ( m_phase == WM_PHASE_SETUP  ||
		 m_phase == WM_PHASE_SETUPp )
	{
		if ( message == WM_BUTTON1 )
		{
			if ( m_speed > 1 )
			{
				m_speed = m_speed>>1;
			}
		}
		if ( message == WM_BUTTON2 )
		{
			if ( m_bSpeed )  max = 8;
			else             max = 2;
			if ( m_speed < max )
			{
				m_speed = m_speed<<1;
			}
		}

		if ( message == WM_BUTTON3 )
		{
			volume = m_pSound->GetAudioVolume();
			if ( volume > 0 )
			{
				m_pSound->SetAudioVolume(volume-1);
			}
		}
		if ( message == WM_BUTTON4 )
		{
			volume = m_pSound->GetAudioVolume();
			if ( volume < MAXVOLUME )
			{
				m_pSound->SetAudioVolume(volume+1);
			}
		}

		if ( message == WM_BUTTON5 )
		{
			volume = m_pSound->GetMidiVolume();
			if ( volume > 0 )
			{
				m_pSound->SetMidiVolume(volume-1);
				m_pSound->SuspendMusic();
			}
		}
		if ( message == WM_BUTTON6 )
		{
			volume = m_pSound->GetMidiVolume();
			if ( volume < MAXVOLUME )
			{
				m_pSound->SetMidiVolume(volume+1);
				m_pSound->SuspendMusic();
			}
		}

		if ( message == WM_BUTTON7 )
		{
			m_bMovie = FALSE;
		}
		if ( message == WM_BUTTON8 )
		{
			m_bMovie = TRUE;
		}

		if ( message == WM_BUTTON9 )
		{
			if ( m_scrollSpeed > 0 )
			{
				m_scrollSpeed --;
			}
		}
		if ( message == WM_BUTTON10 )
		{
			if ( m_scrollSpeed < 3 )
			{
				m_scrollSpeed ++;
			}
		}
	}
}



// Met un sol si nécessaire sous un objet.

void CEvent::BuildFloor(POINT cel, int insIcon)
{
	int		iFloor, channel, icon;

	if ( insIcon == -1 )  return;  // supprime ?

	if ( insIcon <    6 ||  // petite plante ?
		 insIcon == 117 )   // bateau ?
	{
		return;
	}

	iFloor = 1;  // herbe

	if ( insIcon ==  28 ||  // laboratoire ?
		 insIcon ==  61 ||  // cabane de jardin ?
		 insIcon == 113 ||  // maison ?
		 insIcon == 120 )   // usine ?
	{
		iFloor = 16;  // sol brun foncé
	}

	if ( insIcon == 122 )  // mine de fer ?
	{
		iFloor = 71;  // sol avec minerai
	}

	if ( insIcon ==  99 ||  // station de recharge ?
		 insIcon == 100 ||  // usine ennemie ?
		 insIcon == 102 ||  // usine ennemie ?
		 insIcon == 104 ||  // usine ennemie ?
		 (insIcon >= 106 && insIcon <= 112) ||  // barrière ?
		 insIcon == 115 ||  // usine ennemie ?
		 insIcon ==  17 ||  // usine ennemie ?
		 insIcon ==  12 )   // fusée ?
	{
		iFloor = 67;  // sol bleu ennemi
	}

	m_pDecor->GetFloor(cel, channel, icon);
	if ( (channel == CHFLOOR &&
		  icon >= 2 && icon <= 14) ||  // eau ou rive ?
		 iFloor != 1 )
	{
		m_pDecor->PutFloor(cel, CHFLOOR, iFloor);  // met un sol
		m_pDecor->ArrangeFloor(cel);
	}
}

// Enlève si nécessaire un objet sur l'eau.

void CEvent::BuildWater(POINT cel, int insIcon)
{
	int		channel, icon;

	if ( insIcon != 14 )  return;  // rien à faire si pas eau

	m_pDecor->GetObject(cel, channel, icon);
	if ( channel == CHOBJECT &&
		 icon >=   6 &&  // objet (pas petite plante) ?
		 icon != 117 )   // pas bateau ?
	{
		m_pDecor->PutObject(cel, -1, -1);  // enlève l'objet
		m_pDecor->ArrangeObject(cel);
	}
}

// Cette table donne les objets à construire en fonction
// du choix dans le menu.

static int tableFloor[] =
{
	1,49,50,51,0,0,0,0,0,0,				// 0 herbe
	20,66,79,0,0,0,0,0,0,0,				// 1 foncé
	33,46,47,48,71,0,0,0,0,0,			// 2 terre
	14,0,0,0,0,0,0,0,0,0,				// 3 mer
	15,16,17,18,19,65,67,0,0,0,			// 4 dalles
	52,80,0,0,0,0,0,0,0,0,				// 5 couveuse
};

static int tableObject[] =
{
	-1,0,0,0,0,0,0,0,0,0,				// 0 détruit
	0,4,1,2,3,5,0,0,0,0,				// 1 plantes
	6,7,8,9,10,11,0,0,0,0,				// 2 arbres
	81,83,94,0,0,0,0,0,0,0,				// 5 fleurs
};

static int tableHome[] =
{
	-1,0,0,0,0,0,0,0,0,0,				//  0 détruit
	113,61,28,120,0,0,0,0,0,0,			//  1 maison
	27,0,0,0,0,0,0,0,0,0,				//  2 tour de protection
	122,0,0,0,0,0,0,0,0,0,				//  3 mine de fer
	99,100,102,104,115,17,12,0,0,0,		//  4 ennemi
	112,0,0,0,0,0,0,0,0,0,				//  5 barrière
	26,71,0,0,0,0,0,0,0,0,				//  6 palissade
	37,38,39,40,41,42,43,0,0,0,			//  7 rochers
	36,44,60,63,80,123,14,0,0,0,		//  8 matières
	85,125,93,92,0,0,0,0,0,0,			//  9 pièges
	117,118,16,0,0,0,0,0,0,0,			// 10 véhicules
};

// Modifie le décor lorsque le bouton de la souris est pressé.

BOOL CEvent::BuildDown(POINT pos, int fwKeys, BOOL bMix)
{
	POINT		cel;
	int			menu, channel, icon;

	if ( bMix && m_pDecor->MapMove(pos) )  return TRUE;

	if ( pos.x < POSDRAWX || pos.x > POSDRAWX+DIMDRAWX ||
		 pos.y < POSDRAWY || pos.y > POSDRAWY+DIMDRAWY )  return FALSE;

	if ( bMix )
	{
		m_pDecor->UndoCopy();  // copie le décor pour undo év.
	}

	if ( GetState(WM_DECOR1) == 1 )  // pose d'un sol
	{
		cel = m_pDecor->ConvPosToCel2(pos);
		menu = GetMenu(WM_DECOR1);

		if ( !m_pDecor->GetFloor(cel, channel, icon) )  return FALSE;
		
		if ( bMix && tableFloor[menu*10+m_lastFloor[menu]] == icon )
		{
			m_lastFloor[menu] ++;
			if ( tableFloor[menu*10+m_lastFloor[menu]] == 0 )
			{
				m_lastFloor[menu] = 0;
			}
		}

		if ( fwKeys & MK_CONTROL )  // touche Ctrl enfoncée ?
		{
			WaitMouse(TRUE);
			m_pDecor->ArrangeFill(cel, CHFLOOR, tableFloor[menu*10+m_lastFloor[menu]], TRUE);
			WaitMouse(FALSE);
		}
		else
		{
			icon = tableFloor[menu*10+m_lastFloor[menu]];
			if ( menu >= 1 )  // met un sol ?
			{
				BuildWater(cel, icon);  // enlève les objets
			}
			m_pDecor->PutFloor(cel, CHFLOOR, icon);
			m_pDecor->ArrangeFloor(cel);
		}
	}

	if ( GetState(WM_DECOR2) == 1 )  // pose d'un objet
	{
		cel = m_pDecor->ConvPosToCel2(pos);
		menu = GetMenu(WM_DECOR2);

		if ( !m_pDecor->GetObject(cel, channel, icon) )  return FALSE;
		
		if ( bMix && tableObject[menu*10+m_lastObject[menu]] == icon )
		{
			m_lastObject[menu] ++;
			if ( tableObject[menu*10+m_lastObject[menu]] == 0 )
			{
				m_lastObject[menu] = 0;
			}
		}

		if ( fwKeys & MK_CONTROL )  // touche Ctrl enfoncée ?
		{
			WaitMouse(TRUE);
			m_pDecor->ArrangeFill(cel, CHOBJECT, tableObject[menu*10+m_lastObject[menu]], FALSE);
			WaitMouse(FALSE);
		}
		else
		{
			icon = tableObject[menu*10+m_lastObject[menu]];
			BuildFloor(cel, icon);  // met un sol si nécessaire
			m_pDecor->PutObject(cel, CHOBJECT, icon);
			m_pDecor->ArrangeObject(cel);
		}
	}

	if ( GetState(WM_DECOR3) == 1 )  // pose d'un batiment
	{
		cel = m_pDecor->ConvPosToCel2(pos);
		menu = GetMenu(WM_DECOR3);

		if ( !m_pDecor->GetObject(cel, channel, icon) )  return FALSE;
		
		if ( bMix && tableHome[menu*10+m_lastHome[menu]] == icon )
		{
			m_lastHome[menu] ++;
			if ( tableHome[menu*10+m_lastHome[menu]] == 0 )
			{
				m_lastHome[menu] = 0;
			}
		}

		if ( fwKeys & MK_CONTROL )  // touche Ctrl enfoncée ?
		{
			WaitMouse(TRUE);
			m_pDecor->ArrangeFill(cel, CHOBJECT, tableHome[menu*10+m_lastHome[menu]], FALSE);
			WaitMouse(FALSE);
		}
		else
		{
			icon = tableHome[menu*10+m_lastHome[menu]];
			BuildFloor(cel, icon);  // met un sol si nécessaire
			m_pDecor->PutObject(cel, CHOBJECT, icon);
			m_pDecor->ArrangeObject(cel);
		}
	}

	if ( GetState(WM_DECOR4) == 1 )  // pose d'un blupi
	{
		cel = m_pDecor->ConvPosToCel(pos);
		menu = GetMenu(WM_DECOR4);

		if ( menu == 0 )  // supprime ?
		{
			m_pDecor->BlupiDelete(cel);
		}
		if ( menu == 1 )  // ajoute blupi-fatigué ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 0, MAXENERGY/4);
		}
		if ( menu == 2 )  // ajoute blupi-énergique ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 0, MAXENERGY);
		}
		if ( menu == 3 )  // ajoute assistant ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 8, MAXENERGY);
		}
		if ( menu == 4 )  // ajoute araignée ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 1, MAXENERGY);
		}
		if ( menu == 5 )  // ajoute virus ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 2, MAXENERGY);
		}
		if ( menu == 6 )  // ajoute tracks ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 3, MAXENERGY);
		}
		if ( menu == 7 )  // ajoute bombe ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 5, MAXENERGY);
		}
		if ( menu == 8 )  // ajoute électro ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 7, MAXENERGY);
		}
		if ( menu == 9 )  // ajoute robot ?
		{
			m_pDecor->BlupiCreate(cel, ACTION_STOP, DIRECT_S, 4, MAXENERGY);
		}
	}

	if ( GetState(WM_DECOR5) == 1 )  // pose d'une cata
	{
		cel = m_pDecor->ConvPosToCel2(pos);
		menu = GetMenu(WM_DECOR5);

		if ( menu == 0 )  // supprime ?
		{
			m_pDecor->SetFire(cel, FALSE);
		}
		if ( menu == 1 )  // ajoute ?
		{
			m_pDecor->SetFire(cel, TRUE);
		}
	}

	m_pDecor->ArrangeBlupi();  // supprime les blupi bloqués

	return TRUE;
}

// Modifie le décor lorsque la souris est déplacée.

BOOL CEvent::BuildMove(POINT pos, int fwKeys)
{
	if ( fwKeys & MK_LBUTTON )  // bouton souris pressé ?
	{
		BuildDown(pos, fwKeys, FALSE);
	}

	if ( GetState(WM_DECOR4) == 1 )  // pose d'un blupi
	{
		m_pDecor->CelHili(pos, 1);
	}
	else
	{
		m_pDecor->CelHili(pos, 2);
	}

	return TRUE;
}

// Modifie le décor lorsque le bouton de la souris est relâché.

BOOL CEvent::BuildUp(POINT pos, int fwKeys)
{
	return TRUE;
}


// Démarre un film non interractif.

BOOL CEvent::StartMovie(char *pFilename)
{
	RECT		rect;
	char		filename[MAX_PATH];

	if ( !m_pMovie->GetEnable() )  return FALSE;
	if ( !m_bMovie )  return FALSE;

	if ( !m_pMovie->IsExist(pFilename) )  return FALSE;

	rect.left   = 1;  // mystère: plante avec 0,0,LXIMAGE,LYIMAGE !!!
	rect.top    = 1;
	rect.right  = LXIMAGE-2;
	rect.bottom = LYIMAGE-2;

	m_pSound->StopMusic();
	m_pPixmap->SavePalette();

	strcpy(filename, pFilename);
	strcpy(filename+strlen(filename)-4, ".blp");  // remplace .avi par .blp
	m_pSound->Cache(SOUND_MOVIE, filename);

	if ( !m_pMovie->Play(m_hWnd, rect, pFilename) )  return FALSE;
	m_bRunMovie = TRUE;
	m_pSound->Play(SOUND_MOVIE, 0, 0);
	return TRUE;
}

// Stoppe un film non interractif.

void CEvent::StopMovie()
{
	m_pMovie->Stop(m_hWnd);
	m_pPixmap->RestorePalette();
	m_pPixmap->MouseInvalidate();
	m_pSound->Flush(SOUND_MOVIE);
//	m_pSound->RestartMusic();
	ChangePhase(m_phase);
	m_bRunMovie = FALSE;
}

// Indique s'il y a un film en cours.

BOOL CEvent::IsMovie()
{
	return m_bRunMovie;
}


// Lit une partie (user000.blp).

void CEvent::Read(int message)
{
	int		world, time, total;

	m_pDecor->Read(message-WM_READ0, TRUE, world, time, total);
	m_pDecor->SetTime(time);
	m_pDecor->SetTotalTime(total);

	if ( world >= 200 )
	{
		m_private  = world-200;
		m_bSchool  = FALSE;
		m_bPrivate = TRUE;
	}
	else if ( world >= 100 )
	{
		m_mission  = world-100;
		m_bSchool  = FALSE;
		m_bPrivate = FALSE;
	}
	else
	{
		m_exercice = world;
		m_bSchool  = TRUE;
		m_bPrivate = FALSE;
	}
}

// Ecrit une partie (user000.blp).

void CEvent::Write(int message)
{
	int		time, total;

	time  = m_pDecor->GetTime();
	total = m_pDecor->GetTotalTime();

	m_pDecor->Write(message-WM_WRITE0, TRUE,
					GetPhysicalWorld(), time, total);
}


// Initialise le libellé d'une mission privée.

void CEvent::PrivateLibelle()
{
	int			i, res, nb, h1, h2;
	Term		term;
	char		string[100];
	char		buffer[100];

	LoadString(TX_PRIVATE_OBJECTIF, m_libelle, 100);

	memcpy(&term, m_pDecor->GetTerminated(), sizeof(Term));
	nb = 0;
	for ( i=0 ; i<2 ; i++ )  // 2 objectifs au maximum !
	{
		res = 0;
		if ( term.bKillRobots )
		{
			term.bKillRobots = FALSE;
			res = TX_PRIVATE_KILLROBOTS;
		}
		else if ( term.bHachBlupi )
		{
			term.bHachBlupi = FALSE;
			res = TX_PRIVATE_HACHBLUPI;
		}
		else if ( term.bHachPlanche )
		{
			term.bHachPlanche = FALSE;
			res = TX_PRIVATE_HACHPLANCHE;
		}
		else if ( term.bHachTomate )
		{
			term.bHachTomate = FALSE;
			res = TX_PRIVATE_HACHTOMATE;
		}
		else if ( term.bHachMetal )
		{
			term.bHachMetal = FALSE;
			res = TX_PRIVATE_HACHMETAL;
		}
		else if ( term.bHachRobot )
		{
			term.bHachRobot = FALSE;
			res = TX_PRIVATE_HACHROBOT;
		}
		else if ( term.bHomeBlupi )
		{
			term.bHomeBlupi = FALSE;
			res = TX_PRIVATE_HOMEBLUPI;
		}
		else if ( term.bStopFire )
		{
			term.bStopFire = FALSE;
			res = TX_PRIVATE_STOPFIRE;
		}
		if ( res == 0 )  break;

		LoadString(res, string, 100);
		strcat(m_libelle, "\n1|\n");
		strcat(m_libelle, string);
		nb ++;
	}

	if ( nb == 0 || term.nbMaxBlupi > 1 )
	{
		LoadString(TX_PRIVATE_NBBLUPI, buffer, 100);
		sprintf(string, buffer, term.nbMaxBlupi);
		strcat(m_libelle, "\n1|\n");
		strcat(m_libelle, string);
	}

	h1 = GetTextHeight(m_libelle, FONTLITTLE, 1);
	h2 = GetTextHeight(m_libelle, FONTLITTLE, 2);
	if ( h2 > h1 )  h1 = h2;
	m_pDecor->SetInfoHeight(POSDRAWY+h1+10);
}

// Lit le libellé d'un monde.

BOOL CEvent::ReadLibelle(int world, BOOL bSchool, BOOL bHelp)
{
	FILE*		file    = NULL;
	char*		pBuffer = NULL;
	char*		pText;
	char*		pDest;
	char		indic;
	int			nb, h1, h2;

	if ( bSchool )  indic = '$';
	else            indic = '#';
	if ( bHelp )    indic = '@';

	pBuffer = (char*)malloc(sizeof(char)*50000);
	if ( pBuffer == NULL )  goto error;
	memset(pBuffer, 0, sizeof(char)*50000);

	file = fopen("data\\enigmes.blp", "rb");
	if ( file == NULL )  goto error;

	nb = fread(pBuffer, sizeof(char), 50000-1, file);
	pBuffer[nb] = 0;

	pText = pBuffer;
	while ( world >= 0 )
	{
		while ( *pText != 0 && *pText != indic )
		{
			pText++;
		}
		if ( *pText == indic )  pText++;
		world --;
	}
	while ( *pText != 0 && *pText != '\n' )
	{
		pText++;
	}
	if ( *pText == '\n' )  pText++;
	pDest = m_libelle;
	while ( *pText != 0 && *pText != indic &&
			*pText != '$' && *pText != '#' && *pText != '@' )
	{
		*pDest++ = *pText++;
	}
	*pDest = 0;

	h1 = GetTextHeight(m_libelle, FONTLITTLE, 1);
	h2 = GetTextHeight(m_libelle, FONTLITTLE, 2);
	if ( h2 > h1 )  h1 = h2;
	m_pDecor->SetInfoHeight(POSDRAWY+h1+10);

	free(pBuffer);
	fclose(file);
	return TRUE;

	error:
	if ( pBuffer != NULL )  free(pBuffer);
	if ( file    != NULL )  fclose(file);
	return FALSE;
}


// Sauve les informations sur disque.

BOOL CEvent::WriteInfo()
{
	char		filename[MAX_PATH];
	FILE*		file = NULL;
	DescInfo	info;
	int			nb;

	strcpy(filename, "data\\info.blp");
	AddUserPath(filename);

	file = fopen(filename, "wb");
	if ( file == NULL )  goto error;

	info.majRev       = 1;
	info.minRev       = 0;
	info.prive        = m_private;
	info.exercice     = m_exercice;
	info.mission      = m_mission;
	info.maxMission   = m_maxMission;
	info.speed        = m_speed;
	info.bMovie       = m_bMovie;
	info.scrollSpeed  = m_scrollSpeed;
	info.bAccessBuild = m_bAccessBuild;

	info.skill        = m_pDecor->GetSkill();

	info.audioVolume = m_pSound->GetAudioVolume();
	info.midiVolume  = m_pSound->GetMidiVolume();

	nb = fwrite(&info, sizeof(DescInfo), 1, file);
	if ( nb < 1 )  goto error;

	fclose(file);
	return TRUE;

	error:
	if ( file != NULL )  fclose(file);
	return FALSE;
}

// Lit les informations sur disque.

BOOL CEvent::ReadInfo()
{
	char		filename[MAX_PATH];
	FILE*		file = NULL;
	DescInfo	info;
	int			nb;

	strcpy(filename, "data\\info.blp");
	AddUserPath(filename);

	file = fopen(filename, "rb");
	if ( file == NULL )  goto error;

	nb = fread(&info, sizeof(DescInfo), 1, file);
	if ( nb < 1 )  goto error;

	m_private      = info.prive;
	m_exercice     = info.exercice;
	m_mission      = info.mission;
	m_maxMission   = info.maxMission;
	m_speed        = info.speed;
	m_bMovie       = info.bMovie;
	m_scrollSpeed  = info.scrollSpeed;
	m_bAccessBuild = info.bAccessBuild;

	m_pDecor->SetSkill(info.skill);

	m_pSound->SetAudioVolume(info.audioVolume);
	m_pSound->SetMidiVolume(info.midiVolume);

	fclose(file);
	return TRUE;

	error:
	if ( file != NULL )  fclose(file);
	return FALSE;
}


// Modifie la vitesse du jeu.

void CEvent::SetSpeed(int speed)
{
	int		max;

	if ( m_bSpeed )  max = 8;
	else             max = 2;

	if ( speed > max )  speed = max;

	m_speed = speed;
}

int CEvent::GetSpeed()
{
	return m_speed;
}

BOOL CEvent::GetPause()
{
	return m_bPause;
}


// Début de l'enregistrement d'une démo.

void CEvent::DemoRecStart()
{
	m_pDemoBuffer = (DemoEvent*)malloc(MAXDEMO*sizeof(DemoEvent));
	if ( m_pDemoBuffer == NULL )  return;
	memset(m_pDemoBuffer, 0, MAXDEMO*sizeof(DemoEvent));

	m_demoTime  = 0;
	m_demoIndex = 0;
	m_bDemoRec  = TRUE;
	m_bDemoPlay = FALSE;

	InitRandom();
	m_pDecor->SetTime(0);
	m_speed = 1;
}

// Fin de l'enregistrement d'une démo.
// Sauve le fichier sur disque.

void CEvent::DemoRecStop()
{
	FILE*		file = NULL;
	DemoHeader	header;

	if ( m_bDemoPlay )  return;

	if ( m_pDemoBuffer != NULL )
	{
		DeleteFile("data\\demo.blp");
		file = fopen("data\\demo.blp", "wb");
		if ( file != NULL )
		{
			memset(&header, 0, sizeof(DemoHeader));
			header.majRev   = 1;
			header.minRev   = 0;
			header.bSchool  = m_bSchool;
			header.bPrivate = m_bPrivate;
			header.world    = GetPhysicalWorld();
			header.skill    = m_pDecor->GetSkill();
			fwrite(&header, sizeof(DemoHeader), 1, file);
			fwrite(m_pDemoBuffer, sizeof(DemoEvent), m_demoIndex, file);
			fclose(file);
		}
		free(m_pDemoBuffer);
		m_pDemoBuffer = NULL;
	}

	m_bDemoRec = FALSE;
	m_demoTime = 0;
}

// Début de la reproduction d'une démo.
// Lit le fichier sur disque.

BOOL CEvent::DemoPlayStart()
{
	char		filename[MAX_PATH];
	FILE*		file = NULL;
	DemoHeader	header;
	int			nb, world, time, total;

	m_pDemoBuffer = (DemoEvent*)malloc(MAXDEMO*sizeof(DemoEvent));
	if ( m_pDemoBuffer == NULL )  return FALSE;
	memset(m_pDemoBuffer, 0, MAXDEMO*sizeof(DemoEvent));

	sprintf(filename, "data\\demo%.3d.blp", m_demoNumber);
	AddCDPath(filename);  // ajoute l'accès au CD-Rom
	file = fopen(filename, "rb");
	if ( file == NULL )
	{
		DemoPlayStop();
		return FALSE;
	}

	nb = fread(&header, sizeof(DemoHeader), 1, file);
	if ( nb < 1 )
	{
		DemoPlayStop();
		return FALSE;
	}
	m_bSchool  = header.bSchool;
	m_bPrivate = header.bPrivate;
	m_pDecor->SetSkill(header.skill);

	m_demoEnd = fread(m_pDemoBuffer, sizeof(DemoEvent), MAXDEMO, file);
	fclose(file);

	m_demoTime  = 0;
	m_demoIndex = 0;
	m_bDemoPlay = TRUE;
	m_bDemoRec  = FALSE;

	if ( !m_pDecor->Read(header.world, FALSE, world, time, total) )
	{
		DemoPlayStop();
		return FALSE;
	}
	ChangePhase(WM_PHASE_PLAY);
	InitRandom();
	m_pDecor->SetTime(0);
	m_speed = 1;

	return TRUE;
}

// Fin de la reproduction d'une démo.

void CEvent::DemoPlayStop()
{
	if ( m_pDemoBuffer != NULL )
	{
		free(m_pDemoBuffer);
		m_pDemoBuffer = NULL;
	}
	m_bDemoPlay = FALSE;
	m_bDemoRec  = FALSE;
	m_demoTime  = 0;

	ChangePhase(WM_PHASE_INIT);
}

// Avance l'index d'enregistrement ou de reproduction.

void CEvent::DemoStep()
{
	int			time;
	UINT		message;
	WPARAM		wParam;
	LPARAM		lParam;
	POINT		pos;

	if ( m_phase == WM_PHASE_INIT )
	{
		if ( m_demoTime > DEF_TIME_DEMO )  // ~30 secondes écoulées ?
		{
			m_demoNumber = 0;
			DemoPlayStart();  // démarre une démo automatique
		}
	}

	if ( m_bDemoPlay &&  // démo en lecture ?
		 m_pDemoBuffer != NULL )
	{
		while ( TRUE )
		{
			time = m_pDemoBuffer[m_demoIndex].time;
			if ( time > m_demoTime )  break;

			message = m_pDemoBuffer[m_demoIndex].message;
			wParam  = m_pDemoBuffer[m_demoIndex].wParam;
			lParam  = m_pDemoBuffer[m_demoIndex].lParam;
			m_demoIndex ++;

			if ( message == WM_MOUSEMOVE &&
				 m_mouseType == MOUSETYPEWIN )
			{
				pos = ConvLongToPos(lParam);
				ClientToScreen(m_hWnd, &pos);
				SetCursorPos(pos.x, pos.y);
			}

			TreatEventBase(message, wParam, lParam);

			if ( m_demoIndex >= m_demoEnd )
			{
				m_demoNumber ++;  // démo suivante
				if ( !DemoPlayStart() )  // démarre la démo suivante
				{
					m_demoNumber = 0;  // première démo
					DemoPlayStart();   // démarre la démo
				}
				return;
			}
		}
	}

	m_demoTime ++;
}

// Mémorise un événement.

void CEvent::DemoRecEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( m_bDemoRec && m_pDemoBuffer != NULL &&
		 (message == WM_KEYDOWN     ||
		  message == WM_KEYUP       ||
		  message == WM_LBUTTONDOWN ||
		  message == WM_RBUTTONDOWN ||
		  message == WM_MOUSEMOVE   ||
		  message == WM_LBUTTONUP   ||
		  message == WM_RBUTTONUP   ) )
	{
		if ( m_demoIndex > 0 &&
			 message == WM_MOUSEMOVE &&
			 m_pDemoBuffer[m_demoIndex-1].time    == m_demoTime &&
			 m_pDemoBuffer[m_demoIndex-1].message == message )
		{
			m_pDemoBuffer[m_demoIndex-1].wParam  = wParam;
			m_pDemoBuffer[m_demoIndex-1].lParam  = lParam;
		}
		else
		{
			m_pDemoBuffer[m_demoIndex].time    = m_demoTime;
			m_pDemoBuffer[m_demoIndex].message = message;
			m_pDemoBuffer[m_demoIndex].wParam  = wParam;
			m_pDemoBuffer[m_demoIndex].lParam  = lParam;

			m_demoIndex ++;
			if ( m_demoIndex >= MAXDEMO )
			{
				DemoRecStop();
			}
		}
	}
}


// Retourne la dernière position de la souris.

POINT CEvent::GetLastMousePos()
{
	return m_oldMousePos;
}

// Traitement d'un événement.

BOOL CEvent::TreatEvent(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( m_bDemoPlay )  // démo en lecture ?
	{
		if ( message == WM_KEYDOWN     ||  // l'utilisateur clique ?
			 message == WM_KEYUP       ||
//			 message == WM_LBUTTONDOWN ||
//			 message == WM_RBUTTONDOWN ||
			 message == WM_LBUTTONUP   ||
			 message == WM_RBUTTONUP   )
		{
			DemoPlayStop();
			return TRUE;
		}
		if ( message == WM_MOUSEMOVE )  // l'utilisateur bouge ?
		{
			return TRUE;
		}
	}

	return TreatEventBase(message, wParam, lParam);
}

// Traitement d'un événement.

BOOL CEvent::TreatEventBase(UINT message, WPARAM wParam, LPARAM lParam)
{
	POINT		pos;
	int			fwKeys;
	int			i, sound;
	char		c;
	BOOL		bEnable;

	pos = ConvLongToPos(lParam);
	fwKeys = wParam;

	DemoRecEvent(message, wParam, lParam);

    switch( message )
    {
		case WM_KEYDOWN:
			if ( wParam >= 'A' && wParam <= 'Z' )
			{
				if ( m_posCheat == 0 )  // première lettre ?
				{
					m_rankCheat = -1;
					for ( i=0 ; i<9 ; i++ )
					{
						if ( (char)wParam == cheat_code[i][0] )
						{
							m_rankCheat = i;
							break;
						}
					}
				}
				if ( m_rankCheat != -1 )
				{
					c = cheat_code[m_rankCheat][m_posCheat];
					if ( m_posCheat != 0 && m_rankCheat == 8 )  c++;  // CONSTRUIRE ?
					if ( (char)wParam == c )
					{
						m_posCheat ++;
						if ( cheat_code[m_rankCheat][m_posCheat] == 0 )
						{
							bEnable = TRUE;
							if ( m_phase == WM_PHASE_PLAY )
							{
								if ( m_rankCheat == 0 )  // vision ?
								{
									m_pDecor->EnableFog(FALSE);
								}
								if ( m_rankCheat == 1 ||  // power ?
									 m_rankCheat == 2 )   // lonesome ?
								{
									m_pDecor->BlupiCheat(m_rankCheat);
								}
							}

							if ( m_rankCheat == 3 )  // allmissions ?
							{
								m_bAllMissions = !m_bAllMissions;
								bEnable = m_bAllMissions;
								m_bChangeCheat = TRUE;
							}
							if ( m_rankCheat == 4 )  // quick ?
							{
								m_bSpeed = !m_bSpeed;
								bEnable = m_bSpeed;
								m_bChangeCheat = TRUE;
							}
#if !_DEMO
							if ( m_rankCheat == 5 )  // helpme ?
							{
								m_bHelp = !m_bHelp;
								bEnable = m_bHelp;
								m_bChangeCheat = TRUE;
							}
#endif

							if ( m_rankCheat == 6 )  // invincible ?
							{
								m_pDecor->SetInvincible(!m_pDecor->GetInvincible());
								bEnable = m_pDecor->GetInvincible();
								m_bChangeCheat = TRUE;
							}

							if ( m_rankCheat == 7 )  // superblupi ?
							{
								m_pDecor->SetSuper(!m_pDecor->GetSuper());
								bEnable = m_pDecor->GetSuper();
								m_bChangeCheat = TRUE;
							}

#if !_DEMO
							if ( m_rankCheat == 8 )  // construire ?
							{
								m_bAccessBuild = !m_bAccessBuild;
								bEnable = m_bAccessBuild;
								m_bChangeCheat = TRUE;
							}
#endif

							if ( m_phase != WM_PHASE_PLAY )
							{
								ChangePhase(m_phase);
							}

							pos.x = LXIMAGE/2;
							pos.y = LYIMAGE/2;
							if ( bEnable )
							{
								m_pSound->PlayImage(SOUND_BUT, pos);
							}
							else
							{
								m_pSound->PlayImage(SOUND_BOING, pos);
							}

							m_rankCheat = -1;
							m_posCheat = 0;
						}
						return TRUE;
					}
				}
			}
			m_rankCheat = -1;
			m_posCheat = 0;

			if ( m_phase == WM_PHASE_INTRO1 )
			{
				ChangePhase(WM_PHASE_INTRO2);
				return TRUE;
			}

			if ( m_phase == WM_PHASE_INTRO2 )
			{
				ChangePhase(WM_PHASE_INIT);
				return TRUE;
			}

			if ( m_phase == WM_PHASE_BYE )
			{
				PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			}

			switch( wParam )
			{
				case VK_END:
					DemoRecStop();
					return TRUE;
				case VK_ESCAPE:
					if ( m_bRunMovie )
					{
						StopMovie();
						m_pSound->SetSuspendSkip(1);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_PLAY   ||
						 m_phase == WM_PHASE_SETUP  ||
						 m_phase == WM_PHASE_SETUPp ||
						 m_phase == WM_PHASE_READ   ||
						 m_phase == WM_PHASE_WRITE  ||
						 m_phase == WM_PHASE_WRITEp ||
						 m_phase == WM_PHASE_HELP   )
					{
						ChangePhase(WM_PHASE_STOP);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_STOP  ||
						 m_phase == WM_PHASE_LOST  ||
						 m_phase == WM_PHASE_BUILD )
					{
						ChangePhase(WM_PHASE_INFO);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_INFO )
					{
						ChangePhase(WM_PHASE_INIT);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_BUTTON ||
						 m_phase == WM_PHASE_TERM   ||
						 m_phase == WM_PHASE_MUSIC  ||
						 m_phase == WM_PHASE_REGION )
					{
						ChangePhase(WM_PHASE_BUILD);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_INIT )
					{
						ChangePhase(WM_PHASE_BYE);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_BYE )
					{
						PostMessage(m_hWnd, WM_CLOSE, 0, 0);
						break;
					}
					return TRUE;
				case VK_RETURN:
					if ( m_phase == WM_PHASE_PLAY  ||
						 m_phase == WM_PHASE_READ  ||
						 m_phase == WM_PHASE_WRITE ||
						 m_phase == WM_PHASE_SETUP )
					{
						ChangePhase(WM_PHASE_STOP);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_INIT  ||
						 m_phase == WM_PHASE_LOST  ||
						 m_phase == WM_PHASE_BUILD )
					{
						ChangePhase(WM_PHASE_INFO);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_INFO   ||
						 m_phase == WM_PHASE_STOP   ||
						 m_phase == WM_PHASE_HELP   ||
						 m_phase == WM_PHASE_SETUPp ||
						 m_phase == WM_PHASE_WRITEp )
					{
						ChangePhase(WM_PHASE_PLAY);
						return TRUE;
					}
					if ( m_phase == WM_PHASE_BUTTON ||
						 m_phase == WM_PHASE_TERM   ||
						 m_phase == WM_PHASE_MUSIC  ||
						 m_phase == WM_PHASE_REGION )
					{
						ChangePhase(WM_PHASE_BUILD);
						return TRUE;
					}
					return TRUE;
				case VK_LEFT:
//?					DecorShift(-4,4);
					DecorShift(-2,2);
					return TRUE;
				case VK_RIGHT:
//?					DecorShift(4,-4);
					DecorShift(2,-2);
					return TRUE;
				case VK_UP:
//?					DecorShift(-6,-6);
					DecorShift(-3,-3);
					return TRUE;
				case VK_DOWN:
//?					DecorShift(6,6);
					DecorShift(3,3);
					return TRUE;
				case VK_HOME:
					pos = m_pDecor->GetHome();
					m_pDecor->SetCoin(pos);
					return TRUE;
				case VK_SPACE:
					if ( m_bRunMovie )
					{
						StopMovie();
						m_pSound->SetSuspendSkip(1);
						return TRUE;
					}
					m_pDecor->FlipOutline();
					return TRUE;
				case VK_PAUSE:
					m_bPause = !m_bPause;
					if ( m_phase == WM_PHASE_PLAY )
					{
						if ( m_bPause )
						{
							m_pSound->SuspendMusic();
						}
						else
						{
							m_pSound->RestartMusic();
						}
					}
					return TRUE;
				case VK_CONTROL:
					m_bCtrlDown = TRUE;
					if ( m_phase == WM_PHASE_BUILD )
					{
						m_bFillMouse = TRUE;
						MouseSprite(GetMousePos());
					}
					else
					{
						m_bFillMouse = FALSE;
					}
					return TRUE;
				case VK_F1:
					if ( m_phase == WM_PHASE_PLAY )
					{
						// Montre ou cache les infos tout en haut.
						if ( m_pDecor->GetInfoMode() )  sound = SOUND_CLOSE;
						else                            sound = SOUND_OPEN;
						pos.x = LXIMAGE/2;
						pos.y = LYIMAGE/2;
						m_pSound->PlayImage(sound, pos);
						m_pDecor->SetInfoMode(!m_pDecor->GetInfoMode());
					}
					return TRUE;
				case VK_F9:
					if ( m_phase == WM_PHASE_PLAY )
					{
						m_pDecor->MemoPos(0, m_bCtrlDown);
					}
					return TRUE;
				case VK_F10:
					if ( m_phase == WM_PHASE_PLAY )
					{
						m_pDecor->MemoPos(1, m_bCtrlDown);
					}
					return TRUE;
				case VK_F11:
					if ( m_phase == WM_PHASE_PLAY )
					{
						m_pDecor->MemoPos(2, m_bCtrlDown);
					}
					return TRUE;
				case VK_F12:
					if ( m_phase == WM_PHASE_PLAY )
					{
						m_pDecor->MemoPos(3, m_bCtrlDown);
					}
					return TRUE;
			}
			break;

		case WM_KEYUP:
			switch( wParam )
			{
				case VK_CONTROL:
					m_bCtrlDown = FALSE;
					m_bFillMouse = FALSE;
					MouseSprite(GetMousePos());
					return TRUE;
			}
			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			m_bMousePress = TRUE;
			MouseSprite(pos);
//?			DecorAutoShift(pos);
			if ( EventButtons(message, wParam, lParam) )  return TRUE;
			if ( m_phase == WM_PHASE_BUILD )
			{
				if ( BuildDown(pos, fwKeys) )  return TRUE;
			}
			if ( m_phase == WM_PHASE_PLAY )
			{
				if ( PlayDown(pos, fwKeys) )  return TRUE;
			}
			break;

		case WM_MOUSEMOVE:
			if ( m_mouseType == MOUSETYPEGRA )
			{
				if ( m_bShowMouse )
				{
					ShowCursor(FALSE);  // cache la souris
					m_pPixmap->MouseShow(TRUE);  // montre sprite
					m_bShowMouse = FALSE;
				}
			}
			if ( m_mouseType == MOUSETYPEWINPOS &&
				 (pos.x != m_oldMousePos.x ||
				  pos.y != m_oldMousePos.y ) )
			{
				m_oldMousePos = pos;
				ClientToScreen(m_hWnd, &m_oldMousePos);
				SetCursorPos(m_oldMousePos.x, m_oldMousePos.y);  // (*)
			}
			m_oldMousePos = pos;

			MouseSprite(pos);
			if ( EventButtons(message, wParam, lParam) )  return TRUE;
			if ( m_phase == WM_PHASE_BUILD )
			{
				if ( BuildMove(pos, fwKeys) )  return TRUE;
			}
			if ( m_phase == WM_PHASE_PLAY )
			{
				if ( PlayMove(pos, fwKeys) )  return TRUE;
			}
			break;

		case WM_NCMOUSEMOVE:
			if ( m_mouseType == MOUSETYPEGRA )
			{
				if ( !m_bShowMouse )
				{
					ShowCursor(TRUE);  // montre la souris
					m_pPixmap->MouseShow(FALSE);  // cache sprite
					m_bShowMouse = TRUE;
				}
			}
			break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
			m_bMousePress = FALSE;
			if ( EventButtons(message, wParam, lParam) )  return TRUE;
			if ( m_phase == WM_PHASE_BUILD )
			{
				if ( BuildUp(pos, fwKeys) )  return TRUE;
			}
			if ( m_phase == WM_PHASE_PLAY )
			{
				if ( PlayUp(pos, fwKeys) )  return TRUE;
			}
			if ( m_phase == WM_PHASE_BYE )
			{
				PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case WM_PHASE_DEMO:
			m_demoNumber = 0;
			DemoPlayStart();
			break;

		case WM_PHASE_SCHOOL:
			m_bSchool  = TRUE;
			m_bPrivate = FALSE;
			if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
			break;

		case WM_PHASE_MISSION:
			m_bSchool  = FALSE;
			m_bPrivate = FALSE;
			if ( m_mission == 0 )  // première mission ?
			{
				if ( ChangePhase(WM_PHASE_H0MOVIE) )  return TRUE;
			}
			else
			{
				if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
			}
			break;

		case WM_PHASE_PRIVATE:
			m_bSchool  = FALSE;
			m_bPrivate = TRUE;
			if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
			break;

		case WM_PHASE_INTRO1:
		case WM_PHASE_INTRO2:
		case WM_PHASE_INIT:
		case WM_PHASE_HISTORY0:
		case WM_PHASE_HISTORY1:
		case WM_PHASE_INFO:
		case WM_PHASE_PLAY:
		case WM_PHASE_READ:
		case WM_PHASE_WRITE:
		case WM_PHASE_WRITEp:
		case WM_PHASE_BUILD:
		case WM_PHASE_BUTTON:
		case WM_PHASE_TERM:
		case WM_PHASE_STOP:
		case WM_PHASE_HELP:
		case WM_PHASE_MUSIC:
		case WM_PHASE_REGION:
		case WM_PHASE_SETUP:
		case WM_PHASE_SETUPp:
		case WM_PHASE_PLAYMOVIE:
		case WM_PHASE_H0MOVIE:
		case WM_PHASE_H1MOVIE:
		case WM_PHASE_H2MOVIE:
		case WM_PHASE_WINMOVIE:
		case WM_PHASE_BYE:
			if ( ChangePhase(message) )  return TRUE;
			break;

		case WM_PHASE_UNDO:
			m_pDecor->UndoBack();  // revient en arrière
			break;

		case WM_PREV:
			m_pDecor->SetInvincible(FALSE);
			m_pDecor->SetSuper(FALSE);
			if ( m_bPrivate )
			{
				if ( m_private > 0 )
				{
					m_private --;
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			else if ( m_bSchool )
			{
				if ( m_exercice > 0 )
				{
					m_exercice --;
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			else
			{
				if ( m_mission > 0 )
				{
					m_mission --;
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			break;

		case WM_NEXT:
			m_pDecor->SetInvincible(FALSE);
			m_pDecor->SetSuper(FALSE);
			if ( m_bPrivate )
			{
				if ( m_private < 20-1 )
				{
					m_private ++;
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			else if ( m_bSchool )
			{
				if ( m_exercice < 99 )
				{
					m_exercice ++;
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			else
			{
				if ( m_mission < 99 )
				{
					m_mission ++;
					if ( m_maxMission < m_mission ) 
					{
						m_maxMission = m_mission;
					}
					if ( ChangePhase(WM_PHASE_INFO) )  return TRUE;
				}
			}
			break;

		case WM_DECOR1:
			SetState(WM_DECOR1, 1);
			SetState(WM_DECOR2, 0);
			SetState(WM_DECOR3, 0);
			SetState(WM_DECOR4, 0);
			SetState(WM_DECOR5, 0);
			break;

		case WM_DECOR2:
			SetState(WM_DECOR1, 0);
			SetState(WM_DECOR2, 1);
			SetState(WM_DECOR3, 0);
			SetState(WM_DECOR4, 0);
			SetState(WM_DECOR5, 0);
			break;

		case WM_DECOR3:
			SetState(WM_DECOR1, 0);
			SetState(WM_DECOR2, 0);
			SetState(WM_DECOR3, 1);
			SetState(WM_DECOR4, 0);
			SetState(WM_DECOR5, 0);
			break;

		case WM_DECOR4:
			SetState(WM_DECOR1, 0);
			SetState(WM_DECOR2, 0);
			SetState(WM_DECOR3, 0);
			SetState(WM_DECOR4, 1);
			SetState(WM_DECOR5, 0);
			break;

		case WM_DECOR5:
			SetState(WM_DECOR1, 0);
			SetState(WM_DECOR2, 0);
			SetState(WM_DECOR3, 0);
			SetState(WM_DECOR4, 0);
			SetState(WM_DECOR5, 1);
			break;

		case WM_PHASE_SKILL1:
			m_pDecor->SetSkill(0);
			SetState(WM_PHASE_SKILL1, TRUE);
			SetState(WM_PHASE_SKILL2, FALSE);
			break;
		case WM_PHASE_SKILL2:
			m_pDecor->SetSkill(1);
			SetState(WM_PHASE_SKILL1, FALSE);
			SetState(WM_PHASE_SKILL2, TRUE);
			break;

		case WM_BUTTON0:
		case WM_BUTTON1:
		case WM_BUTTON2:
		case WM_BUTTON3:
		case WM_BUTTON4:
		case WM_BUTTON5:
		case WM_BUTTON6:
		case WM_BUTTON7:
		case WM_BUTTON8:
		case WM_BUTTON9:
		case WM_BUTTON10:
		case WM_BUTTON11:
		case WM_BUTTON12:
		case WM_BUTTON13:
		case WM_BUTTON14:
		case WM_BUTTON15:
		case WM_BUTTON16:
		case WM_BUTTON17:
		case WM_BUTTON18:
		case WM_BUTTON19:
		case WM_BUTTON20:
		case WM_BUTTON21:
		case WM_BUTTON22:
		case WM_BUTTON23:
		case WM_BUTTON24:
		case WM_BUTTON25:
		case WM_BUTTON26:
		case WM_BUTTON27:
		case WM_BUTTON28:
		case WM_BUTTON29:
		case WM_BUTTON30:
		case WM_BUTTON31:
		case WM_BUTTON32:
		case WM_BUTTON33:
		case WM_BUTTON34:
		case WM_BUTTON35:
		case WM_BUTTON36:
		case WM_BUTTON37:
		case WM_BUTTON38:
		case WM_BUTTON39:
			ChangeButtons(message);
			break;

		case WM_READ0:
		case WM_READ1:
		case WM_READ2:
		case WM_READ3:
		case WM_READ4:
		case WM_READ5:
		case WM_READ6:
		case WM_READ7:
		case WM_READ8:
		case WM_READ9:
			Read(message);
			ChangePhase(WM_PHASE_PLAY);  // joue
			break;

		case WM_WRITE0:
		case WM_WRITE1:
		case WM_WRITE2:
		case WM_WRITE3:
		case WM_WRITE4:
		case WM_WRITE5:
		case WM_WRITE6:
		case WM_WRITE7:
		case WM_WRITE8:
		case WM_WRITE9:
			Write(message);
			if ( m_phase == WM_PHASE_WRITEp )
			{
				ChangePhase(WM_PHASE_PLAY);  // joue
			}
			else
			{
				ChangePhase(WM_PHASE_STOP);  // pause
			}
			break;

		case WM_MOVIE:
			StartMovie("movie\\essai.avi");
			ChangePhase(WM_PHASE_INIT);
			break;
	}

	return FALSE;
}

// (*)	Sans cela, il existe un drôle de problème sur
//		certains PC: la souris ne suis pas le mouvement
//		réel effectué.
//		Problème résolu par Denis !


// Affiche un caractère de debug tout en haut.

void CEvent::DebugDisplay(char m)
{
	char	text[10];
	POINT	pos;
	RECT	rect;

	m_debugPos.x += 8;
	if ( m_debugPos.x > LXIMAGE-8 )
	{
		m_debugPos.x = 0;
		pos.x = 0;
		pos.y = 0;
		rect.left   = pos.x;
		rect.right  = pos.x+LXIMAGE;
		rect.top    = pos.y;
		rect.bottom = pos.y+DIMLITTLEY;
		m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond
	}

	text[0] = m;
	text[1] = 0;

	DrawText(m_pPixmap, m_debugPos, text, FONTLITTLE);
}

// Passe les images d'introduction.

void CEvent::IntroStep()
{
	m_introTime ++;

	if ( m_introTime > 20*3 )
	{
		if ( m_phase == WM_PHASE_INTRO1 )
		{
			ChangePhase(WM_PHASE_INTRO2);
			return;
		}

		if ( m_phase == WM_PHASE_INTRO2 )
		{
			ChangePhase(WM_PHASE_INIT);
			return;
		}
	}
}

