// blupi.cpp
//

#include "blupi.h"

#define WIN32_LEAN_AND_MEAN

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <mmsystem.h>        // link WINMM.LIB as well
#include "def.h"
#include "resource.h"
#include "ddutil.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "movie.h"
#include "button.h"
#include "menu.h"
#include "jauge.h"
#include "event.h"
#include "misc.h"


// Définitions globales

#define NAME			"Blupi"
#define TITLE			"Blupi"


// Variables globales

HWND		g_hWnd;					// handle à la fenêtre
SDL_Window *g_window;
SDL_Renderer *g_renderer;
CEvent*		g_pEvent  = NULL;
CPixmap*	g_pPixmap = NULL;		// pixmap principal
CSound*		g_pSound  = NULL;		// sound principal
CMovie*		g_pMovie  = NULL;		// movie principal
CDecor*		g_pDecor  = NULL;
char		g_CDPath[MAX_PATH];		// chemin d'accès au CD-Rom
bool		g_bFullScreen = false;	// false si mode de test
int			g_speedRate = 1;
int			g_timerInterval = 50;	// inverval = 50ms
int			g_mouseType = MOUSETYPEGRA;
MMRESULT    g_updateTimer;			// timer général
bool		g_bActive = true;		// is application active ?
bool		g_bTermInit = false;	// initialisation en cours

UINT		g_lastPhase = 999;




// Lit un numéro décimal.

int GetNum(char *p)
{
	int		n = 0;

	while ( *p >= '0' && *p <= '9' )
	{
		n *= 10;
		n += (*p++)-'0';
	}

	return n;
}

// Lit le fichier de configuration.

bool ReadConfig(LPSTR lpCmdLine)
{
	FILE*		file    = NULL;
	char		buffer[200];
	char*		pText;
	size_t		nb;

	file = fopen("data\\config.def", "rb");
	if ( file == NULL )  return false;
	nb = fread(buffer, sizeof(char), 200-1, file);
	buffer[nb] = 0;
	fclose(file);

#if 0
	pText = strstr(buffer, "CD-Rom=");
	if ( pText == NULL )
	{
#if _DEMO
		GetCurrentDirectory(MAX_PATH, g_CDPath);
		i = strlen(g_CDPath);
		if ( i > 0 && g_CDPath[i-1] != '\\' )
		{
			g_CDPath[i++] = '\\';
			g_CDPath[i] = 0;  // met le terminateur
		}
#else
		return false;
#endif
	}
	else
	{
		pText += 7;
		i = 0;
		while ( pText[i] != 0 && pText[i] != '\n' && pText[i] != '\r' )
		{
			g_CDPath[i] = pText[i];
			i ++;
		}
		if ( i > 0 && g_CDPath[i-1] != '\\' )
		{
			g_CDPath[i++] = '\\';
		}
		g_CDPath[i] = 0;  // met le terminateur
	}

#if !_DEMO & !_EGAMES
	if ( strstr(lpCmdLine, "-nocd") == NULL )
	{
		char		drive[10];

		drive[0] = g_CDPath[0];
		drive[1] = ':';
		drive[2] = '\\';
		drive[3] = 0;
		nb = GetDriveType(drive);
		if ( nb != DRIVE_CDROM )  return false;
	}
#endif
#endif

	pText = strstr(buffer, "SpeedRate=");
	if ( pText != NULL )
	{
		g_speedRate = GetNum(pText+10);
		if ( g_speedRate < 1 )  g_speedRate = 1;
		if ( g_speedRate > 2 )  g_speedRate = 2;
	}

	pText = strstr(buffer, "Timer=");
	if ( pText != NULL )
	{
		g_timerInterval = GetNum(pText+6);
		if ( g_timerInterval <   10 )  g_timerInterval =   10;
		if ( g_timerInterval > 1000 )  g_timerInterval = 1000;
	}

	pText = strstr(buffer, "FullScreen=");
	if ( pText != NULL )
	{
		g_bFullScreen = !!GetNum(pText+11);
		if ( g_bFullScreen != 0 )  g_bFullScreen = 1;
	}

	pText = strstr(buffer, "MouseType=");
	if ( pText != NULL )
	{
		g_mouseType = GetNum(pText+10);
		if ( g_mouseType < 1 )  g_mouseType = 1;
		if ( g_mouseType > 9 )  g_mouseType = 9;
	}

	return true;
}


