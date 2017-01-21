// CDecor.cpp
//

#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include "def.h"
#include "resource.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "action.h"
#include "text.h"
#include "misc.h"
#include "fifo.h"
#include "DECMOVE.h"


/////////////////////////////////////////////////////////////////////////////



#define TEXTDELAY		10		// délai avant apparition tooltips

POINT GetCel(int x, int y)
{
	POINT	cel;

	cel.x = x;
	cel.y = y;

	return cel;
}

POINT GetCel(POINT cel, int x, int y)
{
	cel.x += x;
	cel.y += y;

	return cel;
}

// Indique si une coordonnée de cellule est valide.
// On ne peut pas aller dans la dernière cellule tout au
// bord (-2) pour permettre de gérer le brouillard proprement
// jusque dans les bords !

BOOL IsValid(POINT cel)
{
	if ( cel.x < 2 || cel.x >= MAXCELX-2 ||
		 cel.y < 2 || cel.y >= MAXCELX-2 )  return FALSE;
	return TRUE;
}

// Retourne un vecteur orienté dans une direction donnée.

POINT GetVector(int direct)
{
	POINT	vector;

	vector.x = 0;
	vector.y = 0;

	switch ( direct )
	{
		case DIRECT_E:
			vector.x = +1;
			break;
		case DIRECT_SE:
			vector.x = +1;
			vector.y = +1;
			break;
		case DIRECT_S:
			vector.y = +1;
			break;
		case DIRECT_SO:
			vector.x = -1;
			vector.y = +1;
			break;
		case DIRECT_O:
			vector.x = -1;
			break;
		case DIRECT_NO:
			vector.x = -1;
			vector.y = -1;
			break;
		case DIRECT_N:
			vector.y = -1;
			break;
		case DIRECT_NE:
			vector.x = +1;
			vector.y = -1;
			break;
	}
	return vector;
}

// Constructeur.

CDecor::CDecor()
{
	m_hWnd   = NULL;
	m_pSound = NULL;
	m_pUndoDecor = NULL;

	m_celCoin.x = 90;
	m_celCoin.y = 98;

	m_celHili.x     = -1;
	m_celOutline1.x = -1;
	m_celOutline2.x = -1;

	m_bHiliRect = FALSE;  // pas de rectangle de sélection
	m_shiftHili = 0;

	m_shiftOffset.x = 0;
	m_shiftOffset.y = 0;

	m_nbBlupiHili   = 0;
	m_rankBlupiHili = -1;
	m_rankHili      = -1;

	m_bFog          = FALSE;
	m_bBuild        = FALSE;
	m_bInvincible   = FALSE;
	m_bSuper        = FALSE;
	m_bHideTooltips = FALSE;
	m_bInfo         = FALSE;
	m_infoHeight    = 100;
	m_phase         = 0;
	m_totalTime     = 0;
	m_region        = 0;
	m_lastRegion    = 999;
	m_skill         = 0;

	Init(CHFLOOR, 0);
	BlupiFlush();
	MoveFlush();
	InitDrapeau();
}

// Destructeur.

CDecor::~CDecor()
{
	UndoClose();  // libère le buffer du undo
}


// Initialisation générale.

void CDecor::Create(HWND hWnd, CSound *pSound, CPixmap *pPixmap)
{
	m_hWnd     = hWnd;
	m_pSound   = pSound;
	m_pPixmap  = pPixmap;
	m_bOutline = FALSE;
}

// Initialise le décor avec un sol plat partout.

void CDecor::Init(int channel, int icon)
{
	int		x, y;

	for ( x=0 ; x<MAXCELX/2 ; x++ )
	{
		for ( y=0 ; y<MAXCELY/2 ; y++ )
		{
			m_decor[x][y].floorChannel  = channel;
			m_decor[x][y].floorIcon     = icon;

			m_decor[x][y].objectChannel = -1;
			m_decor[x][y].objectIcon    = -1;

			m_decor[x][y].fog           =  FOGHIDE;  // caché
			m_decor[x][y].rankMove      = -1;
			m_decor[x][y].workBlupi     = -1;
			m_decor[x][y].fire          = 0;
		}
	}

	for ( x=0 ; x<MAXCELX ; x++ )
	{
		for ( y=0 ; y<MAXCELY ; y++ )
		{
			m_rankBlupi[x][y] = -1;
		}
	}

	m_bOutline = FALSE;
	m_bGroundRedraw = TRUE;
}

// Initialise le décor après une modification.

void CDecor::InitAfterBuild()
{
	ClearFog();  // met tout sous le brouillard
	ClearFire();
	MoveFixInit();
	InitDrapeau();
	BlupiDeselect();
}

// Initialise les mises en évidence, avant de jouer.

void CDecor::ResetHili()
{
	m_bHiliRect = FALSE;  // plus de rectangle
	InitOutlineRect();
}

// Charge les images nécessaires au décor.

BOOL CDecor::LoadImages()
{
	POINT		totalDim, iconDim;
	char		filename[50];

	if ( m_region == m_lastRegion )  return TRUE;
	m_lastRegion = m_region;

	totalDim.x = DIMCELX*2*16;
	totalDim.y = DIMCELY*2*6;
	iconDim.x = DIMCELX*2;
	iconDim.y = DIMCELY*2;
	sprintf(filename, "image\\floor%.3d.blp", m_region);
	if ( !m_pPixmap->Cache(CHFLOOR, filename, totalDim, iconDim, FALSE) )
		return FALSE;
	m_pPixmap->SetTransparent(CHFLOOR, RGB(0,0,255));  // bleu

	totalDim.x = DIMOBJX*16;
	totalDim.y = DIMOBJY*8;
	iconDim.x = DIMOBJX;
	iconDim.y = DIMOBJY;
	sprintf(filename, "image\\obj%.3d.blp", m_region);
	if ( !m_pPixmap->Cache(CHOBJECT, filename, totalDim, iconDim, FALSE) )
		return FALSE;
	m_pPixmap->SetTransparent(CHOBJECT, RGB(0,0,255));  // bleu

	sprintf(filename, "image\\obj-o%.3d.blp", m_region);
	if ( !m_pPixmap->Cache(CHOBJECTo, filename, totalDim, iconDim, FALSE) )
		return FALSE;
	m_pPixmap->SetTransparent(CHOBJECTo, RGB(255,255,255));  // blanc

	MapInitColors();  // init les couleurs pour la carte

	m_bGroundRedraw = TRUE;
	return TRUE;
}

// Met partout du brouillard, sauf aux endroits des blupi.

void CDecor::ClearFog()
{
	int		x, y, rank;

	for ( x=0 ; x<MAXCELX/2 ; x++ )
	{
		for ( y=0 ; y<MAXCELY/2 ; y++ )
		{
			m_decor[x][y].fog = FOGHIDE;  // caché
		}
	}

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist )
		{
			BlupiPushFog(rank);
		}
	}

	m_bOutline = FALSE;
}

// Permet de nouveau aux cellules brulées de bruler.

void CDecor::ClearFire()
{
	int		x, y;

	for ( x=0 ; x<MAXCELX/2 ; x++ )
	{
		for ( y=0 ; y<MAXCELY/2 ; y++ )
		{
			if ( m_decor[x][y].fire >= MoveMaxFire() )  // déjà brulé ?
			{
				m_decor[x][y].fire = 0;  // pourra de nouveau bruler
			}

			if ( m_decor[x][y].fire > 1 )  // en train de bruler ?
			{
				m_decor[x][y].fire = 1;  // début du feu
			}
		}
	}
}

// Indique le mode jeu/construction.

void CDecor::SetBuild(BOOL bBuild)
{
	m_bBuild = bBuild;
}

// Indique s'il faut tenir compte du brouillard.

void CDecor::EnableFog(BOOL bEnable)
{
	m_bFog     = bEnable;
	m_bOutline = FALSE;
}

// Gestion du mode invincible.

BOOL CDecor::GetInvincible()
{
	return m_bInvincible;
}

void CDecor::SetInvincible(BOOL bInvincible)
{
	m_bInvincible = bInvincible;
}

// Gestion du mode costaud (superblupi).

BOOL CDecor::GetSuper()
{
	return m_bSuper;
}

void CDecor::SetSuper(BOOL bSuper)
{
	m_bSuper = bSuper;
}

// Bascule le mode outline.

void CDecor::FlipOutline()
{
	m_bOutline = !m_bOutline;
	m_timeFlipOutline = m_timeConst+50;
}

// Initialise un sol dans une cellule.

BOOL CDecor::PutFloor(POINT cel, int channel, int icon)
{
	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	m_decor[cel.x/2][cel.y/2].floorChannel = channel;
	m_decor[cel.x/2][cel.y/2].floorIcon    = icon;

	m_bGroundRedraw = TRUE;

//?	SubDrapeau(cel);  // on pourra de nouveau planter un drapeau

	return TRUE;
}

// Initialise un objet dans une cellule.

BOOL CDecor::PutObject(POINT cel, int channel, int icon)
{
	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	if ( icon == -1 )  channel = -1;

	m_decor[cel.x/2][cel.y/2].objectChannel = channel;
	m_decor[cel.x/2][cel.y/2].objectIcon    = icon;

	SubDrapeau(cel);  // on pourra de nouveau planter un drapeau

	return TRUE;
}

// Retourne un sol dans une cellule.

BOOL CDecor::GetFloor(POINT cel, int &channel, int &icon)
{
	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	channel = m_decor[cel.x/2][cel.y/2].floorChannel;
	icon    = m_decor[cel.x/2][cel.y/2].floorIcon;

	return TRUE;
}

// Retourne une objet dans une cellule.

BOOL CDecor::GetObject(POINT cel, int &channel, int &icon)
{
	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	channel = m_decor[cel.x/2][cel.y/2].objectChannel;
	icon    = m_decor[cel.x/2][cel.y/2].objectIcon;

	return TRUE;
}

// Modifie le feu pour une cellule.

BOOL CDecor::SetFire(POINT cel, BOOL bFire)
{
	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	m_decor[cel.x/2][cel.y/2].fire = bFire?1:0;

	return TRUE;
}


// Modifie l'offset pour le shift.

void CDecor::SetShiftOffset(POINT offset)
{
	m_shiftOffset = offset;
	m_bGroundRedraw = TRUE;
}

// Convertit la position d'une cellule en coordonnée graphique.

POINT CDecor::ConvCelToPos(POINT cel)
{
	POINT	pos;

	pos.x = ((cel.x-m_celCoin.x)-(cel.y-m_celCoin.y))*(DIMCELX/2);
	pos.y = ((cel.x-m_celCoin.x)+(cel.y-m_celCoin.y))*(DIMCELY/2);

	pos.x += POSDRAWX+m_shiftOffset.x;
	pos.y += POSDRAWY+m_shiftOffset.y;

	return pos;
}

// Convertit une coordonnée graphique en cellule.

