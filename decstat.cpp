// DecStat.cpp
//

#include <Windows.h>

#include "RESOURCE.H"
#include "DECOR.H"
#include "TEXT.H"
#include "MISC.H"

#define STATNB			12

#define STATBLUPIm		0
#define STATBLUPIf		1
#define STATBLUPI		2
#define STATDISCIPLE	3
#define STATFEU			27
#define STATROBOT		28
#define STATTRACKS		29
#define STATBOMBE		30
#define STATARAIGNEE	31
#define STATVIRUS		32
#define STATELECTRO		33


typedef struct
{
	short		bExist;
	short		perso;		// -1=objet, -2=feu, -3=flèche
	short		firstIcon;	// négatif si sol
	short		lastIcon;	// négatif si sol
	short		drawIcon;
	short		bBigIcon;
	short		text;
	short		nb;
	short		lastShow;
}
Statistic;


static Statistic table_statistic[] =
{
	{				// STATBLUPIm = 0
		TRUE,
		0,			// blupi malade
		0,0,		// 
		76,
		FALSE,
		TX_OBJ_BLUPIm,
		0, 0,
	},
	{				// STATBLUPIf = 1
		TRUE,
		0,			// blupi fatigué
		0,0,		// 
		13,
		FALSE,
		TX_OBJ_BLUPIf,
		0, 0,
	},
	{				// STATBLUPI = 2
		TRUE,
		0,			// blupi énergique
		0,0,		// 
		14,
		FALSE,
		TX_OBJ_BLUPI,
		0, 0,
	},
	{				// STATDISCIPLE = 3
		TRUE,
		8,			// disciple
		0,0,		// 
		85,
		FALSE,
		TX_OBJ_DISCIPLE,
		0, 0,
	},
	{				// 4
		TRUE,
		-1,			// objet
		117,117,	// bateau
		58,
		FALSE,
		TX_OBJ_BATEAU,
		0, 0,
	},
	{				// 5
		TRUE,
		-1,			// objet
		118,118,	// jeep
		65,
		FALSE,
		TX_OBJ_JEEP,
		0, 0,
	},
	{				// 6
		TRUE,
		-1,			// objet
		16,16,		// armure
		106,
		FALSE,
		TX_OBJ_ARMURE,
		0, 0,
	},
	{				// 7
		TRUE,
		-1,			// objet
		93,93,		// piège
		70,
		FALSE,
		TX_OBJ_PIEGE,
		0, 0,
	},
	{				// 8
		TRUE,
		-1,			// objet
		92,92,		// poison
		71,
		FALSE,
		TX_OBJ_POISON,
		0, 0,
	},
	{				// 9
		TRUE,
		-1,			// objet
		85,85,		// dynamite
		57,
		FALSE,
		TX_OBJ_DYNAMITE,
		0, 0,
	},
	{				// 10
		TRUE,
		-1,			// objet
		125,125,	// mine
		63,
		FALSE,
		TX_OBJ_MINE,
		0, 0,
	},
	{				// 11
		TRUE,
		-1,			// objet
		60,60,		// tomate
		28,
		FALSE,
		TX_OBJ_TOMATE,
		0, 0,
	},
	{				// 12
		TRUE,
		-1,			// objet
		80,80,		// bouteille
		34,
		FALSE,
		TX_OBJ_POTION,
		0, 0,
	},
	{				// 13
		TRUE,
		-1,			// objet
		36,36,		// planches
		22,
		FALSE,
		TX_OBJ_PLANCHE,
		0, 0,
	},
	{				// 14
		TRUE,
		-1,			// objet
		44,44,		// pierres
		27,
		FALSE,
		TX_OBJ_PIERRE,
		0, 0,
	},
	{				// 15
		TRUE,
		-1,			// objet
		124,124,	// drapeau
		64,
		TRUE,
		TX_OBJ_DRAPEAU,
		0, 0,
	},
	{				// 16
		TRUE,
		-1,			// objet
		123,123,	// fer
		62,
		FALSE,
		TX_OBJ_FER,
		0, 0,
	},
	{				// 17
		TRUE,
		-1,			// objet
		82,82,		// fleurs1
		72,
		FALSE,
		TX_OBJ_FLEUR1,
		0, 0,
	},
	{				// 18
		TRUE,
		-1,			// objet
		84,84,		// fleurs2
		73,
		FALSE,
		TX_OBJ_FLEUR2,
		0, 0,
	},
	{				// 19
		TRUE,
		-1,			// objet
		95,95,		// fleurs3
		74,
		FALSE,
		TX_OBJ_FLEUR3,
		0, 0,
	},
	{				// 20
		TRUE,
		-1,			// objet
		61,61,		// cabane
		19,
		TRUE,
		TX_OBJ_CABANE,
		0, 0,
	},
	{				// 21
		TRUE,
		-1,			// objet
		-52,-56,	// couveuse
		25,
		FALSE,
		TX_OBJ_COUVEUSE,
		0, 0,
	},
	{				// 22
		TRUE,
		-1,			// objet
		-80,-84,	// téléporteur
		101,
		FALSE,
		TX_OBJ_TELEPORTE,
		0, 0,
	},
	{				// 23
		TRUE,
		-1,			// objet
		28,29,		// laboratoire
		35,
		TRUE,
		TX_OBJ_LABO,
		0, 0,
	},
	{				// 24
		TRUE,
		-1,			// objet
		121,122,	// mine de fer
		61,
		TRUE,
		TX_OBJ_MINEFER,
		0, 0,
	},
	{				// 25
		TRUE,
		-1,			// objet
		119,120,	// usine
		59,
		TRUE,
		TX_OBJ_USINE,
		0, 0,
	},
	{				// 26
		TRUE,
		-1,			// objet
		27,27,		// tour
		33,
		TRUE,
		TX_OBJ_TOUR,
		0, 0,
	},
	{				// STATFEU = 27
		TRUE,
		-2,			// feu
		0,0,		// 
		37,
		TRUE,
		TX_OBJ_FEU,
		0, 0,
	},
	{				// STATROBOT = 28
		TRUE,
		4,			// robot
		0,0,		// 
		56,
		FALSE,
		TX_OBJ_ROBOT,
		0, 0,
	},
	{				// STATTRACKS = 29
		TRUE,
		3,			// tracks
		0,0,		// 
		17,
		FALSE,
		TX_OBJ_TRACKS,
		0, 0,
	},
	{				// STATBOMBE = 30
		TRUE,
		5,			// bombe
		0,0,		// 
		38,
		FALSE,
		TX_OBJ_BOMBE,
		0, 0,
	},
	{				// STATARAIGNEE = 31
		TRUE,
		1,			// araignée
		0,0,		// 
		15,
		FALSE,
		TX_OBJ_ARAIGNEE,
		0, 0,
	},
	{				// STATVIRUS = 32
		TRUE,
		2,			// virus
		0,0,		// 
		16,
		FALSE,
		TX_OBJ_VIRUS,
		0, 0,
	},
	{				// STATELECTRO = 33
		TRUE,
		7,			// électro
		0,0,		// 
		75,
		FALSE,
		TX_OBJ_ELECTRO,
		0, 0,
	},

	{
		FALSE,
		-1,
		0,0,
		-1,
		FALSE,
		0,
		999, 999,
	},
};


