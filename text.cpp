// Text.cpp

#include <stdlib.h>
#include <stdio.h>
#include "def.h"
#include "pixmap.h"
#include "text.h"


/////////////////////////////////////////////////////////////////////////////


// Retourne l'offset pour un caractère donné.

int GetOffset(char c)
{
	int		i;

	static unsigned char table_accents[15] =
	{
		0xFC, 0xE0, 0xE2, 0xE9, 0xE8, 0xEB, 0xEA, 0xEF,
		0xEE, 0xF4, 0xF9, 0xFB, 0xE4, 0xF6, 0xE7
	};

	for ( i=0 ; i<15 ; i++ )
	{
		if ( (unsigned char)c == table_accents[i] )
		{
			return 15+i;
		}
	}
	if ( c<0 || c>128 )  return 1;  // carré

	return c;
}

// Retourne la longueur d'un caractère.

int GetCharWidth(char c, int font)
{
	static unsigned char table_width[128] =
	{
		9,10,10,10,10,10,10,10,10,10,10,10,10,10,10,8,
		9,9,8,8,8,8,5,5,8,8,8,9,8,8,10,10,
		5,6,9,13,11,12,12,6,6,6,12,12,5,9,6,9,
		8,8,9,9,8,9,8,8,9,9,6,6,8,9,10,11,
		12,8,9,9,9,8,8,8,9,4,8,9,8,10,9,9,
		8,9,8,9,10,8,9,11,9,8,10,7,10,7,13,13,
		9,9,8,8,8,8,6,8,8,4,6,8,4,12,8,8,
		8,8,7,6,7,8,8,10,8,8,7,6,6,6,10,0,
	};

	static unsigned char table_width_little[128] =
	{
		6,6,6,6,6,6,6,6,6,6,6,6,5,6,6,7,
		6,6,6,6,6,6,3,3,6,6,6,6,6,6,5,5,
		3,3,5,8,5,11,9,3,4,4,6,6,3,4,3,6,
		5,5,5,5,5,5,5,5,5,5,3,3,7,6,7,6,
		9,8,6,7,7,5,5,8,7,2,4,7,5,10,7,8,
		6,8,7,6,6,6,8,12,7,6,6,3,5,3,6,8,
		4,6,6,6,6,6,4,6,6,2,3,5,2,10,6,6,
		6,6,3,5,3,6,6,8,6,6,5,4,6,4,7,0,
	};

	if ( font == FONTLITTLE )
	{
		return table_width_little[GetOffset(c)];
	}
	else
	{
		return table_width[GetOffset(c)]-1;
	}
}

// Affiche un texte.

void DrawText(CPixmap *pPixmap, POINT pos, char *pText, int font)
{
	int		rank;

	if ( font == FONTLITTLE )
	{
		while ( *pText != 0 )
		{
			rank = GetOffset(*pText);
			pPixmap->DrawIcon(-1, CHLITTLE, rank, pos);

			pos.x += GetCharWidth(*pText++, font);
		}
	}
	else
	{
		while ( *pText != 0 )
		{
			rank = GetOffset(*pText);
			rank += 128*font;
			pPixmap->DrawIcon(-1, CHTEXT, rank, pos);

			pos.x += GetCharWidth(*pText++, font);
		}
	}
}

// Affiche un texte penché.

void DrawTextPente(CPixmap *pPixmap, POINT pos, char *pText,
				   int pente, int font)
{
	int		rank, lg, rel, start;

	start = pos.y;
	rel = 0;
	while ( *pText != 0 )
	{
		rank = GetOffset(*pText);
		rank += 128*font;
		pPixmap->DrawIcon(-1, CHTEXT, rank, pos);

		lg = GetCharWidth(*pText++, font);
		rel += lg;
		pos.x += lg;
		pos.y = start + rel/pente;
	}
}

// Affiche un pavé de texte.
// Une ligne vide est affichée avec un demi interligne !
// Si part != -1, n'affiche que les lignes qui commencent
// par "n|", avec n=part.

