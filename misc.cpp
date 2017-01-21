// misc.cpp
//


#include <dsound.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include "def.h"


// Variables globales

HINSTANCE		g_hInstance;
int				g_lastSprite = 0;
extern BOOL		g_bFullScreen;	// FALSE si mode de test
extern int		g_mouseType;
extern char		g_CDPath[MAX_PATH];



// Initialise HInstance.

void InitHInstance(HINSTANCE hInstance)
{
	g_hInstance = hInstance;
}


// Affiche un message de debug.

void OutputDebug(char *pMessage)
{
#ifdef _DEBUG
	OutputDebugString(pMessage);
#endif
}


// Charge un texte dans une ressource.

void LoadString(UINT nID, char *pBuffer, int lgBuffer)
{
	LoadString(g_hInstance, nID, pBuffer, lgBuffer);
}

// Change le lutin de la souris.

void ChangeSprite(int sprite)
{
	HCURSOR		hCursor = nullptr;

	if ( g_mouseType == MOUSETYPEGRA )  return;
	if ( g_lastSprite == sprite )  return;

	if ( sprite == SPRITE_ARROW   )  hCursor = LoadCursor(g_hInstance, "IDC_ARROW");
	if ( sprite == SPRITE_POINTER )  hCursor = LoadCursor(g_hInstance, "IDC_POINTER");
	if ( sprite == SPRITE_MAP     )  hCursor = LoadCursor(g_hInstance, "IDC_MAP");
	if ( sprite == SPRITE_ARROWU  )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWU");
	if ( sprite == SPRITE_ARROWD  )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWD");
	if ( sprite == SPRITE_ARROWL  )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWL");
	if ( sprite == SPRITE_ARROWR  )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWR");
	if ( sprite == SPRITE_ARROWUL )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWUL");
	if ( sprite == SPRITE_ARROWUR )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWUR");
	if ( sprite == SPRITE_ARROWDL )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWDL");
	if ( sprite == SPRITE_ARROWDR )  hCursor = LoadCursor(g_hInstance, "IDC_ARROWDR");
	if ( sprite == SPRITE_WAIT    )  hCursor = LoadCursor(g_hInstance, "IDC_WAIT");
	if ( sprite == SPRITE_EMPTY   )  hCursor = LoadCursor(g_hInstance, "IDC_EMPTY");
	if ( sprite == SPRITE_FILL    )  hCursor = LoadCursor(g_hInstance, "IDC_FILL");
	SetCursor(hCursor);

	g_lastSprite = sprite;
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

void GetCurrentDir(char *pName, int lg)
{
	int		i;

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
// sur le CD-Rom.

void AddCDPath(char *pFilename)
{
	char	temp[MAX_PATH];
	int		lg;
	BOOL	bDaniel = FALSE;

	if ( g_CDPath[0] == 0 )  return;

	lg = strlen(g_CDPath);
	if ( lg > 14 && strstr(g_CDPath+lg-14, "\\daniel\\blupi\\") )
	{
		bDaniel = TRUE;
	}

#if _DEMO
	strcpy(temp, g_CDPath);
	strcat(temp, pFilename);
#else
	if ( !bDaniel &&
		 (strstr(pFilename, "image\\") == pFilename ||
		  strstr(pFilename, "movie\\") == pFilename) )
	{
		strcpy(temp, g_CDPath);
		strcat(temp, "..\\");
		strcat(temp, pFilename);
	}
	else
	{
		strcpy(temp, g_CDPath);
		strcat(temp, pFilename);
	}
#endif

	strcpy(pFilename, temp);
}

// Ajoute le chemin permettant de lire un fichier
// utilisateur.

void AddUserPath(char *pFilename)
{
	char					temp[MAX_PATH];
	char*					pText;
	int						pos;
	char					last;
	SECURITY_ATTRIBUTES		att;

	if ( g_CDPath[0] != 0 )  return;

#if _EGAMES
	strcpy(temp, "c:\\Planet Blupi Full\\");
#if _DEMO
	strcpy(temp, "c:\\Planet Blupi Demo\\");
#endif
#if _SE
	strcpy(temp, "c:\\Planet Blupi SE\\");
#endif
#else
	strcpy(temp, "c:\\Planète Blupi\\");
#endif

	att.nLength = sizeof(SECURITY_ATTRIBUTES);
	att.lpSecurityDescriptor = NULL;
	att.bInheritHandle = FALSE;
	CreateDirectory(temp, &att);

	pText = strstr(pFilename, "\\");
	if ( pText != NULL )
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



//----------------------------------------------------------------------
// 
// Function     : TraceErrorDD()
//
// Purpose      : Traces an error (DirectDraw)
//
//----------------------------------------------------------------------

void TraceErrorDD(HRESULT hErr, char *sFile, int nLine)
{       
    char dderr[256];
    char err[1024];

    switch (hErr)
    {
        case DD_OK : sprintf(dderr, "DD_OK"); break;
        case DDERR_ALREADYINITIALIZED : sprintf(dderr, "DDERR_ALREADYINITIALIZED"); break;
        case DDERR_CANNOTATTACHSURFACE : sprintf(dderr, "DDERR_CANNOTATTACHSURFACE"); break;
        case DDERR_CANNOTDETACHSURFACE : sprintf(dderr, "DDERR_CANNOTDETACHSURFACE"); break;
        case DDERR_CURRENTLYNOTAVAIL : sprintf(dderr, "DDERR_CURRENTLYNOTAVAIL"); break;
        case DDERR_EXCEPTION : sprintf(dderr, "DDERR_EXCEPTION"); break;
        case DDERR_GENERIC : sprintf(dderr, "DDERR_GENERIC"); break;
        case DDERR_HEIGHTALIGN : sprintf(dderr, "DDERR_HEIGHTALIGN"); break;
        case DDERR_INCOMPATIBLEPRIMARY : sprintf(dderr, "DDERR_INCOMPATIBLEPRIMARY"); break;
        case DDERR_INVALIDCAPS : sprintf(dderr, "DDERR_INVALIDCAPS"); break;
        case DDERR_INVALIDCLIPLIST : sprintf(dderr, "DDERR_INVALIDCLIPLIST"); break;
        case DDERR_INVALIDMODE : sprintf(dderr, "DDERR_INVALIDMODE"); break;
        case DDERR_INVALIDOBJECT : sprintf(dderr, "DDERR_INVALIDOBJECT"); break;
        case DDERR_INVALIDPARAMS : sprintf(dderr, "DDERR_INVALIDPARAMS"); break;
        case DDERR_INVALIDPIXELFORMAT : sprintf(dderr, "DDERR_INVALIDPIXELFORMAT"); break;
        case DDERR_INVALIDRECT : sprintf(dderr, "DDERR_INVALIDRECT"); break;
        case DDERR_LOCKEDSURFACES : sprintf(dderr, "DDERR_LOCKEDSURFACES"); break;
        case DDERR_NO3D : sprintf(dderr, "DDERR_NO3D"); break;
        case DDERR_NOALPHAHW : sprintf(dderr, "DDERR_NOALPHAHW"); break;
        case DDERR_NOCLIPLIST : sprintf(dderr, "DDERR_NOCLIPLIST"); break;
        case DDERR_NOCOLORCONVHW : sprintf(dderr, "DDERR_NOCOLORCONVHW"); break;
        case DDERR_NOCOOPERATIVELEVELSET : sprintf(dderr, "DDERR_NOCOOPERATIVELEVELSET"); break;
        case DDERR_NOCOLORKEY : sprintf(dderr, "DDERR_NOCOLORKEY"); break;
        case DDERR_NOCOLORKEYHW : sprintf(dderr, "DDERR_NOCOLORKEYHW"); break;
        case DDERR_NODIRECTDRAWSUPPORT : sprintf(dderr, "DDERR_NODIRECTDRAWSUPPORT"); break;
        case DDERR_NOEXCLUSIVEMODE : sprintf(dderr, "DDERR_NOEXCLUSIVEMODE"); break;
        case DDERR_NOFLIPHW : sprintf(dderr, "DDERR_NOFLIPHW"); break;
        case DDERR_NOGDI : sprintf(dderr, "DDERR_NOGDI"); break;
        case DDERR_NOMIRRORHW : sprintf(dderr, "DDERR_NOMIRRORHW"); break;
        case DDERR_NOTFOUND : sprintf(dderr, "DDERR_NOTFOUND"); break;
        case DDERR_NOOVERLAYHW : sprintf(dderr, "DDERR_NOOVERLAYHW"); break;
        case DDERR_NORASTEROPHW : sprintf(dderr, "DDERR_NORASTEROPHW"); break;
        case DDERR_NOROTATIONHW : sprintf(dderr, "DDERR_NOROTATIONHW"); break;
        case DDERR_NOSTRETCHHW : sprintf(dderr, "DDERR_NOSTRETCHHW"); break;
        case DDERR_NOT4BITCOLOR : sprintf(dderr, "DDERR_NOT4BITCOLOR"); break;
        case DDERR_NOT4BITCOLORINDEX : sprintf(dderr, "DDERR_NOT4BITCOLORINDEX"); break;
        case DDERR_NOT8BITCOLOR : sprintf(dderr, "DDERR_NOT8BITCOLOR"); break;
        case DDERR_NOTEXTUREHW : sprintf(dderr, "DDERR_NOTEXTUREHW"); break;
        case DDERR_NOVSYNCHW : sprintf(dderr, "DDERR_NOVSYNCHW"); break;
        case DDERR_NOZBUFFERHW : sprintf(dderr, "DDERR_NOZBUFFERHW"); break;
        case DDERR_NOZOVERLAYHW : sprintf(dderr, "DDERR_NOZOVERLAYHW"); break;
        case DDERR_OUTOFCAPS : sprintf(dderr, "DDERR_OUTOFCAPS"); break;
        case DDERR_OUTOFMEMORY : sprintf(dderr, "DDERR_OUTOFMEMORY"); break;
        case DDERR_OUTOFVIDEOMEMORY : sprintf(dderr, "DDERR_OUTOFVIDEOMEMORY"); break;
        case DDERR_OVERLAYCANTCLIP : sprintf(dderr, "DDERR_OVERLAYCANTCLIP"); break;
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE : sprintf(dderr, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"); break;
        case DDERR_PALETTEBUSY : sprintf(dderr, "DDERR_PALETTEBUSY"); break;
        case DDERR_COLORKEYNOTSET : sprintf(dderr, "DDERR_COLORKEYNOTSET"); break;
        case DDERR_SURFACEALREADYATTACHED : sprintf(dderr, "DDERR_SURFACEALREADYATTACHED"); break;
        case DDERR_SURFACEALREADYDEPENDENT : sprintf(dderr, "DDERR_SURFACEALREADYDEPENDENT"); break;
        case DDERR_SURFACEBUSY : sprintf(dderr, "DDERR_SURFACEBUSY"); break;
        case DDERR_CANTLOCKSURFACE : sprintf(dderr, "DDERR_CANTLOCKSURFACE"); break;
        case DDERR_SURFACEISOBSCURED : sprintf(dderr, "DDERR_SURFACEISOBSCURED"); break;
        case DDERR_SURFACELOST : sprintf(dderr, "DDERR_SURFACELOST"); break;
        case DDERR_SURFACENOTATTACHED : sprintf(dderr, "DDERR_SURFACENOTATTACHED"); break;
        case DDERR_TOOBIGHEIGHT : sprintf(dderr, "DDERR_TOOBIGHEIGHT"); break;
        case DDERR_TOOBIGSIZE : sprintf(dderr, "DDERR_TOOBIGSIZE"); break;
        case DDERR_TOOBIGWIDTH : sprintf(dderr, "DDERR_TOOBIGWIDTH"); break;
        case DDERR_UNSUPPORTED : sprintf(dderr, "DDERR_UNSUPPORTED"); break;
        case DDERR_UNSUPPORTEDFORMAT : sprintf(dderr, "DDERR_UNSUPPORTEDFORMAT"); break;
        case DDERR_UNSUPPORTEDMASK : sprintf(dderr, "DDERR_UNSUPPORTEDMASK"); break;
        case DDERR_VERTICALBLANKINPROGRESS : sprintf(dderr, "DDERR_VERTICALBLANKINPROGRESS"); break;
        case DDERR_WASSTILLDRAWING : sprintf(dderr, "DDERR_WASSTILLDRAWING"); break;
        case DDERR_XALIGN : sprintf(dderr, "DDERR_XALIGN"); break;
        case DDERR_INVALIDDIRECTDRAWGUID : sprintf(dderr, "DDERR_INVALIDDIRECTDRAWGUID"); break;
        case DDERR_DIRECTDRAWALREADYCREATED : sprintf(dderr, "DDERR_DIRECTDRAWALREADYCREATED"); break;
        case DDERR_NODIRECTDRAWHW : sprintf(dderr, "DDERR_NODIRECTDRAWHW"); break;
        case DDERR_PRIMARYSURFACEALREADYEXISTS : sprintf(dderr, "DDERR_PRIMARYSURFACEALREADYEXISTS"); break;
        case DDERR_NOEMULATION : sprintf(dderr, "DDERR_NOEMULATION"); break;
        case DDERR_REGIONTOOSMALL : sprintf(dderr, "DDERR_REGIONTOOSMALL"); break;
        case DDERR_CLIPPERISUSINGHWND : sprintf(dderr, "DDERR_CLIPPERISUSINGHWND"); break;
        case DDERR_NOCLIPPERATTACHED : sprintf(dderr, "DDERR_NOCLIPPERATTACHED"); break;
        case DDERR_NOHWND : sprintf(dderr, "DDERR_NOHWND"); break;
        case DDERR_HWNDSUBCLASSED : sprintf(dderr, "DDERR_HWNDSUBCLASSED"); break;
        case DDERR_HWNDALREADYSET : sprintf(dderr, "DDERR_HWNDALREADYSET"); break;
        case DDERR_NOPALETTEATTACHED : sprintf(dderr, "DDERR_NOPALETTEATTACHED"); break;
        case DDERR_NOPALETTEHW : sprintf(dderr, "DDERR_NOPALETTEHW"); break;
        case DDERR_BLTFASTCANTCLIP : sprintf(dderr, "DDERR_BLTFASTCANTCLIP"); break;
        case DDERR_NOBLTHW : sprintf(dderr, "DDERR_NOBLTHW"); break;
        case DDERR_NODDROPSHW : sprintf(dderr, "DDERR_NODDROPSHW"); break;
        case DDERR_OVERLAYNOTVISIBLE : sprintf(dderr, "DDERR_OVERLAYNOTVISIBLE"); break;
        case DDERR_NOOVERLAYDEST : sprintf(dderr, "DDERR_NOOVERLAYDEST"); break;
        case DDERR_INVALIDPOSITION : sprintf(dderr, "DDERR_INVALIDPOSITION"); break;
        case DDERR_NOTAOVERLAYSURFACE : sprintf(dderr, "DDERR_NOTAOVERLAYSURFACE"); break;
        case DDERR_EXCLUSIVEMODEALREADYSET : sprintf(dderr, "DDERR_EXCLUSIVEMODEALREADYSET"); break;
        case DDERR_NOTFLIPPABLE : sprintf(dderr, "DDERR_NOTFLIPPABLE"); break;
        case DDERR_CANTDUPLICATE : sprintf(dderr, "DDERR_CANTDUPLICATE"); break;
        case DDERR_NOTLOCKED : sprintf(dderr, "DDERR_NOTLOCKED"); break;
        case DDERR_CANTCREATEDC : sprintf(dderr, "DDERR_CANTCREATEDC"); break;
        case DDERR_NODC : sprintf(dderr, "DDERR_NODC"); break;
        case DDERR_WRONGMODE : sprintf(dderr, "DDERR_WRONGMODE"); break;
        case DDERR_IMPLICITLYCREATED : sprintf(dderr, "DDERR_IMPLICITLYCREATED"); break;
        case DDERR_NOTPALETTIZED : sprintf(dderr, "DDERR_NOTPALETTIZED"); break;
        case DDERR_UNSUPPORTEDMODE : sprintf(dderr, "DDERR_UNSUPPORTEDMODE"); break;
        case DDERR_NOMIPMAPHW : sprintf(dderr, "DDERR_NOMIPMAPHW"); break;
        case DDERR_INVALIDSURFACETYPE : sprintf(dderr, "DDERR_INVALIDSURFACETYPE"); break;
        case DDERR_DCALREADYCREATED : sprintf(dderr, "DDERR_DCALREADYCREATED"); break;
        case DDERR_CANTPAGELOCK : sprintf(dderr, "DDERR_CANTPAGELOCK"); break;
        case DDERR_CANTPAGEUNLOCK : sprintf(dderr, "DDERR_CANTPAGEUNLOCK"); break;
        case DDERR_NOTPAGELOCKED : sprintf(dderr, "DDERR_NOTPAGELOCKED"); break;
        case DDERR_NOTINITIALIZED : sprintf(dderr, "DDERR_NOTINITIALIZED"); break;

        default : sprintf(dderr, "Unknown Error"); break;
    }
    sprintf(err, "DirectDraw Error %s in file %s at line %d\n", dderr, sFile, nLine);
    OutputDebug(err);
}

//----------------------------------------------------------------------
// 
// Function     : TraceErrorDS()
//
// Purpose      : Traces an error (DirectSound)
//
//----------------------------------------------------------------------

void TraceErrorDS(HRESULT hErr, char *sFile, int nLine)
{       
    char dserr[256];
    char err[1024];

    switch (hErr)
    {
        case DS_OK : sprintf(dserr, "DD_OK"); break;
        case DSERR_ALLOCATED : sprintf(dserr, "DSERR_ALLOCATED"); break;
        case DSERR_CONTROLUNAVAIL : sprintf(dserr, "DSERR_CONTROLUNAVAIL"); break;
        case DSERR_INVALIDPARAM : sprintf(dserr, "DSERR_INVALIDPARAM"); break;
        case DSERR_INVALIDCALL : sprintf(dserr, "DSERR_INVALIDCALL"); break;
        case DSERR_GENERIC : sprintf(dserr, "DSERR_GENERIC"); break;
        case DSERR_PRIOLEVELNEEDED : sprintf(dserr, "DSERR_PRIOLEVELNEEDED"); break;
        case DSERR_OUTOFMEMORY : sprintf(dserr, "DSERR_OUTOFMEMORY"); break;
        case DSERR_BADFORMAT : sprintf(dserr, "DSERR_BADFORMAT"); break;
        case DSERR_UNSUPPORTED : sprintf(dserr, "DSERR_UNSUPPORTED"); break;
        case DSERR_NODRIVER : sprintf(dserr, "DSERR_NODRIVER"); break;
        case DSERR_ALREADYINITIALIZED : sprintf(dserr, "DSERR_ALREADYINITIALIZED"); break;
        case DSERR_NOAGGREGATION : sprintf(dserr, "DSERR_NOAGGREGATION"); break;
        case DSERR_BUFFERLOST : sprintf(dserr, "DSERR_BUFFERLOST"); break;
        case DSERR_OTHERAPPHASPRIO : sprintf(dserr, "DSERR_OTHERAPPHASPRIO"); break;
        case DSERR_UNINITIALIZED : sprintf(dserr, "DSERR_UNINITIALIZED"); break;

        default : sprintf(dserr, "Unknown Error"); break;
    }
    sprintf(err, "DirectSound Error %s in file %s at line %d\n", dserr, sFile, nLine);
    OutputDebug(err);
}

