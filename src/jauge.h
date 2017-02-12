// Jauge.h

#pragma once

class CPixmap;
class CDecor;
class CSound;

/////////////////////////////////////////////////////////////////////////////

class CJauge
{
public:
    CJauge();
    ~CJauge();

    bool    Create (CPixmap *pPixmap, CSound *pSound,
                    POINT pos, Sint32 type, bool bMinimizeRedraw);
    void    Draw();
    void    Redraw();

    void    SetLevel (Sint32 level);
    void    SetType (Sint32 type);

    bool    GetHide();
    void    SetHide (bool bHide);

    POINT   GetPos();
    void    SetRedraw();

protected:
    CPixmap    *m_pPixmap;
    CDecor     *m_pDecor;
    CSound     *m_pSound;
    bool        m_bHide;        // true si bouton caché
    POINT       m_pos;          // coin sup/gauche
    POINT       m_dim;          // dimensions
    Sint32          m_type;
    Sint32          m_level;
    bool        m_bMinimizeRedraw;
    bool        m_bRedraw;      // true -> doit être redessiné
};

/////////////////////////////////////////////////////////////////////////////
