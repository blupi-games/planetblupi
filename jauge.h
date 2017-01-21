// Jauge.h



/////////////////////////////////////////////////////////////////////////////

class CJauge
{
public:
	CJauge();
	~CJauge();

	BOOL	Create(HWND hWnd, CPixmap *pPixmap, CSound *pSound,
				   POINT pos, int type, BOOL bMinimizeRedraw);
	void	Draw();
	void	Redraw();

	void	SetLevel(int level);
	void	SetType(int type);

	BOOL	GetHide();
	void	SetHide(BOOL bHide);

	POINT	GetPos();
	void	SetRedraw();
	
protected:
	HWND		m_hWnd;
	CPixmap*	m_pPixmap;
	CDecor*		m_pDecor;
	CSound*		m_pSound;
	BOOL		m_bHide;		// TRUE si bouton caché
	POINT		m_pos;			// coin sup/gauche
	POINT		m_dim;			// dimensions
	int			m_type;
	int			m_level;
	BOOL		m_bMinimizeRedraw;
	BOOL		m_bRedraw;		// TRUE -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
