// DecMap.cpp
//

#include "DEF.H"
#include "DECOR.H"

#define MAP_CADRE	1
#define MAP_FOG		2
#define MAP_BLUPI	3
#define MAP_SEE		4
#define MAP_TREE	5
#define MAP_HERB1	6
#define MAP_HERB2	7
#define MAP_TERRE	8
#define MAP_DALLE	9
#define MAP_PERSO	10
#define MAP_NURSE	11
#define MAP_ROC		12
#define MAP_MUR		13
#define MAP_EGG		14
#define MAP_FIRE	15
#define MAP_TOMAT	16
#define MAP_BUILD	17
#define MAP_ENNEMI	18
#define MAP_FLEUR	19

#define MAPCADREX	((DIMDRAWX/DIMCELX)*2+1)
#define MAPCADREY	(DIMDRAWY/DIMCELY+1)



// Bitmap de la carte.

static char g_map8_bits[DIMMAPY][DIMMAPX];
static unsigned short g_map16_bits[DIMMAPY][DIMMAPX];
static BOOL g_bPalette;




// Initialise les couleurs pour la carte.

void CDecor::MapInitColors()
{
	g_bPalette = m_pPixmap->IsPalette();

	m_colors[MAP_CADRE] = m_pPixmap->SearchColor(255,0,0);      // rouge
	m_colors[MAP_FOG]   = m_pPixmap->SearchColor(0,0,0);        // noir
	m_colors[MAP_BLUPI] = m_pPixmap->SearchColor(255,255,0);    // jaune
	m_colors[MAP_SEE]   = m_pPixmap->SearchColor(102,102,204);  // bleu
	m_colors[MAP_DALLE] = m_pPixmap->SearchColor(192,192,192);  // gris
	m_colors[MAP_PERSO] = m_pPixmap->SearchColor(255,0,0);      // rouge
	m_colors[MAP_NURSE] = m_pPixmap->SearchColor(255,0,0);      // rouge
	m_colors[MAP_ROC]   = m_pPixmap->SearchColor(214,214,214);  // gris clair
	m_colors[MAP_MUR]   = m_pPixmap->SearchColor(100,100,100);  // gris moyen
	m_colors[MAP_EGG]   = m_pPixmap->SearchColor(255,255,255);  // blanc
	m_colors[MAP_FIRE]  = m_pPixmap->SearchColor(255,0,0);      // rouge
	m_colors[MAP_TOMAT] = m_pPixmap->SearchColor(255,0,0);      // rouge
	m_colors[MAP_BUILD] = m_pPixmap->SearchColor(0,0,0);        // noir
	m_colors[MAP_ENNEMI]= m_pPixmap->SearchColor(0,192,255);    // bleu métal
	m_colors[MAP_FLEUR] = m_pPixmap->SearchColor(255,206,0);    // jaune

	m_colors[MAP_TREE]  = m_pPixmap->SearchColor(0,102,0);      // vert foncé
	m_colors[MAP_HERB1] = m_pPixmap->SearchColor(0,204,51);     // vert clair
	m_colors[MAP_HERB2] = m_pPixmap->SearchColor(0,156,8);      // vert moyen
	m_colors[MAP_TERRE] = m_pPixmap->SearchColor(94,78,12);     // brun

	if ( m_region == 1 )  // palmiers
	{
		m_colors[MAP_TREE]  = m_pPixmap->SearchColor(38,197,42);
		m_colors[MAP_HERB1] = m_pPixmap->SearchColor(184,140,1);
		m_colors[MAP_HERB2] = m_pPixmap->SearchColor(145,110,5);
		m_colors[MAP_TERRE] = m_pPixmap->SearchColor(192,192,192);
	}

	if ( m_region == 2 )  // hiver
	{
		m_colors[MAP_TREE]  = m_pPixmap->SearchColor(152,205,222);
		m_colors[MAP_HERB1] = m_pPixmap->SearchColor(219,234,239);
		m_colors[MAP_HERB2] = m_pPixmap->SearchColor(223,173,90);
		m_colors[MAP_TERRE] = m_pPixmap->SearchColor(152,205,222);
	}

	if ( m_region == 3 )  // sapins
	{
		m_colors[MAP_TREE]  = m_pPixmap->SearchColor(0,102,0);
		m_colors[MAP_HERB1] = m_pPixmap->SearchColor(38,197,42);
		m_colors[MAP_HERB2] = m_pPixmap->SearchColor(140,140,0);
		m_colors[MAP_TERRE] = m_pPixmap->SearchColor(172,178,173);
	}
}


