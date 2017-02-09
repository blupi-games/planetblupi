// Event.h

#pragma once

#include <vector>
#include <unordered_map>
#include "blupi.h"
#include "jauge.h"
#include "menu.h"
#include "button.h"

class CMovie;

/////////////////////////////////////////////////////////////////////////////

typedef struct
{
	unsigned int message;
	int		type;
	int		iconMenu[20];
	int		x, y;
	const char *toolTips[16];
}
Button;

typedef struct
{
	unsigned int phase;
	char	backName[20];
	int		bCDrom;
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
	unsigned int message;
	unsigned int wParam; // WPARAM
	unsigned int lParam; // LPARAM
}
DemoEvent;


class CEvent
{
public:
	CEvent();
	~CEvent();

	POINT	GetMousePos();
	void	Create(CPixmap *pPixmap, CDecor *pDecor, CSound *pSound, CMovie *pMovie);
	void	SetFullScreen(bool bFullScreen);
	void	SetMouseType(int mouseType);
	int		GetWorld();
	int		GetPhysicalWorld();
	int		GetImageWorld();
	bool	IsHelpHide();
	bool	ChangePhase(unsigned int phase);
	void	MovieToStart();
	unsigned int GetPhase();
	void	TryInsert();
	void	RestoreGame();

	int		GetButtonIndex(int button);
	int		GetState(int button);
	void	SetState(int button, int state);
	bool	GetEnable(int button);
	void	SetEnable(int button, bool bEnable);
	bool	GetHide(int button);
	void	SetHide(int button, bool bHide);
	int		GetMenu(int button);
	void	SetMenu(int button, int menu);

	bool	DrawButtons();
	MouseSprites MousePosToSprite(POINT pos);
	void	MouseSprite(POINT pos);
	void	WaitMouse(bool bWait);
	void	HideMouse(bool bHide);
	POINT	GetLastMousePos();
	bool	TreatEvent(const SDL_Event &event);
	bool	TreatEventBase(const SDL_Event &event);

	void	DecorAutoShift(POINT pos);
	
	bool	StartMovie(char *pFilename);
	void	StopMovie();
	bool	IsMovie();

	void	Read(int message);
	void	Write(int message);

	void	SetSpeed(int speed);
	int		GetSpeed();
	bool	GetPause();
	bool	IsShift();

	void	DemoStep();

	void	IntroStep();

public:
	static void PushUserEvent (int code);

protected:
	void	DrawTextCenter(const char *text, int x, int y, int font=0);
	bool	CreateButtons();
	bool	EventButtons(const SDL_Event &event, POINT pos);
	bool	MouseOnButton(POINT pos);
	int		SearchPhase(unsigned int phase);
	void	DecorShift(int dx, int dy);

	bool	PlayDown(POINT pos, const SDL_Event &event);
	bool	PlayMove(POINT pos, Uint16 mod);
	bool	PlayUp(POINT pos, Uint16 mod);
	void	ChangeButtons(int message);

	void	BuildFloor(POINT cel, int insIcon);
	void	BuildWater(POINT cel, int insIcon);
	bool	BuildDown(POINT pos, Uint16 mod, bool bMix=true);
	bool	BuildMove(POINT pos, Uint16 mod, const SDL_Event &event);
	bool	BuildUp(POINT pos);

	void	PrivateLibelle();
	bool	ReadLibelle(int world, bool bSchool, bool bHelp);
	bool	WriteInfo();
	bool	ReadInfo();

	void	DemoRecStart();
	void	DemoRecStop();
	bool	DemoPlayStart();
	void	DemoPlayStop();
	static void WinToSDLEvent (unsigned int msg, WPARAM wParam, LPARAM lParam, SDL_Event &event);
	void	DemoRecEvent(unsigned int message, WPARAM wParam, LPARAM lParam);

protected:
	int			m_speed;
	int			m_exercice;
	int			m_mission;
	int			m_private;
	int			m_maxMission;
	int			m_phase;
	int			m_index;
	bool		m_bSchool;
	bool		m_bPrivate;
	bool		m_bAccessBuild;
	bool		m_bFullScreen;
	int			m_mouseType;
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
	bool		m_bRunMovie;
	bool		m_bBuildModify;
	CJauge		m_jauges[2];
	CMenu		m_menu;
	bool		m_bMenu;
	POINT		m_menuPos;
	int			m_menuNb;
	int			m_menuButtons[MAXBUTTON];
	int			m_menuErrors[MAXBUTTON];
	std::unordered_map<int, const char *> m_menuTexts;
	int			m_menuPerso;
	POINT		m_menuCel;
	POINT		m_oldMousePos;
	bool		m_bMouseDown;
	bool		m_bHili;
	int			m_fileWorld[10];
	int			m_fileTime[10];
	POINT		m_posToolTips;
	char		m_textToolTips[50];
	MouseSprites m_mouseSprite;
	bool		m_bFillMouse;
	bool		m_bWaitMouse;
	bool		m_bHideMouse;
	int			m_rankCheat;
	int			m_posCheat;
	bool		m_bMovie;
	bool		m_bSpeed;
	bool		m_bHelp;
	bool		m_bAllMissions;
	bool		m_bChangeCheat;
	int			m_scrollSpeed;
	bool		m_bPause;
	bool		m_bShift;
	int			m_shiftPhase;
	POINT		m_shiftVector;
	POINT		m_shiftOffset;
	char		m_libelle[1000];
	int			m_tryPhase;
	int			m_tryInsertCount;
	POINT		m_posInfoButton;
	POINT		m_posHelpButton;
	bool		m_bHiliInfoButton;
	bool		m_bHiliHelpButton;
	bool		m_bInfoHelp;
	bool		m_bDemoRec;
	bool		m_bDemoPlay;
	DemoEvent*	m_pDemoBuffer;
	int			m_demoTime;
	size_t		m_demoIndex;
	size_t		m_demoEnd;
	int			m_demoNumber;
	Uint16		m_keymod;
	POINT		m_debugPos;
	int			m_introTime;
};

/////////////////////////////////////////////////////////////////////////////
