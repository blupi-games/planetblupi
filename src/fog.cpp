// Fog.cpp

#include "decor.h"

// Cette table indique les quarts de cases contenant du
// brouillard lorsque la valeur est à un.
//		0  1
//		2  3
static char tableFog[15*4] =
{
	1,1,1,0,	// 0
	1,1,0,0,	// 1
	1,1,0,1,	// 2
	1,0,1,0,	// 3
	1,1,1,1,	// 4
	0,1,0,1,	// 5
	1,0,1,1,	// 6
	0,0,1,1,	// 7
	0,1,1,1,	// 8
	0,0,0,1,	// 9
	0,0,1,0,	// 10
	1,0,0,1,	// 11
	0,1,0,0,	// 12
	1,0,0,0,	// 13
	0,1,1,0,	// 14
};

// Retourne les bits contenant du brouillard.

bool GetFogBits(int icon, char *pBits)
{
	pBits[0] = 0;
	pBits[1] = 0;
	pBits[2] = 0;
	pBits[3] = 0;

	if ( icon < 0 || icon >= 15 )  return true;

	pBits[0] = tableFog[icon*4+0];
	pBits[1] = tableFog[icon*4+1];
	pBits[2] = tableFog[icon*4+2];
	pBits[3] = tableFog[icon*4+3];

	return true;
}

// Retourne l'icône correspondant aux bits de brouillard.

int GetFogIcon(char *pBits)
{
	int		i;

	for ( i=0 ; i<15 ; i++ )
	{
		if ( tableFog[i*4+0] == pBits[0] &&
			 tableFog[i*4+1] == pBits[1] &&
			 tableFog[i*4+2] == pBits[2] &&
			 tableFog[i*4+3] == pBits[3] )  return i;
	}

	return -1;
}


// Table donnant la "vision" d'un blupi dans le
// brouillard.

static char table_fog[17*17] =
{
#if 1
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 1,-1, 5, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 1,-1,-1,-1, 5, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 1,-1,-1,-1,-1,-1, 5, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4, 4, 4, 4,
	 4, 4, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4, 4, 4,
	 4, 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4, 4,
	 4, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4,
	 4, 4, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 8, 4,
	 4, 4, 4, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 4, 4,
	 4, 4, 4, 4, 4, 3,-1,-1,-1,-1,-1,-1,-1, 7, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 3,-1,-1,-1,-1,-1, 7, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 3,-1,-1,-1, 7, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 3,-1, 7, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
#else
	 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 1,-1, 5, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 1,-1,-1,-1, 5, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 1,-1,-1,-1,-1,-1, 5, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4, 4, 4,
	 4, 4, 4, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4, 4,
	 4, 4, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4, 4,
	 4, 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 5, 4,
	 0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 8,
	 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 4,
	 4, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 4, 4,
	 4, 4, 4, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1, 7, 4, 4, 4,
	 4, 4, 4, 4, 3,-1,-1,-1,-1,-1,-1,-1, 7, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 3,-1,-1,-1,-1,-1, 7, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 3,-1,-1,-1, 7, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 3,-1, 7, 4, 4, 4, 4, 4, 4, 4,
	 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4,
#endif
};

// Ecarte le brouillard autour d'un blupi.

void CDecor::BlupiPushFog(int rank)
{
	int			x, y, i;
	POINT		cel;
	char		cBits[4];
	char		nBits[4];

	if ( m_blupi[rank].perso != 0 &&
		 m_blupi[rank].perso != 8 )  return;

	for ( y=0 ; y<17 ; y++ )
	{
		for ( x=0 ; x<17 ; x++ )
		{
			if ( x%2 != y%2 )  continue;
			if ( table_fog[x+y*17] == FOGHIDE )  continue;
			
			cel.x = (x + ((m_blupi[rank].cel.x+1)/4)*2 - 8)*2;
			cel.y = (y + ((m_blupi[rank].cel.y+1)/4)*2 - 8)*2;

			// Ne pas utiliser IsValid pour permettre d'aller
			// jusqu'au bord !
			if ( cel.x >= 0 && cel.x < MAXCELX &&
				 cel.y >= 0 && cel.y < MAXCELX )
			{
				if ( m_decor[cel.x/2][cel.y/2].fog != -1 )
				{
					GetFogBits(m_decor[cel.x/2][cel.y/2].fog, cBits);
					GetFogBits(table_fog[x+y*17],             nBits);

					for ( i=0 ; i<4 ; i++ )
					{
						nBits[i] &= cBits[i];  // "ou" visibilité
					}

					m_decor[cel.x/2][cel.y/2].fog = GetFogIcon(nBits);
				}
			}
		}
	}
}

