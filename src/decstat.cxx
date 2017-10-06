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

#include "decor.h"
#include "gettext.h"
#include "misc.h"
#include "text.h"

#define STATNB 12

// clang-format off
#define STATBLUPIm      0
#define STATBLUPIf      1
#define STATBLUPI       2
#define STATDISCIPLE    3
#define STATFEU         27
#define STATROBOT       28
#define STATTRACKS      29
#define STATBOMBE       30
#define STATARAIGNEE    31
#define STATVIRUS       32
#define STATELECTRO     33
// clang-format on

typedef struct {
  Sint16       bExist;
  Sint16       perso;     // -1=objet, -2=feu, -3=flèche
  Sint16       firstIcon; // négatif si sol
  Sint16       lastIcon;  // négatif si sol
  Sint16       drawIcon;
  Sint16       bBigIcon;
  const char * text;
  Sint16       nb;
  Sint16       lastShow;
} Statistic;

// clang-format off
static Statistic table_statistic[] =
{
    {   // STATBLUPIm = 0
        true,
        0,          // blupi malade
        0, 0,       //
        76,
        false,
        translate ("Sick Blupi"),
        0, 0,
    },
    {   // STATBLUPIf = 1
        true,
        0,          // blupi fatigué
        0, 0,       //
        13,
        false,
        translate ("Tired Blupi"),
        0, 0,
    },
    {   // STATBLUPI = 2
        true,
        0,          // blupi énergique
        0, 0,       //
        14,
        false,
        translate ("Blupi"),
        0, 0,
    },
    {   // STATDISCIPLE = 3
        true,
        8,          // disciple
        0, 0,       //
        85,
        false,
        translate ("Helper robot"),
        0, 0,
    },
    {   // 4
        true,
        -1,         // objet
        117, 117,   // bateau
        58,
        false,
        translate ("Boat"),
        0, 0,
    },
    {   // 5
        true,
        -1,         // objet
        118, 118,   // jeep
        65,
        false,
        translate ("Jeep"),
        0, 0,
    },
    {   // 6
        true,
        -1,         // objet
        16, 16,     // armure
        106,
        false,
        translate ("Armour"),
        0, 0,
    },
    {   // 7
        true,
        -1,         // objet
        93, 93,     // piège
        70,
        false,
        translate ("Sticky trap"),
        0, 0,
    },
    {   // 8
        true,
        -1,         // objet
        92, 92,     // poison
        71,
        false,
        translate ("Poison"),
        0, 0,
    },
    {   // 9
        true,
        -1,         // objet
        85, 85,     // dynamite
        57,
        false,
        translate ("Dynamite"),
        0, 0,
    },
    {   // 10
        true,
        -1,         // objet
        125, 125,   // mine
        63,
        false,
        translate ("Time bomb"),
        0, 0,
    },
    {   // 11
        true,
        -1,         // objet
        60, 60,     // tomate
        28,
        false,
        translate ("Tomatoes"),
        0, 0,
    },
    {   // 12
        true,
        -1,         // objet
        80, 80,     // bouteille
        34,
        false,
        translate ("Medical potion"),
        0, 0,
    },
    {   // 13
        true,
        -1,         // objet
        36, 36,     // planches
        22,
        false,
        translate ("Planks"),
        0, 0,
    },
    {   // 14
        true,
        -1,         // objet
        44, 44,     // pierres
        27,
        false,
        translate ("Stones"),
        0, 0,
    },
    {   // 15
        true,
        -1,         // objet
        124, 124,   // drapeau
        64,
        true,
        translate ("Flag"),
        0, 0,
    },
    {   // 16
        true,
        -1,         // objet
        123, 123,   // fer
        62,
        false,
        translate ("Iron"),
        0, 0,
    },
    {   // 17
        true,
        -1,         // objet
        82, 82,     // fleurs1
        72,
        false,
        translate ("Flowers"),
        0, 0,
    },
    {   // 18
        true,
        -1,         // objet
        84, 84,     // fleurs2
        73,
        false,
        translate ("Flowers"),
        0, 0,
    },
    {   // 19
        true,
        -1,         // objet
        95, 95,     // fleurs3
        74,
        false,
        translate ("Flowers"),
        0, 0,
    },
    {   // 20
        true,
        -1,         // objet
        61, 61,     // cabane
        19,
        true,
        translate ("Garden shed"),
        0, 0,
    },
    {   // 21
        true,
        -1,         // objet
        -52, -56,   // couveuse
        25,
        false,
        translate ("Incubator"),
        0, 0,
    },
    {   // 22
        true,
        -1,         // objet
        -80, -84,   // téléporteur
        101,
        false,
        translate ("Teleporter"),
        0, 0,
    },
    {   // 23
        true,
        -1,         // objet
        28, 29,     // laboratoire
        35,
        true,
        translate ("Laboratory"),
        0, 0,
    },
    {   // 24
        true,
        -1,         // objet
        121, 122,   // mine de fer
        61,
        true,
        translate ("Mine"),
        0, 0,
    },
    {   // 25
        true,
        -1,         // objet
        119, 120,   // usine
        59,
        true,
        translate ("Workshop"),
        0, 0,
    },
    {   // 26
        true,
        -1,         // objet
        27, 27,     // tour
        33,
        true,
        translate ("Protection tower"),
        0, 0,
    },
    {   // STATFEU = 27
        true,
        -2,         // feu
        0, 0,       //
        37,
        true,
        translate ("Fire"),
        0, 0,
    },
    {   // STATROBOT = 28
        true,
        4,          // robot
        0, 0,       //
        56,
        false,
        translate ("Master robot"),
        0, 0,
    },
    {   // STATTRACKS = 29
        true,
        3,          // tracks
        0, 0,       //
        17,
        false,
        translate ("Bulldozer"),
        0, 0,
    },
    {   // STATBOMBE = 30
        true,
        5,          // bombe
        0, 0,       //
        38,
        false,
        translate ("Bouncing bomb"),
        0, 0,
    },
    {   // STATARAIGNEE = 31
        true,
        1,          // araignée
        0, 0,       //
        15,
        false,
        translate ("Spider"),
        0, 0,
    },
    {   // STATVIRUS = 32
        true,
        2,          // virus
        0, 0,       //
        16,
        false,
        translate ("Virus"),
        0, 0,
    },
    {   // STATELECTRO = 33
        true,
        7,          // électro
        0, 0,       //
        75,
        false,
        translate ("Electrocutor"),
        0, 0,
    },

    {
        false,
        -1,
        0, 0,
        -1,
        false,
        "",
        999, 999,
    },
};
// clang-format on