// COnversion d'un cellule en point dans la carte.

POINT CDecor::ConvCelToMap(POINT cel)
{
	POINT	pos;

	pos.x = (cel.x-m_celCoin.x)-(cel.y-m_celCoin.y);
	pos.y = ((cel.x-m_celCoin.x)+(cel.y-m_celCoin.y))/2;

	pos.x += (DIMMAPX-MAPCADREX)/2;
	pos.y += (DIMMAPY-MAPCADREY)/2;

	return pos;
}

// Conversion d'un point dans la carte en cellule.

POINT CDecor::ConvMapToCel(POINT pos)
{
	POINT	cel;

	pos.x -= ((DIMMAPX-MAPCADREX)/4)*2;
	pos.y -= ((DIMMAPY-MAPCADREY)/4)*2;

	cel.x = pos.y + pos.x/2;
	cel.y = pos.y - pos.x/2;

	cel.x += m_celCoin.x;
	cel.y += m_celCoin.y;

	return cel;
}


// Déplace le décor suite à un clic dans la carte.

BOOL CDecor::MapMove(POINT pos)
{
	POINT		cel;

	if ( pos.x >= POSMAPX && pos.x < POSMAPX+DIMMAPX &&
		 pos.y >= POSMAPY && pos.y < POSMAPY+DIMMAPY )
	{
		pos.x -= POSMAPX;
		pos.y -= POSMAPY;
		cel = ConvMapToCel(pos);
		cel.x = cel.x-10;
		cel.y = cel.y;
		SetCoin(cel);
		NextPhase(0);  // faudra refaire la carte tout de suite
		return TRUE;
	}

	return FALSE;
}


static char color_floor[] =
{
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,		// 0
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_HERB1,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_HERB1,
	MAP_HERB1, MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_HERB1, MAP_SEE,   MAP_SEE,   MAP_HERB1,
	MAP_SEE,   MAP_SEE,   MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_SEE,   MAP_SEE,   MAP_HERB1,
	MAP_HERB1, MAP_HERB1, MAP_SEE,   MAP_SEE,
	MAP_SEE,   MAP_HERB1, MAP_HERB1, MAP_SEE,
	MAP_SEE,   MAP_SEE,   MAP_HERB1, MAP_HERB1,
	MAP_SEE,   MAP_HERB1, MAP_HERB1, MAP_SEE,
	MAP_HERB1, MAP_HERB1, MAP_SEE,   MAP_SEE,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_DALLE, MAP_DALLE, MAP_DALLE, MAP_DALLE,
	MAP_DALLE, MAP_DALLE, MAP_DALLE, MAP_DALLE,		// 16
	MAP_DALLE, MAP_DALLE, MAP_DALLE, MAP_DALLE,
	MAP_DALLE, MAP_DALLE, MAP_DALLE, MAP_DALLE,
	MAP_DALLE, MAP_DALLE, MAP_DALLE, MAP_DALLE,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB1,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB1,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB1,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB1,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB1,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB1, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB2,
	MAP_HERB2, MAP_HERB2, MAP_HERB2, MAP_HERB1,		// 32
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_HERB1, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_HERB1,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,		// 48
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_NURSE, MAP_NURSE, MAP_NURSE, MAP_NURSE,
	MAP_NURSE, MAP_NURSE, MAP_NURSE, MAP_NURSE,
	MAP_NURSE, MAP_NURSE, MAP_NURSE, MAP_NURSE,
	MAP_NURSE, MAP_NURSE, MAP_NURSE, MAP_NURSE,
	MAP_NURSE, MAP_NURSE, MAP_NURSE, MAP_NURSE,
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,		// 64
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_SEE,   MAP_SEE,   MAP_SEE,   MAP_SEE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
};

