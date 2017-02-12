// Button.h

#pragma once

#include <vector>

/////////////////////////////////////////////////////////////////////////////

class CButton
{
public:
    CButton();
    ~CButton();

    bool    Create (CPixmap *pPixmap, CSound *pSound,
                    POINT pos, Sint32 type, bool bMinimizeRedraw,
                    Sint32 *pMenu, Sint32 nbMenu,
                    const char **pToolTips,
                    Sint32 region, Uint32 message);
    void    Draw();
    void    Redraw();

    Sint32      GetState();
    void    SetState (Sint32 state);

    Sint32      GetMenu();
    void    SetMenu (Sint32 menu);

    bool    GetEnable();
    void    SetEnable (bool bEnable);

    bool    GetHide();
    void    SetHide (bool bHide);

    bool    TreatEvent (const SDL_Event &event);
    bool    MouseOnButton (POINT pos);
    const char *GetToolTips (POINT pos);


protected:
    bool    Detect (POINT pos);
    bool    MouseDown (POINT pos);
    bool    MouseMove (POINT pos);
    bool    MouseUp (POINT pos);

protected:
    CPixmap    *m_pPixmap;
    CDecor     *m_pDecor;
    CSound     *m_pSound;
    Sint32          m_type;         // type de bouton
    bool        m_bEnable;      // true si bouton actif
    bool        m_bHide;        // true si bouton caché
    Uint32      m_message;      // message envoyé si bouton actionné
    POINT       m_pos;          // coin sup/gauche
    POINT       m_dim;          // dimensions
    Sint32          m_state;        // 0=relâché, 1=pressé, +2=survollé
    Sint32          m_mouseState;   // 0=relâché, 1=pressé, +2=survollé
    Sint32          m_iconMenu[20]; // icônes du sous-menu
    const char **m_toolTips;    // info-bulles
    Sint32          m_nbMenu;       // nb de case du sous-menu
    Sint32          m_nbToolTips;   // nb d'info-bulles
    Sint32          m_selMenu;      // sous-menu sélectionné
    bool        m_bMouseDown;   // true -> bouton souris pressé
    bool        m_bMinimizeRedraw;
    bool        m_bRedraw;      // true -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