// Retourne la statistique correspondant à un rang donné.

Statistic* StatisticGet(int rank)
{
	Statistic*	pStatistic;

	pStatistic = table_statistic;
	while ( pStatistic->nb == 0 )
	{
		pStatistic ++;
	}

	while ( rank > 0 )
	{
		if ( pStatistic->bExist )  pStatistic ++;
		while ( pStatistic->nb == 0 )
		{
			pStatistic ++;
		}
		rank --;
	}

	return pStatistic;
}



// Réinitialise les statistiques.

void CDecor::StatisticInit()
{
	Statistic*	pStatistic;

	pStatistic = table_statistic;
	while ( pStatistic->bExist )
	{
		pStatistic->lastShow = 0;
		pStatistic ++;
	}

	m_statNb    = 0;
	m_statFirst = 0;
	m_bStatUp   = FALSE;
	m_bStatDown = FALSE;
	m_statHili  = -1;
	m_bStatRecalc = TRUE;  // faudra tout recalculer
	m_bStatRedraw = TRUE;  // faudra tout redessiner
}


// Met à jour tous les compteurs des statistiques.

void CDecor::StatisticUpdate()
{
	int			rank, x, y, icon, nb;
	BOOL		bHach;
	Statistic*	pStatistic;

	m_nbStatHach        = 0;
	m_nbStatHachBlupi   = 0;
	m_nbStatHachPlanche = 0;
	m_nbStatHachTomate  = 0;
	m_nbStatHachMetal   = 0;
	m_nbStatHachRobot   = 0;
	m_nbStatHome        = 0;
	m_nbStatHomeBlupi   = 0;
	m_nbStatRobots      = 0;

	pStatistic = table_statistic;
	while ( pStatistic->bExist )
	{
		pStatistic->nb = 0;
		pStatistic ++;
	}

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist )
		{
			if ( m_blupi[rank].perso == 0 )  // blupi ?
			{
				if ( m_blupi[rank].bMalade )
				{
					table_statistic[STATBLUPIm].nb ++;
				}
				else
				{
					if ( m_blupi[rank].energy <= MAXENERGY/4 )
					{
						table_statistic[STATBLUPIf].nb ++;
					}
					else
					{
						table_statistic[STATBLUPI].nb ++;
					}
				}
				x = (m_blupi[rank].cel.x/2)*2;
				y = (m_blupi[rank].cel.y/2)*2;
				if ( m_decor[x/2][y/2].floorChannel == CHFLOOR &&
					 m_decor[x/2][y/2].floorIcon    == 17 )  // dalle hachurée ?
				{
					m_nbStatHachBlupi ++;
				}
				if ( m_decor[x/2][y/2].objectChannel == CHOBJECT &&
					 m_decor[x/2][y/2].objectIcon    == 113 )  // maison ?
				{
					m_nbStatHomeBlupi ++;
				}
			}
			if ( m_blupi[rank].perso == 8 )  // disciple ?
			{
				table_statistic[STATDISCIPLE].nb ++;
			}
			if ( m_blupi[rank].perso == 4 )  // robot ?
			{
				table_statistic[STATROBOT].nb ++;
				m_nbStatRobots ++;
				x = (m_blupi[rank].cel.x/2)*2;
				y = (m_blupi[rank].cel.y/2)*2;
				if ( m_decor[x/2][y/2].floorChannel == CHFLOOR &&
					 m_decor[x/2][y/2].floorIcon    == 17 )  // dalle hachurée ?
				{
					m_nbStatHachRobot ++;
				}
			}
			if ( m_blupi[rank].perso == 3 )  // tracks ?
			{
				table_statistic[STATTRACKS].nb ++;
				if ( !m_term.bHachRobot )  // pas robot sur hachures ?
				{
					m_nbStatRobots ++;
				}
			}
			if ( m_blupi[rank].perso == 1 )  // araignée ?
			{
				table_statistic[STATARAIGNEE].nb ++;
				if ( !m_term.bHachRobot )  // pas robot sur hachures ?
				{
					m_nbStatRobots ++;
				}
			}
			if ( m_blupi[rank].perso == 2 )  // virus ?
			{
				table_statistic[STATVIRUS].nb ++;
			}
			if ( m_blupi[rank].perso == 5 )  // bombe ?
			{
				table_statistic[STATBOMBE].nb ++;
				if ( !m_term.bHachRobot )  // pas robot sur hachures ?
				{
					m_nbStatRobots ++;
				}
			}
			if ( m_blupi[rank].perso == 7 )  // électro ?
			{
				table_statistic[STATELECTRO].nb ++;
				if ( !m_term.bHachRobot )  // pas robot sur hachures ?
				{
					m_nbStatRobots ++;
				}
			}
		}
	}

	for ( x=0 ; x<MAXCELX ; x+=2 )
	{
		for ( y=0 ; y<MAXCELY ; y+=2 )
		{
			bHach = FALSE;
			if ( m_decor[x/2][y/2].floorChannel == CHFLOOR &&
				 m_decor[x/2][y/2].floorIcon    == 17 )  // dalle hachurée ?
			{
				bHach = TRUE;
				m_nbStatHach ++;
			}

			if ( m_decor[x/2][y/2].objectChannel == CHOBJECT &&
				 m_decor[x/2][y/2].objectIcon    == 113 )  // maison ?
			{
				m_nbStatHome ++;
			}

			if ( m_decor[x/2][y/2].objectChannel == CHOBJECT )
			{
				icon = m_decor[x/2][y/2].objectIcon;

				pStatistic = table_statistic;
				while ( pStatistic->bExist )
				{
					if ( pStatistic->perso == -1 &&
						 pStatistic->firstIcon > 0 &&
						 icon >= pStatistic->firstIcon &&
						 icon <= pStatistic->lastIcon )
					{
						pStatistic->nb ++;
						break;
					}
					pStatistic ++;
				}

				if ( icon == 36 && bHach )  // planches ?
				{
					m_nbStatHachPlanche ++;
				}
				if ( icon == 60 && bHach )  // tomates ?
				{
					m_nbStatHachTomate ++;
				}
				if ( icon == 14 && bHach )  // métal ?
				{
					m_nbStatHachMetal ++;
				}
			}

			if ( m_decor[x/2][y/2].floorChannel == CHFLOOR )
			{
				icon = m_decor[x/2][y/2].floorIcon;

				if ( (icon >= 52 && icon <= 56) ||  // couveuse ?
					 (icon >= 80 && icon <= 84) )   // téléporteur ?
				{
					pStatistic = table_statistic;
					while ( pStatistic->bExist )
					{
						if ( pStatistic->perso == -1 &&
							 pStatistic->firstIcon < 0 &&
							 icon >= -(pStatistic->firstIcon) &&
							 icon <= -(pStatistic->lastIcon) )
						{
							pStatistic->nb ++;
							break;
						}
						pStatistic ++;
					}
				}
			}

			if ( m_decor[x/2][y/2].fire > 0 &&
				 m_decor[x/2][y/2].fire < MoveMaxFire() )
			{
				table_statistic[STATFEU].nb ++;  // un feu de plus
			}
		}
	}

	pStatistic = table_statistic;
	m_statNb = 0;
	while ( pStatistic->bExist )
	{
		if ( pStatistic->nb > 0 )
		{
			m_statNb ++;
		}
		pStatistic ++;
	}
	if ( m_statNb <= STATNB )  // tout visible en une page ?
	{
		m_bStatUp   = FALSE;
		m_bStatDown = FALSE;
		m_statFirst = 0;
	}
	else
	{
		// nb <- nb de pages nécessaires
		nb = (m_statNb+STATNB-5)/(STATNB-2);

		m_bStatUp   = TRUE;
		m_bStatDown = TRUE;
		if ( m_statFirst >= 1+(nb-1)*(STATNB-2) )
		{
			m_statFirst = 1+(nb-1)*(STATNB-2);
			m_bStatDown = FALSE;
		}
		if ( m_statFirst == 0 )
		{
			m_bStatUp = FALSE;
		}
	}

	m_bStatRecalc = FALSE;  // c'est calculé
	m_bStatRedraw = TRUE;   // faudra tout redessiner
}