// Retourne la statistique correspondant à un rang donné.

Statistic *
StatisticGet (Sint32 rank)
{
  Statistic * pStatistic;

  pStatistic = table_statistic;
  while (pStatistic->nb == 0)
    pStatistic++;

  while (rank > 0)
  {
    if (pStatistic->bExist)
      pStatistic++;
    while (pStatistic->nb == 0)
      pStatistic++;
    rank--;
  }

  return pStatistic;
}

// Réinitialise les statistiques.

void
CDecor::StatisticInit ()
{
  Statistic * pStatistic;

  pStatistic = table_statistic;
  while (pStatistic->bExist)
  {
    pStatistic->lastShow = 0;
    pStatistic++;
  }

  m_statNb      = 0;
  m_statFirst   = 0;
  m_bStatUp     = false;
  m_bStatDown   = false;
  m_statHili    = -1;
  m_bStatRecalc = true; // faudra tout recalculer
}

// Met à jour tous les compteurs des statistiques.

void
CDecor::StatisticUpdate ()
{
  Sint32      rank, x, y, icon, nb;
  bool        bHach;
  Statistic * pStatistic;

  m_nbStatHach        = 0;
  m_nbStatHachBlupi   = 0;
  m_nbStatHachPlanche = 0;
  m_nbStatHachTomate  = 0;
  m_nbStatHachMetal   = 0;
  m_nbStatHachRobot   = 0;
  m_nbStatHome        = 0;
  m_nbStatHomeBlupi   = 0;
  m_nbStatRobots      = 0;

  pStatistic = table_statistic;
  while (pStatistic->bExist)
  {
    pStatistic->nb = 0;
    pStatistic++;
  }

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (m_blupi[rank].bExist)
    {
      if (m_blupi[rank].perso == 0) // blupi ?
      {
        if (m_blupi[rank].bMalade)
          table_statistic[STATBLUPIm].nb++;
        else
        {
          if (m_blupi[rank].energy <= MAXENERGY / 4)
            table_statistic[STATBLUPIf].nb++;
          else
            table_statistic[STATBLUPI].nb++;
        }
        x = (m_blupi[rank].cel.x / 2) * 2;
        y = (m_blupi[rank].cel.y / 2) * 2;
        if (
          m_decor[x / 2][y / 2].floorChannel == CHFLOOR &&
          m_decor[x / 2][y / 2].floorIcon == 17) // dalle hachurée ?
          m_nbStatHachBlupi++;
        if (
          m_decor[x / 2][y / 2].objectChannel == CHOBJECT &&
          m_decor[x / 2][y / 2].objectIcon == 113) // maison ?
          m_nbStatHomeBlupi++;
      }
      if (m_blupi[rank].perso == 8) // disciple ?
        table_statistic[STATDISCIPLE].nb++;

      // Hide enemies from the stat when hidden by the fog
      bool hide = false;
      if (this->GetSkill () >= 1)
      {
        auto fogCel = m_blupi[rank].cel;
        fogCel.x    = (fogCel.x / 4) * 4;
        fogCel.y    = (fogCel.y / 4) * 4;
        if (m_decor[fogCel.x / 2][fogCel.y / 2].fog == FOGHIDE) // hidden?
          hide = true;
      }

      if (m_blupi[rank].perso == 4) // robot ?
      {
        if (!hide)
          table_statistic[STATROBOT].nb++;
        m_nbStatRobots++;
        x = (m_blupi[rank].cel.x / 2) * 2;
        y = (m_blupi[rank].cel.y / 2) * 2;
        if (
          m_decor[x / 2][y / 2].floorChannel == CHFLOOR &&
          m_decor[x / 2][y / 2].floorIcon == 17) // dalle hachurée ?
          m_nbStatHachRobot++;
      }
      if (m_blupi[rank].perso == 3) // tracks ?
      {
        if (!hide)
          table_statistic[STATTRACKS].nb++;
        if (!m_term.bHachRobot) // pas robot sur hachures ?
          m_nbStatRobots++;
      }
      if (m_blupi[rank].perso == 1) // araignée ?
      {
        if (!hide)
          table_statistic[STATARAIGNEE].nb++;
        if (!m_term.bHachRobot) // pas robot sur hachures ?
          m_nbStatRobots++;
      }
      if (m_blupi[rank].perso == 2) // virus ?
        if (!hide)
          table_statistic[STATVIRUS].nb++;
      if (m_blupi[rank].perso == 5) // bombe ?
      {
        if (!hide)
          table_statistic[STATBOMBE].nb++;
        if (!m_term.bHachRobot) // pas robot sur hachures ?
          m_nbStatRobots++;
      }
      if (m_blupi[rank].perso == 7) // électro ?
      {
        if (!hide)
          table_statistic[STATELECTRO].nb++;
        if (!m_term.bHachRobot) // pas robot sur hachures ?
          m_nbStatRobots++;
      }
    }
  }

  for (x = 0; x < MAXCELX; x += 2)
  {
    for (y = 0; y < MAXCELY; y += 2)
    {
      bHach = false;
      if (
        m_decor[x / 2][y / 2].floorChannel == CHFLOOR &&
        m_decor[x / 2][y / 2].floorIcon == 17) // dalle hachurée ?
      {
        bHach = true;
        m_nbStatHach++;
      }

      if (
        m_decor[x / 2][y / 2].objectChannel == CHOBJECT &&
        m_decor[x / 2][y / 2].objectIcon == 113) // maison ?
        m_nbStatHome++;

      if (m_decor[x / 2][y / 2].objectChannel == CHOBJECT)
      {
        icon = m_decor[x / 2][y / 2].objectIcon;

        pStatistic = table_statistic;
        while (pStatistic->bExist)
        {
          if (
            pStatistic->perso == -1 && pStatistic->firstIcon > 0 &&
            icon >= pStatistic->firstIcon && icon <= pStatistic->lastIcon)
          {
            pStatistic->nb++;
            break;
          }
          pStatistic++;
        }

        if (icon == 36 && bHach) // planches ?
          m_nbStatHachPlanche++;
        if (icon == 60 && bHach) // tomates ?
          m_nbStatHachTomate++;
        if (icon == 14 && bHach) // métal ?
          m_nbStatHachMetal++;
      }

      if (m_decor[x / 2][y / 2].floorChannel == CHFLOOR)
      {
        icon = m_decor[x / 2][y / 2].floorIcon;

        if (
          (icon >= 52 && icon <= 56) || // couveuse ?
          (icon >= 80 && icon <= 84))   // téléporteur ?
        {
          pStatistic = table_statistic;
          while (pStatistic->bExist)
          {
            if (
              pStatistic->perso == -1 && pStatistic->firstIcon < 0 &&
              icon >= -(pStatistic->firstIcon) &&
              icon <= -(pStatistic->lastIcon))
            {
              pStatistic->nb++;
              break;
            }
            pStatistic++;
          }
        }
      }

      if (
        m_decor[x / 2][y / 2].fire > 0 &&
        m_decor[x / 2][y / 2].fire < MoveMaxFire ())
      {
        table_statistic[STATFEU].nb++; // un feu de plus
      }
    }
  }

  pStatistic = table_statistic;
  m_statNb   = 0;
  while (pStatistic->bExist)
  {
    if (pStatistic->nb > 0)
      m_statNb++;
    pStatistic++;
  }
  if (m_statNb <= STATNB) // tout visible en une page ?
  {
    m_bStatUp   = false;
    m_bStatDown = false;
    m_statFirst = 0;
  }
  else
  {
    // nb <- nb de pages nécessaires
    nb = (m_statNb + STATNB - 5) / (STATNB - 2);

    m_bStatUp   = true;
    m_bStatDown = true;
    if (m_statFirst >= 1 + (nb - 1) * (STATNB - 2))
    {
      m_statFirst = 1 + (nb - 1) * (STATNB - 2);
      m_bStatDown = false;
    }
    if (m_statFirst == 0)
      m_bStatUp = false;
  }

  m_bStatRecalc = false; // c'est calculé
}

