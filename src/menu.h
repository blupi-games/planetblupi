
#pragma once

#include <unordered_map>
#include "def.h"

/////////////////////////////////////////////////////////////////////////////

class CMenu
{
public:
    CMenu();
    ~CMenu();

    bool    Create (CPixmap *pPixmap, CSound *pSound,
                    POINT pos, Sint32 nb, Sint32 *pButtons, Sint32 *pErrors,
                    std::unordered_map<Sint32, const char *> &texts,
                    Sint32 perso);
    void    Update (Sint32 nb, Sint32 *pButtons, Sint32 *pErrors,
                    std::unordered_map<Sint32, const char *> &texts);
    void    Delete();
    void    Draw();
    Sint32      GetSel();
    Sint32      GetRank();
    bool    IsError();
    bool    IsExist();
    void    Message();

    bool    TreatEvent (const SDL_Event &event);

protected:
    Sint32      Detect (POINT pos);
    bool    MouseDown (POINT pos);
    bool    MouseMove (POINT pos);
    bool    MouseUp (POINT pos);

protected:
    CPixmap    *m_pPixmap;
    CDecor     *m_pDecor;
    CSound     *m_pSound;
    POINT       m_pos;          // coin sup/gauche
    POINT       m_dim;          // dimensions
    Sint32          m_nbButtons;
    POINT       m_nbCel;
    Sint32          m_perso;
    Sint32          m_buttons[MAXBUTTON];
    Sint32          m_errors[MAXBUTTON];
    std::unordered_map<Sint32, const char *> m_texts;
    Uint32      m_messages[MAXBUTTON];
    Sint32          m_selRank;
};

/////////////////////////////////////////////////////////////////////////////
