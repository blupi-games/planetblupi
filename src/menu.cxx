/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017, Mathieu Schroeter
 * http://epsitec.ch; http://www.blupi.org; http://github.com/blupi-games
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://gnu.org/licenses
 */

#include <stdio.h>
#include <stdlib.h>

#include "blupi.h"
#include "button.h"
#include "decor.h"
#include "def.h"
#include "event.h"
#include "gettext.h"
#include "menu.h"
#include "misc.h"
#include "pixmap.h"
#include "sound.h"
#include "text.h"

/////////////////////////////////////////////////////////////////////////////

#define MARGMENU 0

static const Sint16 table_button_icon[] = {
  24,  // go
  40,  // stop
  32,  // mange
  30,  // carry
  31,  // depose
  22,  // abat
  27,  // roc
  28,  // cultive
  19,  // build1 (cabane)
  25,  // build2 (couveuse)
  35,  // build3 (laboratoire)
  61,  // build4 (mine)
  59,  // build5 (usine)
  101, // build6 (téléporteur)
  20,  // mur
  26,  // palis
  42,  // abat n
  43,  // roc n
  23,  // pont
  33,  // tour
  34,  // boit
  39,  // labo
  54,  // fleur
  55,  // fleur n
  41,  // dynamite
  58,  // bateau
  60,  // djeep
  64,  // drapeau
  62,  // extrait du fer
  65,  // fabrique jeep
  63,  // fabrique mine
  83,  // fabrique disciple
  100, // repeat
  107, // qarmure
  106, // fabarmure
};

static const char * GetText (Sint32 rank)
{
  static const char * list[] = {
    translate ("Go"),
    translate ("Stop"),
    translate ("Eat"),
    translate ("Take"),
    translate ("Drop"),
    translate ("Cut down a tree"),
    translate ("Carve a rock"),
    translate ("Grow tomatoes"),
    translate ("Garden shed"),
    translate ("Incubator"),
    translate ("Laboratory"),
    translate ("Mine"),
    translate ("Workshop"),
    translate ("Teleporter"),
    translate ("Wall"),
    translate ("Palisade"),
    translate ("Cut down trees"),
    translate ("Carve rocks"),
    translate ("Bridge"),
    translate ("Protection tower"),
    translate ("Drink"),
    translate ("Transform"),
    translate ("Make bunch of flowers"),
    translate ("Make bunches of flowers"),
    translate ("Blow up"),
    translate ("Boat"),
    translate ("Leave Jeep"),
    translate ("Prospect for iron"),
    translate ("Extract iron"),
    translate ("Make a Jeep"),
    translate ("Make a time bomb"),
    translate ("Make a helper robot"),
    translate ("Repeat"),
    translate ("Quit"),
    translate ("Make armour"),
  };

  return gettext (list[rank]);
}

static const char * GetErr (Sint32 rank)
{
  static const char * list[] = {
    translate ("Impossible"),         translate ("Inadequate ground"),
    translate ("Occupied ground"),    translate ("Opposite bank no good"),
    translate ("Bridge finished"),    translate ("(isolated tower)"),
    translate ("Too close to water"), translate ("Already two teleporters"),
  };

  return gettext (list[rank]);
}

/////////////////////////////////////////////////////////////////////////////

// Constructeur.

CMenu::CMenu ()
{
  m_nbButtons = 0;
  m_selRank   = -1;
  m_pPixmap   = nullptr;
  m_pDecor    = nullptr;
  m_pSound    = nullptr;
  m_pEvent    = nullptr;
}

// Destructeur.

CMenu::~CMenu ()
{
}

// Crée un nouveau bouton.

bool CMenu::Create (
  CPixmap * pPixmap, CSound * pSound, CEvent * pEvent, POINT pos, Sint32 nb,
  Sint32 * pButtons, Errors * pErrors,
  std::unordered_map<Sint32, const char *> & texts, Sint32 perso)
{
  pos.x -= DIMBUTTONX / 2;
  pos.y -= DIMBUTTONY / 2;

  m_pPixmap   = pPixmap;
  m_pSound    = pSound;
  m_pEvent    = pEvent;
  m_nbButtons = nb;
  m_pos       = pos;
  m_perso     = perso;

  Update (nb, pButtons, pErrors, texts);

  if (m_pos.x < POSDRAWX)
    m_pos.x = POSDRAWX;
  if (m_pos.y < POSDRAWY)
    m_pos.y = POSDRAWY;

  if (m_pos.x > POSDRAWX + DIMDRAWX - 2 - m_dim.x)
    m_pos.x = POSDRAWX + DIMDRAWX - 2 - m_dim.x;
  if (m_pos.y > POSDRAWY + DIMDRAWY - 2 - m_dim.y)
    m_pos.y = POSDRAWY + DIMDRAWY - 2 - m_dim.y;

  if (m_pos.x != pos.x || m_pos.y != pos.y)
  {
    pos = m_pos;
    pos.x += DIMBUTTONX / 2;
    pos.y += DIMBUTTONY / 2;
    SDL_WarpMouseInWindow (
      g_window, pos.x * m_pEvent->GetWindowScale (),
      pos.y * m_pEvent->GetWindowScale ());
  }

  m_selRank = Detect (pos);

  return true;
}

