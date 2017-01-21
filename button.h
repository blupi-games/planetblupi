// Button.h



/////////////////////////////////////////////////////////////////////////////

class CButton
{
public:
	CButton();
	~CButton();

	BOOL	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, BOOL bMinimizeRedraw,
				   int *pMenu, int nbMenu,
				   int *pTooltips, int nbToolTips,
				   int region, UINT message);
	void	Draw();
	void	Redraw();

	int		GetState();
	void	SetState(int state);

	int		GetMenu();
	void	SetMenu(int menu);

	BOOL	GetEnable();
	void	SetEnable(BOOL bEnable);

	BOOL	GetHide();
	void	SetHide(BOOL bHide);

	BOOL	TreatEvent(UINT message, WPARAM wParam, LPARAM lParam);
	BOOL	MouseOnButton(POINT pos);
	int		GetToolTips(POINT pos);

	
protected:
	BOOL	Detect(POINT pos);
	BOOL	MouseDown(POINT pos);
	BOOL	MouseMove(POINT pos);
	BOOL	MouseUp(POINT pos);

protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	int			m_type;			// type de bouton
	BOOL		m_bEnable;		// TRUE si bouton actif
	BOOL		m_bHide;		// TRUE si bouton caché
	UINT		m_message;		// message envoyé si bouton actionné
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_state;		// 0=relâché, 1=pressé, +2=survollé
	int			m_mouseState;	// 0=relâché, 1=pressé, +2=survollé
	int			m_iconMenu[20];	// icônes du sous-menu
	int			m_toolTips[20];	// info-bulles
	int			m_nbMenu;		// nb de case du sous-menu
	int			m_nbToolTips;	// nb d'info-bulles
	int			m_selMenu;		// sous-menu sélectionné
	BOOL		m_bMouseDown;	// TRUE -> bouton souris pressé
	BOOL		m_bMinimizeRedraw;
	BOOL		m_bRedraw;		// TRUE -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