void DrawTextRect(CPixmap *pPixmap, POINT pos, char *pText,
				  int pente, int font, int part)
{
	char		text[100];
	char*		pDest;
	int			itl;

	if ( font == FONTLITTLE )  itl = DIMLITTLEY;
	else                       itl = DIMTEXTY;

	while ( *pText != 0 )
	{
		pDest = text;
		while ( *pText != 0 && *pText != '\r' && *pText != '\n' )
		{
			*pDest++ = *pText++;
		}
		*pDest = 0;
		if ( *pText == '\r' )  pText ++;  // saute '\r'
		if ( *pText == '\n' )  pText ++;  // saute '\n'

		pDest = text;
		if ( text[0] != 0 && text[1] == '|' )  // commence par "n|" ?
		{
			if ( part != -1 && part != text[0]-'0' )  continue;
			pDest += 2;  // saute "n|"
		}
		else
		{
			if ( part != -1 )  continue;
		}

		if ( pente == 0 )
		{
			DrawText(pPixmap, pos, pDest, font);
		}
		else
		{
			DrawTextPente(pPixmap, pos, pDest, pente, font);
		}

		if ( pDest[0] == 0 )  // ligne vide ?
		{
			pos.y += itl/2;  // descend de 1/2 ligne
		}
		else
		{
			pos.y += itl;  // passe à la ligne suivante
		}
	}
}

// Affiche un texte centré pouvant éventuellement
// contenir plusieurs lignes séparées par des '\n'.

void DrawTextCenter(CPixmap *pPixmap, POINT pos, char *pText, int font)
{
	char		text[100];
	char*		pDest;
	int			itl;
	POINT		start;

	if ( font == FONTLITTLE )  itl = DIMLITTLEY;
	else                       itl = DIMTEXTY;

	while ( *pText != 0 )
	{
		pDest = text;
		while ( *pText != 0 && *pText != '\r' && *pText != '\n' )
		{
			*pDest++ = *pText++;
		}
		*pDest = 0;
		if ( *pText == '\r' )  pText ++;  // saute '\r'
		if ( *pText == '\n' )  pText ++;  // saute '\n'

		pDest = text;
		start.x = pos.x - GetTextWidth(pDest)/2;
		start.y = pos.y;
		DrawText(pPixmap, start, pDest, font);

		if ( pDest[0] == 0 )  // ligne vide ?
		{
			pos.y += itl/2;  // descend de 1/2 ligne
		}
		else
		{
			pos.y += itl;  // passe à la ligne suivante
		}
	}
}

// Retourne la hauteur d'un texte.

int GetTextHeight(char *pText, int font, int part)
{
	char		text[100];
	char*		pDest;
	int			itl;
	int			h=0;

	if ( font == FONTLITTLE )  itl = DIMLITTLEY;
	else                       itl = DIMTEXTY;

	while ( *pText != 0 )
	{
		pDest = text;
		while ( *pText != 0 && *pText != '\r' && *pText != '\n' )
		{
			*pDest++ = *pText++;
		}
		*pDest = 0;
		if ( *pText == '\r' )  pText ++;  // saute '\r'
		if ( *pText == '\n' )  pText ++;  // saute '\n'

		pDest = text;
		if ( text[0] != 0 && text[1] == '|' )  // commence par "n|" ?
		{
			if ( part != -1 && part != text[0]-'0' )  continue;
			pDest += 2;  // saute "n|"
		}
		else
		{
			if ( part != -1 )  continue;
		}

		if ( pDest[0] == 0 )  // ligne vide ?
		{
			h += itl/2;  // descend de 1/2 ligne
		}
		else
		{
			h += itl;  // passe à la ligne suivante
		}
	}

	return h;
}

// Retourne la longueur d'un texte.

int GetTextWidth(char *pText, int font)
{
	int		width = 0;

	while ( *pText != 0 )
	{
		width += GetCharWidth(*pText++, font);
	}

	return width;
}


// Retourne la longueur d'un grand chiffre.

void GetBignumInfo(int num, int &start, int &lg)
{
	static int table[11] =
	{
		0,53,87,133,164,217,253,297,340,382,426
	};

	start = table[num];
	lg    = table[num+1]-table[num];
}

// Affiche un grand nombre.

void DrawBignum(CPixmap *pPixmap, POINT pos, int num)
{
	char	string[10];
	int		i = 0;
	int		start, lg;
	RECT	rect;

	sprintf(string, "%d", num);

	rect.top    = 0;
	rect.bottom = 52;
	while ( string[i] != 0 )
	{
		GetBignumInfo(string[i]-'0', start, lg);

		rect.left  = start;
		rect.right = start+lg;
		pPixmap->DrawPart(-1, CHBIGNUM, pos, rect);
		pos.x += lg+4;

		i ++;
	}
}

// Retourne la longueur d'un grand nombre.

int GetBignumWidth(int num)
{
	char	string[10];
	int		i = 0;
	int		start, lg;
	int		width = -4;

	sprintf(string, "%d", num);

	while ( string[i] != 0 )
	{
		GetBignumInfo(string[i]-'0', start, lg);
		width += lg+4;
		i ++;
	}

	return width;
}

