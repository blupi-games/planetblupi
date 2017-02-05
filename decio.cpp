// DecIO.cpp
//

#include "DEF.H"
#include "DECOR.H"
#include "MISC.H"

typedef struct
{
	short			majRev;
	short			minRev;
	long			nbDecor;
	long			lgDecor;
	long			nbBlupi;
	long			lgBlupi;
	long			nbMove;
	long			lgMove;
	short			reserve1[100];
	POINT			celCoin;
	short			world;
	long			time;
	char			buttonExist[MAXBUTTON];
	Term			term;
	short			music;
	short			region;
	long			totalTime;
	short			skill;
	POINT			memoPos[4];
	short			reserve2[29];
}
DescFile;

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
}
OldBlupi;


// Sauve le décor sur disque.

bool CDecor::Write(int rank, bool bUser, int world, int time, int total)
{
	char		filename[MAX_PATH];
	FILE*		file    = nullptr;
	DescFile*	pBuffer = nullptr;
	int			i;
	size_t		nb;

	if ( bUser )
	{
		sprintf(filename, "data\\user%.3d.blp", rank);
		AddUserPath(filename);
	}
	else
	{
		sprintf(filename, "data\\world%.3d.blp", rank);
	}

	file = fopen(filename, "wb");
	if ( file == nullptr )  goto error;

	pBuffer = (DescFile*)malloc(sizeof(DescFile));
	if ( pBuffer == nullptr )  goto error;
	memset(pBuffer, 0, sizeof(DescFile));

	pBuffer->majRev    = 1;
	pBuffer->minRev    = 5;
	pBuffer->celCoin   = m_celCoin;
	pBuffer->world     = world;
	pBuffer->time      = time;
	pBuffer->totalTime = total;
	pBuffer->term      = m_term;
	pBuffer->music     = m_music;
	pBuffer->region    = m_region;
	pBuffer->skill     = m_skill;
	pBuffer->nbDecor   = MAXCELX*MAXCELY;
	pBuffer->lgDecor   = sizeof(Cellule);
	pBuffer->nbBlupi   = MAXBLUPI;
	pBuffer->lgBlupi   = sizeof(Blupi);
	pBuffer->nbMove    = MAXMOVE;
	pBuffer->lgMove    = sizeof(Move);

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		pBuffer->buttonExist[i] = m_buttonExist[i];
	}

	for ( i=0 ; i<4 ; i++ )
	{
		pBuffer->memoPos[i] = m_memoPos[i];
	}

	nb = fwrite(pBuffer, sizeof(DescFile), 1, file);
	if ( nb < 1 )  goto error;

	nb = fwrite(m_decor, sizeof(Cellule), MAXCELX*MAXCELY/4, file);
	if ( nb < MAXCELX*MAXCELY/4 )  goto error;

	nb = fwrite(m_blupi, sizeof(Blupi), MAXBLUPI, file);
	if ( nb < MAXBLUPI )  goto error;

	nb = fwrite(m_move, sizeof(Move), MAXMOVE, file);
	if ( nb < MAXMOVE )  goto error;

	nb = fwrite(m_lastDrapeau, sizeof(POINT), MAXLASTDRAPEAU, file);
	if ( nb < MAXLASTDRAPEAU )  goto error;

	free(pBuffer);
	fclose(file);
	return true;

	error:
	if ( pBuffer != nullptr )  free(pBuffer);
	if ( file    != nullptr )  fclose(file);
	return false;
}

// Lit le décor sur disque.