// Retourne le nombre de blupi.

Sint32
CDecor::StatisticGetBlupi ()
{
  return table_statistic[STATBLUPIf].nb + table_statistic[STATBLUPIm].nb +
         table_statistic[STATBLUPI].nb;
}

// Retourne le nombre de cellules en feu.

Sint32
CDecor::StatisticGetFire ()
{
  return table_statistic[STATFEU].nb;
}

// Dessine toutes les statistiques.

void
CDecor::StatisticDraw ()
{
  Point        pos;
  Rect         rect;
  Sint32       rank, icon, nb;
  Statistic *  pStatistic;
  char         text[50];
  const char * textRes;

  pStatistic = table_statistic;

  while (pStatistic->nb == 0)
    pStatistic++;

  nb = m_statFirst;
  while (nb > 0)
  {
    if (pStatistic->bExist)
      pStatistic++;
    while (pStatistic->nb == 0)
      pStatistic++;
    nb--;
  }

  textRes = "";
  for (rank = 0; rank < STATNB; rank++)
  {
    pos.x       = POSSTATX + DIMSTATX * (rank / (STATNB / 2));
    pos.y       = POSSTATY + DIMSTATY * (rank % (STATNB / 2));
    rect.left   = pos.x;
    rect.right  = pos.x + DIMSTATX;
    rect.top    = pos.y;
    rect.bottom = pos.y + DIMSTATY;

    m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1); // dessine le fond

    if (rank == 0 && m_bStatUp)
    {
      icon = 6 + 66;          // flèche up
      if (rank == m_statHili) // statistique survolée ?
        icon++;
      pos.x -= 3;
      pos.y -= 5;
      if (pStatistic->drawIcon == 68)
        pos.x += 26;
      m_pPixmap->DrawIcon (-1, CHBUTTON, icon, pos); // flèche up
      continue;
    }

    if (rank == STATNB - 1 && m_bStatDown)
    {
      icon = 6 + 68;          // flèche down
      if (rank == m_statHili) // statistique survolée ?
        icon++;
      pos.x += 23;
      pos.y -= 5;
      m_pPixmap->DrawIcon (-1, CHBUTTON, icon, pos); // flèche down
      continue;
    }

    if (!pStatistic->bExist)
      goto next;

    icon = 6 + pStatistic->drawIcon;

    if (rank == m_statHili) // statistique survolée ?
    {
      m_pPixmap->DrawIconDemi (-1, CHBLUPI, ICON_HILI_STAT, pos);
      textRes = gettext (pStatistic->text);
    }

    if (pStatistic->nb > 0)
    {
      pos.x -= 3;
      pos.y -= 5;
      m_pPixmap->DrawIcon (-1, CHBUTTON, icon, pos);

      nb = pStatistic->nb;
      snprintf (text, sizeof (text), "%d", nb);
      pos.x += 3 + 34;
      pos.y += 5 + 7;
      DrawText (m_pPixmap, pos, text);
    }

  next:
    if (pStatistic->bExist)
      pStatistic++;
    while (pStatistic->nb == 0)
      pStatistic++;
  }

  // Dans un bouton stop/setup/write ?
  if (!strlen (textRes) && m_statHili >= 100)
  {
    if (m_statHili == 100)
      textRes = gettext ("Interrupt");
    if (m_statHili == 101)
      textRes = gettext ("Settings");
    if (m_statHili == 102)
      textRes = gettext ("Save");
  }

  // Dessine le nom de la statistique survolée.
  pos.x       = 0;
  pos.y       = 404;
  rect.left   = pos.x;
  rect.right  = pos.x + POSDRAWX;
  rect.top    = pos.y;
  rect.bottom = pos.y + 16;
  m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1); // dessine le fond

  if (strlen (textRes))
  {
    nb = GetTextWidth (textRes);
    pos.x += (POSDRAWX - nb) / 2;
    DrawText (m_pPixmap, pos, textRes);
  }
}