// Mise à jour principale.

void UpdateFrame(void)
{
    RECT            clip, rcRect;
	UINT			phase;
	POINT			posMouse;
	int				i, term, speed;

	posMouse = g_pEvent->GetLastMousePos();

	phase = g_pEvent->GetPhase();

	if ( phase == g_lastPhase &&
		 phase == WM_PHASE_PLAY )
	{
//?		rcRect.left   = POSDRAWX;
//?		rcRect.top    = POSDRAWY;
//?		rcRect.right  = POSDRAWX+DIMDRAWX;
//?		rcRect.bottom = POSDRAWY+DIMDRAWY;
//?		g_pPixmap->DrawImage(-1, CHBACK, rcRect, 1);  // dessine le fond
	}
	else
	{
		rcRect.left   = 0;
		rcRect.top    = 0;
		rcRect.right  = LXIMAGE;
		rcRect.bottom = LYIMAGE;
		g_pPixmap->DrawImage(-1, CHBACK, rcRect, 1);  // dessine le fond
	}

	if ( phase == WM_PHASE_INTRO1 ||
		 phase == WM_PHASE_INTRO2 )
	{
		g_pEvent->IntroStep();
	}

	if ( phase == WM_PHASE_PLAY )
	{
		clip.left   = POSDRAWX;
		clip.top    = POSDRAWY+g_pDecor->GetInfoHeight();
		clip.right  = POSDRAWX+DIMDRAWX;
		clip.bottom = POSDRAWY+DIMDRAWY;

		if ( g_pEvent->IsShift() )  // shift en cours ?
		{
			g_pEvent->DecorAutoShift(posMouse);
			g_pDecor->Build(clip, posMouse);  // construit juste le décor
		}
		else
		{
			if ( !g_pEvent->GetPause() )
			{
				speed = g_pEvent->GetSpeed() * g_speedRate;
				for ( i=0 ; i<speed ; i++ )
				{
					g_pDecor->BlupiStep(i==0);  // avance tous les blupi
					g_pDecor->MoveStep(i==0);   // avance tous les décors
					g_pEvent->DemoStep();       // avance enregistrement/reproduction
				}
			}

			g_pEvent->DecorAutoShift(posMouse);
			g_pDecor->Build(clip, posMouse);  // construit le décor
			g_pDecor->NextPhase(1);  // refait la carte de temps en temps
		}
	}

	if ( phase == WM_PHASE_BUILD )
	{
		clip.left   = POSDRAWX;
		clip.top    = POSDRAWY;
		clip.right  = POSDRAWX+DIMDRAWX;
		clip.bottom = POSDRAWY+DIMDRAWY;
		g_pEvent->DecorAutoShift(posMouse);
		g_pDecor->Build(clip, posMouse);  // construit le décor
		g_pDecor->NextPhase(-1);  // refait la carte chaque fois
	}

	if ( phase == WM_PHASE_INIT )
	{
		g_pEvent->DemoStep();  // démarre év. démo automatique
	}

	g_pEvent->DrawButtons();

	g_lastPhase = phase;

	if ( phase == WM_PHASE_H0MOVIE   ||
		 phase == WM_PHASE_H1MOVIE   ||
		 phase == WM_PHASE_H2MOVIE   ||
		 phase == WM_PHASE_PLAYMOVIE ||
		 phase == WM_PHASE_WINMOVIE  )
	{
		g_pEvent->MovieToStart();  // fait démarrer un film si nécessaire
	}

	if ( phase == WM_PHASE_INSERT )
	{
		g_pEvent->TryInsert();
	}

	if ( phase == WM_PHASE_PLAY )
	{
		term = g_pDecor->IsTerminated();
		if ( term == 1 )  g_pEvent->ChangePhase(WM_PHASE_LOST);  // perdu
		if ( term == 2 )  g_pEvent->ChangePhase(WM_PHASE_WINMOVIE);   // gagné
	}
}