// Retourne le nombre de blupi.

int CDecor::StatisticGetBlupi()
{
	return table_statistic[STATBLUPIf].nb +
		   table_statistic[STATBLUPIm].nb +
		   table_statistic[STATBLUPI].nb;
}

// Retourne le nombre de cellules en feu.

int CDecor::StatisticGetFire()
{
	return table_statistic[STATFEU].nb;
}

// Dessine toutes les statistiques.

void CDecor::StatisticDraw()
{
	POINT		pos;
	RECT		rect;
	int			rank, icon, nb, textRes;
	Statistic*	pStatistic;
	char		text[50];

	pStatistic = table_statistic;

	while ( pStatistic->nb == 0 )
	{
		pStatistic ++;
	}

	nb = m_statFirst;
	while ( nb > 0 )
	{
		if ( pStatistic->bExist )  pStatistic ++;
		while ( pStatistic->nb == 0 )
		{
			pStatistic ++;
		}
		nb --;
	}

	textRes = 0;
	for ( rank=0 ; rank<STATNB ; rank++ )
	{
		pos.x = POSSTATX+DIMSTATX*(rank/(STATNB/2));
		pos.y = POSSTATY+DIMSTATY*(rank%(STATNB/2));
		rect.left   = pos.x;
		rect.right  = pos.x+DIMSTATX;
		rect.top    = pos.y;
		rect.bottom = pos.y+DIMSTATY;

		m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond

		if ( rank == 0 && m_bStatUp )
		{
			icon = 6+66;  // flèche up
			if ( rank == m_statHili )  // statistique survolée ?
			{
				icon ++;
			}
			pos.x -= 3;
			pos.y -= 5;
			if ( pStatistic->drawIcon == 68 )  pos.x += 26;
			m_pPixmap->DrawIcon(-1, CHBUTTON, icon, pos);  // flèche up
			continue;
		}

		if ( rank == STATNB-1 && m_bStatDown )
		{
			icon = 6+68;  // flèche down
			if ( rank == m_statHili )  // statistique survolée ?
			{
				icon ++;
			}
			pos.x += 23;
			pos.y -= 5;
			m_pPixmap->DrawIcon(-1, CHBUTTON, icon, pos);  // flèche down
			continue;
		}

		if ( !pStatistic->bExist )  goto next;

		icon = 6+pStatistic->drawIcon;

		if ( rank == m_statHili )  // statistique survolée ?
		{
			m_pPixmap->DrawIconDemi(-1, CHBLUPI, ICON_HILI_STAT, pos);
			textRes = pStatistic->text;
		}

		if ( pStatistic->nb > 0 )
		{
			pos.x -= 3;
			pos.y -= 5;
			m_pPixmap->DrawIcon(-1, CHBUTTON, icon, pos);

			nb = pStatistic->nb;
			sprintf(text, "%d", nb);
			pos.x += 3+34;
			pos.y += 5+7;
			DrawText(m_pPixmap, pos, text);
		}

		next:
		if ( pStatistic->bExist )  pStatistic ++;
		while ( pStatistic->nb == 0 )
		{
			pStatistic ++;
		}
	}

	// Dans un bouton stop/setup/write ?
	if ( textRes == 0 && m_statHili >= 100 )
	{
		if ( m_statHili == 100 )  textRes = TX_BUTTON_PLAY_STOP;
		if ( m_statHili == 101 )  textRes = TX_BUTTON_PLAY_SETUP;
		if ( m_statHili == 102 )  textRes = TX_BUTTON_PLAY_WRITE;
	}

	// Dessine le nom de la statistique survolée.
	pos.x = 0;
	pos.y = 404;
	rect.left   = pos.x;
	rect.right  = pos.x+POSDRAWX;
	rect.top    = pos.y;
	rect.bottom = pos.y+16;
	m_pPixmap->DrawPart(-1, CHBACK, pos, rect, 1);  // dessine le fond

	if ( textRes != 0 )
	{
		LoadString(textRes, text, 50);
		nb = GetTextWidth(text);
		pos.x += (POSDRAWX-nb)/2;
		DrawText(m_pPixmap, pos, text);
	}

	m_bStatRedraw = FALSE;  // dessin plus nécessaire
}