// Met à jour le menu.

void CMenu::Update (
  Sint32 nb, Sint32 * pButtons, Errors * pErrors,
  std::unordered_map<Sint32, const char *> & texts)
{
  Sint32 i;

  m_nbButtons = nb;

  if (nb < 5)
    m_nbCel.x = 1;
  else
    m_nbCel.x = 2;

  m_nbCel.y = (nb + m_nbCel.x - 1) / m_nbCel.x;

  m_dim.x = (DIMBUTTONX + MARGMENU) * m_nbCel.x;
  m_dim.y = (DIMBUTTONY + MARGMENU) * m_nbCel.y;

  for (i = 0; i < nb; i++)
  {
    m_buttons[i] = pButtons[i];
    m_errors[i]  = pErrors[i];
  }
  m_texts = texts;
}

// Détruit le menu.

void CMenu::Delete ()
{
  m_nbButtons = 0;
  m_selRank   = -1;
}

// Dessine un bouton dans son état.

void CMenu::Draw ()
{
  Sint32 i, state, icon;
  POINT  pos;
  RECT   oldClip, clipRect;
  char   text[50];
  char * pText;

  if (m_nbButtons == 0)
    return;

  oldClip         = m_pPixmap->GetClipping ();
  clipRect.left   = POSDRAWX;
  clipRect.top    = POSDRAWY;
  clipRect.right  = POSDRAWX + DIMDRAWX;
  clipRect.bottom = POSDRAWY + DIMDRAWY;
  m_pPixmap->SetClipping (clipRect);

  for (i = 0; i < m_nbButtons; i++)
  {
    pos.x = m_pos.x + ((i / m_nbCel.y) * (DIMBUTTONX + MARGMENU));
    pos.y = m_pos.y + ((i % m_nbCel.y) * (DIMBUTTONY + MARGMENU));

    if (i == m_selRank)
      state = 2; // hilite
    else
      state = 0; // release
    if (
      m_errors[i] != 0 && m_errors[i] != Errors::TOURISOL && m_errors[i] < 100)
    {
      state = 4; // disable
    }
    m_pPixmap->DrawIcon (-1, CHBUTTON, state, pos);

    icon = table_button_icon[m_buttons[i]];
    if (m_perso == 8) // disciple ?
    {
      if (icon == 30)
        icon = 88; // prend
      if (icon == 31)
        icon = 89; // dépose
    }
    m_pPixmap->DrawIcon (-1, CHBUTTON, icon + 6, pos);
  }

  // Affiche le texte d'aide.
  if (m_selRank != -1)
  {
    i     = m_selRank;
    pos.y = m_pos.y + ((i % m_nbCel.y) * (DIMBUTTONY + MARGMENU));

    if (m_errors[i] == 0)
      pos.y += (DIMBUTTONY - DIMTEXTY) / 2;
    else
      pos.y += (DIMBUTTONY - DIMTEXTY * 2) / 2;

    if (m_errors[i] >= 100) // no ressource au lieu erreur ?
    {
      snprintf (text, sizeof (text), "%s", m_texts[i]);
      pText = strchr (text, '\n');
      if (pText != nullptr)
        *pText = 0;
    }
    else
    {
      const auto tr = GetText (m_buttons[i]);
      snprintf (text, sizeof (text), "%s", tr);
    }

    if (m_nbCel.x > 1 && i < m_nbCel.y)
    {
      //          if ( bLeft )
      //          {
      pos.x = m_pos.x - 4 - GetTextWidth (text); // texte à gauche
                                                 //          }
                                                 //          else
                                                 //          {
      //              pos.x = m_pos.x+((i/m_nbCel.y)+1)+(DIMBUTTONX+MARGMENU)+4;
      //          }
    }
    else
    {
      //          if ( bRight )
      //          {
      pos.x = m_pos.x + m_dim.x + 4; // texte à droite
                                     //          }
                                     //          else
                                     //          {
                                     //              pos.x =
                                     //              m_pos.x+(i/m_nbCel.y)*(DIMBUTTONX+MARGMENU)-4-GetTextWidth(text);
                                     //          }
    }

    DrawText (m_pPixmap, pos, text, FONTWHITE);

    if (m_errors[i] != 0)
    {
      if (m_errors[i] >= 100) // no ressource au lieu erreur ?
      {
        snprintf (text, sizeof (text), "%s", m_texts[i]);
        pText = strchr (text, '\n');
        if (pText != nullptr)
          strcpy (text, pText + 1);
      }
      else
      {
        const auto tr = GetErr (m_errors[i] - 1); // impossible ici
        snprintf (text, sizeof (text), "%s", tr);
      }

      if (m_nbCel.x > 1 && i < m_nbCel.y)
      {
        //              if ( bLeft )
        //              {
        pos.x = m_pos.x - 4 - GetTextWidth (text); // texte à gauche
                                                   //              }
                                                   //              else
                                                   //              {
                                                   //                  pos.x =
        //                  m_pos.x+((i/m_nbCel.y)+1)+(DIMBUTTONX+MARGMENU)+4;
        //              }
      }
      else
      {
        //              if ( bRight )
        //              {
        pos.x = m_pos.x + m_dim.x + 4; // texte à droite
                                       //              }
                                       //              else
                                       //              {
                                       //                  pos.x =
                                       //                  m_pos.x+(i/m_nbCel.y)*(DIMBUTTONX+MARGMENU)-4-GetTextWidth(text);
                                       //              }
      }

      pos.y += DIMTEXTY;
      if (m_errors[i] >= 100) // no ressource au lieu erreur ?
        DrawText (m_pPixmap, pos, text, FONTWHITE);
      else
        DrawText (m_pPixmap, pos, text, FONTRED);
    }
  }

  m_pPixmap->SetClipping (oldClip);
}