POINT CDecor::ConvPosToCel(POINT pos, BOOL bMap)
{
	POINT	cel;

	if ( bMap &&
		 pos.x >= POSMAPX && pos.x < POSMAPX+DIMMAPX &&
		 pos.y >= POSMAPY && pos.y < POSMAPY+DIMMAPY )
	{
		pos.x -= POSMAPX;
		pos.y -= POSMAPY;
		return ConvMapToCel(pos);
	}

	pos.x -= POSDRAWX+DIMCELX/2;
	pos.y -= POSDRAWY;

	cel.x = (pos.y*DIMCELX + pos.x*DIMCELY) / (DIMCELX*DIMCELY);
//	cel.y = (pos.y*DIMCELX - pos.x*DIMCELY) / (DIMCELX*DIMCELY);
	cel.y = (pos.y*DIMCELX - pos.x*DIMCELY);
	if ( cel.y < 0 )  cel.y -= (DIMCELX*DIMCELY);
	cel.y /= (DIMCELX*DIMCELY);

	cel.x += m_celCoin.x;
	cel.y += m_celCoin.y;

	return cel;
}

// Convertit une coordonnée graphique en grande cellule (2x2).

POINT CDecor::ConvPosToCel2(POINT pos)
{
	POINT	cel;

	pos.x -= POSDRAWX+DIMCELX/2;
	pos.y -= POSDRAWY;

	if ( m_celCoin.x%2 != 0 && m_celCoin.y%2 == 0 )
	{
		pos.x += DIMCELX/2;
		pos.y += DIMCELY/2;
	}

	if ( m_celCoin.x%2 == 0 && m_celCoin.y%2 != 0 )
	{
		pos.x -= DIMCELX/2;
		pos.y += DIMCELY/2;
	}

	if ( m_celCoin.x%2 != 0 && m_celCoin.y%2 != 0 )
	{
		pos.y += DIMCELY;
	}

	cel.x = (pos.y*DIMCELX*2 + pos.x*DIMCELY*2) / (DIMCELX*2*DIMCELY*2);
//	cel.y = (pos.y*DIMCELX*2 - pos.x*DIMCELY*2) / (DIMCELX*2*DIMCELY*2);
	cel.y = (pos.y*DIMCELX*2 - pos.x*DIMCELY*2);
	if ( cel.y < 0 )  cel.y -= (DIMCELX*2*DIMCELY*2);
	cel.y /= (DIMCELX*2*DIMCELY*2);

	cel.x = (cel.x*2+m_celCoin.x)/2*2;
	cel.y = (cel.y*2+m_celCoin.y)/2*2;

	return cel;
}



// Attribution des blupi aux différentes cellules.
// Lorsque un blupi a deux positions (courante et destination),
// il faut toujours mettre blupi le plus au fond possible
// (minimiser x et y).

void CDecor::BuildPutBlupi()
{
	int		x, y, dx, dy, xMin, yMin, rank, clipLeft;
	POINT	pos;

#if 0  // déjà fait au point (1), voir Build
	for ( x=0 ; x<MAXCELX ; x++ )
	{
		for ( y=0 ; y<MAXCELY ; y++ )
		{
			m_rankBlupi[x][y] = -1;
		}
	}
#endif

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist        &&
			 m_blupi[rank].channel != -1 &&
			 m_blupi[rank].icon    != -1 )
		{
			xMin = m_blupi[rank].destCel.x;
			if ( xMin > m_blupi[rank].cel.x )  xMin = m_blupi[rank].cel.x;
			yMin = m_blupi[rank].destCel.y;
			if ( yMin > m_blupi[rank].cel.y )  yMin = m_blupi[rank].cel.y;

			// Si blupi entre dans une maison, il faut initialiser
			// le clipping à gauche.
			m_blupi[rank].clipLeft = 0;  // pas de clipping

			if ( !m_bOutline &&
				 xMin > 0 && xMin%2 == 1 && yMin%2 == 1 &&
				 m_decor[xMin/2][yMin/2].objectChannel == CHOBJECT &&
				 (m_decor[xMin/2][yMin/2].objectIcon == 28  ||  // maison ?
				  m_decor[xMin/2][yMin/2].objectIcon == 101 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 103 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 105 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 116 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 120 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon ==  18 ||  // usine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 122 ||  // mine ?
				  m_decor[xMin/2][yMin/2].objectIcon == 113) && // maison ?
				 m_blupi[rank].posZ > -DIMBLUPIY )
			{
				pos = ConvCelToPos(GetCel(xMin,yMin));
				clipLeft = pos.x+34;
				if ( clipLeft < POSDRAWX )  clipLeft = POSDRAWX;
				m_blupi[rank].clipLeft = clipLeft;
			}

			x = m_blupi[rank].cel.x;
			y = m_blupi[rank].cel.y;

			dx = m_blupi[rank].destCel.x - x;
			dy = m_blupi[rank].destCel.y - y;

			if ( dx != -dy )  // déplacement non horizontal (ne/so) ?
			{
				if ( dx < 0 )  x = m_blupi[rank].destCel.x;
				if ( dy < 0 )  y = m_blupi[rank].destCel.y;
			}
			if ( dx == -1 && dy == 1 )  // déplacement "so" ?
			{
				x = m_blupi[rank].destCel.x;
				y = m_blupi[rank].destCel.y;
			}

			if ( x%2 != 0 )
			{
				if (  IsFreeCelObstacle(GetCel(x,y+0)) &&
					 !IsFreeCelObstacle(GetCel(x,y+1)) )  x --;
			}

			if ( x%2 == 0 && y%2 != 0 )
			{
				if ( !IsFreeCelObstacle(GetCel(x+1,y)) )  y --;
			}

			if ( x%2 != 0 && y%2 != 0 && dx != 0 && dy == 0 )
			{
				if ( !IsFreeCelObstacle(GetCel(x+1,y-1)) )  x ++;
			}

			if ( m_rankBlupi[x][y] != -1 )  // déjà occupé ?
			{
				if ( x == m_blupi[rank].cel.x )
				{
					x --;
				}
				else
				{
					x = m_blupi[rank].cel.x;
				}

				if ( m_rankBlupi[x][y] != -1 )  // déjà occupé ?
				{
					if ( y == m_blupi[rank].cel.y )
					{
						y --;
					}
					else
					{
						y = m_blupi[rank].cel.y;
					}

					if ( m_rankBlupi[x][y] != -1 )  // déjà occupé ?
					{
//?						OutputDebug(">>> Manque un blupi <<<\n");
						continue;  // que faire d'autre ?
					}
				}
			}
			m_rankBlupi[x][y] = rank;
		}
	}
}

// Dessine une cellule du décor contenant un sol animé.

void CDecor::BuildMoveFloor(int x, int y, POINT pos, int rank)
{
	int		icon, nb;
	short*	pTable;

	if ( m_move[rank].rankIcons == 0 )
	{
		icon = m_move[rank].maskIcon+m_move[rank].cTotal;

		m_pPixmap->BuildIconMask(m_move[rank].maskChannel,
								 icon,
								 m_move[rank].channel,
								 m_move[rank].icon, 0);

		m_pPixmap->DrawIcon(-1, m_move[rank].channel, 0,
							pos, 0, TRUE);
	}
	else
	{
		pTable = GetListIcons(m_move[rank].rankIcons);
		nb = pTable[0];
		icon = pTable[1+m_move[rank].cTotal%nb];

		if ( m_move[rank].cel.x%2 == 1 )
		{
			pos.x += DIMCELX/2;
			pos.y += DIMCELY/2;
		}
		if ( m_move[rank].cel.y%2 == 1 )
		{
			pos.x -= DIMCELX/2;
			pos.y += DIMCELY/2;
		}

		m_pPixmap->DrawIcon(-1, m_move[rank].channel, icon, pos);
	}
}

// Dessine une cellule du décor contenant un objet animé.

void CDecor::BuildMoveObject(int x, int y, POINT pos, int rank)
{
	int		hBuild, offset, startY, endY;
	int		channel, icon, nb;
	short*	pTable;

	if ( m_move[rank].rankMoves != 0 )
	{
		pTable = GetListMoves(m_move[rank].rankMoves);
		offset = m_move[rank].phase;
		if ( offset < pTable[0] )
		{
			pos.x += pTable[1+2*offset+0];
			pos.y += pTable[1+2*offset+1];
		}
		else
		{
			m_move[rank].rankMoves = 0;
		}
	}

	// Dessine un chiffre par-dessus
	if ( m_move[rank].icon >= MOVEICONNB     &&
		 m_move[rank].icon <= MOVEICONNB+100 )
	{
		POINT	textPos;
		char	string[20];

		m_pPixmap->DrawIcon(-1, m_decor[x/2][y/2].objectChannel,
								m_decor[x/2][y/2].objectIcon,
								pos);
		sprintf(string, "%d", m_move[rank].icon-MOVEICONNB);
		textPos.x = pos.x+DIMCELX/2+32;
		textPos.y = pos.y+(DIMOBJY-DIMCELY*2)+36;
		DrawTextCenter(m_pPixmap, textPos, string, FONTLITTLE);
	}
	else
	{
		hBuild = (m_move[rank].cTotal * m_move[rank].stepY) /100;
		if ( m_move[rank].stepY >= 0 )
		{
			if ( hBuild <= 0      )  hBuild = 0;
			if ( hBuild > DIMOBJY )  hBuild = DIMOBJY;
		}
		else
		{
			if ( hBuild >= 0       )  hBuild = 0;
			if ( hBuild < -DIMOBJY )  hBuild = -DIMOBJY;
		}

		// Dessine l'objet actuellement dans le décor.
		if ( m_decor[x/2][y/2].objectChannel >= 0 )
		{
			if ( hBuild >= 0 )
			{
				startY = 0;
				endY   = DIMOBJY-hBuild;
			}
			else
			{
				startY = -hBuild;
				endY   = DIMOBJY;
			}

			channel = m_decor[x/2][y/2].objectChannel;
			if ( m_bOutline && channel == CHOBJECT )
			{
				channel = CHOBJECTo;
			}

			m_pPixmap->DrawIconPart(-1, channel,
									m_decor[x/2][y/2].objectIcon,
									pos, startY, endY);
		}

		// Dessine le nouvel objet par-dessus.
		if ( m_move[rank].icon >= 0 )
		{
			if ( hBuild >= 0 )
			{
				startY = DIMOBJY-hBuild;
				endY   = DIMOBJY;
			}
			else
			{
				startY = 0;
				endY   = -hBuild;
			}

			channel = m_move[rank].channel;
			if ( m_bOutline && channel == CHOBJECT )
			{
				channel = CHOBJECTo;
			}

			m_pPixmap->DrawIconPart(-1, channel, m_move[rank].icon,
									pos, startY, endY);
		}
	}

	// Dessine le feu ou les rayons.
	if ( m_move[rank].rankIcons != 0 )
	{
		pTable = GetListIcons(m_move[rank].rankIcons);
		nb = pTable[0];
		icon = pTable[1+m_move[rank].cTotal%nb];

		m_pPixmap->DrawIcon(-1, m_move[rank].channel, icon, pos);
	}
}

// Déplace l'objet transporté par blupi.