bool CDecor::Read(int rank, bool bUser, int &world, int &time, int &total)
{
	char		filename[MAX_PATH];
	FILE*		file    = nullptr;
	DescFile*	pBuffer = nullptr;
	int			majRev, minRev;
	int			i, x, y;
	size_t		nb;
	OldBlupi	oldBlupi;

	Init(-1, -1);

	if ( bUser )
	{
		sprintf(filename, "data\\user%.3d.blp", rank);
		AddUserPath(filename);
	}
	else
	{
		sprintf(filename, "data\\world%.3d.blp", rank);
	}

	file = fopen(filename, "rb");
	if ( file == nullptr )  goto error;

	pBuffer = (DescFile*)malloc(sizeof(DescFile));
	if ( pBuffer == nullptr )  goto error;

	nb = fread(pBuffer, sizeof(DescFile), 1, file);
	if ( nb < 1 )  goto error;

	majRev = pBuffer->majRev;
	minRev = pBuffer->minRev;

	if ( majRev == 1 && minRev == 0 )  goto error;

	if ( majRev == 1 && minRev == 3 )
	{
		if ( pBuffer->nbDecor != MAXCELX*MAXCELY ||
			 pBuffer->lgDecor != sizeof(Cellule) ||
			 pBuffer->nbBlupi != MAXBLUPI        ||
			 pBuffer->lgBlupi != sizeof(OldBlupi)||
			 pBuffer->nbMove  != MAXMOVE         ||
			 pBuffer->lgMove  != sizeof(Move)    )  goto error;
	}
	else
	{
		if ( pBuffer->nbDecor != MAXCELX*MAXCELY ||
			 pBuffer->lgDecor != sizeof(Cellule) ||
			 pBuffer->nbBlupi != MAXBLUPI        ||
			 pBuffer->lgBlupi != sizeof(Blupi)   ||
			 pBuffer->nbMove  != MAXMOVE         ||
			 pBuffer->lgMove  != sizeof(Move)    )  goto error;
	}

	SetCoin(pBuffer->celCoin);
	if ( bUser )
	{
		world = pBuffer->world;
		time  = pBuffer->time;
		total = pBuffer->totalTime;
	}
	m_celHome = pBuffer->celCoin;
	m_term    = pBuffer->term;
	m_music   = pBuffer->music;
	m_region  = pBuffer->region;

	if ( bUser )
	{
		m_skill = pBuffer->skill;
	}

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		m_buttonExist[i] = pBuffer->buttonExist[i];
	}

	for ( i=0 ; i<4 ; i++ )
	{
		m_memoPos[i] = pBuffer->memoPos[i];
	}

	nb = fread(m_decor, sizeof(Cellule), MAXCELX*MAXCELY/4, file);
	if ( nb < MAXCELX*MAXCELY/4 )  goto error;
	if ( majRev == 1 && minRev < 5 )
	{
		for ( x=0 ; x<MAXCELX/2 ; x++ )
		{
			for ( y=0 ; y<MAXCELY/2 ; y++ )
			{
				if ( m_decor[x][y].objectIcon >= 128 &&
					 m_decor[x][y].objectIcon <= 130 )
				{
					m_decor[x][y].objectIcon -= 128-17;
				}
			}
		}
	}

	if ( majRev == 1 && minRev == 3 )
	{
		memset(m_blupi, 0, sizeof(Blupi)*MAXBLUPI);
		for ( i=0 ; i<MAXBLUPI ; i++ )
		{
			nb = fread(&oldBlupi, sizeof(OldBlupi), 1, file);
			if ( nb != 1 )  goto error;
			memcpy(m_blupi+i, &oldBlupi, sizeof(OldBlupi));
			ListFlush(i);
		}
	}
	else
	{
		nb = fread(m_blupi, sizeof(Blupi), MAXBLUPI, file);
		if ( nb < MAXBLUPI )  goto error;
	}

	nb = fread(m_move, sizeof(Move), MAXMOVE, file);
	if ( nb < MAXMOVE )  goto error;

	nb = fread(m_lastDrapeau, sizeof(POINT), MAXLASTDRAPEAU, file);
	if ( nb < MAXLASTDRAPEAU )
	{
		InitDrapeau();
	}

	BlupiDeselect();  // désélectionne tous les blupi

	free(pBuffer);
	fclose(file);
	return true;

	error:
	if ( pBuffer != nullptr )  free(pBuffer);
	if ( file    != nullptr )  fclose(file);

	Flush();  // initialise un décor neutre
	return false;
}

// Indique si un fichier existe sur disque.