void Benchmark()
{
	int		i;
	POINT	pos = { 0, 0 };

	g_pPixmap->DrawIcon(-1, 2, 10, pos, 0);

	pos.x = 300;
	pos.y = 350;
	for ( i=0 ; i<10000 ; i++ )
	{
		g_pPixmap->DrawIcon(-1, 2, i%4, pos, 0);
	}

	g_pPixmap->DrawIcon(-1, 2, 10, pos, 0);
	g_pSound->Play(0);
}


// Restitue le jeu après une activation en mode fullScreen.

bool RestoreGame()
{
	if ( g_pPixmap == NULL )  return false;

	g_pEvent->RestoreGame();
	return g_pPixmap->Restore();
}

// Libère le jeu avant une désactivation en mode fullScreen.

bool FlushGame()
{
	if ( g_pPixmap == NULL )  return false;

	return g_pPixmap->Flush();
}


// Finished with all objects we use; release them.

static void FinishObjects(void)
{
	if ( g_pMovie != NULL )
	{
		g_pEvent->StopMovie();

		delete g_pMovie;
		g_pMovie = NULL;
	}

	if ( g_pEvent != NULL )
	{
		delete g_pEvent;
		g_pEvent = NULL;
	}

	if ( g_pDecor != NULL )
	{
		delete g_pDecor;
		g_pDecor = NULL;
	}

	if ( g_pSound != NULL )
	{
		g_pSound->StopMusic();  // stoppe la musique Midi

		delete g_pSound;
		g_pSound = NULL;
	}

	if ( g_pPixmap != NULL )
	{
		delete g_pPixmap;
		g_pPixmap = NULL;
	}
}