void BuildMoveTransport(int icon, POINT &pos)
{
	pos.x -= DIMCELX/2;
	pos.y -= 96;

	static int offset_bateau[16*2] =
	{
		-4,-3,	// e
		-2,-3,
		-1,-3,	// se
		+1,-3,
		+2,-3,	// s
		+5,-2,
		+6,-2,	// so
		+5,-1,
		+1, 0,	// o
		-1, 0,
		-2, 0,	// no
		-2, 0,
		-3, 0,	// n
		-4,-1,
		-5,-1,	// ne
		-4,-2,
	};

	static int offset_jeep[16*2] =
	{
		-2,-6,	// e
		-1,-6,
		-1,-6,	// se
		-1,-6,
		+3,-6,	// s
		+1,-6,
		+4,-6,	// so
		+4,-5,
		+4,-5,	// o
		+2,-5,
		+1,-4,	// no
		+1,-4,
		-3,-3,	// n
		-4,-4,
		-3,-4,	// ne
		-4,-4,
	};

	if ( icon >= 0 && icon <= 47 )
	{
		pos.y -= (icon%3)*2;
	}

	if ( icon == 114 )  // mange ?
	{
		pos.x += 1;
		pos.y += 1;
	}

	if ( icon == 106 )  // se penche (mèche dynamite) ?
	{
		pos.x += 8;
		pos.y += 10;
	}

	if ( icon == 194 )  // se penche (mèche dynamite) ?
	{
		pos.x += 9;
		pos.y += 9;
	}

	if ( icon == 347 )  // se penche (armure) ?
	{
		pos.x += 2;
		pos.y += 2;
	}

	if ( icon >= 234 && icon <= 249 )  // blupi en bateau ?
	{
		pos.x += offset_bateau[(icon-234)*2+0];
		pos.y += offset_bateau[(icon-234)*2+1];
	}

	if ( icon >= 250 && icon <= 265 )  // blupi en jeep ?
	{
		pos.x += offset_jeep[(icon-250)*2+0];
		pos.y += offset_jeep[(icon-250)*2+1];
	}

	if ( icon == 270 )  pos.y += 3;  // blupi électrocuté
	if ( icon == 271 )  pos.y -= 2;
	if ( icon == 272 )  pos.y -= 7;
}

// Construit tous les sols fixes dans CHGROUND.