// Retourne le bouton sélectionné.

Sint32 CMenu::GetSel ()
{
  if (m_selRank == -1)
    return -1;

  return m_buttons[m_selRank];
}

// Retourne le rang sélectionné.

Sint32 CMenu::GetRank ()
{
  return m_selRank;
}

// Retourne true si le bouton sélectionné a une erreur.

bool CMenu::IsError ()
{
  if (m_selRank == -1)
    return true;

  if (m_errors[m_selRank] != 0 && m_errors[m_selRank] < 100)
    return true;

  return false;
}

// Indique si le menu existe.

bool CMenu::IsExist ()
{
  return (m_nbButtons == 0) ? false : true;
}

// Traitement d'un événement.

bool CMenu::TreatEvent (const SDL_Event & event)
{
  POINT pos;

  if (m_nbButtons == 0)
    return false;

  // pos = ConvLongToPos(lParam);

  switch (event.type)
  {
  case SDL_MOUSEBUTTONDOWN:
    if (
      event.button.button != SDL_BUTTON_LEFT &&
      event.button.button != SDL_BUTTON_RIGHT)
      break;

    pos.x = event.button.x;
    pos.y = event.button.y;
    if (MouseDown (pos))
      return true;
    break;

  case SDL_MOUSEMOTION:
    pos.x = event.motion.x;
    pos.y = event.motion.y;
    if (MouseMove (pos))
      return true;
    break;

  case SDL_MOUSEBUTTONUP:
    if (
      event.button.button != SDL_BUTTON_LEFT &&
      event.button.button != SDL_BUTTON_RIGHT)
      break;

    pos.x = event.button.x;
    pos.y = event.button.y;
    if (MouseUp (pos))
      return true;
    break;
  }

  return false;
}

// Détecte dans quel bouton est la souris.

Sint32 CMenu::Detect (POINT pos)
{
  Sint32 rank;

  if (
    pos.x < m_pos.x || pos.x > m_pos.x + m_dim.x || pos.y < m_pos.y ||
    pos.y > m_pos.y + m_dim.y)
    return -1;

  rank = (pos.y - m_pos.y) / (DIMBUTTONY + MARGMENU);
  rank += ((pos.x - m_pos.x) / (DIMBUTTONX + MARGMENU)) * m_nbCel.y;

  if (rank >= m_nbButtons)
    return -1;
  return rank;
}

// Bouton de la souris pressé.

bool CMenu::MouseDown (POINT pos)
{
  return false;
}

// Souris déplacés.

bool CMenu::MouseMove (POINT pos)
{
  m_selRank = Detect (pos);

  if (
    pos.x < m_pos.x - (DIMBUTTONX + MARGMENU) ||
    pos.x > m_pos.x + m_dim.x + (DIMBUTTONX + MARGMENU) ||
    pos.y < m_pos.y - (DIMBUTTONY + MARGMENU) ||
    pos.y > m_pos.y + m_dim.y + (DIMBUTTONY + MARGMENU))
  {
    Delete (); // enlève le menu si souris trop loin !
  }

  return false;
}

// Bouton de la souris relâché.

bool CMenu::MouseUp (POINT pos)
{
  m_selRank = Detect (pos);

  return false;
}

// Envoie le message.

void CMenu::Message ()
{
  if (m_selRank != -1)
    CEvent::PushUserEvent (WM_BUTTON0 + m_selRank);
}
