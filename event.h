// Event.h

#pragma once

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
	UINT	message;
	int		type;
	int		iconMenu[20];
	int		x, y;
	int		toolTips[20];
}
Button;

typedef struct
{
	UINT	phase;
	char	backName[20];
	BOOL	bCDrom;
	Button	buttons[MAXBUTTON];
}
Phase;


typedef struct
{
	short	majRev;
	short	minRev;
	short	bSchool;
	short	bPrivate;
	short	world;
	short	skill;
	short	reserve1[99];
}
DemoHeader;

typedef struct
{
	int		time;
	UINT	message;
	WPARAM	wParam;
	LPARAM	lParam;
}
DemoEvent;


class CEvent
{
public:
	CEvent();
	~CEvent();

	POINT	GetMousePos();
	void	Create(HWND hWnd, CPixmap *pPixmap, CDecor *pDecor, CSound *pSound, CMovie *pMovie);
	void	SetFullScreen(BOOL bFullScreen);
	void	SetMouseType(int mouseType);
	int		GetWorld();
	int		GetPhysicalWorld();
	int		GetImageWorld();
	BOOL	IsHelpHide();
	BOOL	ChangePhase(UINT phase);
	void	MovieToStart();
	UINT	GetPhase();
	void	TryInsert();
	void	RestoreGame();

	int		GetButtonIndex(int button);
	int		GetState(int button);
	void	SetState(int button, int state);
	BOOL	GetEnable(int button);
	void	SetEnable(int button, BOOL bEnable);
	BOOL	GetHide(int button);
	void	SetHide(int button, BOOL bHide);
	int		GetMenu(int button);
	void	SetMenu(int button, int menu);

	BOOL	DrawButtons();
	int		MousePosToSprite(POINT pos);
	void	MouseSprite(POINT pos);
	void	WaitMouse(BOOL bWait);
	void	HideMouse(BOOL bHide);
	POINT	GetLastMousePos();
	BOOL	TreatEvent(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL	TreatEventBase(UINT message, WPARAM wParam, LPARAM lParam);

	void	DecorAutoShift(POINT pos);
	
	BOOL	StartMovie(char *pFilename);
	void	StopMovie();
	BOOL	IsMovie();

	BOOL	FlipObject();

	void	Read(int message);
	void	Write(int message);

	void	SetSpeed(int speed);
	int		GetSpeed();
	BOOL	GetPause();
	BOOL	IsShift();

	void	DemoStep();
	void	DebugDisplay(char m);

	void	IntroStep();

protected:
	void	DrawTextCenter(int res, int x, int y, int font=0);
	BOOL	CreateButtons();
	BOOL	EventButtons(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL	MouseOnButton(POINT pos);
	int		SearchPhase(UINT phase);
	void	DecorShift(int dx, int dy);

	BOOL	PlayDown(POINT pos, int fwKeys);
	BOOL	PlayMove(POINT pos, int fwKeys);
	BOOL	PlayUp(POINT pos, int fwKeys);
	void	ChangeButtons(int message);

	void	BuildFloor(POINT cel, int insIcon);
	void	BuildWater(POINT cel, int insIcon);
	BOOL	BuildDown(POINT pos, int fwKeys, BOOL bMix=TRUE);
	BOOL	BuildMove(POINT pos, int fwKeys);
	BOOL	BuildUp(POINT pos, int fwKeys);

	void	PrivateLibelle();
	BOOL	ReadLibelle(int world, BOOL bSchool, BOOL bHelp);
	BOOL	WriteInfo();
	BOOL	ReadInfo();

	void	DemoRecStart();
	void	DemoRecStop();
	BOOL	DemoPlayStart();
	void	DemoPlayStop();
	void	DemoRecEvent(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	int			m_speed;
	int			m_exercice;
	int			m_mission;
	int			m_private;
	int			m_maxMission;
	int			m_phase;
	int			m_index;
	BOOL		m_bSchool;
	BOOL		m_bPrivate;
	BOOL		m_bAccessBuild;
	BOOL		m_bFullScreen;
	int			m_mouseType;
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	CMovie*		m_pMovie;
	char		m_movieToStart[MAX_PATH];
	int			m_phaseAfterMovie;
	CButton		m_buttons[MAXBUTTON];
	int			m_lastFloor[MAXBUTTON];
	int			m_lastObject[MAXBUTTON];
	int			m_lastHome[MAXBUTTON];
	BOOL		m_bRunMovie;
	BOOL		m_bBuildModify;
	CJauge		m_jauges[2];
	CMenu		m_menu;
	BOOL		m_bMenu;
	POINT		m_menuPos;
	int			m_menuNb;
	int			m_menuButtons[MAXBUTTON];
	int			m_menuErrors[MAXBUTTON];
	int			m_menuPerso;
	POINT		m_menuCel;
	POINT		m_oldMousePos;
	BOOL		m_bMousePress;
	BOOL		m_bMouseDown;
	BOOL		m_bHili;
	int			m_fileWorld[10];
	int			m_fileTime[10];
	POINT		m_posToolTips;
	char		m_textToolTips[50];
	int			m_mouseSprite;
	BOOL		m_bFillMouse;
	BOOL		m_bWaitMouse;
	BOOL		m_bHideMouse;
	BOOL		m_bShowMouse;
	int			m_rankCheat;
	int			m_posCheat;
	BOOL		m_bMovie;
	BOOL		m_bSpeed;
	BOOL		m_bHelp;
	BOOL		m_bAllMissions;
	BOOL		m_bChangeCheat;
	int			m_scrollSpeed;
	BOOL		m_bPause;
	BOOL		m_bShift;
	int			m_shiftPhase;
	POINT		m_shiftVector;
	POINT		m_shiftOffset;
	char		m_libelle[1000];
	int			m_tryPhase;
	int			m_tryInsertCount;
	POINT		m_posInfoButton;
	POINT		m_posHelpButton;
	BOOL		m_bHiliInfoButton;
	BOOL		m_bHiliHelpButton;
	BOOL		m_bInfoHelp;
	BOOL		m_bDemoRec;
	BOOL		m_bDemoPlay;
	DemoEvent*	m_pDemoBuffer;
	int			m_demoTime;
	int			m_demoIndex;
	int			m_demoEnd;
	int			m_demoNumber;
	BOOL		m_bCtrlDown;
	POINT		m_debugPos;
	int			m_introTime;
};

/////////////////////////////////////////////////////////////////////////////