void CDecor::BuildGround(RECT clip)
{
//?	OutputDebug("BuildGround\n");
	int		x, y, i, j, nbx, nby, width, height, channel, icon;
	POINT	iCel, mCel, iPos, mPos, cPos, pos;

	width  = clip.right-clip.left;
	height = clip.bottom-clip.top;

	pos.x = clip.left;
	pos.y = clip.top;
	iCel = ConvPosToCel(pos);
	mCel = iCel;

	if ( mCel.x%2 == 0 && mCel.y%2 == 0 )
	{
		iCel.x -= 2;
		width  += DIMCELX;
		height += DIMCELY;
	}

	if ( mCel.x%2 != 0 && mCel.y%2 != 0 )
	{
		iCel.x -= 3;
		iCel.y -= 1;
		width  += DIMCELX;
		height += DIMCELY*2;
	}

	if ( mCel.x%2 == 0 && mCel.y%2 != 0 )
	{
		iCel.x -= 2;
		iCel.y -= 1;
		width  += DIMCELX/2;
		height += (DIMCELY/2)*3;
	}

	if ( mCel.x%2 != 0 && mCel.y%2 == 0 )
	{
		iCel.x -= 3;
		width  += (DIMCELX/2)*3;
		height += (DIMCELY/2)*3;
	}

	iPos = ConvCelToPos(iCel);

	nbx = (width/DIMCELX)+1;
	nby = (height/(DIMCELY/2))+0;

	if ( GetInfoHeight() != 0 )
	{
		nbx += 2;
		nby += 2;
	}

	// Construit les sols.
	mCel = iCel;
	mPos = iPos;
	for ( j=0 ; j<nby ; j++ )
	{
		x = mCel.x;
		y = mCel.y;
		cPos = mPos;
		for ( i=0 ; i<nbx ; i++ )
		{
//			if ( x >= 0 && x < MAXCELX &&
//				 y >= 0 && y < MAXCELY &&
//				 x%2 == 0 && y%2 == 0 &&
//				 m_decor[x/2][y/2].floorChannel >= 0 &&
//				 m_decor[x/2][y/2].floorIcon    >= 0 )
			if ( x%2 == 0 && y%2 == 0 )
			{
				pos.x = cPos.x-DIMCELX/2;
				pos.y = cPos.y;

				if ( x >= 2 && x < MAXCELX-2 &&
					 y >= 2 && y < MAXCELY-2 &&
					 m_decor[x/2][y/2].floorChannel >= 0 &&
					 m_decor[x/2][y/2].floorIcon    >= 0 )
				{
					channel = m_decor[x/2][y/2].floorChannel;
					icon    = m_decor[x/2][y/2].floorIcon;
				}
				else
				{
					channel = CHFLOOR;
					icon    = 78;  // losange noir
				}

				if ( !m_bBuild && icon == 71 )  // terre à fer ?
				{
					icon = 33;  // terre normale !
				}

				// Dessine l'eau sous les rives et les ponts.
				if ( (icon >=  2 && icon <= 13) ||  // rive ?
					 (icon >= 59 && icon <= 64) )   // pont ?
				{
					m_pPixmap->DrawIcon(CHGROUND, CHFLOOR,14, pos);  // eau
				}

				m_pPixmap->DrawIcon(CHGROUND, channel, icon, pos);
			}

			x ++;
			y --;
			cPos.x += DIMCELX;
		}

		if ( j%2 == 0 )
		{
			mCel.y ++;
			mPos.x -= DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
		else
		{
			mCel.x ++;
			mPos.x += DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
	}

	m_bGroundRedraw = FALSE;
}

// Construit le décor dans un pixmap.

void CDecor::Build(RECT clip, POINT posMouse)
{
	int		x, y, i, j, nbx, nby, width, height, rank, icon, channel, n;
	POINT	iCel, mCel, cel, iPos, mPos, cPos, pos, tPos;
	RECT	oldClip, clipRect;

	static int table_eau[6] =
	{
		70,68,14,69,14,68
	};
	static int table_random_x[10] =
	{
		2,5,1,9,4,0,6,3,8,7
	};
	static int table_random_y[10] =
	{
		4,8,3,5,9,1,7,2,0,6
	};

	oldClip = m_pPixmap->GetClipping();
	m_pPixmap->SetClipping(clip);

	if ( m_bGroundRedraw )
	{
		BuildGround(clip);  // refait les sols fixes
	}

	// Dessine tous les sols fixes.
	m_pPixmap->DrawImage(-1, CHGROUND, clip, 1);

	width  = clip.right-clip.left;
	height = clip.bottom-clip.top;

	pos.x = clip.left;
	pos.y = clip.top;
	iCel = ConvPosToCel(pos);
	mCel = iCel;

	if ( mCel.x%2 == 0 && mCel.y%2 == 0 )
	{
		iCel.x -= 2;
		width  += DIMCELX;
		height += DIMCELY;
	}

	if ( mCel.x%2 != 0 && mCel.y%2 != 0 )
	{
		iCel.x -= 3;
		iCel.y -= 1;
		width  += DIMCELX;
		height += DIMCELY*2;
	}

	if ( mCel.x%2 == 0 && mCel.y%2 != 0 )
	{
		iCel.x -= 2;
		iCel.y -= 1;
		width  += DIMCELX/2;
		height += (DIMCELY/2)*3;
	}

	if ( mCel.x%2 != 0 && mCel.y%2 == 0 )
	{
		iCel.x -= 3;
		width  += (DIMCELX/2)*3;
		height += (DIMCELY/2)*3;
	}

	iPos = ConvCelToPos(iCel);

	nbx = (width/DIMCELX)+1;
	nby = (height/(DIMCELY/2))+0;

	if ( GetInfoHeight() != 0 )
	{
		nbx += 2;
		nby += 2;
	}

	// Construit les sols.
	mCel = iCel;
	mPos = iPos;
	for ( j=0 ; j<nby ; j++ )
	{
		x = mCel.x;
		y = mCel.y;
		cPos = mPos;
		for ( i=0 ; i<nbx ; i++ )
		{
			if ( x >= 2 && x < MAXCELX-2 &&
				 y >= 2 && y < MAXCELY-2 )
			{
				m_rankBlupi[x][y] = -1;  // (1), voir BuildPutBlupi

				if ( x%2 == 0 && y%2 == 0 )
				{
					icon = m_decor[x/2][y/2].floorIcon;

					if ( !m_bBuild && icon == 71 )  // terre à fer ?
					{
						icon = 33;  // terre normale !
					}

					// Dessine l'eau sous les rives et les ponts.
					if ( (icon >=  2 && icon <= 14) ||  // rive ?
						 (icon >= 59 && icon <= 64) )   // pont ?
					{
						// Dessine l'eau en mouvement.
						pos.x = cPos.x-DIMCELX/2;
						pos.y = cPos.y;
						n = table_eau[(m_timeConst/2+  // lent !
									   table_random_x[x%10]+
									   table_random_y[y%10])%6];
						m_pPixmap->DrawIcon(CHGROUND, CHFLOOR, n, pos);  // eau
						if ( icon != 14 )
						{
							m_pPixmap->DrawIcon(CHGROUND, CHFLOOR, icon, pos);
						}
					}

					rank = m_decor[x/2][y/2].rankMove;
					if ( rank != -1 &&  // décor animé ?
						 m_move[rank].bFloor )
					{
						pos.x = cPos.x-DIMCELX/2;
						pos.y = cPos.y;

						BuildMoveFloor(x, y, pos, rank);
					}
				}
			}

			if ( m_celHili.x != -1 &&
				 x >= m_celHili.x-1 && x <= m_celHili.x+2 &&
				 y >= m_celHili.y-1 && y <= m_celHili.y+2 )
			{
				icon = m_iconHili[x-(m_celHili.x-1)]
								 [y-(m_celHili.y-1)];
				if ( icon != -1 )
				{
					// hilight cellule
					m_pPixmap->DrawIconDemi(-1, CHBLUPI, icon, cPos);
				}
			}

			if ( m_bHiliRect )  // rectangle de sélection existe ?
			{
				if ( (m_p1Hili.x == x && m_p1Hili.y == y) ||
					 (m_p2Hili.x == x && m_p2Hili.y == y) )
				{
					m_pPixmap->DrawIconDemi(-1, CHBLUPI, ICON_HILI_SEL, cPos);
				}
			}

			x ++;
			y --;
			cPos.x += DIMCELX;
		}

		if ( j%2 == 0 )
		{
			mCel.y ++;
			mPos.x -= DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
		else
		{
			mCel.x ++;
			mPos.x += DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
	}

	for ( j=nby ; j<nby+3 ; j++ )
	{
		x = mCel.x;
		y = mCel.y;
		for ( i=0 ; i<nbx ; i++ )
		{
			if ( x >= 2 && x < MAXCELX-2 &&
				 y >= 2 && y < MAXCELY-2 )
			{
				m_rankBlupi[x][y] = -1;  // (1), voir BuildPutBlupi
			}
			x ++;
			y --;
		}

		if ( j%2 == 0 )
		{
			mCel.y ++;
		}
		else
		{
			mCel.x ++;
		}
	}

	BlupiDrawHili();  // dessine le rectangle de sélection

	// Construit les objets et les blupi.
	BuildPutBlupi();  // m_rankBlupi[x][y] <- rangs des blupi

	mCel = iCel;
	mPos = iPos;
	for ( j=0 ; j<nby+3 ; j++ )
	{
		x = mCel.x;
		y = mCel.y;
		cPos = mPos;
		for ( i=0 ; i<nbx ; i++ )
		{
			if ( x >= 2 && x < MAXCELX-2 &&
				 y >= 2 && y < MAXCELY-2 )
			{
				rank = m_rankBlupi[x][y];
				if ( rank != -1 &&  // un blupi sur cette cellule ?
					 !m_blupi[rank].bCache )
				{
					cel.x = m_blupi[rank].cel.x;
					cel.y = m_blupi[rank].cel.y;
					pos = ConvCelToPos(cel);
					pos.x += m_blupi[rank].pos.x;
					pos.y += m_blupi[rank].pos.y-(DIMBLUPIY-DIMCELY)-SHIFTBLUPIY;

					if ( m_blupi[rank].bHili )
					{
						icon = 120+(m_blupi[rank].energy*18)/MAXENERGY;
						if ( icon < 120 )  icon = 120;
						if ( icon > 137 )  icon = 137;
						tPos = pos;
						tPos.y += DIMCELY;
						if ( m_blupi[rank].vehicule == 1 )  // en bateau ?
						{
							tPos.y -= 6;
						}
						// Dessine la sélection/énergie
						if ( m_blupi[rank].clipLeft == 0 )
						{
							m_pPixmap->DrawIconDemi(-1, CHBLUPI, icon, tPos);
						}
						else
						{
							clipRect = clip;
							clipRect.left = m_blupi[rank].clipLeft;
							m_pPixmap->SetClipping(clipRect);
							m_pPixmap->DrawIconDemi(-1, CHBLUPI, icon, tPos);
							m_pPixmap->SetClipping(clip);
						}
					}

					// Dessine la flèche ronde "répète" sous blupi.
					if ( m_blupi[rank].repeatLevel != -1 )
					{
						tPos = pos;
						tPos.y += DIMCELY;
						if ( m_blupi[rank].vehicule == 1 )  // en bateau ?
						{
							tPos.y -= 6;
						}
						// Dessine la sélection/énergie
						if ( m_blupi[rank].clipLeft == 0 )
						{
							m_pPixmap->DrawIconDemi(-1, CHBLUPI, 116, tPos);
						}
						else
						{
							clipRect = clip;
							clipRect.left = m_blupi[rank].clipLeft;
							m_pPixmap->SetClipping(clipRect);
							m_pPixmap->DrawIconDemi(-1, CHBLUPI, 116, tPos);
							m_pPixmap->SetClipping(clip);
						}
					}

					// Dessine la flèche jaune sur blupi.
					if ( m_blupi[rank].bArrow )
					{
						tPos = pos;
						if ( m_phase%(6*2) < 6 )
						{
							tPos.y -= DIMBLUPIY+(m_phase%6)*4;
						}
						else
						{
							tPos.y -= DIMBLUPIY+(6-(m_phase%6)-1)*4;
						}
						m_pPixmap->DrawIcon(-1, CHBLUPI,132, tPos);
					}

					// Dessine le stop sur blupi.
					if ( m_blupi[rank].stop == 1 )
					{
						tPos = pos;
						tPos.x += 9;
						tPos.y -= 24;
						m_pPixmap->DrawIcon(-1, CHBUTTON,46, tPos);
					}

					// Dessine blupi
					pos.y += m_blupi[rank].posZ;

					if ( m_blupi[rank].clipLeft == 0 )
					{
						m_pPixmap->DrawIcon(-1, m_blupi[rank].channel,
											m_blupi[rank].icon, pos);

						// Dessine l'objet transporté.
						if ( m_blupi[rank].takeChannel != -1 )
						{
							BuildMoveTransport(m_blupi[rank].icon, pos);
							m_pPixmap->DrawIcon(-1, m_blupi[rank].takeChannel,
												m_blupi[rank].takeIcon, pos);
						}
					}
					else
					{
						clipRect = clip;
						clipRect.left = m_blupi[rank].clipLeft;
						m_pPixmap->SetClipping(clipRect);
						m_pPixmap->DrawIcon(-1, m_blupi[rank].channel,
											m_blupi[rank].icon, pos);

						// Dessine l'objet transporté.
						if ( m_blupi[rank].takeChannel != -1 )
						{
							BuildMoveTransport(m_blupi[rank].icon, pos);
							m_pPixmap->DrawIcon(-1, m_blupi[rank].takeChannel,
												m_blupi[rank].takeIcon, pos);
						}
						m_pPixmap->SetClipping(clip);
					}
				}

				if ( x%2 == 0 && y%2 == 0 )
				{
					rank = m_decor[x/2][y/2].rankMove;
					if ( m_decor[x/2][y/2].objectChannel >= 0 )
					{
						pos.x = cPos.x-DIMCELX/2;
						pos.y = cPos.y-(DIMOBJY-DIMCELY*2);

						// Dessine l'objet
						if ( rank == -1 ||  // décor fixe ?
							 m_move[rank].bFloor ||
							 m_bBuild )
						{
							channel = m_decor[x/2][y/2].objectChannel;
							if ( m_bOutline && channel == CHOBJECT )
							{
								channel = CHOBJECTo;
							}
							if ( m_celOutline1.x != -1 &&
								 x >= m_celOutline1.x && y >= m_celOutline1.y &&
								 x <= m_celOutline2.x && y <= m_celOutline2.y )
							{
								if ( channel == CHOBJECT )  channel = CHOBJECTo;
								else                        channel = CHOBJECT;
							}
							m_pPixmap->DrawIcon(-1, channel,
												m_decor[x/2][y/2].objectIcon,
												pos);

							if ( m_decor[x/2][y/2].objectIcon == 12 )  // fusée ?
							{
								pos.y -= DIMOBJY;
								m_pPixmap->DrawIcon(-1, channel, 13, pos);
							}
						}
						else	// décor animé ?
						{
							BuildMoveObject(x, y, pos, rank);
						}
					}
					else
					{
						if ( rank != -1 &&  // décor animé ?
							 !m_move[rank].bFloor &&
							 !m_bBuild )
						{
							pos.x = cPos.x-DIMCELX/2;
							pos.y = cPos.y-(DIMOBJY-DIMCELY*2);
							BuildMoveObject(x, y, pos, rank);
						}
					}

					// Dessine le feu en mode construction.
					if ( m_bBuild &&
						 m_decor[x/2][y/2].fire > 0 &&
						 m_decor[x/2][y/2].fire < MoveMaxFire() )
					{
						pos.x = cPos.x-DIMCELX/2;
						pos.y = cPos.y-(DIMOBJY-DIMCELY*2);
						m_pPixmap->DrawIcon(-1, CHOBJECT, 49, pos);  // petite flamme
					}
				}
			}

			x ++;
			y --;
			cPos.x += DIMCELX;
		}

		if ( j%2 == 0 )
		{
			mCel.y ++;
			mPos.x -= DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
		else
		{
			mCel.x ++;
			mPos.x += DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
	}

	// Construit le brouillard.
	if ( !m_bFog )  goto term;

	if ( m_shiftOffset.x < 0 )  // décalage à droite ?
	{
		nbx += 2;
	}
	if ( m_shiftOffset.y < 0 )  // décalage en bas ?
	{
		nby += 2;
	}
	if ( m_shiftOffset.x > 0 )  // décalage à gauche ?
	{
		nbx += 2;
		iCel.x --;
		iCel.y ++;
		iPos = ConvCelToPos(iCel);
	}
	if ( m_shiftOffset.y > 0 )  // décalage en haut ?
	{
		nby += 2;
		iCel.x --;
		iCel.y --;
		iPos = ConvCelToPos(iCel);
	}

	mCel = iCel;
	mPos = iPos;
	for ( j=0 ; j<nby ; j++ )
	{
		x = mCel.x;
		y = mCel.y;
		cPos = mPos;
		for ( i=0 ; i<nbx ; i++ )
		{
			if ( x >= 0 && x < MAXCELX &&
				 y >= 0 && y < MAXCELY &&
				 x%2 == 0 && y%2 == 0 )
			{
				icon = m_decor[x/2][y/2].fog;
			}
			else
			{
				icon = FOGHIDE;  // caché
			}

			if ( abs(x)%4 == abs(y)%4 &&
				 (abs(x)%4 == 0 || abs(x)%4 == 2) &&
				 icon != -1 )
			{
				pos.x = cPos.x-DIMCELX/2;
				pos.y = cPos.y;

				m_pPixmap->DrawIcon(-1, CHFOG, icon, pos);
			}

			x ++;
			y --;
			cPos.x += DIMCELX;
		}

		if ( j%2 == 0 )
		{
			mCel.y ++;
			mPos.x -= DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
		else
		{
			mCel.x ++;
			mPos.x += DIMCELX/2;
			mPos.y += DIMCELY/2;
		}
	}

	term:
	// Dessine la flèche jaune sur un objet.
	if ( m_celArrow.x != -1 )
	{
		tPos = ConvCelToPos(m_celArrow);
		if ( m_phase%(6*2) < 6 )
		{
			tPos.y -= DIMBLUPIY+(m_phase%6)*4;
		}
		else
		{
			tPos.y -= DIMBLUPIY+(6-(m_phase%6)-1)*4;
		}
		m_pPixmap->DrawIcon(-1, CHBLUPI,132, tPos);
	}

	// Dessine le nom de l'objet pointé par la souris.
	if ( posMouse.x == m_textLastPos.x &&
		 posMouse.y == m_textLastPos.y )
	{
		if ( m_textCount == 0 )
		{
			n = GetResHili(posMouse);
			if ( n != -1 )
			{
				LoadString(n, m_text, 50);
				posMouse.x += 10;
				posMouse.y += 20;
				DrawText(m_pPixmap, posMouse, m_text);
			}
		}
		else
		{
			m_textCount --;
		}
	}
	else
	{
		m_textLastPos = posMouse;
		m_textCount = TEXTDELAY;
	}

	m_pPixmap->SetClipping(oldClip);

	GenerateMap();  // dessine la carte miniature
	GenerateStatictic();  // dessine les statistiques
}


// Augmente la phase.
//	-1	mise à jour continue
//	 0	début de mise à jour périodique
//	 1	mise à jour périodique suivante

void CDecor::NextPhase(int mode)
{
	if ( mode == -1 )
	{
		m_phase = -1;
	}

	if ( mode == 0 )
	{
		m_phase = 0;
	}

	if ( mode == 1 )
	{
		m_phase ++;
	}

	m_totalTime ++;
}

// Modifie le temps total passé dans cette partie.

void CDecor::SetTotalTime(int total)
{
	m_totalTime = total;
}

// Retourne le temps total passé dans cette partie.

int CDecor::GetTotalTime()
{
	return m_totalTime;
}


// Compte le nombre total de sols contenus dans les décors.

int CDecor::CountFloor(int channel, int icon)
{
	int		x, y;
	int		nb = 0;

	for ( x=0 ; x<MAXCELX/2 ; x++ )
	{
		for ( y=0 ; y<MAXCELY/2 ; y++ )
		{
			if ( channel == m_decor[x][y].floorChannel &&
				 icon    == m_decor[x][y].floorIcon    )  nb ++;
		}
	}

	return nb;
}

// Indique si une cellule est ok pour une action.
// Le rang du blupi qui effectuera le travail est donnée dans rank.
//	action = 0			sélection jeu
//			 1			construction d'une cellule 1x1
//			 2			construction d'une cellule 2x2
//			 WM_ACTION*	action

int CDecor::CelOkForAction(POINT cel, int action, int rank,
						   int icons[4][4],
						   POINT &celOutline1,
						   POINT &celOutline2)
{
	int		x, y, i, j, channel, icon, nb, start, direct;
	int		error = 0;
	BOOL	bStrong    = FALSE;
	BOOL	bTransport = FALSE;
	BOOL	bVehicule  = FALSE;
	BOOL	bVehiculeA = FALSE;
	POINT	vector;

	for ( x=0 ; x<4 ; x++ )
	{
		for ( y=0 ; y<4 ; y++ )
		{
			icons[x][y] = -1;
		}
	}

	celOutline1.x = -1;
	celOutline2.x = -1;

	if ( action == 2 ||
		 action == WM_ACTION_ABAT1    ||
		 action == WM_ACTION_ROC1     ||
		 action == WM_ACTION_DEPOSE   ||
		 action == WM_ACTION_LABO     ||
		 action == WM_ACTION_FLEUR1   ||
		 action == WM_ACTION_CULTIVE  ||
		 action == WM_ACTION_DRAPEAU  )
	{
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
	}

	if ( rank >= 0 )
	{
		if ( m_blupi[rank].energy > MAXENERGY/4 )  // blupi fort ?
		{
			bStrong = TRUE;
		}
		if ( m_blupi[rank].takeChannel != -1 )  // porte qq chose ?
		{
			bTransport = TRUE;
		}
		if ( m_blupi[rank].vehicule != 0 )  // pas à pied ?
		{
			bVehicule = TRUE;
		}
		if ( m_blupi[rank].vehicule != 0 &&  // pas à pied ?
			 m_blupi[rank].vehicule != 3 )   // pas armure ?
		{
			bVehiculeA = TRUE;
		}
	}

	if ( action == 0 )
	{
		if ( IsBlupiHere(cel, FALSE) )
		{
			icons[1][1] = ICON_HILI_SEL;
		}
		else
		{
			if ( IsFreeCel(cel, -1) && m_nbBlupiHili > 0 )
			{
				icons[1][1] = ICON_HILI_ANY;
			}
			else
			{
				icons[1][1] = ICON_HILI_ERR;
			}
		}
	}

	if ( action == 1 )
	{
		icons[1][1] = ICON_HILI_BUILD;  // action
	}

	if ( action == 2 )
	{
		icons[1][1] = ICON_HILI_BUILD;  // action
		icons[2][1] = ICON_HILI_BUILD;
		icons[1][2] = ICON_HILI_BUILD;
		icons[2][2] = ICON_HILI_BUILD;
	}

	if ( action == WM_ACTION_STOP )
	{
		error = ERROR_MISC;
		if ( m_blupi[rank].stop == 0 &&
			 (m_blupi[rank].goalAction == WM_ACTION_GO ||
			  (m_blupi[rank].goalAction >= WM_ACTION_ABAT1 &&
			   m_blupi[rank].goalAction <= WM_ACTION_ABAT6) ||
			  (m_blupi[rank].goalAction >= WM_ACTION_ROC1 &&
			   m_blupi[rank].goalAction <= WM_ACTION_ROC7) ||
			  m_blupi[rank].goalAction == WM_ACTION_CULTIVE ||
			  m_blupi[rank].goalAction == WM_ACTION_CULTIVE2 ||
			  m_blupi[rank].goalAction == WM_ACTION_DRAPEAU ||
			  m_blupi[rank].goalAction == WM_ACTION_DRAPEAU2 ||
			  m_blupi[rank].goalAction == WM_ACTION_DRAPEAU3 ||
			  m_blupi[rank].goalAction == WM_ACTION_FLEUR1 ||
			  m_blupi[rank].goalAction == WM_ACTION_FLEUR2 ||
			  m_blupi[rank].goalAction == WM_ACTION_FLEUR3) )
		{
			error = 0;
		}
		if ( m_blupi[rank].stop == 0 &&
			 m_blupi[rank].goalAction != 0 &&
			 m_blupi[rank].interrupt == 1 )
		{
			error = 0;
		}
		if ( m_blupi[rank].repeatLevel != -1 )
		{
			error = 0;
		}
	}

	if ( action == WM_ACTION_GO )
	{
		if ( m_decor[cel.x/2][cel.y/2].objectIcon == 113 )  // maison ?
		{
			cel.x = (cel.x/2)*2+1;
			cel.y = (cel.y/2)*2+1;
		}
		error = ERROR_MISC;
		if ( m_nbBlupiHili > 0 )
		{
			nb = m_nbBlupiHili;
			if ( nb > 16 )  nb = 16;
			for ( i=0 ; i<nb ; i++ )
			{
				x = table_multi_goal[i*2+0];
				y = table_multi_goal[i*2+1];
				rank = GetHiliRankBlupi(i);
				if ( ((m_blupi[rank].takeChannel == -1) ||
					  (m_blupi[rank].energy > MAXENERGY/4)) &&
					 IsFreeCelGo(GetCel(cel.x+x,cel.y+y), rank) &&
					 !IsBlupiHere(GetCel(cel.x+x,cel.y+y), TRUE) )
				{
//?					icons[1+x][1+y] = ICON_HILI_GO;  // flèche
					icons[1+x][1+y] = ICON_HILI_OP;  // action
					error = 0;
				}
				else
				{
					icons[1+x][1+y] = ICON_HILI_ERR;
				}
			}
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;
		}
	}

	if ( action == WM_ACTION_ABAT1 )
	{
		GetObject(cel, channel, icon);
		if ( bStrong && !bTransport && !bVehicule &&
			 channel == CHOBJECT && icon >= 6 && icon <= 11 &&  // arbre ?
			 !MoveIsUsed(cel) &&
			 IsWorkableObject(cel, rank) )
		{
			icons[1][1] = ICON_HILI_OP;  // action
			icons[2][1] = ICON_HILI_OP;
			icons[1][2] = ICON_HILI_OP;
			icons[2][2] = ICON_HILI_OP;
			celOutline1 = cel;
			celOutline2 = cel;
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;  // croix
			icons[2][1] = ICON_HILI_ERR;
			icons[1][2] = ICON_HILI_ERR;
			icons[2][2] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
	}

	if ( action == WM_ACTION_ROC1 )
	{
		GetObject(cel, channel, icon);
		if ( bStrong && !bTransport && !bVehicule &&
			 m_blupi[rank].perso != 8 &&  // pas disciple ?
			 channel == CHOBJECT && icon >= 37 && icon <= 43 &&  // rochers ?
			 !MoveIsUsed(cel) &&
			 IsWorkableObject(cel, rank) )
		{
			icons[1][1] = ICON_HILI_OP;  // action
			icons[2][1] = ICON_HILI_OP;
			icons[1][2] = ICON_HILI_OP;
			icons[2][2] = ICON_HILI_OP;
			celOutline1 = cel;
			celOutline2 = cel;
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;  // croix
			icons[2][1] = ICON_HILI_ERR;
			icons[1][2] = ICON_HILI_ERR;
			icons[2][2] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
	}

	if ( action >= WM_ACTION_BUILD1 &&
		 action <= WM_ACTION_BUILD6 )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule )
			{
				error = ERROR_MISC;  // pas assez fort
			}

			if ( action == WM_ACTION_BUILD1 ||  // cabane ?
				 action == WM_ACTION_BUILD2 ||  // couveuse ?
				 action == WM_ACTION_BUILD6 )   // téléporteur ?
			{
				GetFloor(cel, channel, icon);
				if ( channel != CHFLOOR ||
					(icon != 1 &&                 // herbe claire ?
					 (icon < 19 || icon > 32)) )  // herbe foncée ?
				{
					error = ERROR_GROUND;  // sol pas adéquat
				}
			}

			if ( action == WM_ACTION_BUILD4 )  // mine ?
			{
				GetFloor(cel, channel, icon);
				if ( channel != CHFLOOR || icon != 71 )  // terre à fer ?
				{
					error = ERROR_GROUND;  // sol pas adéquat
				}
			}

			if ( action == WM_ACTION_BUILD6 &&   // téléporteur ?
				 CountFloor(CHFLOOR, 80) >= 2 )  // déjà 2 ?
			{
				error = ERROR_TELE2;  // déjà 2 téléporteurs
			}

			if ( action == WM_ACTION_BUILD3 ||
				 action == WM_ACTION_BUILD5 )  start = 44;  // pierres
			else                               start = 36;  // planches
			if ( start == 44 &&
				 m_blupi[rank].perso == 8 )  start = 999;  // disciple ?

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != start )  // planches ?
			{
				error = ERROR_MISC;  // pas de planches !
			}

			for ( x=-1 ; x<3 ; x++ )
			{
				for ( y=-1 ; y<3 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=-1 ; x<3 ; x++ )
			{
				for ( y=-1 ; y<3 ; y++ )
				{
					if ( (x<0 || x>1 || y<0 || y>1) &&
						!IsFreeCel(GetCel(cel,x,y), rank) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_MUR )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule ||
				 m_blupi[rank].perso == 8 )  // disciple ?
			{
				error = ERROR_MISC;  // pas assez fort
			}

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != 44 )  // pierres ?
			{
				error = ERROR_MISC;  // pas de pierres !
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_TOUR )
	{
		BOOL	bTour;

		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule ||
				 m_blupi[rank].perso == 8 )  // disciple ?
			{
				error = ERROR_MISC;  // pas assez fort
			}

//			GetFloor(cel, channel, icon);
//			if ( channel != CHFLOOR ||
//				(icon != 1 &&                 // herbe claire ?
//				 (icon < 19 || icon > 32)) )  // herbe foncée ?
//			{
//				error = ERROR_GROUND;  // sol pas adéquat
//			}

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != 44 )  // pierres ?
			{
				error = ERROR_MISC;  // pas de pierres !
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=-1 ; x<3 ; x++ )
			{
				for ( y=-1 ; y<3 ; y++ )
				{
					if ( x<0 || x>1 || y<0 || y>1 )  // périphérie ?
					{
						GetFloor(GetCel(cel,x,y), channel, icon);
						if ( channel == CHFLOOR &&
							 (icon >= 2 && icon <= 13) )  // rive ?
						{
							error = ERROR_TOUREAU;
							icons[x+1][y+1] = ICON_HILI_ERR;  // croix
						}
					}
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}

			if ( error == 0 )
			{
				bTour = FALSE;
				for ( i=0 ; i<4 ; i++ )
				{
					vector = GetVector(i*2*16);
					x = cel.x;
					y = cel.y;

					for ( j=0 ; j<3 ; j++ )
					{
						x += vector.x*2;
						y += vector.y*2;

						if ( m_decor[x/2][y/2].objectIcon == 27 )  // tour ?
						{
							bTour = TRUE;
						}

						if ( MoveGetObject(GetCel(x,y), channel, icon) &&
							 channel == CHOBJECT && icon == 27 )  // tour en construction ?
						{
							bTour = TRUE;
						}
					}
				}
				if ( !bTour )  error = ERROR_TOURISOL;
			}
		}
	}

	if ( action == WM_ACTION_PALIS )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule )
			{
				error = ERROR_MISC;  // pas assez fort
			}

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != 36 )  // planches ?
			{
				error = ERROR_MISC;  // pas de pierres !
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_PONTE )
	{
		POINT	test;

		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule )
			{
				error = ERROR_MISC;  // pas assez fort
			}

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != 36 )  // planches ?
			{
				error = ERROR_MISC;  // pas de pierres !
			}

			test = cel;
			if ( error == 0 )  error = IsBuildPont(test, icon);

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_CARRY )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			GetObject(GetCel(cel,-1,-1), channel, icon);
			if ( bStrong && !bTransport && !bVehiculeA &&
				 channel == CHOBJECT &&
				 (icon ==  14 ||   // métal ?
				  icon ==  36 ||   // planches ?
				  icon ==  44 ||   // pierres ?
				  icon ==  60 ||   // tomates ?
				  icon ==  63 ||   // oeufs ?
				  icon ==  80 ||   // bouteille ?
				  icon ==  82 ||   // fleurs ?
				  icon ==  84 ||   // fleurs ?
				  icon ==  95 ||   // fleurs ?
				  icon ==  85 ||   // dynamite ?
				  icon ==  92 ||   // poison ?
				  icon ==  93 ||   // piège ?
				  icon == 123 ||   // fer ?
				  icon == 125) &&  // mine ?
				 (!IsBlupiHereEx(GetCel(cel,-1,0), rank, FALSE) ||
				  !IsBlupiHereEx(GetCel(cel,0,-1), rank, FALSE)) )
			{
				icons[1][1] = ICON_HILI_OP;  // action
			}
			else
			{
				icons[1][1] = ICON_HILI_ERR;  // croix
				error = ERROR_MISC;
			}
		}
	}

	if ( action == WM_ACTION_DEPOSE )
	{
		if ( !bTransport || bVehiculeA )
		{
			error = ERROR_MISC;  // ne transporte rien
		}

		GetObject(GetCel((cel.x/2)*2,(cel.y/2)*2), channel, icon);
		if ( icon != -1 && icon != 124 )  // pas drapeau ?
		{
			error = ERROR_MISC;
		}

		start = 0;
		if ( error == 0 )
		{
			GetFloor(cel, channel, icon);
			if ( channel == CHFLOOR && icon == 52 &&  // nurserie ?
				 m_blupi[rank].takeChannel == CHOBJECT &&
				 m_blupi[rank].takeIcon    == 63 )  // oeufs ?
			{
				for ( x=-1 ; x<2 ; x++ )
				{
					for ( y=0 ; y<2 ; y++ )
					{
						if ( !IsFreeCelDepose(GetCel(cel,x,y), rank) ||
							 IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
						{
							error = ERROR_MISC;
						}
					}
				}
				start = -1;
			}
			else
			{
				if ( !IsFreeCelDepose(GetCel(cel,1,1), rank) ||
					 IsBlupiHereEx(GetCel(cel,1,1), rank, FALSE) )
				{
					error = ERROR_MISC;
				}
				else
				{
					if ( !IsFreeCelDepose(GetCel(cel,0,1), rank) ||
						 IsBlupiHereEx(GetCel(cel,0,1), rank, FALSE) )
					{
						if ( !IsFreeCelDepose(GetCel(cel,1,0), rank) ||
							 IsBlupiHereEx(GetCel(cel,1,0), rank, FALSE) )
						{
							error = ERROR_MISC;
						}
					}
				}
			}
		}

		for ( x=start ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}
	}

	if ( action == WM_ACTION_CULTIVE )
	{
		if ( !bStrong || bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 61 )  // maison ?
		{
			error = ERROR_MISC;  // pas de maison !
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	if ( action == WM_ACTION_LABO )
	{
		if ( !bStrong || !bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 28 ||  // laboratoire ?
			 m_blupi[rank].takeChannel != CHOBJECT ||
			 (m_blupi[rank].takeIcon != 82 &&  // fleurs ?
			  m_blupi[rank].takeIcon != 84 &&  // fleurs ?
			  m_blupi[rank].takeIcon != 95 &&  // fleurs ?
			  m_blupi[rank].takeIcon != 60) )  // tomates ?
		{
			error = ERROR_MISC;  // pas de laboratoire !
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	if ( action == WM_ACTION_FLEUR1 )
	{
		GetObject(cel, channel, icon);
		if ( bStrong && !bTransport && !bVehicule &&
			 channel == CHOBJECT &&
			 (icon == 81 || icon == 83 || icon == 94) &&  // fleurs ?
			 !MoveIsUsed(cel) &&
			 IsWorkableObject(cel, rank) )
		{
			icons[1][1] = ICON_HILI_OP;  // action
			icons[2][1] = ICON_HILI_OP;
			icons[1][2] = ICON_HILI_OP;
			icons[2][2] = ICON_HILI_OP;
			celOutline1 = cel;
			celOutline2 = cel;
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;  // croix
			icons[2][1] = ICON_HILI_ERR;
			icons[1][2] = ICON_HILI_ERR;
			icons[2][2] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
	}

	if ( action == WM_ACTION_DYNAMITE )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

//?			if ( !bStrong || bVehicule )
			if ( bVehiculeA )
			{
				error = ERROR_MISC;  // pas assez fort
			}

			GetObject(cel, channel, icon);
			if ( icon != 85 && icon != 125 )  // dynamite/mine ?
			{
				error = ERROR_MISC;  // pas de dynamite !
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=1 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=1 ; y<2 ; y++ )
				{
					if ( (x<0 || x>1 || y<0 || y>1) &&
						!IsFreeCel(GetCel(cel,x,y), rank) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_MANGE )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			GetObject(GetCel(cel,-1,-1), channel, icon);
			if ( !m_blupi[rank].bMalade && !bVehicule &&
				 m_blupi[rank].perso != 8 &&  // pas disciple ?
				 channel == CHOBJECT &&
				 icon == 60 &&  // tomates ?
				 (!IsBlupiHereEx(GetCel(cel,-1,0), rank, FALSE) ||
				  !IsBlupiHereEx(GetCel(cel,0,-1), rank, FALSE)) )
			{
				icons[1][1] = ICON_HILI_OP;  // action
			}
			else
			{
				icons[1][1] = ICON_HILI_ERR;  // croix
				error = ERROR_MISC;
			}
		}
	}

	if ( action == WM_ACTION_BOIT )
	{
		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			GetObject(GetCel(cel,-1,-1), channel, icon);
			if ( m_blupi[rank].bMalade && !bVehicule &&
				 m_blupi[rank].perso != 8 &&  // pas disciple ?
				 channel == CHOBJECT &&
				 icon == 80 &&  // bouteille ?
				 (!IsBlupiHereEx(GetCel(cel,-1,0), rank, FALSE) ||
				  !IsBlupiHereEx(GetCel(cel,0,-1), rank, FALSE)) )
			{
				icons[1][1] = ICON_HILI_OP;  // action
			}
			else
			{
				icons[1][1] = ICON_HILI_ERR;  // croix
				error = ERROR_MISC;
			}
		}
	}

	if ( action == WM_ACTION_BATEAUE )
	{
		POINT	test;

		if ( cel.x%2 != 1 || cel.y%2 != 1 )
		{
			icons[1][1] = ICON_HILI_ERR;
			error = ERROR_MISC;
		}
		else
		{
			cel.x --;
			cel.y --;

			if ( !bStrong || bTransport || bVehicule )
			{
				error = ERROR_MISC;  // pas assez fort
			}

			GetObject(cel, channel, icon);
			if ( channel != CHOBJECT || icon != 36 )  // planches ?
			{
				error = ERROR_MISC;  // pas de pierres !
			}

			test = cel;
			if ( error == 0 && !IsBuildBateau(test, direct) )
			{
				error = ERROR_MISC;  // impossible ici !
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
					else          icons[x+1][y+1] = ICON_HILI_OP;
				}
			}

			for ( x=0 ; x<2 ; x++ )
			{
				for ( y=0 ; y<2 ; y++ )
				{
					if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
					{
						error = ERROR_FREE;
						icons[x+1][y+1] = ICON_HILI_ERR;  // croix
					}
				}
			}
		}
	}

	if ( action == WM_ACTION_DJEEP )
	{
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
		error = ERROR_MISC;
		if ( m_blupi[rank].vehicule == 2 &&  // en jeep ?
			 m_decor[cel.x/2][cel.y/2].objectIcon == -1 &&
			 m_decor[cel.x/2][cel.y/2].floorIcon != 80 )  // pas téléporteur ?
		{
			if ( IsFreeCelGo(GetCel(cel,+1, 0), rank) &&
				 IsFreeCelGo(GetCel(cel,+1,+1), rank) &&
				!IsBlupiHereEx(GetCel(cel,+1, 0), rank, FALSE) &&
				!IsBlupiHereEx(GetCel(cel,+1,+1), rank, FALSE) )
			{
				icons[1][1] = ICON_HILI_OP;  // action
				icons[2][1] = ICON_HILI_OP;
				icons[1][2] = ICON_HILI_OP;
				icons[2][2] = ICON_HILI_OP;
				error = 0;
			}
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;  // croix
			icons[2][1] = ICON_HILI_ERR;
			icons[1][2] = ICON_HILI_ERR;
			icons[2][2] = ICON_HILI_ERR;
		}
	}

	if ( action == WM_ACTION_DARMURE )
	{
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
		error = ERROR_MISC;
		if ( m_blupi[rank].vehicule == 3 &&  // armure ?
			 !bTransport &&
			 m_decor[cel.x/2][cel.y/2].objectIcon == -1 &&
			 m_decor[cel.x/2][cel.y/2].floorIcon != 80 )  // pas téléporteur ?
		{
			if ( IsFreeCelGo(GetCel(cel,+1, 0), rank) &&
				 IsFreeCelGo(GetCel(cel,+1,+1), rank) &&
				!IsBlupiHereEx(GetCel(cel,+1, 0), rank, FALSE) &&
				!IsBlupiHereEx(GetCel(cel,+1,+1), rank, FALSE) )
			{
				icons[1][1] = ICON_HILI_OP;  // action
				icons[2][1] = ICON_HILI_OP;
				icons[1][2] = ICON_HILI_OP;
				icons[2][2] = ICON_HILI_OP;
				error = 0;
			}
		}
		else
		{
			icons[1][1] = ICON_HILI_ERR;  // croix
			icons[2][1] = ICON_HILI_ERR;
			icons[1][2] = ICON_HILI_ERR;
			icons[2][2] = ICON_HILI_ERR;
		}
	}

	if ( action == WM_ACTION_DRAPEAU )
	{
		if ( !bStrong || bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}

		GetFloor(cel, channel, icon);
		if ( (icon < 33 || icon > 48) &&
			 icon != 71 )  // pas terre ?
		{
			error = ERROR_MISC;  // terrain pas adapté
		}

		GetObject(cel, channel, icon);
		if ( channel == CHOBJECT )  // y a-t-il un objet ?
		{
			error = ERROR_MISC;  // terrain pas adapté
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	if ( action == WM_ACTION_EXTRAIT )
	{
		if ( !bStrong || bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 122 )  // mine de fer ?
		{
			error = ERROR_MISC;  // pas de mine
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	if ( action == WM_ACTION_FABJEEP   ||
		 action == WM_ACTION_FABMINE   ||
		 action == WM_ACTION_FABARMURE )
	{
		if ( !bStrong || !bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}
		if ( action == WM_ACTION_FABJEEP &&
			 m_blupi[rank].perso == 8    )  // disciple ?
		{
			error = ERROR_MISC;  // impossible
		}
		if ( action == WM_ACTION_FABARMURE &&
			 m_blupi[rank].perso == 8    )  // disciple ?
		{
			error = ERROR_MISC;  // impossible
		}

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 120 ||  // usine ?
			 m_blupi[rank].takeChannel != CHOBJECT ||
			 m_blupi[rank].takeIcon != 123 )  // fer ?
		{
			error = ERROR_MISC;  // pas d'usine !
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	if ( action == WM_ACTION_FABDISC )
	{
		if ( !bStrong || !bTransport || bVehicule )
		{
			error = ERROR_MISC;  // pas assez fort
		}

		GetObject(cel, channel, icon);
		if ( channel != CHOBJECT || icon != 120 ||  // usine ?
			 m_blupi[rank].takeChannel != CHOBJECT ||
			 m_blupi[rank].takeIcon != 14 )  // métal ?
		{
			error = ERROR_MISC;  // pas d'usine !
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( error )  icons[x+1][y+1] = ICON_HILI_ERR;
				else          icons[x+1][y+1] = ICON_HILI_OP;
			}
		}

		for ( x=0 ; x<2 ; x++ )
		{
			for ( y=0 ; y<2 ; y++ )
			{
				if ( IsBlupiHereEx(GetCel(cel,x,y), rank, FALSE) )
				{
					error = ERROR_MISC;
					icons[x+1][y+1] = ICON_HILI_ERR;  // croix
				}
			}
		}
	}

	return error;
}

// Indique si une cellule est ok pour une action.
// Le rang du blupi qui effectuera le travail est donnée dans rank.

int CDecor::CelOkForAction(POINT cel, int action, int rank)
{
	int		icons[4][4];
	POINT	celOutline1, celOutline2;

	return CelOkForAction(cel, action, rank, icons,
						  celOutline1, celOutline2);
}

// Retourne le rang du nième blupi sélectionné.

int CDecor::GetHiliRankBlupi(int nb)
{
	int		rank;

	if ( m_nbBlupiHili == 0 )  return -1;
	if ( m_nbBlupiHili == 1 )
	{
		if ( nb == 0 )  return m_rankBlupiHili;
		return -1;
	}

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist &&
			 m_blupi[rank].bHili  )
		{
			if ( nb == 0 )  return rank;
			nb --;
		}
	}

	return -1;
}

// Marque la cellule visée par la souris.
//	action = 0			sélection jeu
//			 1			construction d'une cellule 1x1
//			 2			construction d'une cellule 2x2

void CDecor::CelHili(POINT pos, int action)
{
	int		x, y, i, channel, icon, rank, nb;
	POINT	cel;

	for ( x=0 ; x<4 ; x++ )
	{
		for ( y=0 ; y<4 ; y++ )
		{
			m_iconHili[x][y] = -1;
		}
	}

	m_celOutline1.x = -1;
	m_celOutline2.x = -1;
	m_rankHili = -1;

	if ( action == 0 )  // sélection pendant jeu ?
	{
		rank = GetTargetBlupi(pos);
		if ( rank >= 0 )
		{
			m_celHili = m_blupi[rank].cel;
			m_rankHili = rank;
			m_iconHili[1][1] = ICON_HILI_SEL;
		}
		else
		{
			m_celHili = ConvPosToCel(pos);

			if ( IsBlupiHere(m_celHili, FALSE) )
			{
				m_rankHili = m_blupiHere;
				m_iconHili[1][1] = ICON_HILI_SEL;
			}
			else
			{
				if ( m_nbBlupiHili > 0 )
				{
					nb = m_nbBlupiHili;
					if ( nb > 16 )  nb = 16;
					for ( i=0 ; i<nb ; i++ )
					{
						x = table_multi_goal[i*2+0];
						y = table_multi_goal[i*2+1];
						cel.x = m_celHili.x + x;
						cel.y = m_celHili.y + y;
						rank = GetHiliRankBlupi(i);
						if ( IsFreeCelHili(cel, rank) )
						{
							m_iconHili[1+x][1+y] = ICON_HILI_ANY;
						}
						else
						{
							m_iconHili[1+x][1+y] = ICON_HILI_ERR;
						}
					}
				}
				else
				{
					m_iconHili[1][1] = ICON_HILI_ERR;
				}

				m_celOutline1.x = (m_celHili.x/2)*2;
				m_celOutline1.y = (m_celHili.y/2)*2;
				GetObject(m_celOutline1, channel, icon);
				if ( channel == CHOBJECT &&
					 (icon ==  14 ||	// métal ?
					  icon ==  36 ||	// planches ?
					  icon ==  44 ||	// pierres ?
					  icon ==  60 ||	// tomates ?
					  icon ==  64 ||	// oeufs ?
					  icon ==  80 ||	// bouteille ?
					  icon ==  82 ||	// fleurs ?
					  icon ==  84 ||	// fleurs ?
					  icon ==  95 ||	// fleurs ?
					  icon ==  85 ||	// dynamite ?
					  icon ==  92 ||	// poison ?
					  icon ==  93 ||	// piège ?
					  icon == 123 ||	// fer ?
					  icon == 125 ) )	// mine ?
				{
					if ( m_celHili.x%2 == 0 ||
						 m_celHili.y%2 == 0 )
					{
						m_celOutline1.x = -1;
					}
				}
				m_celOutline2 = m_celOutline1;
			}
		}
	}

	if ( action == 1 )  // construction d'une cellule 1x1 ?
	{
		m_celHili = ConvPosToCel(pos);

		m_iconHili[1][1] = ICON_HILI_BUILD;  // action
	}

	if ( action == 2 )  // construction d'une cellule 2x2 ?
	{
		m_celHili = ConvPosToCel2(pos);

		m_iconHili[1][1] = ICON_HILI_BUILD;  // action
		m_iconHili[2][1] = ICON_HILI_BUILD;
		m_iconHili[1][2] = ICON_HILI_BUILD;
		m_iconHili[2][2] = ICON_HILI_BUILD;
	}
}

// Marque la cellule visée par la souris pour un bouton donné.

void CDecor::CelHiliButton(POINT cel, int button)
{
	POINT	celOutline1, celOutline2;

	CelOkForAction(cel, table_actions[button],
				   m_rankBlupiHili, m_iconHili,
				   celOutline1, celOutline2);

	if ( button == BUTTON_ABAT      ||
		 button == BUTTON_ABATn     ||
		 button == BUTTON_ROC       ||
		 button == BUTTON_ROCn      ||
		 button == BUTTON_MUR       ||
		 button == BUTTON_TOUR      ||
		 button == BUTTON_PALIS     ||
		 button == BUTTON_PONT      ||
		 button == BUTTON_CULTIVE   ||
		 button == BUTTON_DEPOSE    ||
		 button == BUTTON_LABO      ||
		 button == BUTTON_FLEUR     ||
		 button == BUTTON_FLEURn    ||
		 button == BUTTON_DYNAMITE  ||
		 button == BUTTON_BATEAU    ||
		 button == BUTTON_DJEEP     ||
		 button == BUTTON_DARMURE   ||
		 button == BUTTON_DRAPEAU   ||
		 button == BUTTON_EXTRAIT   ||
		 button == BUTTON_FABJEEP   ||
		 button == BUTTON_FABARMURE ||
		 button == BUTTON_FABMINE   ||
		 button == BUTTON_FABDISC   ||
		 (button >= BUTTON_BUILD1 &&
		  button <= BUTTON_BUILD6) )
	{
		m_celHili.x = (cel.x/2)*2;
		m_celHili.y = (cel.y/2)*2;
	}
	else
	{
		m_celHili = cel;
	}
}

// Marque la cellule visée par la souris pour une répétition donnée.

void CDecor::CelHiliRepeat(int list)
{
	int			rank, button, x, y, i;
	POINT		cel;

	for ( x=0 ; x<4 ; x++ )
	{
		for ( y=0 ; y<4 ; y++ )
		{
			m_iconHili[x][y] = -1;
		}
	}

	if ( m_nbBlupiHili != 1 )  return;
	rank = m_rankBlupiHili;

	i = m_blupi[rank].repeatLevelHope - list;
	if ( i < 0 || i > m_blupi[rank].repeatLevelHope )  return;

	button = m_blupi[rank].listButton[i];
	if ( button == BUTTON_ABAT      ||
		 button == BUTTON_ABATn     ||
		 button == BUTTON_ROC       ||
		 button == BUTTON_ROCn      ||
		 button == BUTTON_MUR       ||
		 button == BUTTON_TOUR      ||
		 button == BUTTON_PALIS     ||
		 button == BUTTON_PONT      ||
		 button == BUTTON_CULTIVE   ||
		 button == BUTTON_DEPOSE    ||
		 button == BUTTON_LABO      ||
		 button == BUTTON_FLEUR     ||
		 button == BUTTON_FLEURn    ||
		 button == BUTTON_DYNAMITE  ||
		 button == BUTTON_BATEAU    ||
		 button == BUTTON_DJEEP     ||
		 button == BUTTON_DARMURE   ||
		 button == BUTTON_DRAPEAU   ||
		 button == BUTTON_EXTRAIT   ||
		 button == BUTTON_FABJEEP   ||
		 button == BUTTON_FABARMURE ||
		 button == BUTTON_FABMINE   ||
		 button == BUTTON_FABDISC   ||
		 (button >= BUTTON_BUILD1 &&
		  button <= BUTTON_BUILD6) )
	{
		m_iconHili[1][1] = ICON_HILI_OP;  // action
		m_iconHili[2][1] = ICON_HILI_OP;  // action
		m_iconHili[1][2] = ICON_HILI_OP;  // action
		m_iconHili[2][2] = ICON_HILI_OP;  // action

		cel = m_blupi[rank].listCel[i];
		cel.x = (cel.x/2)*2;
		cel.y = (cel.y/2)*2;
	}
	else
	{
		m_iconHili[1][1] = ICON_HILI_OP;  // action

		cel = m_blupi[rank].listCel[i];
	}
	m_celHili = cel;
}

// Retourne l'identificateur du texte correspondant à
// l'objet ou au blupi visé par la souris.

int CDecor::GetResHili(POINT posMouse)
{
	int		res, icon;

	// Les valeurs négatives correspondent aux objets placés
	// au coin inf/droite de la cellule.
	static short table_object[] =
	{
		131,		// nb
		0,0,0,0,0,0,
		TX_OBJ_ARBRE, TX_OBJ_ARBRE, TX_OBJ_ARBRE,
		TX_OBJ_ARBRE, TX_OBJ_ARBRE, TX_OBJ_ARBRE,
		TX_OBJ_FUSEE,0,
		TX_OBJ_METAL,
		0, -TX_OBJ_ARMURE,
		0,0,0,
		TX_OBJ_MUR, TX_OBJ_MUR, TX_OBJ_MUR, TX_OBJ_MUR,
		TX_OBJ_MUR, TX_OBJ_MUR, TX_OBJ_MUR,
		TX_OBJ_TOUR,
		TX_OBJ_LABO, TX_OBJ_LABO,
		TX_OBJ_ARBREb, TX_OBJ_ARBREb, TX_OBJ_ARBREb,
		TX_OBJ_ARBREb, TX_OBJ_ARBREb, TX_OBJ_ARBREb,
		-TX_OBJ_PLANCHE,
		TX_OBJ_ROC, TX_OBJ_ROC, TX_OBJ_ROC, TX_OBJ_ROC,
		TX_OBJ_ROC, TX_OBJ_ROC, TX_OBJ_ROC,
		-TX_OBJ_PIERRE,
		TX_OBJ_FEU, TX_OBJ_FEU, TX_OBJ_FEU, TX_OBJ_FEU,
		TX_OBJ_FEU, TX_OBJ_FEU, TX_OBJ_FEU, TX_OBJ_FEU,
		0,0,0,0,
		-TX_OBJ_TOMATE, -TX_OBJ_TOMATE, -TX_OBJ_TOMATE, -TX_OBJ_TOMATE,
		TX_OBJ_CABANE, TX_OBJ_CABANE,
		-TX_OBJ_OEUF, TX_OBJ_OEUF,
		TX_OBJ_PALISSADE, TX_OBJ_PALISSADE, TX_OBJ_PALISSADE, TX_OBJ_PALISSADE,
		TX_OBJ_PALISSADE, TX_OBJ_PALISSADE, TX_OBJ_PALISSADE,
		TX_OBJ_PONT, TX_OBJ_PONT,
		0,0,0,0,0,0,
		-TX_OBJ_POTION,
		TX_OBJ_FLEUR1, -TX_OBJ_BOUQUET1,
		TX_OBJ_FLEUR2, -TX_OBJ_BOUQUET2,
		-TX_OBJ_DYNAMITE, -TX_OBJ_DYNAMITE, -TX_OBJ_DYNAMITE,
		0,0,0,0,
		-TX_OBJ_POISON,
		-TX_OBJ_PIEGE,
		TX_OBJ_FLEUR3, -TX_OBJ_BOUQUET3,
		-TX_OBJ_ENNEMIp, -TX_OBJ_ENNEMIp, -TX_OBJ_ENNEMIp,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		TX_OBJ_MAISON,
		-TX_OBJ_ENNEMIp,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		-TX_OBJ_BATEAU,
		-TX_OBJ_JEEP,
		TX_OBJ_USINE, TX_OBJ_USINE,
		TX_OBJ_MINEFER, TX_OBJ_MINEFER,
		-TX_OBJ_FER,
		TX_OBJ_DRAPEAU,
		-TX_OBJ_MINE,
		TX_OBJ_MINEFER,
		-TX_OBJ_MINE,
		TX_OBJ_ENNEMI, TX_OBJ_ENNEMI,
		-TX_OBJ_ENNEMIp,
	};

	static short table_floor[] =
	{
		85,			// nb
		0,
		TX_OBJ_HERBE,
		TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE,
		TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE,
		TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE, TX_OBJ_RIVE,
		TX_OBJ_EAU,
		TX_OBJ_DALLE, TX_OBJ_DALLE,
		TX_OBJ_HACHURE,
		TX_OBJ_GLACE,
		TX_OBJ_MOUSSEb,
		TX_OBJ_MOUSSE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_TERRE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_TERRE, TX_OBJ_TERRE, TX_OBJ_TERRE,
		TX_OBJ_HERBE, TX_OBJ_HERBE, TX_OBJ_HERBE,
		TX_OBJ_COUVEUSE, TX_OBJ_COUVEUSE, TX_OBJ_COUVEUSE,
		TX_OBJ_COUVEUSE, TX_OBJ_COUVEUSE,
		TX_OBJ_HERBE,
		TX_OBJ_MOUSSE,
		TX_OBJ_PONT, TX_OBJ_PONT, TX_OBJ_PONT,
		TX_OBJ_PONT, TX_OBJ_PONT, TX_OBJ_PONT,
		TX_OBJ_ENNEMIs, TX_OBJ_MIXTE, TX_OBJ_ENNEMIs,
		TX_OBJ_EAU, TX_OBJ_EAU, TX_OBJ_EAU,
		TX_OBJ_TERRE,
		0,0,0,0,
		0,0,
		TX_OBJ_MIXTE, TX_OBJ_MIXTE,
		TX_OBJ_TELEPORTE, TX_OBJ_TELEPORTE, TX_OBJ_TELEPORTE,
		TX_OBJ_TELEPORTE, TX_OBJ_TELEPORTE,
	};

	if ( m_bHideTooltips )  return -1;  // rien si menu présent

	if ( posMouse.x < POSDRAWX          ||
		 posMouse.x > POSDRAWX+DIMDRAWX ||
		 posMouse.y < POSDRAWY          ||
		 posMouse.y > POSDRAWY+DIMDRAWY )  return -1;

	if ( m_celHili.x != -1 )
	{
		if ( m_rankHili != -1 )  // blupi visé ?
		{
			if ( m_blupi[m_rankHili].perso == 0 )  // blupi ?
			{
				res = TX_OBJ_BLUPI;
				if ( m_blupi[m_rankHili].energy <= MAXENERGY/4 )
				{
					res = TX_OBJ_BLUPIf;
				}
				if ( m_blupi[m_rankHili].bMalade )
				{
					res = TX_OBJ_BLUPIm;
				}
			}
			if ( m_blupi[m_rankHili].perso == 1 )  // araignée ?
			{
				res = TX_OBJ_ARAIGNEE;
			}
			if ( m_blupi[m_rankHili].perso == 2 )  // virus ?
			{
				res = TX_OBJ_VIRUS;
			}
			if ( m_blupi[m_rankHili].perso == 3 )  // tracks ?
			{
				res = TX_OBJ_TRACKS;
			}
			if ( m_blupi[m_rankHili].perso == 4 )  // robot ?
			{
				res = TX_OBJ_ROBOT;
			}
			if ( m_blupi[m_rankHili].perso == 5 )  // bombe ?
			{
				res = TX_OBJ_BOMBE;
			}
			if ( m_blupi[m_rankHili].perso == 7 )  // electro ?
			{
				res = TX_OBJ_ELECTRO;
			}
			if ( m_blupi[m_rankHili].perso == 8 )  // disciple ?
			{
				res = TX_OBJ_DISCIPLE;
			}

			return res;
		}
		else
		{
			icon = m_decor[m_celHili.x/2][m_celHili.y/2].objectIcon;
			if ( icon != -1 && icon < table_object[0] )
			{
				res = table_object[1+icon];
				if ( res != 0 )
				{
					if ( res > 0 )  return res;
					if ( m_celHili.x%2 != 0 &&
						 m_celHili.y%2 != 0 )  return -res;
				}
			}

			icon = m_decor[m_celHili.x/2][m_celHili.y/2].floorIcon;
			if ( icon != -1 && icon < table_floor[0] )
			{
				res = table_floor[1+icon];
				if ( res != 0 )  return res;
			}
		}
	}

	return -1;
}

// Indique si le menu est présent et qu'il faut cacher
// les tooltips du décor.

void CDecor::HideTooltips(BOOL bHide)
{
	m_bHideTooltips = bHide;
}


// Modifie l'origine supérieure/gauche du décor.

void CDecor::SetCoin(POINT coin, BOOL bCenter)
{
	if ( bCenter )
	{
		coin.x -= 10;
		coin.y -= 2;
	}

	if ( coin.x < -8         )  coin.x = -8;
	if ( coin.x > MAXCELX-12 )  coin.x = MAXCELX-12;
	if ( coin.y < -2         )  coin.y = -2;
	if ( coin.y > MAXCELY-4  )  coin.y = MAXCELY-4;

	m_celCoin = coin;
	m_bGroundRedraw = TRUE;  // faudra redessiner les sols
	m_celHili.x = -1;
	m_textLastPos.x = -1;  // tooltips plus lavable !
}

POINT CDecor::GetCoin()
{
	return m_celCoin;
}

POINT CDecor::GetHome()
{
	return m_celHome;
}


// Mémoirise une position pendant le jeu.

void CDecor::MemoPos(int rank, BOOL bRecord)
{
	POINT		pos;

	pos.x = LXIMAGE/2;
	pos.y = LYIMAGE/2;

	if ( rank < 0 || rank >= 4 )  return;

	if ( bRecord )
	{
		m_pSound->PlayImage(SOUND_CLOSE, pos);
		m_memoPos[rank] = m_celCoin;
	}
	else
	{
		if ( m_memoPos[rank].x == 0 &&
			 m_memoPos[rank].y == 0 )
		{
			m_pSound->PlayImage(SOUND_BOING, pos);
		}
		else
		{
			m_pSound->PlayImage(SOUND_BUT, pos);
			SetCoin(m_memoPos[rank], FALSE);
		}
	}
}


// Gestion du temps absolu global.

void CDecor::SetTime(int time)
{
	m_time            = time;
	m_timeConst       = time;  // vraiment ?
	m_timeFlipOutline = time;
}

int CDecor::GetTime()
{
	return m_time;
}


// Gestion de la musique midi.

void CDecor::SetMusic(int music)
{
	m_music = music;
}

int CDecor::GetMusic()
{
	return m_music;
}


// Gestion de la difficulté.

void CDecor::SetSkill(int skill)
{
	m_skill = skill;
}

int CDecor::GetSkill()
{
	return m_skill;
}


// Gestion de la région.
//	0 = normal
//	1 = palmier
//	2 = hiver
//	3 = sapin

void CDecor::SetRegion(int region)
{
	m_region = region;
}

int CDecor::GetRegion()
{
	return m_region;
}


// Gestion des infos.

void CDecor::SetInfoMode(BOOL bInfo)
{
	m_bInfo = bInfo;
	m_bGroundRedraw = TRUE;  // faudra redessiner les sols
}

BOOL CDecor::GetInfoMode()
{
	return m_bInfo;
}

void CDecor::SetInfoHeight(int height)
{
	m_infoHeight = height;
	m_bGroundRedraw = TRUE;  // faudra redessiner les sols
}

int CDecor::GetInfoHeight()
{
	if ( m_bInfo )  return m_infoHeight;
	else            return 0;
}


// Retourne le pointeur à la liste des boutons existants.

char* CDecor::GetButtonExist()
{
	return m_buttonExist;
}



// Ouvre le buffer pour le undo pendant la construction.

void CDecor::UndoOpen()
{
	if ( m_pUndoDecor == NULL )
	{
		m_pUndoDecor = (Cellule*)malloc(sizeof(Cellule)*(MAXCELX/2)*(MAXCELY/2));
	}
}

// Ferme le buffer pour le undo pendant la construction.

void CDecor::UndoClose()
{
	if ( m_pUndoDecor != NULL )
	{
		free(m_pUndoDecor);
		m_pUndoDecor = NULL;
	}
}

// Copie le décor dans le buffer pour le undo.

void CDecor::UndoCopy()
{
	UndoOpen();  // ouvre le buffer du undo si nécessaire

	if ( m_pUndoDecor != NULL )
	{
		memcpy(m_pUndoDecor, &m_decor, sizeof(Cellule)*(MAXCELX/2)*(MAXCELY/2));
	}
}

// Revient en arrière pour tout le décor.

void CDecor::UndoBack()
{
	if ( m_pUndoDecor != NULL )
	{
		memcpy(&m_decor, m_pUndoDecor, sizeof(Cellule)*(MAXCELX/2)*(MAXCELY/2));
		UndoClose();
		m_bGroundRedraw = TRUE;
	}
}

// Indique s'il est possible d'effectuer un undo.

BOOL CDecor::IsUndo()
{
	return ( m_pUndoDecor != NULL );
}
