// movie.h
//

class CMovie
{
public:
	CMovie();
	~CMovie();

	BOOL	Create();
	BOOL	GetEnable();
	BOOL	IsExist(char *pFilename);
	BOOL	Play(HWND hWnd, RECT rect, char *pFilename);
	void	Stop(HWND hWnd);

protected:
	void	playMovie(HWND hWnd, int nDirection);
	BOOL	fileOpenMovie(HWND hWnd, RECT rect, char *pFilename);
	void	fileCloseMovie(HWND hWnd);
	void	positionMovie(HWND hWnd, RECT rect);
	void	termAVI();
	BOOL	initAVI();

protected:
	BOOL		m_bEnable;
	MCIDEVICEID	m_wMCIDeviceID;	// MCI Device ID for the AVI file
	HWND		m_hwndMovie;	// window handle of the movie
	BOOL		m_fPlaying;		// Play flag: TRUE == playing, FALSE == paused
	BOOL		m_fMovieOpen;	// Open flag: TRUE == movie open, FALSE = none
};