static char color_object[] =
{
	0,         MAP_TREE,  0,         MAP_TREE,
	MAP_TREE,  0,         MAP_TREE,  0,
	0,         MAP_TREE,  0,         MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,

	0,         MAP_TREE,  0,         MAP_TREE,	// arbre
	MAP_TREE,  0,         MAP_TREE,  0,
	0,         MAP_TREE,  0,         MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,

	0,         MAP_TREE,  0,         MAP_TREE,	// sapin
	MAP_TREE,  0,         MAP_TREE,  0,
	0,         MAP_TREE,  0,         MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,

	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,	// palmiers
	MAP_TREE,  MAP_TREE,  MAP_TREE,  MAP_TREE,

	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,	// mur
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,

	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,	// maison
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,

	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,	// arbre sans feuille
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,
	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,

	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,	// planches

	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,	// rochers
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,
	MAP_ROC,   MAP_ROC,   MAP_ROC,   MAP_ROC,	// pierres

	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,	// feu
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,

	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,	// test
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,

	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// tomates
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,

	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,	// cabane
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,

	MAP_EGG,   MAP_EGG,   MAP_EGG,   MAP_EGG,	// oeufs
	MAP_EGG,   MAP_EGG,   MAP_EGG,   MAP_EGG,

	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,	// palissade
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,

	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,	// pont construction
	MAP_MUR,   MAP_MUR,   MAP_MUR,   MAP_MUR,

	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,	// rayon
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,

	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,	// bouteille

	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,		// fleurs
	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,
	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,
	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,

	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,	// dynamite
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,
	MAP_FIRE,  MAP_FIRE,  MAP_FIRE,  MAP_FIRE,

	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// poison
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,

	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,
	MAP_FLEUR, MAP_FLEUR, MAP_FLEUR, MAP_FLEUR,

	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,	// ennemi piégé
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,

	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,	// usine
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,

	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,	// barrière
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,

	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,	// maison
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,	// ennemi piégé

	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,
	MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,MAP_ENNEMI,

	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// bateau
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// jeep

	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,	// usine
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,	// mine de fer
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,

	MAP_TERRE, MAP_TERRE, MAP_TERRE, MAP_TERRE,	// fer
	MAP_EGG,   MAP_EGG,   MAP_EGG,   MAP_EGG,	// drapeau
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// mine
	MAP_BUILD, MAP_BUILD, MAP_BUILD, MAP_BUILD,
	MAP_TOMAT, MAP_TOMAT, MAP_TOMAT, MAP_TOMAT,	// mine
};

static char color_deffog[4] =
{
	MAP_FOG, MAP_FOG, MAP_FOG, MAP_FOG, 
};

static char color_deftree[4] =
{
	MAP_TREE, MAP_TREE, MAP_TREE, MAP_TREE,
};

static char color_deffloor[4] =
{
	MAP_HERB1, MAP_HERB1, MAP_HERB1, MAP_HERB1,
};

static char color_fire[4] =
{
	MAP_FIRE, MAP_FIRE, MAP_FIRE, MAP_FIRE,
};

// Met le contenu d'une cellule dans le bitmap de la carte
// (sol, objets et brouillard).

void CDecor::MapPutCel(POINT pos)
{
	POINT		cel, fogCel;
	int			icon, i;
	char*		pColors;

	cel = ConvMapToCel(pos);
	cel.x = (cel.x/2)*2;
	cel.y = (cel.y/2)*2;

	if ( !IsValid(cel) )
	{
		pColors = color_deffog;
		goto color;
	}

	if ( m_bFog )
	{
//		fogPos.x = (pos.x/4)*4;
//		fogPos.y = (pos.y/4)*4;
//		fogCel = ConvMapToCel(fogPos);
		fogCel = cel;
		fogCel.x = (fogCel.x/4)*4;
		fogCel.y = (fogCel.y/4)*4;
		if ( fogCel.x < 0 || fogCel.x >= MAXCELX ||
			 fogCel.y < 0 || fogCel.y >= MAXCELY ||
			 m_decor[fogCel.x/2][fogCel.y/2].fog == FOGHIDE )  // caché ?
		{
			pColors = color_deffog;
			goto color;
		}
	}

	icon = m_decor[cel.x/2][cel.y/2].objectIcon;
	if ( icon != -1 )
	{
		if ( m_decor[cel.x/2][cel.y/2].fire > 0 &&
			 m_decor[cel.x/2][cel.y/2].fire < MoveMaxFire() )
		{
			pColors = color_fire;
			goto color;
		}

		if ( icon >= 0 && icon <= 127 )  pColors = color_object+4*(icon-0);
		else                             pColors = color_deftree;
		goto color;
	}

	icon = m_decor[cel.x/2][cel.y/2].floorIcon;
	if ( icon >= 0 && icon <= 71 )  pColors = color_floor+4*(icon-0);
	else                            pColors = color_deffloor;

	color:
	if ( g_bPalette )
	{
		for ( i=0 ; i<4 ; i++ )
		{
			if ( pos.x+i >= 0 && pos.x+i < DIMMAPX )
			{
				icon = *pColors++;
				if ( icon != 0 )
				{
					g_map8_bits[pos.y][pos.x+i] = (char)m_colors[icon];
				}
			}
		}
	}
	else
	{
		for ( i=0 ; i<4 ; i++ )
		{
			if ( pos.x+i >= 0 && pos.x+i < DIMMAPX )
			{
				icon = *pColors++;
				if ( icon != 0 )
				{
					g_map16_bits[pos.y][pos.x+i] = m_colors[icon];
				}
			}
		}
	}
}