LRESULT CALLBACK WindowProc2 (HWND hWnd, UINT message,
							 WPARAM wParam, LPARAM lParam,
							 const SDL_Event *event)
{
	static HINSTANCE	hInstance;
	POINT				mousePos, totalDim, iconDim;

	// La touche F10 envoie un autre message pour activer
	// le menu dans les applications Windows standard !
	if ( message == WM_SYSKEYDOWN && wParam == VK_F10 )
	{
		message = WM_KEYDOWN;
	}
	if ( message == WM_SYSKEYUP && wParam == VK_F10 )
	{
		message = WM_KEYUP;
	}

	if ( g_pEvent != NULL &&
		 g_pEvent->TreatEvent(event) )  return 0;

    switch( message )
    {
		case WM_TIMER:
		case WM_UPDATE:
			if ( !g_pEvent->IsMovie() )  // pas de film en cours ?
			{
				if ( g_bActive )
				{
					UpdateFrame();
				}
				g_pPixmap->Display();
			}
			break;

		case WM_CREATE:
			hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
			return 0;

		case WM_ACTIVATEAPP:
			g_bActive = (wParam != 0);
			if ( g_bActive )  // active ?
			{
				if ( g_bFullScreen )
				{
					RestoreGame();
					g_lastPhase = 999;
				}
				if ( !g_bFullScreen && g_bTermInit )
				{
					totalDim.x = 64;
					totalDim.y = 66;
					iconDim.x = 64;
					iconDim.y = 66/2;
					g_pPixmap->Cache(CHHILI, "image\\hili.blp", totalDim, iconDim, true);
					g_pPixmap->SetTransparent(CHHILI, RGB(0,0,255));  // bleu

					g_pPixmap->SavePalette();
					g_pPixmap->InitSysPalette();
				}
				SetWindowText(hWnd, "Blupi");
				if ( g_pSound != NULL )  g_pSound->RestartMusic();
			}
			else		// désactive ?
			{
				if ( g_bFullScreen )
				{
					FlushGame();
				}
				SetWindowText(hWnd, "Blupi -- stop");
				if ( g_pSound != NULL )  g_pSound->SuspendMusic();
			}
			return 0;

		case WM_SYSCOLORCHANGE:
			OutputDebug("Event WM_SYSCOLORCHANGE\n");
			break;

	    case WM_QUERYNEWPALETTE:
			OutputDebug("Event WM_QUERYNEWPALETTE\n");
			break;

		case WM_PALETTECHANGED:
			OutputDebug("Event WM_PALETTECHANGED\n");
			break;

		case WM_DISPLAYCHANGE:
			OutputDebug("Event WM_DISPLAYCHANGE\n");
			break;

		case MM_MCINOTIFY:
			OutputDebug("Event MM_MCINOTIFY\n");
			if ( g_pEvent->IsMovie() )  // film en cours ?
			{
				if ( wParam == MCI_NOTIFY_SUCCESSFUL )
				{
					g_pEvent->StopMovie();
				}
			}
			else
			{
				// music over, play it again
				g_pSound->SuspendMusic();
				// if music finished, play it again. Otherwise assume that
				// it was aborted by the user or otherwise
				if ( wParam == MCI_NOTIFY_SUCCESSFUL )
				{
					OutputDebug("Event MCI_NOTIFY_SUCCESSFUL\n");
					g_pSound->RestartMusic();
				}
				else
				{
					char s[50];
					sprintf(s, "wParam=%d\n", static_cast<int> (wParam));
					OutputDebug(s);
				}
			}
			break;

		case WM_SETCURSOR:
//			ChangeSprite();
//			SetCursor(NULL);  // pas de souris visible !
			return true;

		case WM_LBUTTONDOWN:
//?			Benchmark();
			GetCursorPos(&mousePos);
			ScreenToClient(hWnd, &mousePos);
			break;

		case WM_RBUTTONDOWN:
			break;

		case WM_MOUSEMOVE:
			break;

		case WM_KEYDOWN:
			switch( wParam )
			{
				case VK_F5:
					g_pEvent->SetSpeed(1);
					break;
				case VK_F6:
					g_pEvent->SetSpeed(2);
					break;
				case VK_F7:
					g_pEvent->SetSpeed(4);
					break;
				case VK_F8:
					g_pEvent->SetSpeed(8);
					break;
#if 0
				case VK_F2:
					KillTimer(g_hWnd, 1);
					SetTimer(g_hWnd, 1, g_timerInterval/2, NULL);
					break;
				case VK_F3:
					KillTimer(g_hWnd, 1);
					SetTimer(g_hWnd, 1, g_timerInterval, NULL);
					break;
				case VK_F4:
					KillTimer(g_hWnd, 1);
					SetTimer(g_hWnd, 1, g_timerInterval*2, NULL);
					break;
#endif
			}
			break;

		case WM_DESTROY:
			KillTimer(g_hWnd, 1);
			FinishObjects();
			PostQuitMessage(0);
			break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WindowProc (HWND hWnd, UINT message,
					WPARAM wParam, LPARAM lParam)
{
	return WindowProc2 (hWnd, message, wParam, lParam, nullptr);
}

// Erreur dans DoInit.

bool InitFail(char *msg, bool bDirectX)
{
	char	buffer[100];

	if ( bDirectX )  strcpy(buffer, "DirectX Init FAILED\n(while ");
	else             strcpy(buffer, "Error (");
	strcat(buffer, msg);
	strcat(buffer, ")");
    MessageBox(g_hWnd, buffer, TITLE, MB_OK);

    FinishObjects();
    DestroyWindow(g_hWnd);
    return false;
}

// Initialisation de l'application.

static bool DoInit(HINSTANCE hInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS		wc;
	POINT			totalDim, iconDim;
	RECT			rcRect;
	bool			bOK;

	bOK = ReadConfig(lpCmdLine);  // lit le fichier config.def

	InitHInstance(hInstance);

	// Set up and register window class.
	wc.style         = CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc   = WindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
//?	wc.hIcon         = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hIcon         = LoadIcon(hInstance, "IDR_MAINFRAME");
	wc.hCursor       = LoadCursor(hInstance, "IDC_POINTER");
	wc.hbrBackground = GetStockBrush(BLACK_BRUSH);
	wc.lpszMenuName  = NAME;
	wc.lpszClassName = NAME;
	RegisterClass(&wc);

	SDL_SetMainReady ();
	auto res = SDL_Init (SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (res < 0)
		return false;


	// Create a window.
	if ( g_bFullScreen )
	{
		g_hWnd = CreateWindowEx
					(
						WS_EX_TOPMOST,
						NAME,
						TITLE,
						WS_POPUP,
						0, 0,
						GetSystemMetrics(SM_CXSCREEN),
						GetSystemMetrics(SM_CYSCREEN),
						NULL,
						NULL,
						hInstance,
						NULL
					);

		g_window = SDL_CreateWindow (
			NAME,
			0, 0,
			GetSystemMetrics (SM_CXSCREEN),
			GetSystemMetrics (SM_CYSCREEN),
			SDL_WINDOW_FULLSCREEN
		);
	}
	else
	{
		int			sx, sy;
		RECT		WindowRect;

		sx = GetSystemMetrics(SM_CXSCREEN);
		sy = GetSystemMetrics(SM_CYSCREEN);

		SetRect(&WindowRect, (sx-LXIMAGE)/2, (sy-LYIMAGE)/2,
							 (sx+LXIMAGE)/2, (sy+LYIMAGE)/2);
		AdjustWindowRect(&WindowRect,  WS_POPUPWINDOW|WS_CAPTION, true);
		WindowRect.top += GetSystemMetrics(SM_CYCAPTION);

		g_hWnd = CreateWindow
					(
						NAME,
						TITLE,
						WS_POPUPWINDOW|WS_CAPTION|WS_VISIBLE,
						(sx-LXIMAGE)/2, (sy-LYIMAGE)/2,
						WindowRect.right - WindowRect.left,
						WindowRect.bottom - WindowRect.top,
						HWND_DESKTOP,
						NULL,
						hInstance,
						NULL
					);

		g_window = SDL_CreateWindow (NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LXIMAGE, LYIMAGE, 0);
	}

	g_renderer = SDL_CreateRenderer (g_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if ( !g_hWnd )  return false;

	if (!g_window)
	{
		printf ("Could not create window: %s\n", SDL_GetError ());
		return false;
	}

	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	SetFocus(g_hWnd);

	if ( !bOK )  // config.def pas correct ?
	{
		return InitFail("Game not correctly installed", false);
	}

	// Crée le pixmap principal.
	g_pPixmap = new CPixmap;
	if ( g_pPixmap == NULL )  return InitFail("New pixmap", true);

	totalDim.x = LXIMAGE;
	totalDim.y = LYIMAGE;
	if ( !g_pPixmap->Create(g_hWnd, totalDim, g_bFullScreen, g_mouseType) )
		return InitFail("Create pixmap", true);

	OutputDebug("Image: init\n");
	totalDim.x = LXIMAGE;
	totalDim.y = LYIMAGE;
	iconDim.x  = 0;
	iconDim.y  = 0;
#if _INTRO
	if ( !g_pPixmap->Cache(CHBACK, "image\\intro1.blp", totalDim, iconDim, true) )
#else
	if ( !g_pPixmap->Cache(CHBACK, "image\\init.blp", totalDim, iconDim, true) )
#endif
		return false;

	OutputDebug("SavePalette\n");
	g_pPixmap->SavePalette();
	OutputDebug("InitSysPalette\n");
	g_pPixmap->InitSysPalette();

	OutputDebug("Image: init\n");
	totalDim.x = LXIMAGE;
	totalDim.y = LYIMAGE;
	iconDim.x  = 0;
	iconDim.y  = 0;
	if ( !g_pPixmap->Cache(CHGROUND, "image\\init.blp", totalDim, iconDim, true) )
		return false;

	g_pPixmap->SetDebug(false);

	rcRect.left   = 0;
	rcRect.top    = 0;
	rcRect.right  = LXIMAGE;
	rcRect.bottom = LYIMAGE;
	g_pPixmap->DrawImage(-1, CHBACK, rcRect, 1);  // dessine le fond
	g_pPixmap->Display();

	totalDim.x = DIMCELX*2*16;
	totalDim.y = DIMCELY*2*6;
	iconDim.x = DIMCELX*2;
	iconDim.y = DIMCELY*2;
	if ( !g_pPixmap->Cache(CHFLOOR, "image\\floor000.blp", totalDim, iconDim, false) )
		return InitFail("Cache floor000.blp", true);
	g_pPixmap->SetTransparent(CHFLOOR, RGB(0,0,255));  // bleu

	totalDim.x = DIMOBJX*16;
	totalDim.y = DIMOBJY*8;
	iconDim.x = DIMOBJX;
	iconDim.y = DIMOBJY;
	if ( !g_pPixmap->Cache(CHOBJECT, "image\\obj000.blp", totalDim, iconDim, false) )
		return InitFail("Cache obj000.blp", true);
	g_pPixmap->SetTransparent(CHOBJECT, RGB(0,0,255));  // bleu

	if ( !g_pPixmap->Cache(CHOBJECTo, "image\\obj-o000.blp", totalDim, iconDim, false) )
		return InitFail("Cache obj-o000.blp", true);
	g_pPixmap->SetTransparent(CHOBJECTo, RGB(255,255,255));  // blanc

	totalDim.x = DIMBLUPIX*16;
	totalDim.y = DIMBLUPIY*23;
	iconDim.x = DIMBLUPIX;
	iconDim.y = DIMBLUPIY;
	if ( !g_pPixmap->Cache(CHBLUPI, "image\\blupi.blp", totalDim, iconDim, false) )
		return InitFail("Cache blupi.blp", true);
	g_pPixmap->SetTransparent(CHBLUPI, RGB(0,0,255));  // bleu

	totalDim.x = 64;
	totalDim.y = 66;
	iconDim.x = 64;
	iconDim.y = 66/2;
	if ( !g_pPixmap->Cache(CHHILI, "image\\hili.blp", totalDim, iconDim, false) )
		return InitFail("Cache hili.blp", true);
	g_pPixmap->SetTransparent(CHHILI, RGB(0,0,255));  // bleu

	totalDim.x = DIMCELX*2*3;
	totalDim.y = DIMCELY*2*5;
	iconDim.x = DIMCELX*2;
	iconDim.y = DIMCELY*2;
	if ( !g_pPixmap->Cache(CHFOG, "image\\fog.blp", totalDim, iconDim, false) )
		return InitFail("Cache fog.blp", true);
	g_pPixmap->SetTransparent(CHFOG, RGB(255,255,255));  // blanc

	totalDim.x = DIMCELX*2*16;
	totalDim.y = DIMCELY*2*1;
	iconDim.x = DIMCELX*2;
	iconDim.y = DIMCELY*2;
	if ( !g_pPixmap->Cache(CHMASK1, "image\\mask1.blp", totalDim, iconDim, false) )
		return InitFail("Cache mask1.blp", true);
	g_pPixmap->SetTransparent(CHMASK1, RGB(0,0,0));  // noir

	totalDim.x = DIMBUTTONX*6;
	totalDim.y = DIMBUTTONY*21;
	iconDim.x = DIMBUTTONX;
	iconDim.y = DIMBUTTONY;
	if ( !g_pPixmap->Cache(CHBUTTON, "image\\button00.blp", totalDim, iconDim, false) )
		return InitFail("Cache button00.blp", true);
	g_pPixmap->SetTransparent(CHBUTTON, RGB(0,0,255));  // bleu

	totalDim.x = DIMJAUGEX*1;
	totalDim.y = DIMJAUGEY*4;
	iconDim.x = DIMJAUGEX;
	iconDim.y = DIMJAUGEY;
	if ( !g_pPixmap->Cache(CHJAUGE, "image\\jauge.blp", totalDim, iconDim, false) )
		return InitFail("Cache jauge.blp", true);
	g_pPixmap->SetTransparent(CHJAUGE, RGB(0,0,255));  // bleu

	totalDim.x = DIMTEXTX*16;
	totalDim.y = DIMTEXTY*8*3;
	iconDim.x = DIMTEXTX;
	iconDim.y = DIMTEXTY;
	if ( !g_pPixmap->Cache(CHTEXT, "image\\text.blp", totalDim, iconDim, false) )
		return InitFail("Cache text.blp", true);
	g_pPixmap->SetTransparent(CHTEXT, RGB(0,0,255));  // bleu

	totalDim.x = DIMLITTLEX*16;
	totalDim.y = DIMLITTLEY*8;
	iconDim.x = DIMLITTLEX;
	iconDim.y = DIMLITTLEY;
	if ( !g_pPixmap->Cache(CHLITTLE, "image\\little.blp", totalDim, iconDim, false) )
		return InitFail("Cache little.blp", true);
	g_pPixmap->SetTransparent(CHLITTLE, RGB(0,0,255));  // bleu

	totalDim.x = 426;
	totalDim.y = 52;
	iconDim.x = 426;
	iconDim.y = 52;
	if ( !g_pPixmap->Cache(CHBIGNUM, "image\\bignum.blp", totalDim, iconDim, false) )
		return InitFail("Cache bignum.blp", true);
	g_pPixmap->SetTransparent(CHBIGNUM, RGB(0,0,255));  // bleu

	// Load all cursors
	g_pPixmap->LoadCursors ();
	g_pPixmap->ChangeSprite (SPRITE_WAIT); // met le sablier maison

	// Crée le gestionnaire de son.
	g_pSound = new CSound;
	if ( g_pSound == NULL )  return InitFail("New sound", true);

	g_pSound->Create(g_hWnd);
	g_pSound->CacheAll();
	g_pSound->SetState(true);

	// Crée le gestionnaire de films.
	g_pMovie = new CMovie;
	if ( g_pMovie == NULL )  return InitFail("New movie", false);

	g_pMovie->Create();

	// Crée le gestionnaire de décors.
	g_pDecor = new CDecor;
	if ( g_pDecor == NULL )  return InitFail("New decor", false);

	g_pDecor->Create(g_hWnd, g_pSound, g_pPixmap);
	g_pDecor->MapInitColors();

	// Crée le gestionnaire d'événements.
	g_pEvent = new CEvent;
	if ( g_pEvent == NULL )  return InitFail("New event", false);

	g_pEvent->Create(g_hWnd, g_pPixmap, g_pDecor, g_pSound, g_pMovie);
	g_pEvent->SetFullScreen(g_bFullScreen);
	g_pEvent->SetMouseType(g_mouseType);
#if _INTRO
	g_pEvent->ChangePhase(WM_PHASE_INTRO1);
#else
	g_pEvent->ChangePhase(WM_PHASE_TESTCD);
#endif

	g_bTermInit = true;  // initialisation terminée
	return true;
}


// Programme principal.

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow)
{
	MSG		msg;

	if ( !DoInit(hInstance, lpCmdLine, nCmdShow) )
	{
		return false;
	}

	SetTimer(g_hWnd, 1, g_timerInterval, NULL);

	while (SDL_TRUE)
	{
		if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if ( !GetMessage(&msg, NULL, 0, 0) )
			{
				goto out;
			}
			TranslateMessage(&msg); 
			DispatchMessage(&msg);
		}
		else
		{
			// make sure we go to sleep if we have nothing else to do
			if (!g_bActive)
				WaitMessage ();
			else
				Sleep (1);
		}

		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			WindowProc2 (nullptr, 0, 0, 0, &event);
		}

	}

out:
	if (g_window)
		SDL_DestroyWindow (g_window);

	SDL_Quit ();
	return static_cast<int> (msg.wParam);
}