// Génère les statistiques.

void
CDecor::GenerateStatictic ()
{
  if (m_bBuild)
    return;

  if (m_bStatRecalc || m_phase % 20 == 10)
  {
    StatisticUpdate (); // met à jour les compteurs
  }

  StatisticDraw (); // redessine tout
}

// Bouton pressé dans les statistiques.

bool
CDecor::StatisticDown (Point pos)
{
  Sint32      hili, rank, x, y, show, icon;
  Point       cel;
  Statistic * pStatistic;

  StatisticUpdate ();

  hili = StatisticDetect (pos);
  if (hili < 0)
    return false;

  if (m_bStatUp && hili == 0) // flèche up ?
  {
    m_statFirst -= STATNB - 2;
    if (m_statFirst < STATNB - 1)
      m_statFirst = 0;
    StatisticUpdate ();
    pos.x = LXIMAGE / 2;
    pos.y = LYIMAGE / 2;
    m_pSound->PlayImage (SOUND_OPEN, pos);
    return true;
  }

  if (m_bStatDown && hili == STATNB - 1) // flèche down ?
  {
    if (m_statFirst == 0)
      m_statFirst = STATNB - 1;
    else
      m_statFirst += STATNB - 2;
    StatisticUpdate ();
    pos.x = LXIMAGE / 2;
    pos.y = LYIMAGE / 2;
    m_pSound->PlayImage (SOUND_OPEN, pos);
    return true;
  }

  rank = m_statFirst + hili;
  if (rank > 0 && m_bStatUp)
    rank--;
  pStatistic = StatisticGet (rank);
  if (!pStatistic->bExist)
    return false;

  show = pStatistic->lastShow % pStatistic->nb;
  pStatistic->lastShow++;

  if (pStatistic->perso >= 0) // blupi/araignée ?
  {
    for (rank = 0; rank < MAXBLUPI; rank++)
    {
      if (m_blupi[rank].bExist)
      {
        if (m_blupi[rank].perso != pStatistic->perso)
          continue;

        if (
          m_blupi[rank].perso != 0 ||
          (m_blupi[rank].bMalade && pStatistic->drawIcon == 76) || // malade ?
          (!m_blupi[rank].bMalade && m_blupi[rank].energy <= MAXENERGY / 4 &&
           pStatistic->drawIcon == 13) || // fatigué ?
          (m_blupi[rank].energy > MAXENERGY / 4 &&
           pStatistic->drawIcon == 14)) // énergique ?
        {
          if (show == 0)
          {
            if (
              m_blupi[rank].perso == 0 || // blupi ?
              m_blupi[rank].perso == 8)   // disciple ?
            {
              BlupiDeselect ();
              m_blupi[rank].bHili = true;
              m_rankBlupiHili     = rank; // sélectionne
              m_nbBlupiHili       = 1;
            }
            BlupiSetArrow (rank, true);
            cel = m_blupi[rank].cel;
            goto select;
          }
          show--;
        }
      }
    }
  }

  if (
    pStatistic->perso == -1 && // objet ?
    pStatistic->firstIcon > 0)
  {
    for (x = 0; x < MAXCELX; x += 2)
    {
      for (y = 0; y < MAXCELY; y += 2)
      {
        if (m_decor[x / 2][y / 2].objectChannel == CHOBJECT)
        {
          icon = m_decor[x / 2][y / 2].objectIcon;

          if (icon >= pStatistic->firstIcon && icon <= pStatistic->lastIcon)
          {
            if (show == 0)
            {
              cel = GetCel (x, y);
              if (pStatistic->bBigIcon)
              {
                // Flèche plus haute.
                m_celArrow = GetCel (cel, -2, -2);
              }
              else
                m_celArrow = cel;
              goto select;
            }
            show--;
          }
        }
      }
    }
  }

  if (
    pStatistic->perso == -1 && // sol ?
    pStatistic->firstIcon < 0)
  {
    for (x = 0; x < MAXCELX; x += 2)
    {
      for (y = 0; y < MAXCELY; y += 2)
      {
        if (m_decor[x / 2][y / 2].floorChannel == CHFLOOR)
        {
          icon = m_decor[x / 2][y / 2].floorIcon;

          if (
            icon >= -(pStatistic->firstIcon) && icon <= -(pStatistic->lastIcon))
          {
            if (show == 0)
            {
              cel = GetCel (x, y);
              if (pStatistic->bBigIcon)
              {
                // Flèche plus haute.
                m_celArrow = GetCel (cel, -2, -2);
              }
              else
                m_celArrow = cel;
              goto select;
            }
            show--;
          }
        }
      }
    }
  }

  if (pStatistic->perso == -2) // feu ?
  {
    for (x = 0; x < MAXCELX; x += 2)
    {
      for (y = 0; y < MAXCELY; y += 2)
      {
        if (
          m_decor[x / 2][y / 2].fire > 0 &&
          m_decor[x / 2][y / 2].fire < MoveMaxFire ())
        {
          if (show == 0)
          {
            cel        = GetCel (x, y);
            m_celArrow = cel;
            goto select;
          }
          show--;
        }
      }
    }
  }

  return false;

select:
  SetCoin (cel, true);
  NextPhase (0); // faudra refaire la carte tout de suite
  return true;
}

