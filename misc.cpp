// misc.cpp
//

#include <SDL_mouse.h>
#include <stdio.h>
#include <stdlib.h>
#include "def.h"


// Variables globales

extern bool		g_bFullScreen;	// false si mode de test
extern int		g_mouseType;


// Affiche un message de debug.

void OutputDebug(char *pMessage)
{
#ifdef _DEBUG
	OutputDebugString(pMessage);
#endif
}

// Conversion de la position de la souris.

POINT ConvLongToPos(LPARAM lParam)
{
	POINT	pos;

	pos.x = LOWORD(lParam);  // horizontal position of cursor 
	pos.y = HIWORD(lParam);  // vertical position of cursor

//	if ( !g_bFullScreen )
//	{
//		pos.y -= GetSystemMetrics(SM_CYCAPTION);
//	}

	return pos;
}


// Réinitialise le générateur aléatoire.

void InitRandom()
{
	srand(1);
}

// Retourne un nombre aléatoire compris entre
// deux bornes (inclues).

int Random(int min, int max)
{
	long	n;
	
	n = rand();
	n = min+(n%(max-min+1));

	return (int)n;
}


// Retourne le nom de dossier en cours.

void GetCurrentDir(char *pName, size_t lg)
{
	size_t		i;

	strncpy(pName, _pgmptr, lg-1);
	pName[lg-1] = 0;

	lg = strlen(pName);
	if ( lg == 0 )  return;

	for ( i=0 ; i<lg ; i++ )
	{
		pName[i] = tolower(pName[i]);
	}

	while ( lg > 0 )
	{
		lg --;
		if ( pName[lg] == '\\' )
		{
			pName[lg+1] = 0;
			break;
		}
	}

	if ( lg > 6 && strcmp(pName+lg-6, "\\debug\\") == 0 )
	{
		pName[lg-5] = 0;  // ignore le dossier \debug !
	}
}

// Ajoute le chemin permettant de lire un fichier
// utilisateur.

void AddUserPath(char *pFilename)
{
	char					temp[MAX_PATH];
	char*					pText;
	size_t					pos;
	char					last;
	SECURITY_ATTRIBUTES		att;

	strcpy(temp, "c:\\Planète Blupi\\");

	att.nLength = sizeof(SECURITY_ATTRIBUTES);
	att.lpSecurityDescriptor = nullptr;
	att.bInheritHandle = false;
	CreateDirectory(temp, &att);

	pText = strstr(pFilename, "\\");
	if ( pText != nullptr )
	{
		pos = strlen(temp)+(pText-pFilename)+1;
		strcat(temp, pFilename);
		last = temp[pos];
		temp[pos] = 0;
		CreateDirectory(temp, &att);
		temp[pos] = last;
	}
	else
	{
		strcat(temp, pFilename);
	}

	strcpy(pFilename, temp);
}