// Génère les statistiques.

void CDecor::GenerateStatictic()
{
	if ( m_bBuild )  return;

	if ( m_bStatRecalc || m_phase%20 == 10 )
	{
		StatisticUpdate();  // met à jour les compteurs
	}

	if ( m_bStatRedraw )
	{
		StatisticDraw();  // redessine tout
	}
}


// Bouton pressé dans les statistiques.

BOOL CDecor::StatisticDown(POINT pos, int fwKeys)
{
	int			hili, rank, x, y, show, icon;
	POINT		cel;
	Statistic*	pStatistic;

	StatisticUpdate();

	hili = StatisticDetect(pos);
	if ( hili < 0 )  return FALSE;

	if ( m_bStatUp && hili == 0 )  // flèche up ?
	{
		m_statFirst -= STATNB-2;
		if ( m_statFirst < STATNB-1 )
		{
			m_statFirst = 0;
		}
		StatisticUpdate();
		pos.x = LXIMAGE/2;
		pos.y = LYIMAGE/2;
		m_pSound->PlayImage(SOUND_OPEN, pos);
		return TRUE;
	}

	if ( m_bStatDown && hili == STATNB-1 )  // flèche down ?
	{
		if ( m_statFirst == 0 )
		{
			m_statFirst = STATNB-1;
		}
		else
		{
			m_statFirst += STATNB-2;
		}
		StatisticUpdate();
		pos.x = LXIMAGE/2;
		pos.y = LYIMAGE/2;
		m_pSound->PlayImage(SOUND_OPEN, pos);
		return TRUE;
	}

	rank = m_statFirst+hili;
	if ( rank > 0 && m_bStatUp )  rank --;
	pStatistic = StatisticGet(rank);
	if ( !pStatistic->bExist )  return FALSE;

	show = pStatistic->lastShow % pStatistic->nb;
	pStatistic->lastShow ++;

	if ( pStatistic->perso >= 0 )  // blupi/araignée ?
	{
		for ( rank=0 ; rank<MAXBLUPI ; rank++ )
		{
			if ( m_blupi[rank].bExist )
			{
				if ( m_blupi[rank].perso !=
					 pStatistic->perso )  continue;

				if ( m_blupi[rank].perso != 0 ||
					 (m_blupi[rank].bMalade &&
					  pStatistic->drawIcon == 76) ||  // malade ?
					 (!m_blupi[rank].bMalade &&
					  m_blupi[rank].energy <= MAXENERGY/4 &&
					  pStatistic->drawIcon == 13) ||  // fatigué ?
					 (m_blupi[rank].energy > MAXENERGY/4 &&
					  pStatistic->drawIcon == 14) )  // énergique ?
				{
					if ( show == 0 )
					{
						if ( m_blupi[rank].perso == 0 ||  // blupi ?
							 m_blupi[rank].perso == 8 )   // disciple ?
						{
							BlupiDeselect();
							m_blupi[rank].bHili = TRUE;
							m_rankBlupiHili = rank;  // sélectionne
							m_nbBlupiHili = 1;
						}
						BlupiSetArrow(rank, TRUE);
						cel = m_blupi[rank].cel;
						goto select;
					}
					show --;
				}
			}
		}
	}

	if ( pStatistic->perso == -1 &&  // objet ?
		 pStatistic->firstIcon > 0 )
	{
		for ( x=0 ; x<MAXCELX ; x+=2 )
		{
			for ( y=0 ; y<MAXCELY ; y+=2 )
			{
				if ( m_decor[x/2][y/2].objectChannel == CHOBJECT )
				{
					icon = m_decor[x/2][y/2].objectIcon;

					if ( icon >= pStatistic->firstIcon &&
						 icon <= pStatistic->lastIcon )
					{
						if ( show == 0 )
						{
							cel = GetCel(x,y);
							if ( pStatistic->bBigIcon )
							{
								// Flèche plus haute.
								m_celArrow = GetCel(cel,-2,-2);
							}
							else
							{
								m_celArrow = cel;
							}
							goto select;
						}
						show --;
					}
				}
			}
		}
	}

	if ( pStatistic->perso == -1 &&  // sol ?
		 pStatistic->firstIcon < 0 )
	{
		for ( x=0 ; x<MAXCELX ; x+=2 )
		{
			for ( y=0 ; y<MAXCELY ; y+=2 )
			{
				if ( m_decor[x/2][y/2].floorChannel == CHFLOOR )
				{
					icon = m_decor[x/2][y/2].floorIcon;

					if ( icon >= -(pStatistic->firstIcon) &&
						 icon <= -(pStatistic->lastIcon) )
					{
						if ( show == 0 )
						{
							cel = GetCel(x,y);
							if ( pStatistic->bBigIcon )
							{
								// Flèche plus haute.
								m_celArrow = GetCel(cel,-2,-2);
							}
							else
							{
								m_celArrow = cel;
							}
							goto select;
						}
						show --;
					}
				}
			}
		}
	}

	if ( pStatistic->perso == -2 )  // feu ?
	{
		for ( x=0 ; x<MAXCELX ; x+=2 )
		{
			for ( y=0 ; y<MAXCELY ; y+=2 )
			{
				if ( m_decor[x/2][y/2].fire > 0 &&
					 m_decor[x/2][y/2].fire < MoveMaxFire() )
				{
					if ( show == 0 )
					{
						cel = GetCel(x,y);
						m_celArrow = cel;
						goto select;
					}
					show --;
				}
			}
		}
	}

	return FALSE;

	select:
	SetCoin(cel, TRUE);
	NextPhase(0);  // faudra refaire la carte tout de suite
	return TRUE;
}