// Souris déplacée dans les statistiques.

bool
CDecor::StatisticMove (Point pos)
{
  Sint32 rank;

  rank = StatisticDetect (pos);

  if (rank != m_statHili) // autre mise en évidence ?
    m_statHili = rank;

  return false;
}

// Bouton relâché dans les statistiques.

bool
CDecor::StatisticUp (Point pos)
{
  return false;
}

// Détecte dans quelle statistique est la souris.

Sint32
CDecor::StatisticDetect (Point pos)
{
  Sint32 rank;

  // Dans un bouton stop/setup/write ?
  if (pos.x >= 10 && pos.x <= 10 + 42 * 3 && pos.y >= 422 && pos.y <= 422 + 40)
  {
    pos.x -= 10;
    if (pos.x % 42 > 40)
      return -1;
    return 100 + pos.x / 42;
  }

  if (
    pos.x >= POSSTATX && pos.x <= POSSTATX + DIMSTATX * 2 &&
    pos.y >= POSSTATY && pos.y <= POSSTATY + DIMSTATY * (STATNB / 2))
  {
    rank = ((pos.x - POSSTATX) / DIMSTATX) * (STATNB / 2);
    rank += ((pos.y - POSSTATY) / DIMSTATY);
    if (rank >= STATNB)
      return -1;

    auto pStatistic = StatisticGet (rank);
    if (
      this->GetSkill () >= 1 && pStatistic->perso >= 0 &&
      (pStatistic->perso != 0 && pStatistic->perso != 8))
    {
      return -1;
    }

    return rank;
  }

  return -1;
}
