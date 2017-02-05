// movie.h
//

#pragma once

class CMovie
{
public:
	CMovie();
	~CMovie();

	bool	Create();
	bool	GetEnable();
	bool	IsExist(char *pFilename);
	bool	Play(RECT rect, char *pFilename);
	void	Stop();

protected:
	void	playMovie(int nDirection);
	bool	fileOpenMovie(RECT rect, char *pFilename);
	void	fileCloseMovie();
	void	positionMovie(RECT rect);
	void	termAVI();
	bool	initAVI();

protected:
	bool		m_bEnable;
	MCIDEVICEID	m_wMCIDeviceID;	// MCI Device ID for the AVI file
	HWND		m_hwndMovie;	// window handle of the movie
	bool		m_fPlaying;		// Play flag: true == playing, false == paused
	bool		m_fMovieOpen;	// Open flag: true == movie open, false = none
};