// Souris déplacée dans les statistiques.

BOOL CDecor::StatisticMove(POINT pos, int fwKeys)
{
	int		rank;

	rank = StatisticDetect(pos);

	if ( rank != m_statHili )  // autre mise en évidence ?
	{
		m_statHili = rank;
		m_bStatRedraw = TRUE;  // faudra tout redessiner
	}

	return FALSE;
}

// Bouton relâché dans les statistiques.

BOOL CDecor::StatisticUp(POINT pos, int fwKeys)
{
	return FALSE;
}

// Détecte dans quelle statistique est la souris.

int CDecor::StatisticDetect(POINT pos)
{
	int		rank;

	// Dans un bouton stop/setup/write ?
	if ( pos.x >= 10 && pos.x <= 10+42*3 &&
		 pos.y >= 422 && pos.y <= 422+40 )
	{
		pos.x -= 10;
		if ( pos.x%42 > 40 )  return -1;
		return 100+pos.x/42;
	}

	if ( pos.x >= POSSTATX && pos.x <= POSSTATX+DIMSTATX*2 &&
		 pos.y >= POSSTATY && pos.y <= POSSTATY+DIMSTATY*(STATNB/2) )
	{
		rank  = ((pos.x-POSSTATX)/DIMSTATX)*(STATNB/2);
		rank += ((pos.y-POSSTATY)/DIMSTATY);
		if ( rank >= STATNB )  return -1;

		return rank;
	}

	return -1;
}