// Génère la carte.

BOOL CDecor::GenerateMap()
{
	HBITMAP		hbm;
	POINT		dim, pos, cel;
	int			dx, rank, i;

	if ( m_phase != -1 && m_phase%20 != 0 )  return TRUE;

	// Dessine le décor (sol, objets et brouillard).
	for ( pos.y=0 ; pos.y<DIMMAPY ; pos.y++ )
	{
		dx = pos.y%2;
		for ( pos.x=-dx*2 ; pos.x<DIMMAPX ; pos.x+=4 )
		{
			MapPutCel(pos);
		}
	}

	// Dessine les blupi.
	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist )
		{
			pos = ConvCelToMap(m_blupi[rank].cel);
			if ( pos.x >= 0 && pos.x < DIMMAPX-1 &&
				 pos.y >= 0 && pos.y < DIMMAPY-1 )
			{
				if ( m_blupi[rank].perso == 0 ||
					 m_blupi[rank].perso == 8 )  i = MAP_BLUPI;
				else                             i = MAP_PERSO;
				if ( g_bPalette )
				{
					g_map8_bits[pos.y+0][pos.x+0] = (char)m_colors[i];
					g_map8_bits[pos.y+0][pos.x+1] = (char)m_colors[i];
					g_map8_bits[pos.y+1][pos.x+0] = (char)m_colors[i];
					g_map8_bits[pos.y+1][pos.x+1] = (char)m_colors[i];
				}
				else
				{
					g_map16_bits[pos.y+0][pos.x+0] = m_colors[i];
					g_map16_bits[pos.y+0][pos.x+1] = m_colors[i];
					g_map16_bits[pos.y+1][pos.x+0] = m_colors[i];
					g_map16_bits[pos.y+1][pos.x+1] = m_colors[i];
				}
			}
		}
	}

	// Dessine le cadre.
	cel = m_celCoin;
	pos = ConvCelToMap(cel);

	if ( g_bPalette )
	{
		for ( i=pos.x ; i<pos.x+MAPCADREX ; i++ )
		{
			g_map8_bits[pos.y]          [i] = (char)m_colors[MAP_CADRE];
			g_map8_bits[pos.y+MAPCADREY][i] = (char)m_colors[MAP_CADRE];
		}
		for ( i=pos.y ; i<=pos.y+MAPCADREY ; i++ )
		{
			g_map8_bits[i][pos.x]           = (char)m_colors[MAP_CADRE];
			g_map8_bits[i][pos.x+MAPCADREX] = (char)m_colors[MAP_CADRE];
		}
	}
	else
	{
		for ( i=pos.x ; i<pos.x+MAPCADREX ; i++ )
		{
			g_map16_bits[pos.y]          [i] = m_colors[MAP_CADRE];
			g_map16_bits[pos.y+MAPCADREY][i] = m_colors[MAP_CADRE];
		}
		for ( i=pos.y ; i<=pos.y+MAPCADREY ; i++ )
		{
			g_map16_bits[i][pos.x]           = m_colors[MAP_CADRE];
			g_map16_bits[i][pos.x+MAPCADREX] = m_colors[MAP_CADRE];
		}
	}

	if ( g_bPalette )
	{
		hbm = CreateBitmap(DIMMAPX, DIMMAPY, 1, 8, g_map8_bits);
	}
	else
	{
		hbm = CreateBitmap(DIMMAPX, DIMMAPY, 1, 16, g_map16_bits);
	}
	if ( hbm == NULL )  return FALSE;

	dim.x = DIMMAPX;
	dim.y = DIMMAPY;
	m_pPixmap->Cache(CHMAP, hbm, dim);

	pos.x = POSMAPX;
	pos.y = POSMAPY;
	m_pPixmap->DrawIcon(-1, CHMAP, 0, pos);

	DeleteObject(hbm);
	return TRUE;
}