bool CDecor::FileExist(int rank, bool bUser, int &world, int &time, int &total)
{
	char		filename[MAX_PATH];
	FILE*		file    = nullptr;
	DescFile*	pBuffer = nullptr;
	int			majRev, minRev;
	size_t		nb;

	if ( bUser )
	{
		sprintf(filename, "data\\user%.3d.blp", rank);
		AddUserPath(filename);
	}
	else
	{
		sprintf(filename, "data\\world%.3d.blp", rank);
	}

	file = fopen(filename, "rb");
	if ( file == nullptr )  goto error;

	pBuffer = (DescFile*)malloc(sizeof(DescFile));
	if ( pBuffer == nullptr )  goto error;

	nb = fread(pBuffer, sizeof(DescFile), 1, file);
	if ( nb < 1 )  goto error;

	majRev = pBuffer->majRev;
	minRev = pBuffer->minRev;

	if ( majRev == 1 && minRev == 0 )  goto error;

	if ( majRev == 1 && minRev == 3 )
	{
		if ( pBuffer->nbDecor != MAXCELX*MAXCELY ||
			 pBuffer->lgDecor != sizeof(Cellule) ||
			 pBuffer->nbBlupi != MAXBLUPI        ||
			 pBuffer->lgBlupi != sizeof(OldBlupi)||
			 pBuffer->nbMove  != MAXMOVE         ||
			 pBuffer->lgMove  != sizeof(Move)    )  goto error;
	}
	else
	{
		if ( pBuffer->nbDecor != MAXCELX*MAXCELY ||
			 pBuffer->lgDecor != sizeof(Cellule) ||
			 pBuffer->nbBlupi != MAXBLUPI        ||
			 pBuffer->lgBlupi != sizeof(Blupi)   ||
			 pBuffer->nbMove  != MAXMOVE         ||
			 pBuffer->lgMove  != sizeof(Move)    )  goto error;
	}

	world = pBuffer->world;
	time  = pBuffer->time;
	total = pBuffer->totalTime;

	free(pBuffer);
	fclose(file);
	return true;

	error:
	if ( pBuffer != nullptr )  free(pBuffer);
	if ( file    != nullptr )  fclose(file);
	return false;
}


#define MARG	18

// Initialise un décor neutre.

void CDecor::Flush()
{
	int			x, y, i, icon;

	Init(-1, -1);

	for ( x=0 ; x<MAXCELX ; x+=2 )
	{
		for ( y=0 ; y<MAXCELY ; y+=2 )
		{
			if ( x < MARG || x > MAXCELX-MARG ||
				 y < MARG || y > MAXCELY-MARG )
			{
				icon = 14;  // eau
				goto put;
			}

			if ( x == MARG && y == MARG )
			{
				icon = 12;
				goto put;
			}
			if ( x == MAXCELX-MARG && y == MARG )
			{
				icon = 13;
				goto put;
			}
			if ( x == MARG && y == MAXCELY-MARG )
			{
				icon = 11;
				goto put;
			}
			if ( x == MAXCELX-MARG && y == MAXCELY-MARG )
			{
				icon = 10;
				goto put;
			}

			if ( x == MARG )
			{
				icon = 4;
				goto put;
			}
			if ( x == MAXCELX-MARG )
			{
				icon = 2;
				goto put;
			}
			if ( y == MARG )
			{
				icon = 5;
				goto put;
			}
			if ( y == MAXCELY-MARG )
			{
				icon = 3;
				goto put;
			}

			icon = 1;  // terre

			put:
			m_decor[x/2][y/2].floorChannel = CHFLOOR;
			m_decor[x/2][y/2].floorIcon    = icon;
		}
	}

	for ( i=0 ; i<MAXBLUPI ; i++ )
	{
		m_blupi[i].bExist = false;
	}

	BlupiCreate(GetCel(102,100), ACTION_STOP, DIRECT_S, 0, MAXENERGY);
	m_decor[98/2][100/2].floorChannel  = CHFLOOR;
	m_decor[98/2][100/2].floorIcon     = 16;
	m_decor[98/2][100/2].objectChannel = CHOBJECT;
	m_decor[98/2][100/2].objectIcon    = 113;

	for ( i=0 ; i<MAXBUTTON ; i++ )
	{
		m_buttonExist[i] = 1;
	}

	for ( i=0 ; i<4 ; i++ )
	{
		m_memoPos[i].x = 0;
		m_memoPos[i].y = 0;
	}

	memset(&m_term, 0, sizeof(Term));
	m_term.bHomeBlupi = true;
	m_term.nbMinBlupi = 1;
	m_term.nbMaxBlupi = 1;

	m_music  = 0;
	m_region = 0;

	m_celHome.x = 90;
	m_celHome.y = 98;
	SetCoin(m_celHome);
	InitAfterBuild();
	LoadImages();
}

