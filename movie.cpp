// movie.cpp
//

#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <direct.h>
#include <mmsystem.h>		
#include <digitalv.h>		
#include "def.h"
#include "movie.h"
#include "misc.h"


//----------------------------------------------------------------------------

#define AVI_VIDEO "avivideo"

#define IDM_PLAY   10
#define IDM_RPLAY  11

//----------------------------------------------------------------------------

// Initialize avi libraries.

BOOL CMovie::initAVI()
{
	MCI_DGV_OPEN_PARMS	mciOpen;
		
	// set up the open parameters
	mciOpen.dwCallback 		 = 0L;
	mciOpen.wDeviceID 		 = 0;
	mciOpen.lpstrDeviceType  = AVI_VIDEO;
	mciOpen.lpstrElementName = NULL;
	mciOpen.lpstrAlias 		 = NULL;
	mciOpen.dwStyle 		 = 0;
	mciOpen.hWndParent 		 = NULL;
		
	// try to open the driver
	return (mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE),
                           (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0);
}

// Closes the opened AVI file and the opened device type.                                               |

void CMovie::termAVI()
{
	MCIDEVICEID        mciID;
	MCI_GENERIC_PARMS  mciClose;
	
	// Get the device ID for the opened device type and then close
	// the device type.
	mciID = mciGetDeviceID(AVI_VIDEO);
	mciSendCommand(mciID, MCI_CLOSE, 0L,
                   (DWORD)(LPMCI_GENERIC_PARMS)&mciClose);
}


// Sets the movie rectange <rcMovie> to be
// centered within the app's window.

void CMovie::positionMovie(HWND hWnd, RECT rect)
{
	// reposition the playback (child) window
	MoveWindow(m_hwndMovie,
			   rect.left, rect.top,
			   rect.right, rect.bottom, TRUE);
}

// Close the movie and anything associated with it.					|
// This function clears the <m_fPlaying> and <m_fMovieOpen> flags	|

void CMovie::fileCloseMovie(HWND hWnd)
{
	MCI_GENERIC_PARMS  mciGeneric;

	mciSendCommand(m_wMCIDeviceID, MCI_CLOSE, 0L,
				   (DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);

	m_fPlaying   = FALSE;	// can't be playing any longer
	m_fMovieOpen = FALSE;	// no more movies open
	
	// cause a total repaint to occur
	InvalidateRect(hWnd, NULL, TRUE);
	UpdateWindow(hWnd);
}


// Open an AVI movie. Use CommDlg open box to
// open and then handle the initialization to
// show the movie and position it properly.  Keep
// the movie paused when opened.
// Sets <m_fMovieOpen> on success.

BOOL CMovie::fileOpenMovie(HWND hWnd, RECT rect, char *pFilename)
{
	MCI_DGV_OPEN_PARMS		mciOpen;
	MCI_DGV_WINDOW_PARMS	mciWindow;
	MCI_DGV_STATUS_PARMS	mciStatus;
	char					string[MAX_PATH];

	if ( pFilename[1] == ':' )  // nom complet "D:\REP..." ?
	{
		strcpy(string, pFilename);
	}
	else
	{
		GetCurrentDir(string, MAX_PATH-30);
		strcat(string, pFilename);
	}

	// we got a filename, now close any old movie and open the new one.					*/
	if ( m_fMovieOpen )  fileCloseMovie(hWnd);	

	// we have a .AVI movie to open, use MCI
	// set up the open parameters
	mciOpen.dwCallback       = 0L;
	mciOpen.wDeviceID        = 0;
	mciOpen.lpstrDeviceType  = NULL;
	mciOpen.lpstrElementName = string;
	mciOpen.lpstrAlias       = NULL;
	mciOpen.dwStyle          = WS_CHILD;
	mciOpen.hWndParent       = hWnd;

	// try to open the file
	if ( mciSendCommand(0, MCI_OPEN,
				(DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS),
				(DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0 )
	{
		// we opened the file o.k., now set up to play it.
		m_wMCIDeviceID = mciOpen.wDeviceID;	// save ID
		m_fMovieOpen = TRUE;	 // a movie was opened

		// show the playback window
		mciWindow.dwCallback = 0L;
		mciWindow.hWnd       = NULL;
		mciWindow.nCmdShow   = SW_SHOW;
		mciWindow.lpstrText  = (LPSTR)NULL;
//		mciSendCommand(m_wMCIDeviceID, MCI_WINDOW,
//					   MCI_DGV_WINDOW_STATE,
//					   (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

		// get the window handle
		mciStatus.dwItem = MCI_DGV_STATUS_HWND;
		mciSendCommand(m_wMCIDeviceID,
					   MCI_STATUS, MCI_STATUS_ITEM,
					   (DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
		m_hwndMovie = (HWND)mciStatus.dwReturn;

		// now get the movie centered
		positionMovie(hWnd, rect);

		// cause an update to occur
		InvalidateRect(hWnd, NULL, FALSE);
		UpdateWindow(hWnd);

		return TRUE;
	}
	else
	{
		// generic error for open
		m_fMovieOpen = FALSE;

		return FALSE;
	}
}

// Play/pause the movie depending on the state
// of the <m_fPlaying> flag.			|
// This function sets the <m_fPlaying> flag appropriately when done|

void CMovie::playMovie(HWND hWnd, int nDirection)
{
	m_fPlaying = !m_fPlaying;	// swap the play flag

	if( !nDirection )
		m_fPlaying = FALSE;	// wDirection == NULL means PAUSE

	// play/pause the AVI movie
	if ( m_fPlaying )
	{
		DWORD			dwFlags;
		MCI_DGV_PLAY_PARMS	mciPlay;
		
		// init to play all
		mciPlay.dwCallback = MAKELONG(hWnd,0);
		mciPlay.dwFrom = mciPlay.dwTo = 0;
		dwFlags = MCI_NOTIFY;
		if ( nDirection == IDM_RPLAY )
			dwFlags |= MCI_DGV_PLAY_REVERSE;
		
		mciSendCommand(m_wMCIDeviceID, MCI_PLAY, dwFlags,
		               (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay);
	}
	else
	{
		MCI_DGV_PAUSE_PARMS	mciPause;
	
		// tell it to pause
		mciSendCommand(m_wMCIDeviceID,MCI_PAUSE,0L,
                      (DWORD)(LPMCI_DGV_PAUSE_PARMS)&mciPause);
	}
}


//----------------------------------------------------------------------------


// Constructeur.

CMovie::CMovie()
{
	m_bEnable      = FALSE;
	m_wMCIDeviceID = 0;
	m_fPlaying     = FALSE;
	m_fMovieOpen   = FALSE;
}

// Destructeur.

CMovie::~CMovie()
{
	termAVI();
}

// Ouvre la librairie avi.

BOOL CMovie::Create()
{
#if _EGAMES
	m_bEnable = FALSE;
	return FALSE;
#else
	if ( initAVI() )
	{
		m_bEnable = TRUE;
		return TRUE;
	}
	else
	{
		m_bEnable = FALSE;
		return FALSE;
	}
#endif
}

// Retourne l'état de DirectMovie.

BOOL CMovie::GetEnable()
{
	return m_bEnable;
}

// Indique si un film existe.

BOOL CMovie::IsExist(char *pFilename)
{
	char	string[MAX_PATH];
	FILE*	file;

	if ( pFilename[1] == ':' )  // nom complet "D:\REP..." ?
	{
		strcpy(string, pFilename);
	}
	else
	{
		GetCurrentDir(string, MAX_PATH-30);
		strcat(string, pFilename);
	}

	file = fopen(string, "rb");
	if ( file == NULL )  return FALSE;

	fclose(file);
	return TRUE;
}

// Montre un film avi.

BOOL CMovie::Play(HWND hWnd, RECT rect, char *pFilename)
{
	if ( !m_bEnable )  return FALSE;
	if ( !fileOpenMovie(hWnd, rect, pFilename) )  return FALSE;
	playMovie(hWnd, IDM_PLAY);

	return TRUE;
}

// Stoppe le film avi.

void CMovie::Stop(HWND hWnd)
{
	if ( !m_bEnable )  return;
	fileCloseMovie(hWnd);
}
