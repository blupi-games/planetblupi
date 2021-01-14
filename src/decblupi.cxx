/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997, Daniel Roux & EPSITEC SA
 * Copyright (C) 2017-2021, Mathieu Schroeter
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

#include <unordered_map>

#include "action.h"
#include "decgoal.h"
#include "decor.h"
#include "def.h"
#include "gettext.h"
#include "misc.h"

// Cette table donne l'action à effectuer pour un bouton
// enfoncé.
const Sint16 table_actions[] = {
  EV_ACTION_GO,       EV_ACTION_STOP,    EV_ACTION_EAT,       EV_ACTION_CARRY,
  EV_ACTION_DROP,     EV_ACTION_ABAT1,   EV_ACTION_ROC1,      EV_ACTION_CULTIVE,
  EV_ACTION_BUILD1,   EV_ACTION_BUILD2,  EV_ACTION_BUILD3,    EV_ACTION_BUILD4,
  EV_ACTION_BUILD5,   EV_ACTION_BUILD6,  EV_ACTION_WALL,      EV_ACTION_PALIS,
  EV_ACTION_ABAT1,    EV_ACTION_ROC1,    EV_ACTION_BRIDGEE,   EV_ACTION_TOWER,
  EV_ACTION_DRINK,    EV_ACTION_LABO,    EV_ACTION_FLOWER1,   EV_ACTION_FLOWER1,
  EV_ACTION_DYNAMITE, EV_ACTION_BOATE,   EV_ACTION_DJEEP,     EV_ACTION_FLAG,
  EV_ACTION_EXTRAIT,  EV_ACTION_FABJEEP, EV_ACTION_FABMINE,   EV_ACTION_FABDISC,
  EV_ACTION_REPEAT,   EV_ACTION_DARMURE, EV_ACTION_FABARMURE,
};

// Supprime tous les blupi.

void
CDecor::BlupiFlush ()
{
  Sint32 i;

  memset (m_blupi, 0, sizeof (Blupi) * MAXBLUPI);

  for (i = 0; i < MAXBLUPI; i++)
    m_blupi[i].bExist = false;
}

// Crée un nouveau blupi, et retourne son rang.

Sint32
CDecor::BlupiCreate (
  Point cel, Sint32 action, Sint32 direct, Sint32 perso, Sint32 energy)
{
  Sint32 rank;

  if (
    perso == 0 && action == ACTION_STOP && // blupi ?
    energy <= MAXENERGY / 4)
    action = ACTION_STOPTIRED;

  if (perso == 1 && action == ACTION_STOP) // araignée ?
    action = ACTION_S_STOP;

  if (perso == 2 && action == ACTION_STOP) // virus ?
    action = ACTION_V_STOP;

  if (perso == 3 && action == ACTION_STOP) // tracks ?
    action = ACTION_T_STOP;

  if (perso == 4 && action == ACTION_STOP) // robot ?
    action = ACTION_R_STOP;

  if (perso == 5 && action == ACTION_STOP) // bombe ?
    action = ACTION_B_STOP;

  if (perso == 7 && action == ACTION_STOP) // électro ?
    action = ACTION_E_STOP;

  if (perso == 8 && action == ACTION_STOP) // disciple ?
    action = ACTION_D_STOP;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist && m_blupi[rank].cel.x == cel.x &&
      m_blupi[rank].cel.y == cel.y)
    {
      m_blupi[rank].aDirect = ((m_blupi[rank].aDirect / 16 + 1) % 8) * 16;
      m_blupi[rank].sDirect = m_blupi[rank].aDirect;
      m_blupi[rank].perso   = perso;
      m_blupi[rank].energy  = energy;
      m_blupi[rank].action  = action;
      BlupiActualise (rank);
      return rank;
    }
  }

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (!m_blupi[rank].bExist)
    {
      m_blupi[rank].bExist      = true;
      m_blupi[rank].bHili       = false;
      m_blupi[rank].perso       = perso;
      m_blupi[rank].energy      = energy;
      m_blupi[rank].goalAction  = 0;
      m_blupi[rank].goalCel.x   = -1;
      m_blupi[rank].goalCel.y   = -1;
      m_blupi[rank].cel         = cel;
      m_blupi[rank].fix         = cel;
      m_blupi[rank].action      = action;
      m_blupi[rank].interrupt   = 1;
      m_blupi[rank].aDirect     = direct;
      m_blupi[rank].sDirect     = direct;
      m_blupi[rank].phase       = 0;
      m_blupi[rank].step        = rank * 13; // un peu de hazard !
      m_blupi[rank].channel     = -1;
      m_blupi[rank].icon        = -1;
      m_blupi[rank].lastIcon    = -1;
      m_blupi[rank].pos.x       = 0;
      m_blupi[rank].pos.y       = 0;
      m_blupi[rank].posZ        = 0;
      m_blupi[rank].takeChannel = -1;
      m_blupi[rank].passCel.x   = -1;
      m_blupi[rank].jaugePhase  = -1;
      m_blupi[rank].jaugeMax    = -1;
      m_blupi[rank].stop        = 0;
      m_blupi[rank].bArrow      = false;
      m_blupi[rank].bRepeat     = false;
      m_blupi[rank].bMalade     = false;
      m_blupi[rank].bCache      = false;
      m_blupi[rank].vehicule    = 0;
      m_blupi[rank].busyCount   = 0;
      m_blupi[rank].busyDelay   = 0;
      m_blupi[rank].clicCount   = 0;
      m_blupi[rank].clicDelay   = 0;

      ListFlush (rank);
      FlushUsed (rank);
      BlupiDestCel (rank);
      BlupiPushFog (rank);
      BlupiActualise (rank);
      return rank;
    }
  }

  return -1;
}

// Supprime un blupi existant.
// Si perso == -1, supprime n'importe quel personnage ici.
// Si perso >= 0, supprime seulement ce personnage.

bool
CDecor::BlupiDelete (Point cel, Sint32 perso)
{
  Sint32 rank;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist && m_blupi[rank].cel.x == cel.x &&
      m_blupi[rank].cel.y == cel.y &&
      (perso == -1 || m_blupi[rank].perso == perso))
    {
      m_blupi[rank].bExist = false;

      if (!m_bBuild) // phase de jeu ?
      {
        if (perso == 6) // détonnateur invisible ?
        {
          MoveFinish (rank); // stoppe décompte à rebourd
        }
        if (m_nbBlupiHili > 0 && m_rankBlupiHili == rank) // est-ce le blupi
                                                          // sélectionné ?
        {
          m_nbBlupiHili   = 0;
          m_rankBlupiHili = -1;
        }
      }
      return true;
    }
  }

  return false;
}

// Supprime un blupi existant.

void
CDecor::BlupiDelete (Sint32 rank)
{
  m_blupi[rank].bExist = false;
  this->m_pSound->StopSound (true, rank);

  if (
    !m_bBuild && // phase de jeu ?
    m_nbBlupiHili > 0 &&
    m_rankBlupiHili == rank) // est-ce le blupi sélectionné ?
  {
    m_nbBlupiHili   = 0;
    m_rankBlupiHili = -1;
  }
}

// Supprime tout dans un périmètre donné suite à une explosion.
//  type=0  ->  explosion
//  type=1  ->  électro

void
CDecor::BlupiKill (Sint32 exRank, Point cel, Sint32 type)
{
  Sint32 rank, action, x, y, icon;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    // Supprime sans condition les blupi placés
    // dans la cellule où a lieu l'explosion.
    if (
      rank != exRank && m_blupi[rank].bExist &&
      m_blupi[rank].vehicule != 3 && // pas armure ?
      !m_bInvincible && m_blupi[rank].cel.x >= cel.x &&
      m_blupi[rank].cel.x <= cel.x + 1 && m_blupi[rank].cel.y >= cel.y &&
      m_blupi[rank].cel.y <= cel.y + 1)
    {
      GoalUnwork (rank);

      if (type == 0) // explosion ?
      {
        m_blupi[rank].bExist = false; // mort instantannée
        this->m_pSound->StopSound (true, rank);
      }

      if (type == 1) // électro ?
      {
        x    = m_blupi[rank].cel.x;
        y    = m_blupi[rank].cel.y;
        icon = m_decor[x / 2][y / 2].objectIcon;
        if (
          m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 0 && // à pied ?
          !m_bInvincible && icon != 113 &&                           // maison ?
          icon != 28 && icon != 29 &&   // laboratoire ?
          icon != 119 && icon != 120 && // usine ?
          icon != 121 && icon != 122)   // mine de fer ?
        {
          if (m_blupi[rank].bMalade)
            action = EV_ACTION_ELECTROm;
          else
            action = EV_ACTION_ELECTRO;
          GoalStart (rank, action, m_blupi[rank].cel);
          //?                 BlupiChangeAction(rank, ACTION_ELECTRO);
        }
      }
    }

    // Supprime les blupi placés une case autour de la
    // cellule où a lieu l'explosion, seulement s'ils
    // ne sont pas cachés (pas dans un batiment).
    if (
      type == 0 && rank != exRank && m_blupi[rank].bExist &&
      m_blupi[rank].vehicule != 3 &&             // pas armure ?
      !m_bInvincible && !m_blupi[rank].bCache && // pas dans un batiment ?
      m_blupi[rank].cel.x >= cel.x - 1 && m_blupi[rank].cel.x <= cel.x + 2 &&
      m_blupi[rank].cel.y >= cel.y - 1 && m_blupi[rank].cel.y <= cel.y + 2)
    {
      GoalUnwork (rank);
      m_blupi[rank].bExist = false;
    }
  }
}

// Test si un blupi existe.

bool
CDecor::BlupiIfExist (Sint32 rank)
{
  return !!m_blupi[rank].bExist;
}

// Triche pour tous les blupi.
//  #1  ->  (POWER)    redonne l'énergie maximale
//  #2  ->  (LONESOME) tue toutes les araignées/virus/etc.

void
CDecor::BlupiCheat (Sint32 cheat)
{
  Sint32 rank;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (cheat == 1) // power ?
    {
      if (m_blupi[rank].bExist && m_blupi[rank].perso == 0)
      {
        m_blupi[rank].energy  = MAXENERGY;
        m_blupi[rank].bMalade = false;
      }
    }

    if (cheat == 2) // lonesome ?
    {
      if (
        m_blupi[rank].bExist && m_blupi[rank].perso != 0 &&
        m_blupi[rank].perso != 8) // araignée/virus/etc. ?
        m_blupi[rank].bExist = false;
    }
  }
}

// Actualise un blupi pour pouvoir le dessiner dans son état.

void
CDecor::BlupiActualise (Sint32 rank)
{
  Sounds sound;

  Action (
    m_blupi[rank].action, m_blupi[rank].aDirect, m_blupi[rank].phase,
    m_blupi[rank].step, m_blupi[rank].channel, m_blupi[rank].icon,
    m_blupi[rank].pos, m_blupi[rank].posZ, sound);
  BlupiAdaptIcon (rank);

  m_blupi[rank].lastIcon = -1;
  m_blupi[rank].phase    = 0;
  m_blupi[rank].pos.x    = 0;
  m_blupi[rank].pos.y    = 0;
  m_blupi[rank].posZ     = 0;
}

// Adapte une icône.

void
CDecor::BlupiAdaptIcon (Sint32 rank)
{
  Sint32 direct;

  if (m_blupi[rank].icon == -1)
    return;

  if (m_blupi[rank].bMalade) // malade ?
  {
    if (m_blupi[rank].icon >= 69 && m_blupi[rank].icon <= 92)
    {
      m_blupi[rank].icon += 100; // 169..192 (tout vert)
    }
    else if (m_blupi[rank].icon == 270) // écrasé ?
      m_blupi[rank].icon = 271;
    else if (
      m_blupi[rank].icon < 318 || // pas télétransporté ?
      m_blupi[rank].icon > 321)
    {
      direct = GetIconDirect (m_blupi[rank].icon);
      if (direct < 0)
      {
        m_blupi[rank].icon = 169; // debout direction est !
      }
      else
        m_blupi[rank].icon = 169 + 3 * (direct / 16);
    }
  }

  if (m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 1) // en bateau ?
  {
    direct = GetIconDirect (m_blupi[rank].icon);
    if (direct < 0)
    {
      m_blupi[rank].icon = 234; // bateau direction est !
    }
    else
      m_blupi[rank].icon = 234 + (direct / (16 / 2));
  }

  if (m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 2) // en jeep ?
  {
    direct = GetIconDirect (m_blupi[rank].icon);
    if (direct < 0)
    {
      m_blupi[rank].icon = 250; // bateau direction est !
    }
    else
      m_blupi[rank].icon = 250 + (direct / (16 / 2));
  }

  if (m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 3) // armure ?
  {
    if (m_blupi[rank].icon < 322 || m_blupi[rank].icon > 347)
    {
      if (
        m_blupi[rank].icon == 106 || // élan pour saut ?
        m_blupi[rank].icon == 194)   // mèche ?
        m_blupi[rank].icon = 347;
      else
      {
        direct = GetIconDirect (m_blupi[rank].icon);
        if (direct < 0)
        {
          m_blupi[rank].icon = 322; // armure direction est !
        }
        else
          m_blupi[rank].icon = 322 + 3 * (direct / 16);
      }
    }
  }

  if (m_blupi[rank].perso == 8) // disciple ?
  {
    direct = GetIconDirect (m_blupi[rank].icon);
    if (direct < 0)
    {
      m_blupi[rank].icon = 290; // disciple direction est !
    }
    else
      m_blupi[rank].icon = 290 + (direct / (16 / 2));
  }
}

// Fait entendre un son pour un blupi.
// Si bStop=true, on stoppe le son précédent associé
// à ce blupi (rank), si nécessaire.

void
CDecor::BlupiSound (Sint32 rank, Sounds sound, Point pos, bool bStop)
{
  Sounds newSound;

  if (rank == -1)
    rank = m_rankBlupiHili;

  if (rank != -1 && m_blupi[rank].perso == 8) // disciple ?
  {
    if (
      sound == SOUND_HOP || sound == SOUND_BURN || sound == SOUND_TCHAO ||
      sound == SOUND_FLOWER || sound == SOUND_ARROSE)
      newSound = SOUND_NONE;
    else
      newSound = sound;

    if (
      sound == SOUND_BOING || sound == SOUND_BOING1 || sound == SOUND_BOING2 ||
      sound == SOUND_BOING3)
      newSound = SOUND_D_BOING;
    if (
      sound == SOUND_GO1 || sound == SOUND_GO2 || sound == SOUND_GO3 ||
      sound == SOUND_GO4 || sound == SOUND_GO5 || sound == SOUND_GO6)
      newSound = SOUND_D_GO;
    if (
      sound == SOUND_OK1 || sound == SOUND_OK2 || sound == SOUND_OK3 ||
      sound == SOUND_OK4 || sound == SOUND_OK5 || sound == SOUND_OK6 ||
      sound == SOUND_OK1f || sound == SOUND_OK2f || sound == SOUND_OK3f ||
      sound == SOUND_OK1e || sound == SOUND_OK2e || sound == SOUND_OK3e)
      newSound = SOUND_D_OK;
    if (
      sound == SOUND_TERM1 || sound == SOUND_TERM2 || sound == SOUND_TERM3 ||
      sound == SOUND_TERM4 || sound == SOUND_TERM5 || sound == SOUND_TERM6)
      newSound = SOUND_D_TERM;

    if (newSound == SOUND_NONE)
      return;
    sound = newSound;
  }

  if (bStop)
    m_pSound->PlayImage (sound, pos, rank, bStop);
  else
    m_pSound->PlayImage (sound, pos, rank);
}

// Sons associés à des actions.
static const struct {
  Sint16 action;
  Sounds sound;
} tableSound[] = {
  {ACTION_BURN, SOUND_BURN},   {ACTION_TCHAO, SOUND_TCHAO},
  {ACTION_EAT, SOUND_EAT},     {ACTION_DRINK, SOUND_DRINK},
  {ACTION_SLIDE, SOUND_SLIDE}, {ACTION_R_LOAD, SOUND_R_LOAD},
};

// Effectue quelques initialisations pour une nouvelle action.

void
CDecor::BlupiInitAction (Sint32 rank, Sint32 action, Sint32 direct)
{
  Point  pos;
  Sint32 rand;

  for (size_t i = 0; i < countof (tableSound); ++i)
  {
    if (tableSound[i].action == action)
    {
      pos = ConvCelToPos (m_blupi[rank].cel);
      BlupiSound (rank, tableSound[i].sound, pos);
      break;
    }
  }

  m_blupi[rank].action = action;

  if (m_blupi[rank].perso == 0) // blupi ?
  {
    if (m_blupi[rank].vehicule == 1) // en bateau ?
    {
      if (m_blupi[rank].action == ACTION_STOP)
        m_blupi[rank].action = ACTION_STOPb;

      if (m_blupi[rank].action == ACTION_WALK)
        m_blupi[rank].action = ACTION_MARCHEb;
    }

    if (m_blupi[rank].vehicule == 2) // en jeep ?
    {
      if (m_blupi[rank].action == ACTION_STOP)
        m_blupi[rank].action = ACTION_STOPJEEP;

      if (m_blupi[rank].action == ACTION_WALK)
        m_blupi[rank].action = ACTION_WALKJEEP;
    }

    if (m_blupi[rank].vehicule == 3) // armure ?
    {
      if (m_blupi[rank].action == ACTION_STOP)
        m_blupi[rank].action = ACTION_STOPARMOR;

      if (m_blupi[rank].action == ACTION_WALK)
      {
        m_blupi[rank].action = ACTION_WALKARMOR;
        m_blupi[rank].step   = (m_blupi[rank].step / 2) * 2;
      }
    }

    if (m_blupi[rank].energy <= MAXENERGY / 4) // peu de forces ?
    {
      if (m_blupi[rank].action == ACTION_STOP)
        m_blupi[rank].action = ACTION_STOPTIRED;

      if (m_blupi[rank].action == ACTION_WALK)
      {
        m_blupi[rank].action = ACTION_WALKTIRED;
        m_blupi[rank].step   = (m_blupi[rank].step / 2) * 2;
      }
    }

    if (
      m_blupi[rank].action == ACTION_STOP &&
      m_blupi[rank].goalAction == 0) // à pied ?
    {
      rand = Random (0, 400);
      if (rand >= 10 && rand <= 15 && m_blupi[rank].takeChannel == -1)
      {
        m_blupi[rank].action = ACTION_MISC1; // épaules
        m_blupi[rank].step   = 0;
      }
      if (rand >= 20 && rand <= 23)
      {
        m_blupi[rank].action = ACTION_MISC2; // grat-grat
        m_blupi[rank].step   = 0;
      }
      if (rand == 30 && m_blupi[rank].takeChannel == -1)
      {
        m_blupi[rank].action = ACTION_MISC3; // yoyo
        m_blupi[rank].step   = 0;
      }
      if (rand >= 40 && rand <= 45)
      {
        m_blupi[rank].action = ACTION_MISC4; // ferme les yeux
        m_blupi[rank].step   = 0;
      }
      if (rand == 50 && m_blupi[rank].takeChannel == -1)
      {
        m_blupi[rank].action = ACTION_MISC5; // ohé
        m_blupi[rank].step   = 0;
      }
      if (rand == 60)
      {
        m_blupi[rank].action = ACTION_MISC6; // diabolo
        m_blupi[rank].step   = 0;
      }
    }
    if (
      m_blupi[rank].action == ACTION_STOPTIRED && m_blupi[rank].goalAction == 0)
    {
      rand = Random (0, 100);
      if (
        rand == 10 && // propabilité 1/100
        m_blupi[rank].takeChannel == -1)
      {
        m_blupi[rank].action = ACTION_MISC1f;
        m_blupi[rank].step   = 0;
      }
    }

    if (direct != -1)
      m_blupi[rank].sDirect = direct;

    if (
      m_blupi[rank].action == ACTION_BUILD ||
      m_blupi[rank].action == ACTION_BUILDBREF ||
      m_blupi[rank].action == ACTION_BUILDSEC ||
      m_blupi[rank].action == ACTION_BUILDSOURD ||
      m_blupi[rank].action == ACTION_BUILDPIERRE ||
      m_blupi[rank].action == ACTION_PICKAXE ||
      m_blupi[rank].action == ACTION_PIOCHEPIERRE ||
      m_blupi[rank].action == ACTION_PIOCHESOURD ||
      m_blupi[rank].action == ACTION_ARROSE ||
      m_blupi[rank].action == ACTION_BECHE ||
      m_blupi[rank].action == ACTION_SAW ||
      m_blupi[rank].action == ACTION_CARRY ||
      m_blupi[rank].action == ACTION_DROP ||
      m_blupi[rank].action == ACTION_BURN ||
      m_blupi[rank].action == ACTION_TCHAO ||
      m_blupi[rank].action == ACTION_GRILL1 ||
      m_blupi[rank].action == ACTION_GRILL2 ||
      m_blupi[rank].action == ACTION_GRILL3 ||
      m_blupi[rank].action == ACTION_ELECTRO ||
      m_blupi[rank].action == ACTION_EAT ||
      m_blupi[rank].action == ACTION_DRINK ||
      m_blupi[rank].action == ACTION_BORN ||
      m_blupi[rank].action == ACTION_JUMPJEEP ||
      m_blupi[rank].action == ACTION_JUMP2 ||
      m_blupi[rank].action == ACTION_JUMP3 ||
      m_blupi[rank].action == ACTION_JUMP4 ||
      m_blupi[rank].action == ACTION_JUMP5 ||
      //?          m_blupi[rank].action == ACTION_GLISSE       ||
      m_blupi[rank].action == ACTION_BRIDGE ||
      m_blupi[rank].action == ACTION_MECHE ||
      m_blupi[rank].action == ACTION_S_GRILL ||
      m_blupi[rank].action == ACTION_V_GRILL ||
      m_blupi[rank].action == ACTION_T_CRUSHED ||
      m_blupi[rank].action == ACTION_R_WALK ||
      m_blupi[rank].action == ACTION_R_APLAT ||
      m_blupi[rank].action == ACTION_R_BUILD ||
      m_blupi[rank].action == ACTION_R_LOAD ||
      m_blupi[rank].action == ACTION_B_WALK ||
      m_blupi[rank].action == ACTION_E_WALK ||
      m_blupi[rank].action == ACTION_MARCHEb ||
      m_blupi[rank].action == ACTION_WALKJEEP ||
      m_blupi[rank].action == ACTION_TELEPORTE1 ||
      m_blupi[rank].action == ACTION_TELEPORTE2 ||
      m_blupi[rank].action == ACTION_TELEPORTE3 ||
      m_blupi[rank].action == ACTION_ARMOROPEN ||
      m_blupi[rank].action == ACTION_ARMORCLOSE)
      m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 1) // araignée ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_S_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
    {
      m_blupi[rank].action  = ACTION_S_STOP;
      m_blupi[rank].sDirect = Random (0, 7) * 16;
    }

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 2) // virus ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_V_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_V_STOP;

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 3) // tracks ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_T_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_T_STOP;

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 4) // robot ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_R_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_R_STOP;

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 5) // bombe ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_B_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_B_STOP;

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 7) // électro ?
  {
    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_E_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_E_STOP;

    m_blupi[rank].step = 0;
  }

  if (m_blupi[rank].perso == 8) // disciple ?
  {
    if (direct != -1)
      m_blupi[rank].sDirect = direct;

    if (m_blupi[rank].action == ACTION_WALK)
      m_blupi[rank].action = ACTION_D_WALK;

    if (m_blupi[rank].action == ACTION_STOP)
      m_blupi[rank].action = ACTION_D_STOP;

    if (
      m_blupi[rank].action == ACTION_PICKAXE ||
      m_blupi[rank].action == ACTION_BUILDSEC ||
      m_blupi[rank].action == ACTION_BUILDBREF)
      m_blupi[rank].action = ACTION_D_PICKAXE;

    if (m_blupi[rank].action == ACTION_BUILD)
      m_blupi[rank].action = ACTION_D_BUILD;

    if (
      m_blupi[rank].action == ACTION_SAW ||
      m_blupi[rank].action == ACTION_BUILDSOURD ||
      m_blupi[rank].action == ACTION_PIOCHESOURD)
      m_blupi[rank].action = ACTION_D_SAW;

    if (m_blupi[rank].action == ACTION_TCHAO)
      m_blupi[rank].action = ACTION_D_TCHAO;

    if (m_blupi[rank].action == ACTION_CUEILLE1)
      m_blupi[rank].action = ACTION_D_CUEILLE1;

    if (m_blupi[rank].action == ACTION_CUEILLE2)
      m_blupi[rank].action = ACTION_D_CUEILLE2;

    if (m_blupi[rank].action == ACTION_MECHE)
      m_blupi[rank].action = ACTION_D_MECHE;

    if (m_blupi[rank].action == ACTION_ARROSE)
      m_blupi[rank].action = ACTION_D_ARROSE;

    if (m_blupi[rank].action == ACTION_BECHE)
      m_blupi[rank].action = ACTION_D_BECHE;

    m_blupi[rank].step = 0;
  }
}

// Change l'action de blupi.

void
CDecor::BlupiChangeAction (Sint32 rank, Sint32 action, Sint32 direct)
{
  if (rank < 0)
    return;

  BlupiInitAction (rank, action, direct);
  BlupiDestCel (rank);
  m_blupi[rank].phase = 0;
  m_blupi[rank].pos.x = 0;
  m_blupi[rank].pos.y = 0;
  BlupiNextAction (rank);
}

// Vide la liste des actions.

void
CDecor::ListFlush (Sint32 rank)
{
  Sint32 i;

  for (i = 0; i < MAXLIST; i++)
    m_blupi[rank].listButton[i] = BUTTON_NONE;
  m_blupi[rank].repeatLevelHope = -1;
  m_blupi[rank].repeatLevel     = -1;
}

// Retourne le paramètre associé à une action.

Sint32
CDecor::ListGetParam (Sint32 rank, Buttons button, Point cel)
{
  Sint32 icon;

  if (button == BUTTON_CARRY || button == BUTTON_FLOWER)
    return m_decor[cel.x / 2][cel.y / 2].objectIcon;

  if (button == BUTTON_DEPOSE)
    return m_blupi[rank].takeIcon;

  if (button == BUTTON_GO && cel.x % 2 == 1 && cel.y % 2 == 1)
  {
    icon = m_decor[cel.x / 2][cel.y / 2].objectIcon;
    if (
      icon == 117 || // bateau ?
      icon == 118)   // voiture ?
      return icon;
  }

  return -1;
}

// Ajoute une action dans la liste.

bool
CDecor::ListPut (Sint32 rank, Buttons button, Point cel, Point cMem)
{
  Sint32 i, last;

  if (button == BUTTON_REPEAT || button == BUTTON_GO)
    return true;

  // Mémorise "mange" seulement après un "cultive".
  if (button == BUTTON_EAT && m_blupi[rank].listButton[0] != BUTTON_CULTIVE)
    return true;

  // Si prend/dépose à la suite au même endroit,
  // il est inutile de mémoriser !
  last = m_blupi[rank].listButton[0];
  if (
    (button == BUTTON_CARRY && last == BUTTON_DEPOSE) ||
    (button == BUTTON_DEPOSE && last == BUTTON_CARRY))
  {
    if (
      cel.x / 2 == m_blupi[rank].listCel[0].x / 2 &&
      cel.y / 2 == m_blupi[rank].listCel[0].y / 2)
    {
      ListRemove (rank);
      return true;
    }
  }

  for (i = MAXLIST - 1; i > 0; i--)
  {
    m_blupi[rank].listButton[i] = m_blupi[rank].listButton[i - 1];
    m_blupi[rank].listCel[i]    = m_blupi[rank].listCel[i - 1];
    m_blupi[rank].listParam[i]  = m_blupi[rank].listParam[i - 1];
  }

  m_blupi[rank].listButton[0] = button;
  m_blupi[rank].listCel[0]    = cMem;
  m_blupi[rank].listParam[0]  = ListGetParam (rank, button, cel);

  return true;
}

// Enlève la dernière action ajoutée dans la liste.

void
CDecor::ListRemove (Sint32 rank)
{
  Sint32 i;

  if (m_blupi[rank].listButton[0] == BUTTON_CULTIVE)
    return;

  for (i = 0; i < MAXLIST - 1; i++)
  {
    m_blupi[rank].listButton[i] = m_blupi[rank].listButton[i + 1];
    m_blupi[rank].listCel[i]    = m_blupi[rank].listCel[i + 1];
    m_blupi[rank].listParam[i]  = m_blupi[rank].listParam[i + 1];
  }

  m_blupi[rank].listButton[MAXLIST - 1] = BUTTON_NONE;
}

// Cherche une action à répéter dans la liste.
// Retourne la profondeur de la répétition.
// Retourne -1 si aucune répétiton n'est possible.

Sint32
CDecor::ListSearch (
  Sint32 rank, Buttons button, Point cel, const char *& textForButton)
{
  Sint32 i, j, param, nb;

  static const char * errors[] = {
    /*  0 */ translate ("1: Grow tomatoes\n2: Eat"),
    /*  1 */ translate ("1: Make a bunch\n2: Transform"),
    /*  2 */ translate ("1: Take\n2: Transform"),
    /*  3 */ translate ("1: Extract iron\n2: Make a bomb"),
    /*  4 */ translate ("1: Extract iron\n2: Make a Jeep"),
    /*  5 */ translate ("1: Extract iron\n2: Make an armour"),
    /*  6 */ translate ("1: Cut down a tree \n2: Make a palisade"),
    /*  7 */ translate ("1: Take\n2: Build palisade"),
    /*  8 */ translate ("1: Cut down a tree \n2: Build a bridge"),
    /*  9 */ translate ("1: Take\n2: Build a bridge"),
    /* 10 */ translate ("1: Cut down a tree \n2: Make a boat"),
    /* 11 */ translate ("1: Take\n2: Make a boat"),
  };

  static Sint32 table_series[] = {
    0, // errors
    2,  BUTTON_CULTIVE, BUTTON_EAT,

    1, // errors
    4,  BUTTON_FLOWER,  BUTTON_CARRY,     BUTTON_LABO,   BUTTON_DEPOSE,

    2, // errors
    3,  BUTTON_CARRY,   BUTTON_LABO,      BUTTON_DEPOSE,

    3, // errors
    3,  BUTTON_EXTRAIT, BUTTON_FABMINE,   BUTTON_DEPOSE,

    4, // errors
    3,  BUTTON_EXTRAIT, BUTTON_FABJEEP,   BUTTON_DJEEP,

    5, // errors
    3,  BUTTON_EXTRAIT, BUTTON_MAKEARMOR, BUTTON_DARMOR,

    6, // errors
    4,  BUTTON_ABAT,    BUTTON_CARRY,     BUTTON_DEPOSE, BUTTON_PALIS,

    7, // errors
    3,  BUTTON_CARRY,   BUTTON_DEPOSE,    BUTTON_PALIS,

    8, // errors
    4,  BUTTON_ABAT,    BUTTON_CARRY,     BUTTON_DEPOSE, BUTTON_BRIDGE,

    9, // errors
    3,  BUTTON_CARRY,   BUTTON_DEPOSE,    BUTTON_BRIDGE,

    10, // errors
    4,  BUTTON_ABAT,    BUTTON_CARRY,     BUTTON_DEPOSE, BUTTON_BOAT,

    11, // errors
    3,  BUTTON_CARRY,   BUTTON_DEPOSE,    BUTTON_BOAT,

    -1,
  };

  param = ListGetParam (rank, button, cel);

  i = 0;
  while (table_series[i] != -1)
  {
    nb = table_series[i + 1];
    if (
      button == m_blupi[rank].listButton[nb - 1] &&
      param == m_blupi[rank].listParam[nb - 1] &&
      cel.x >= m_blupi[rank].listCel[nb - 1].x - 50 &&
      cel.x <= m_blupi[rank].listCel[nb - 1].x + 50 &&
      cel.y >= m_blupi[rank].listCel[nb - 1].y - 50 &&
      cel.y <= m_blupi[rank].listCel[nb - 1].y + 50)
    {
      for (j = 0; j < nb; j++)
      {
        if (table_series[i + 2 + j] != m_blupi[rank].listButton[nb - 1 - j])
          goto next;
      }
      textForButton = gettext (errors[table_series[i]]);
      return nb - 1;
    }

  next:
    i += nb + 2;
  }

#if 0
    for (i = 0 ; i < MAXLIST ; i++)
    {
        if (button == m_blupi[rank].listButton[i]   &&
            param  == m_blupi[rank].listParam[i]    &&
            cel.x  >= m_blupi[rank].listCel[i].x - 50 &&
            cel.x  <= m_blupi[rank].listCel[i].x + 50 &&
            cel.y  >= m_blupi[rank].listCel[i].y - 50 &&
            cel.y  <= m_blupi[rank].listCel[i].y + 50)
            return i;
    }
#endif

  return -1;
}

// Ajuste une action à répéter.

bool
CDecor::RepeatAdjust (
  Sint32 rank, Sint32 button, Point & cel, Point & cMem, Sint32 param,
  Sint32 list)
{
  Sint32 i, channel, icon, icon1, icon2, flags;
  Point  test;

  static Sint32 table_object[] = {
    BUTTON_ABAT,
    CHOBJECT,
    6,
    11,
    BUTTON_ROC,
    CHOBJECT,
    37,
    43,
    BUTTON_EAT,
    CHOBJECT,
    60,
    60,
    BUTTON_PALIS,
    CHOBJECT,
    36,
    36,
    BUTTON_BOAT,
    CHOBJECT,
    36,
    36,
    BUTTON_DEPOSE,
    -1,
    -1,
    -1,
    BUTTON_DJEEP,
    -1,
    -1,
    -1,
    BUTTON_DARMOR,
    -1,
    -1,
    -1,
    0,
  };

  static Sint32 table_mur[] = {
    +2, 0,  // 1<<0
    0,  +2, // 1<<1
    -2, 0,  // 1<<2
    0,  -2, // 1<<3
  };

  if (
    button == BUTTON_DEPOSE && // dépose pour une palissade ?
    list > 0 && m_blupi[rank].listButton[list - 1] == BUTTON_PALIS)
  {
    icon = m_decor[cel.x / 2][cel.y / 2].objectIcon;

    flags = 0;
    if (icon == 65)
      flags = (1 << 0) | (1 << 2);
    if (icon == 66)
      flags = (1 << 1) | (1 << 3);
    if (icon == 67)
      flags = (1 << 0) | (1 << 1);
    if (icon == 68)
      flags = (1 << 1) | (1 << 2);
    if (icon == 69)
      flags = (1 << 2) | (1 << 3);
    if (icon == 70)
      flags = (1 << 0) | (1 << 3);
    if (icon == 71)
    {
      for (i = 0; i < 4; i++)
      {
        test.x = cel.x + table_mur[i * 2 + 0];
        test.y = cel.y + table_mur[i * 2 + 1];
        if (
          IsValid (test) &&
          m_decor[test.x / 2][test.y / 2].floorIcon == 15 && // dalle grise ?
          CelOkForAction (test, EV_ACTION_DROP, rank) == 0)
        {
          cel  = test;
          cMem = test;
          goto ok;
        }
      }
      flags = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);
    }

    if (flags == 0)
    {
      if (CelOkForAction (cel, EV_ACTION_DROP, rank) == 0)
        goto ok;
    }

    for (i = 0; i < 4; i++)
    {
      if (flags & (1 << i))
      {
        test.x = cel.x + table_mur[i * 2 + 0];
        test.y = cel.y + table_mur[i * 2 + 1];
        if (CelOkForAction (test, EV_ACTION_DROP, rank) == 0)
        {
          cel  = test;
          cMem = test;
          goto ok;
        }
      }
    }
    return false;
  }

  if (
    button == BUTTON_DEPOSE && // dépose pour un bateau ?
    list > 0 && m_blupi[rank].listButton[list - 1] == BUTTON_BOAT)
  {
    if (SearchOtherBateau (rank, cel, 100, test, icon))
    {
      cel  = test;
      cMem = test;
      goto ok;
    }
    return false;
  }

  //? if ( button == BUTTON_MANGE )
  //? {
  //?     cel = m_blupi[rank].cel;  // cherche là où est blupi !
  //? }

  i       = 0;
  channel = -2;
  icon1   = -1;
  icon2   = -1;
  while (table_object[i] != 0)
  {
    if (button == table_object[i])
    {
      channel = table_object[i + 1];
      icon1   = table_object[i + 2];
      icon2   = table_object[i + 3];
      break;
    }
    i += 4;
  }

  if (button == BUTTON_CARRY || button == BUTTON_FLOWER)
  {
    channel = CHOBJECT;
    icon1   = param;
    icon2   = param;
  }
  if (
    button == BUTTON_GO && // va en bateau/jeep ?
    param != -1)
  {
    channel = CHOBJECT;
    icon1   = param;
    icon2   = param;
  }

  if (channel != -2)
  {
    if (!SearchOtherObject (
          rank, cel, table_actions[button], 50 * 2, channel, icon1, icon2, -1,
          -1, cel, icon1))
      return false;
  }

  if (button == BUTTON_PALIS)
    cMem = cel;

ok:
  if (cel.x % 2 == 0)
    cel.x++;
  if (cel.y % 2 == 0)
    cel.y++; // sur l'objet

  m_blupi[rank].interrupt = 1;
  return true;
}

// Démarre une action.

void
CDecor::GoalStart (Sint32 rank, Sint32 action, Point cel)
{
  m_blupi[rank].goalHili   = cel;
  m_blupi[rank].goalAction = action;
  m_blupi[rank].goalPhase  = 0;
  m_blupi[rank].goalCel.x  = -1;
  m_blupi[rank].bRepeat    = false;

  GoalInitJauge (rank);
  FlushUsed (rank);
}

// Effectue la méta opération suivante.
// Retourne false lorsque c'est fini !

bool
CDecor::GoalNextPhase (Sint32 rank)
{
  Sint16 * pTable;
  Sint32   i, nb;

  if (m_blupi[rank].goalAction == 0)
    return false;

  pTable = GetTableGoal (m_blupi[rank].goalAction);
  if (pTable == nullptr)
  {
    GoalStop (rank, true);
    return false;
  }

  for (i = 0; i < m_blupi[rank].goalPhase; i++)
  {
    if (*pTable == 0)
      return false;
    pTable += 1 + table_goal_nbop[*pTable];
  }

  if (*pTable == GOAL_GROUP)
  {
    m_blupi[rank].goalPhase++;
    nb = pTable[1];
    pTable += 2;

    for (i = 0; i < nb; i++)
    {
      m_blupi[rank].goalPhase++;
      if (!GoalNextOp (rank, pTable))
        return false;
      pTable += 1 + table_goal_nbop[*pTable];
    }
  }

  m_blupi[rank].goalPhase++;
  return GoalNextOp (rank, pTable);
}

// Initialise la jauge pour une méta opération.

void
CDecor::GoalInitJauge (Sint32 rank)
{
  Sint16 * pTable;
  Sint32   max = 0, op;

  m_blupi[rank].jaugePhase = -1;
  m_blupi[rank].jaugeMax   = -1;

  if (m_blupi[rank].perso != 0 && m_blupi[rank].perso != 8)
    return; // araignée/virus/etc. ?
  if (m_blupi[rank].goalAction == 0)
    return;

  pTable = GetTableGoal (m_blupi[rank].goalAction);
  if (pTable == nullptr)
    goto term;

  while (true)
  {
    op = *pTable;
    if (op == 0 || op == GOAL_TERM)
      goto term;

    if (op == GOAL_ACTION && pTable[1] != ACTION_STOP)
      max++;

    pTable += 1 + table_goal_nbop[*pTable];
  }

term:
  if (max > 0)
  {
    m_blupi[rank].jaugePhase = 0;
    m_blupi[rank].jaugeMax   = max;
  }
}

// Permet de passer à travers certains arbres.

void
CDecor::GoalInitPassCel (Sint32 rank)
{
  Point  cel;
  Sint32 channel, icon;

  cel.x = (m_blupi[rank].goalCel.x / 2) * 2;
  cel.y = (m_blupi[rank].goalCel.y / 2) * 2;

  GetObject (cel, channel, icon);

  if (
    channel == CHOBJECT &&
    ((icon >= 8 && icon <= 11) ||  // arbres touffus ?
     (icon >= 30 && icon <= 35) || // arbres touffus sans feuilles ?
     (icon >= 37 && icon <= 43) || // rochers ?
     (icon == 81 || icon == 83 || icon == 94) || // fleurs ?
     (icon >= 100 && icon <= 105) ||             // usine ?
     (icon == 115 || icon == 116) ||             // usine ?
     (icon == 17 || icon == 18) ||               // usine ?
     (icon == 117) ||                            // bateau ?
     (icon == 118) ||                            // jeep ?
     (icon == 16)))                              // armure ?
    m_blupi[rank].passCel = m_blupi[rank].goalCel;
  else
    m_blupi[rank].passCel.x = -1;
}

// Ajuste une coordonnée de cellule.

void
CDecor::GoalAdjustCel (Sint32 rank, Sint32 & x, Sint32 & y)
{
  if (x == -10 && y == -10)
  {
    if (m_blupi[rank].goalAction == EV_ACTION_BRIDGEEL)
    {
      x = m_blupi[rank].fix.x + m_blupi[rank].cLoop * 2;
      y = m_blupi[rank].fix.y;
      return;
    }
    if (m_blupi[rank].goalAction == EV_ACTION_BRIDGEOL)
    {
      x = m_blupi[rank].fix.x - m_blupi[rank].cLoop * 2;
      y = m_blupi[rank].fix.y;
      return;
    }
    if (m_blupi[rank].goalAction == EV_ACTION_BRIDGESL)
    {
      x = m_blupi[rank].fix.x;
      y = m_blupi[rank].fix.y + m_blupi[rank].cLoop * 2;
      return;
    }
    if (m_blupi[rank].goalAction == EV_ACTION_BRIDGENL)
    {
      x = m_blupi[rank].fix.x;
      y = m_blupi[rank].fix.y - m_blupi[rank].cLoop * 2;
      return;
    }
  }

  x += m_blupi[rank].cel.x;
  y += m_blupi[rank].cel.y;
}

// Liste des buts multiples.

Sint32 table_multi_goal[16 * 2] = {
  0,  0,  +1, 0, 0,  +1, +1, +1, 0,  -1, +1, -1, 0,  +2, +1, +2,
  -1, -1, -1, 0, -1, +1, -1, +2, +2, -1, +2, 0,  +2, +1, +2, +2,
};

// Effectue une méta opération.

bool
CDecor::GoalNextOp (Sint32 rank, Sint16 * pTable)
{
  Sint32  op, x, y;
  Sint32  action, direct, channel, icon, mchannel, micon;
  Sint32  total, step, delai, first, last, first2, last2, flag, i;
  Sint32  param;
  Buttons button;
  Point   pos, cel, cMem, destCel;
  bool    bOK, bError = true;

  pos = ConvCelToPos (m_blupi[rank].cel);

  op = *pTable++;

  if (op == GOAL_ACTION && *pTable != ACTION_STOP)
    m_blupi[rank].jaugePhase++;

  if (op == GOAL_GOHILI)
  {
    m_blupi[rank].goalCel.x = m_blupi[rank].goalHili.x + (*pTable++);
    m_blupi[rank].goalCel.y = m_blupi[rank].goalHili.y + (*pTable++);
    flag                    = *pTable++;
    //?     m_blupi[rank].passCel.x = -1;
    FlushUsed (rank);
    return true;
  }

  if (op == GOAL_GOHILI2)
  {
    cel.x = (m_blupi[rank].goalHili.x / 2) * 2 + (*pTable++);
    cel.y = (m_blupi[rank].goalHili.y / 2) * 2 + (*pTable++);
    flag  = *pTable++;
    if (!!flag)
    {
      m_blupi[rank].goalCel = cel;
      GoalInitPassCel (rank);
    }
    m_blupi[rank].goalCel = cel;
    FlushUsed (rank);
    return true;
  }

  if (op == GOAL_GOBLUPI)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    flag  = *pTable++;
    if (!!flag)
    {
      if (IsBlupiHereEx (cel, rank, false)) // destination occupée ?
      {
        m_blupi[rank].goalPhase--; // on attend ...
        return true;
      }
      m_blupi[rank].goalCel = cel;
      GoalInitPassCel (rank);
    }
    m_blupi[rank].goalCel = cel;
    FlushUsed (rank);
    return true;
  }

  if (op == GOAL_TESTOBJECT)
  {
    cel.x   = m_blupi[rank].cel.x + (*pTable++);
    cel.y   = m_blupi[rank].cel.y + (*pTable++);
    channel = *pTable++;
    icon    = *pTable++;
    GetObject (cel, mchannel, micon);
    if (channel != mchannel || icon != micon)
      goto error;
    return true;
  }

  if (op == GOAL_PUTFLOOR)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    channel = *pTable++;
    icon    = *pTable++;
    if (icon == -2)
      icon = m_blupi[rank].vIcon;
    PutFloor (GetCel (x, y), channel, icon);
    return true;
  }

  if (op == GOAL_PUTOBJECT)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    channel = *pTable++;
    icon    = *pTable++;
    if (channel == -3 && icon == -3) // l'objet transporté ?
    {
      channel = m_blupi[rank].takeChannel;
      icon    = m_blupi[rank].takeIcon;
    }
    PutObject (GetCel (x, y), channel, icon);
    return true;
  }

  if (op == GOAL_BUILDFLOOR)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    cel.x = (x / 2) * 2;
    cel.y = (y / 2) * 2;
    GetFloor (cel, channel, i);
    channel  = *pTable++;
    icon     = *pTable++;
    mchannel = *pTable++;
    micon    = *pTable++;
    total    = *pTable++;
    delai    = *pTable++;
    step     = *pTable++;

    if (
      i >= 19 && i <= 32 && // herbe foncée ?
      icon == 57)
      icon = 58; // sol tomate foncé
    if (i == 58 && icon == 1)
      icon = 20; // remet herbe foncée

    if (!MoveCreate (
          cel, rank, true, channel, icon, mchannel, micon, total, delai, step))
      goto error;
    return true;
  }

  if (op == GOAL_BUILDOBJECT)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    cel.x    = (x / 2) * 2;
    cel.y    = (y / 2) * 2;
    channel  = *pTable++;
    icon     = *pTable++;
    mchannel = *pTable++;
    micon    = *pTable++;
    total    = *pTable++;
    delai    = *pTable++;
    step     = *pTable++;
    if (channel == -2 && icon == -2) // l'objet actuel ?
      GetObject (cel, channel, icon);
    ArrangeBuild (cel, channel, icon); // arrange les murs autour
    if (!MoveCreate (
          cel, rank, false, channel, icon, mchannel, micon, total, delai, step))
      goto error;
    return true;
  }

  if (op == GOAL_FINISHMOVE)
  {
    MoveFinish (rank);
    return true;
  }

  if (op == GOAL_ARRANGEOBJECT)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    MoveFinish (GetCel (x, y));
    ArrangeObject (GetCel (x, y));
    return true;
  }

  if (op == GOAL_EXPLOSE1)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    cel = GetCel (x, y);

    BlupiKill (rank, cel, 0);
    MoveFinish (cel);

    // Faut-il démarrer une explosion en chaîne.
    GetObject (cel, channel, icon);
    if (channel == CHOBJECT && icon == 85) // dynamite ?
    {
      rank = BlupiCreate (
        GetCel (cel, 1, 1), ACTION_STOP, DIRECT_E, 6,
        MAXENERGY); // crée un détonnateur
      if (rank >= 0)
      {
        GoalStart (rank, EV_ACTION_T_DYNAMITE, cel);
        m_blupi[rank].bCache = true;
      }
    }
    else
    {
      PutObject (cel, -1, -1); // supprime l'objet
      ArrangeObject (cel);
    }
    return true;
  }

  if (op == GOAL_EXPLOSE2)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    cel = GetCel ((x / 2) * 2, (y / 2) * 2);

    GetObject (cel, channel, icon);
    if (channel != CHOBJECT || icon != 85) // dynamite ?
    {
      channel = CHOBJECT;
      icon    = -1;
      ArrangeBuild (cel, channel, icon); // arrange les murs autour
      if (!MoveCreate (cel, rank, false, CHOBJECT, -1, -1, -1, 10, 1, -1 * 100))
        goto error;
      MoveAddIcons (cel, 6); // explosion
    }
    return true;
  }

  if (op == GOAL_ADDMOVES)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    icon = *pTable++;
    MoveAddMoves (GetCel (x, y), icon);
    return true;
  }

  if (op == GOAL_ADDICONS)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    icon = *pTable++;
    MoveAddIcons (GetCel (x, y), icon);
    return true;
  }

  if (op == GOAL_ACTION)
  {
    action = *pTable++;
    direct = *pTable++;
    BlupiInitAction (rank, action, direct);
    return true;
  }

  if (op == GOAL_ELECTRO)
  {
    x = *pTable++;
    y = *pTable++;
    GoalAdjustCel (rank, x, y);
    cel  = GetCel ((x / 2) * 2, (y / 2) * 2);
    icon = *pTable++;
    if (MoveCreate (
          cel, rank, true, CHFLOOR, -1, -1, -1, 100, 1, 100, false, true))
      MoveAddIcons (cel, icon);
    BlupiKill (rank, cel, 1);
    return true;
  }

  if (op == GOAL_MALADE)
  {
    m_blupi[rank].bMalade = *pTable++;
    return true;
  }

  if (op == GOAL_WORK)
  {
    cel.x                                   = m_blupi[rank].cel.x + (*pTable++);
    cel.y                                   = m_blupi[rank].cel.y + (*pTable++);
    m_decor[cel.x / 2][cel.y / 2].workBlupi = rank;
    return true;
  }

  if (op == GOAL_INTERRUPT)
  {
    m_blupi[rank].interrupt = *pTable++; // change le niveau
    return true;
  }

  if (op == GOAL_ENERGY)
  {
    if (m_blupi[rank].energy <= *pTable++)
      goto error;
    return true;
  }

  if (op == GOAL_ISNOMALADE)
  {
    if (m_blupi[rank].bMalade)
      goto error;
    return true;
  }

  if (op == GOAL_TAKE)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    MoveFinish (rank);
    m_blupi[rank].takeChannel = m_decor[cel.x / 2][cel.y / 2].objectChannel;
    m_blupi[rank].takeIcon    = m_decor[cel.x / 2][cel.y / 2].objectIcon;
    m_decor[cel.x / 2][cel.y / 2].objectChannel = -1;
    m_decor[cel.x / 2][cel.y / 2].objectIcon    = -1;
    return true;
  }

  if (op == GOAL_TAKEOBJECT)
  {
    cel.x                     = m_blupi[rank].cel.x + (*pTable++);
    cel.y                     = m_blupi[rank].cel.y + (*pTable++);
    channel                   = *pTable++;
    icon                      = *pTable++;
    m_blupi[rank].takeChannel = channel;
    m_blupi[rank].takeIcon    = icon;
    return true;
  }

  if (op == GOAL_LABO)
  {
    m_blupi[rank].takeChannel = CHOBJECT;
    if (m_blupi[rank].takeIcon == 82) // fleurs normales ?
    {
      m_blupi[rank].takeIcon = 80; // bouteille
    }
    if (m_blupi[rank].takeIcon == 84) // fleurs foncées ?
    {
      m_blupi[rank].takeIcon = 85; // dynamite
    }
    if (m_blupi[rank].takeIcon == 95) // fleurs vertes ?
    {
      m_blupi[rank].takeIcon = 93; // piège
    }
    if (m_blupi[rank].takeIcon == 60) // tomates ?
    {
      m_blupi[rank].takeIcon = 92; // poison
    }
    return true;
  }

  if (op == GOAL_CACHE)
  {
    m_blupi[rank].bCache = *pTable++;
    if (
      *pTable++ &&                                               // bDynamite ?
      m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 3 && // armure ?
      !m_bInvincible)
      m_blupi[rank].bCache = false;
    return true;
  }

  if (op == GOAL_DELETE)
  {
    if (
      m_blupi[rank].perso == 0 &&    // blupi ?
      m_blupi[rank].vehicule == 3 && // armure ?
      !m_bInvincible)
      return true;
    BlupiDelete (rank); // snif ...
    return true;
  }

  if (op == GOAL_DEPOSE)
  {
    m_blupi[rank].takeChannel = -1;
    return true;
  }

  if (op == GOAL_NEWBLUPI)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    PutObject (cel, -1, -1); // enlève les oeufs

    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY / 4);
    if (rank >= 0)
    {
      m_blupi[rank].energy = MAXENERGY / 4;
      BlupiInitAction (rank, ACTION_BORN);
    }

    cel.x++;
    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY / 4);
    if (rank >= 0)
    {
      m_blupi[rank].energy = MAXENERGY / 4;
      BlupiInitAction (rank, ACTION_BORN);
    }

    cel.x--;
    cel.y++;
    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY / 4);
    if (rank >= 0)
    {
      m_blupi[rank].energy = MAXENERGY / 4;
      BlupiInitAction (rank, ACTION_BORN);
    }

    cel.x++;
    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, 0, MAXENERGY / 4);
    if (rank >= 0)
    {
      m_blupi[rank].energy = MAXENERGY / 4;
      BlupiInitAction (rank, ACTION_BORN);
    }
    return true;
  }

  if (op == GOAL_NEWPERSO)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    icon  = *pTable++;

    destCel = cel;
    destCel.x++;
    if (IsBlupiHereEx (destCel, rank, false)) // destination occupée ?
    {
      m_blupi[rank].goalPhase--; // on attend ...
      return true;
    }
    destCel.x++;
    if (IsBlupiHereEx (destCel, rank, false)) // destination occupée ?
    {
      destCel.y--;
      if (
        icon == 5 ||                          // bombe ?
        IsBlupiHereEx (destCel, rank, false)) // destination occupée ?
      {
        m_blupi[rank].goalPhase--; // on attend ...
        return true;
      }
    }

    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, icon, MAXENERGY);
    if (rank >= 0)
      m_blupi[rank].goalCel = destCel;
    return true;
  }

  if (op == GOAL_USINEBUILD)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    if (!IsUsineBuild (rank, cel))
      goto error;
    return true;
  }

  if (op == GOAL_USINEFREE)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    if (!IsUsineFree (rank, cel))
    {
      GoalStop (rank, true);
      m_blupi[rank].goalCel = GetCel (cel, 1, -1); // à côté de la porte
      //?         m_blupi[rank].goalAction = 0;  // stoppe sitôt après
      //?         m_blupi[rank].interrupt = 1;
      //?         GoalUnwork(rank);
      //?         FlushUsed(rank);
    }
    return true;
  }

  if (op == GOAL_AMORCE)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    if (IsBlupiHereEx (cel, rank, false))
      goto error;
    // Crée un détonnateur de mine (blupi invisible).
    rank = BlupiCreate (cel, ACTION_STOP, DIRECT_E, 6, MAXENERGY);
    if (rank >= 0)
    {
      m_blupi[rank].bCache     = true; // invisible
      m_blupi[rank].goalAction = EV_ACTION_MINE2;
      m_blupi[rank].goalPhase  = 0;
      m_blupi[rank].goalCel.x  = -1;
      m_blupi[rank].interrupt  = 1;
    }
    return true;
  }

  if (op == GOAL_VEHICULE)
  {
    m_blupi[rank].vehicule = *pTable++;
    if (
      m_blupi[rank].vehicule != 0 &&         // pas à pied ?
      m_blupi[rank].takeChannel != -1 &&     // porte qq chose ?
      m_blupi[rank].energy <= MAXENERGY / 4) // faible ?
      m_blupi[rank].energy = MAXENERGY / 4 + 1;
    return true;
  }

  if (op == GOAL_ACTUALISE)
  {
    BlupiActualise (rank);
    return true;
  }

  if (op == GOAL_SOUND)
  {
    icon = *pTable++;
    BlupiSound (rank, static_cast<Sounds> (icon), pos);
    return true;
  }

  if (op == GOAL_REPEAT)
  {
    icon                  = *pTable++;
    m_blupi[rank].bRepeat = icon;
    return true;
  }

  if (op == GOAL_OTHER)
  {
    if (!m_blupi[rank].bRepeat)
      goto term;

    // Bouton stop pressé ?
    if (m_blupi[rank].stop == 1)
      goto term;

    channel = *pTable++;
    first   = *pTable++;
    last    = *pTable++;
    first2  = *pTable++;
    last2   = *pTable++;
    action  = *pTable++;
    if (!SearchOtherObject (
          rank, m_blupi[rank].cel, action, 100, channel, first, last, first2,
          last2, m_blupi[rank].goalHili, icon))
      goto term;
    if (action == EV_ACTION_ABAT1 || action == EV_ACTION_ROC1)
    {
      action += icon - first; // EV_ACTION_ABAT1..6
    }
    m_blupi[rank].goalAction = action;
    m_blupi[rank].goalPhase  = 0;
    m_blupi[rank].goalCel.x  = -1;
    m_blupi[rank].interrupt  = 1;
    GoalInitJauge (rank);
    GoalUnwork (rank);
    FlushUsed (rank);
    return true;
  }

  if (op == GOAL_OTHERFIX)
  {
    if (!m_blupi[rank].bRepeat)
      goto term;

    // Bouton stop pressé ?
    if (m_blupi[rank].stop == 1)
      goto term;

    channel = *pTable++;
    first   = *pTable++;
    last    = *pTable++;
    first2  = *pTable++;
    last2   = *pTable++;
    action  = *pTable++;
    if (!SearchOtherObject (
          rank, m_blupi[rank].fix, action, 100, channel, first, last, first2,
          last2, m_blupi[rank].goalHili, icon))
      goto term;
    if (action == EV_ACTION_ABAT1 || action == EV_ACTION_ROC1)
    {
      action += icon - first; // EV_ACTION_ABAT1..6
    }
    m_blupi[rank].goalAction = action;
    m_blupi[rank].goalPhase  = 0;
    m_blupi[rank].goalCel.x  = -1;
    m_blupi[rank].interrupt  = 1;
    GoalInitJauge (rank);
    GoalUnwork (rank);
    FlushUsed (rank);
    return true;
  }

  if (op == GOAL_OTHERLOOP)
  {
    action = *pTable++;
    if (m_blupi[rank].cLoop < m_blupi[rank].nLoop)
    {
      m_blupi[rank].goalAction = action;
      m_blupi[rank].goalPhase  = 0;
      m_blupi[rank].goalCel.x  = -1;
      m_blupi[rank].interrupt  = 1;
      GoalInitJauge (rank);
      GoalUnwork (rank);
      FlushUsed (rank);
    }
    return true;
  }

  if (op == GOAL_NEXTLOOP)
  {
    m_blupi[rank].cLoop++;
    return true;
  }

  if (op == GOAL_FIX)
  {
    m_blupi[rank].fix.x = m_blupi[rank].cel.x + (*pTable++);
    m_blupi[rank].fix.y = m_blupi[rank].cel.y + (*pTable++);
    return true;
  }

  if (op == GOAL_FLOORJUMP)
  {
    channel = *pTable++;
    icon    = *pTable++;
    action  = *pTable++;
    GetFloor (m_blupi[rank].cel, mchannel, micon);
    if (channel == mchannel && icon == micon)
    {
      m_blupi[rank].goalAction = action;
      m_blupi[rank].goalPhase  = 0;
      m_blupi[rank].goalCel.x  = -1;
      m_blupi[rank].interrupt  = 1;
      GoalInitJauge (rank);
      GoalUnwork (rank);
      FlushUsed (rank);
    }
    return true;
  }

  if (op == GOAL_ADDDRAPEAU)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    AddDrapeau (cel); // cellule sondée
    return true;
  }

  if (op == GOAL_TELEPORTE)
  {
    pos.x = *pTable++;
    pos.y = *pTable++;

    cMem = m_blupi[rank].cel;
    GetFloor (cMem, channel, icon);
    PutFloor (cMem, -1, -1);
    bOK = SearchOtherObject (
      rank, m_blupi[rank].cel, EV_ACTION_GO, 1000, CHFLOOR, 80, 80, -1, -1, cel,
      i);
    PutFloor (cMem, channel, icon);
    if (!bOK)
      goto error;

    cel.x += pos.x;
    cel.y += pos.y;
    if (IsBlupiHereEx (cel, rank, false) || !IsFreeCel (cel, rank))
      goto error;
    m_blupi[rank].cel = cel;
    BlupiPushFog (rank);
    if (m_blupi[rank].bHili)
      SetCorner (cel, true);
    return true;
  }

  if (op == GOAL_IFTERM)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    if (!IsFreeCel (cel, rank) || IsBlupiHereEx (cel, rank, false))
      goto term;
    return true;
  }

  if (op == GOAL_IFDEBARQUE)
  {
    cel.x                  = m_blupi[rank].cel.x + (*pTable++);
    cel.y                  = m_blupi[rank].cel.y + (*pTable++);
    m_blupi[rank].vehicule = 0; // à pied
    bOK = IsFreeCel (cel, rank) && !IsBlupiHereEx (cel, rank, false);
    m_blupi[rank].vehicule = 1; // en bateau
    if (!bOK)
      goto term;
    return true;
  }

  if (op == GOAL_SKIPSKILL)
  {
    i     = (*pTable++);
    total = (*pTable++);
    if (m_skill == i)
    {
      m_blupi[rank].goalPhase += total; // saute qq instructions
    }
    return true;
  }

  if (op == GOAL_TERM)
  {
  term:
    bError = false;
  }

  if (op == GOAL_WAITFREE)
  {
    cel.x = m_blupi[rank].cel.x + (*pTable++);
    cel.y = m_blupi[rank].cel.y + (*pTable++);
    if (IsBlupiHereEx (cel, rank, false)) // destination occupée ?
    {
      m_blupi[rank].goalPhase--; // on attend ...

      rank = m_blupiHere;
      if (
        m_blupi[rank].goalAction != EV_ACTION_GO &&
        m_blupi[rank].goalAction != EV_ACTION_ELECTRO &&
        m_blupi[rank].goalAction != EV_ACTION_ELECTROm)
      {
        destCel.x = cel.x;
        destCel.y = cel.y - 1;
        if (!IsBlupiHereEx (destCel, rank, false))
        {
          GoalStart (rank, EV_ACTION_GO, destCel);
          return true;
        }

        destCel.x = cel.x + 1;
        destCel.y = cel.y;
        if (!IsBlupiHereEx (destCel, rank, false))
        {
          GoalStart (rank, EV_ACTION_GO, destCel);
          return true;
        }

        destCel.x = cel.x;
        destCel.y = cel.y + 1;
        if (!IsBlupiHereEx (destCel, rank, false))
        {
          GoalStart (rank, EV_ACTION_GO, destCel);
          return true;
        }

        destCel.x = cel.x + 1;
        destCel.y = cel.y - 1;
        if (!IsBlupiHereEx (destCel, rank, false))
        {
          GoalStart (rank, EV_ACTION_GO, destCel);
          return true;
        }

        destCel.x = cel.x + 1;
        destCel.y = cel.y + 1;
        if (!IsBlupiHereEx (destCel, rank, false))
        {
          GoalStart (rank, EV_ACTION_GO, destCel);
          return true;
        }

        if (m_blupi[rank].perso == 0)
        {
          if (m_blupi[rank].bMalade)
            action = EV_ACTION_ELECTROm;
          else
            action = EV_ACTION_ELECTRO;
          GoalStart (rank, action, m_blupi[rank].cel);
          return true;
        }
      }
    }
    return true;
  }

error:
  i = m_blupi[rank].repeatLevel;
  GoalStop (rank, bError, i == -1);
  if (i != -1) // répétition en cours ?
  {
    button = static_cast<Buttons> (m_blupi[rank].listButton[i]);
    cMem   = m_blupi[rank].listCel[i];
    param  = m_blupi[rank].listParam[i];
    cel    = cMem;
    if (RepeatAdjust (rank, button, cel, cMem, param, i))
    {
      if (IsBlupiHereEx (cel, rank, false)) // destination occupée ?
      {
        m_blupi[rank].repeatLevel = i;                     // on continue ...
        GoalStart (rank, EV_ACTION_GO, m_blupi[rank].cel); // on attend ...
        return true;
      }
      if (BlupiGoal (rank, button, cel, cMem))
      {
        m_blupi[rank].repeatLevel = i; // on continue ...
        return true;
      }
    }
  }
  return false;
}

// Supprime le blocage de la cellule dans laquelle
// blupi travaille.

void
CDecor::GoalUnwork (Sint32 rank)
{
  Sint32 x, y;

  for (x = 0; x < MAXCELX / 2; x++)
  {
    for (y = 0; y < MAXCELY / 2; y++)
    {
      if (m_decor[x][y].workBlupi == rank)
      {
        m_decor[x][y].workBlupi = -1; // débloque
      }
    }
  }
}

// Stoppe complètement une action.

void
CDecor::GoalStop (Sint32 rank, bool bError, bool bSound)
{
  Point pos;

  static Sounds table_sound_term[] = {
    SOUND_TERM1, SOUND_TERM2, SOUND_TERM3,
    SOUND_TERM4, SOUND_TERM5, SOUND_TERM6,
  };

  static Sounds table_sound_boing[] = {
    SOUND_BOING1,
    SOUND_BOING2,
    SOUND_BOING3,
  };

  if (bError && bSound)
  {
    ListRemove (rank); // supprime la dernière action mémorisée
  }

  m_blupi[rank].goalAction  = 0;
  m_blupi[rank].goalPhase   = 0;
  m_blupi[rank].goalCel.x   = -1;
  m_blupi[rank].jaugePhase  = -1;
  m_blupi[rank].jaugeMax    = -1;
  m_blupi[rank].interrupt   = 1; // remet le niveau normal
  m_blupi[rank].busyCount   = 0;
  m_blupi[rank].busyDelay   = 0;
  m_blupi[rank].repeatLevel = -1; // stoppe la répétition
  FlushUsed (rank);
  MoveFinish (rank);
  GoalUnwork (rank);

  // En cas d'erreur, il faut accepter de traverser la
  // construction avortée (par exemple, le massif d'arbres
  // dans lequel blupi se trouve, mais qui n'a pas pu
  // être abattu).
  if (!bError)
    m_blupi[rank].passCel.x = -1;

  m_blupi[rank].stop = 0; // relâche bouton stop

  if (
    bSound && (m_blupi[rank].perso == 0 || // blupi ?
               m_blupi[rank].perso == 8))  // assistant ?
  {
    if (bError)
    {
      pos.x = LXIMAGE () / 2;
      pos.y = LYIMAGE () / 2;
      BlupiSound (
        rank, table_sound_boing[Random (0, countof (table_sound_boing) - 1)],
        pos, true);
    }
    else
    {
      pos = ConvCelToPos (m_blupi[rank].cel);
      BlupiSound (
        rank, table_sound_term[Random (0, countof (table_sound_term) - 1)], pos,
        true);
    }
  }
}

// Teste si une cellule est déjà utilisée comme but pour
// n'importe quel blupi.

bool
CDecor::BlupiIsGoalUsed (Point cel)
{
  Sint32 rank;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist && m_blupi[rank].goalCel.x / 2 == cel.x / 2 &&
      m_blupi[rank].goalCel.y / 2 == cel.y / 2)
      return true;
  }

  return false;
}

// Démarre ou stoppe un rayon entre deux tours.

void
CDecor::BlupiStartStopRayon (Sint32 rank, Point startCel, Point endCel)
{
  Sint32 i, icon, icon2;
  Point  cel, cel2, vector, pos;

  if (
    m_blupi[rank].perso == 1 || // araignée ?
    m_blupi[rank].perso == 2)   // virus ?
    return;

  // Stoppe un rayon.
  cel.x = (endCel.x / 2) * 2;
  cel.y = (endCel.y / 2) * 2;
  icon  = m_decor[cel.x / 2][cel.y / 2].objectIcon;
  if ((icon == 10000 || icon == 10001) && MoveIsUsed (cel))
  {
    if (MoveIsUsed (cel))
    {
      MoveFinish (cel);

      pos = ConvCelToPos (cel);
      BlupiSound (rank, SOUND_RAYON2, pos);
    }

    for (i = 0; i < 4; i++)
    {
      vector = GetVector (i * 2 * 16);
      cel.x  = (endCel.x / 2) * 2 + vector.x * 2;
      cel.y  = (endCel.y / 2) * 2 + vector.y * 2;
      icon   = m_decor[cel.x / 2][cel.y / 2].objectIcon;
      if (icon == 10000 || icon == 10001)
        MoveFinish (cel);
    }
  }

  // Démarre un rayon.
  cel.x  = (startCel.x / 2) * 2;
  cel.y  = (startCel.y / 2) * 2;
  cel2.x = (endCel.x / 2) * 2;
  cel2.y = (endCel.y / 2) * 2;
  icon   = m_decor[cel.x / 2][cel.y / 2].objectIcon;
  icon2  = m_decor[cel2.x / 2][cel2.y / 2].objectIcon;
  if ((icon == 10000 || icon == 10001) && icon2 != 10000 && icon2 != 10001)
  {
    if (MoveCreate (cel, -1, false, CHOBJECT, -1, -1, -1, 9999, 1, 0, true))
    {
      MoveAddIcons (cel, icon == 10000 ? 4 : 5, true); // éclairs
    }

    pos = ConvCelToPos (cel);
    BlupiSound (rank, SOUND_RAYON1, pos);

    for (i = 0; i < 4; i++)
    {
      vector = GetVector (i * 2 * 16);
      cel.x  = (startCel.x / 2) * 2 + vector.x * 2;
      cel.y  = (startCel.y / 2) * 2 + vector.y * 2;
      icon   = m_decor[cel.x / 2][cel.y / 2].objectIcon;
      if (icon == 10000 || icon == 10001)
      {
        if (MoveCreate (cel, -1, false, CHOBJECT, -1, -1, -1, 9999, 1, 0, true))
        {
          MoveAddIcons (cel, icon == 10000 ? 4 : 5, true); // éclairs
        }
      }
    }
  }
}

// Tourne un blupi, si nécessaire.
// Retourne false si ce n'est pas nécessaire.

bool
CDecor::BlupiRotate (Sint32 rank)
{
  Sint32 aDirect, sDirect, ip, in, sens = 0;
  bool   bOK;
  Point  pos;

  aDirect = m_blupi[rank].aDirect;
  sDirect = m_blupi[rank].sDirect;

  if (aDirect == sDirect)
    return false;

  if (sDirect > aDirect)
    ip = sDirect + 0 * 16 - aDirect;
  else
    ip = sDirect + 8 * 16 - aDirect;

  if (aDirect > sDirect)
    in = aDirect + 0 * 16 - sDirect;
  else
    in = aDirect + 8 * 16 - sDirect;

  if (ip == 0 || in == 0)
  {
    m_blupi[rank].aDirect = m_blupi[rank].sDirect;
    return false;
  }

  if (
    m_blupi[rank].perso == 0 &&  // blupi ?
    m_blupi[rank].vehicule == 1) // en bateau ?
  {
    if (ip == in)
      sens = Random (0, 1) ? 8 : 8 * 16 - 8;
    if (ip < in)
      sens = 8;
    if (ip > in)
      sens = 8 * 16 - 8;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else if (
    m_blupi[rank].perso == 0 &&  // blupi ?
    m_blupi[rank].vehicule == 2) // en jeep ?
  {
    if (ip == in)
      sens = Random (0, 1) ? 8 : 8 * 16 - 8;
    if (ip < in)
      sens = 8;
    if (ip > in)
      sens = 8 * 16 - 8;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else if (
    m_blupi[rank].perso == 0 &&  // blupi ?
    m_blupi[rank].vehicule == 3) // armure ?
  {
    if (ip == in)
      sens = Random (0, 1) ? 4 : 8 * 16 - 4;
    if (ip < in)
      sens = 4;
    if (ip > in)
      sens = 8 * 16 - 4;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else if (m_blupi[rank].perso == 3) // tracks ?
  {
    if (ip == in)
      sens = Random (0, 1) ? 4 : 8 * 16 - 4;
    if (ip < in)
      sens = 4;
    if (ip > in)
      sens = 8 * 16 - 4;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else if (m_blupi[rank].perso == 4) // robot ?
  {
    pos = ConvCelToPos (m_blupi[rank].cel);
    BlupiSound (rank, SOUND_R_ROTATE, pos);

    if (ip == in)
      sens = Random (0, 1) ? 2 : 8 * 16 - 2;
    if (ip < in)
      sens = 2;
    if (ip > in)
      sens = 8 * 16 - 2;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else if (m_blupi[rank].perso == 8) // disciple ?
  {
    if (ip == in)
      sens = Random (0, 1) ? 8 : 8 * 16 - 8;
    if (ip < in)
      sens = 8;
    if (ip > in)
      sens = 8 * 16 - 8;
    aDirect = (aDirect + sens) % (8 * 16);
  }
  else
  {
    if (ip == in)
      sens = Random (0, 1) ? 1 : 7;
    if (ip < in)
      sens = 1;
    if (ip > in)
      sens = 7;
    aDirect = ((aDirect / 16 + sens) % 8) * 16;
  }

  m_blupi[rank].lastIcon = m_blupi[rank].icon;
  bOK                    = Rotate (m_blupi[rank].icon, aDirect);
  if (bOK)
  {
    m_blupi[rank].aDirect = aDirect;
    return true;
  }
  else
  {
    m_blupi[rank].aDirect = m_blupi[rank].sDirect;
    return false;
  }
}

// Avance un blupi existant.

bool
CDecor::BlupiNextAction (Sint32 rank)
{
  bool   bOK;
  Point  pos, iCel;
  Sint32 a, min;
  Sounds sound;

  if (!m_blupi[rank].bExist)
    return false;

  if (!g_restoreBugs)
  {
    /* Check if a Blupi is already doing a conflicting action at the same place.
     * It happens for example when a blupi wants to carry an object from a
     * direction and a second from the other direction. Without this check, the
     * object is duplicated.
     */
    if ((m_blupi[rank].goalAction == EV_ACTION_CARRY ||
         m_blupi[rank].goalAction == EV_ACTION_EAT ||
         m_blupi[rank].goalAction == EV_ACTION_DRINK))
      for (int i = 0; i < MAXBLUPI; ++i)
      {
        if (rank == i || !m_blupi[i].bExist)
          continue;

        if (
          (m_blupi[i].goalAction == EV_ACTION_CARRY2 ||
           m_blupi[i].goalAction == EV_ACTION_EAT2 ||
           m_blupi[i].goalAction == EV_ACTION_DRINK2) &&
          m_blupi[rank].goalHili.x == m_blupi[i].goalHili.x &&
          m_blupi[rank].goalHili.y == m_blupi[i].goalHili.y)
        {
          BlupiInitAction (i, ACTION_STOP);
          GoalStop (i, true);
          return false;
        }

        /* Prevent time bomb duplication */
        if (m_blupi[i].goalAction == EV_ACTION_MINE2)
        {
          BlupiInitAction (rank, ACTION_STOP);
          GoalStop (rank, true);
          return false;
        }
      }

    /* Prevent Blupi to take a trap when an enemy is already captured. */
    if (m_blupi[rank].perso == 0 && m_blupi[rank].action == ACTION_CARRY)
    {
      Sint32 ch, icon;

      auto exists = this->GetObject (m_blupi[rank].goalHili, ch, icon);
      if (exists && ch == CHOBJECT)
      {
        switch (icon)
        {
        case 96:  // spider in trap
        case 97:  // track in trap
        case 98:  // robot in trap
        case 114: // bomb in trap
          BlupiInitAction (rank, ACTION_STOP);
          GoalStop (rank, true);
          return false;

        default:
          break;
        }
      }
    }
  }

  if (m_blupi[rank].clicDelay > 0)
    m_blupi[rank].clicDelay--;
  if (m_blupi[rank].clicDelay == 0)
    m_blupi[rank].clicCount = 0;

  bOK = true;
  if (!BlupiRotate (rank)) // si rotation pas nécessaire
  {
    m_blupi[rank].lastIcon = m_blupi[rank].icon;

    bOK = Action (
      m_blupi[rank].action, m_blupi[rank].aDirect, m_blupi[rank].phase,
      m_blupi[rank].step, m_blupi[rank].channel, m_blupi[rank].icon,
      m_blupi[rank].pos, m_blupi[rank].posZ, sound);
    BlupiAdaptIcon (rank);

    if (sound != SOUND_NONE)
    {
      pos = ConvCelToPos (m_blupi[rank].cel);
      BlupiSound (rank, sound, pos);
    }
  }

  a = GetAmplitude (m_blupi[rank].action);

  iCel = m_blupi[rank].cel;

  if (
    m_blupi[rank].pos.x == (DIMCELX / 2) * a &&
    m_blupi[rank].pos.y == (DIMCELY / 2) * a)
  {
    m_blupi[rank].cel.x += a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (
    m_blupi[rank].pos.x == -(DIMCELX / 2) * a &&
    m_blupi[rank].pos.y == -(DIMCELY / 2) * a)
  {
    m_blupi[rank].cel.x -= a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (
    m_blupi[rank].pos.x == -(DIMCELX / 2) * a &&
    m_blupi[rank].pos.y == (DIMCELY / 2) * a)
  {
    m_blupi[rank].cel.y += a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (
    m_blupi[rank].pos.x == (DIMCELX / 2) * a &&
    m_blupi[rank].pos.y == -(DIMCELY / 2) * a)
  {
    m_blupi[rank].cel.y -= a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (m_blupi[rank].pos.x == 0 && m_blupi[rank].pos.y == DIMCELY * a)
  {
    m_blupi[rank].cel.x += a;
    m_blupi[rank].cel.y += a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (m_blupi[rank].pos.x == 0 && m_blupi[rank].pos.y == -DIMCELY * a)
  {
    m_blupi[rank].cel.x -= a;
    m_blupi[rank].cel.y -= a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (m_blupi[rank].pos.x == DIMCELX * a && m_blupi[rank].pos.y == 0)
  {
    m_blupi[rank].cel.x += a;
    m_blupi[rank].cel.y -= a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  if (m_blupi[rank].pos.x == -DIMCELX * a && m_blupi[rank].pos.y == 0)
  {
    m_blupi[rank].cel.x -= a;
    m_blupi[rank].cel.y += a;
    BlupiPushFog (rank);
    BlupiStartStopRayon (rank, iCel, m_blupi[rank].cel);
  }

  // Blupi perd de l'énergie s'il fait qq chose.
  if (
    m_blupi[rank].action != ACTION_STOP &&
    m_blupi[rank].action != ACTION_STOPTIRED &&
    m_blupi[rank].action != ACTION_STOPb &&
    m_blupi[rank].action != ACTION_STOPJEEP &&
    m_blupi[rank].action != ACTION_MISC1 &&
    m_blupi[rank].action != ACTION_MISC2 &&
    m_blupi[rank].action != ACTION_MISC3 &&
    m_blupi[rank].action != ACTION_MISC4 &&
    m_blupi[rank].action != ACTION_MISC5 &&
    m_blupi[rank].action != ACTION_MISC6 &&
    m_blupi[rank].action != ACTION_MISC1f)
  {
    if (
      m_blupi[rank].energy > 0 && m_blupi[rank].perso == 0 && // blupi ?
      m_blupi[rank].vehicule == 0)                            // à pied ?
    {
      if (m_bSuper)
        a = 0;
      else
        a = 1;
      min = 0;
      if (
        m_blupi[rank].action != ACTION_WALK &&
        (m_blupi[rank].goalAction == EV_ACTION_WALL ||
         m_blupi[rank].goalAction == EV_ACTION_TOWER))
      {
        a   = 5;
        min = 1;
      }
      if (m_blupi[rank].action == ACTION_SLIDE)
      {
        if (m_bSuper)
          a = 0;
        else
          a = 40;
      }
      m_blupi[rank].energy -= a; // blupi se fatigue +/-
      if (m_blupi[rank].energy < min)
        m_blupi[rank].energy = min;
    }
  }

  // Blupi prend de l'énergie s'il mange.
  if (m_blupi[rank].action == ACTION_EAT)
  {
    if (m_blupi[rank].energy < MAXENERGY)
      m_blupi[rank].energy += MAXENERGY / (40 * 3);
  }

  // Le robot perd de l'énergie s'il fait qq chose.
  if (m_blupi[rank].action != ACTION_R_STOP)
  {
    if (m_blupi[rank].energy > 0 && m_blupi[rank].perso == 4)
    {
      m_blupi[rank].energy -= 3; // le robot se fatigue
      if (m_blupi[rank].energy < 1)
        m_blupi[rank].energy = 1;
    }
  }

  // Le robot prend de l'énergie s'il se recharge.
  if (m_blupi[rank].action == ACTION_R_LOAD)
  {
    if (m_blupi[rank].energy < MAXENERGY)
      m_blupi[rank].energy = MAXENERGY;
  }

  // Blupi guérrit s'il boit.
  if (m_blupi[rank].action == ACTION_DRINK)
  {
    m_blupi[rank].bMalade = false;
    if (m_blupi[rank].energy < MAXENERGY)
      m_blupi[rank].energy += MAXENERGY / (40 * 3);
  }

  // Si blupi est presque complètement épuisé, il stoppe.
  if (
    !bOK && m_blupi[rank].perso == 0 && m_blupi[rank].energy < 50 &&
    m_blupi[rank].energy != 0 && m_blupi[rank].action == ACTION_WALKTIRED)
  {
    BlupiInitAction (rank, ACTION_STOP);
    GoalStop (rank, true);
  }

  return bOK;
}

// Action suivante pour un blupi existant.

void
CDecor::BlupiNextGoal (Sint32 rank)
{
  Sint32 direct, action, channel, icon, min, lg, fRank, i;
  Point  pos, cel, vector;

  if (!m_blupi[rank].bExist)
    return;

  pos = ConvCelToPos (m_blupi[rank].cel);

  // Si blupi termine une action "mort", il doit disparaître.
  if (
    m_blupi[rank].action == ACTION_BURN ||
    m_blupi[rank].action == ACTION_TCHAO ||
    m_blupi[rank].action == ACTION_S_GRILL ||
    m_blupi[rank].action == ACTION_V_GRILL)
  {
    BlupiDelete (rank); // snif ...
    return;
  }

  // Si blupi passe trop prêt du feu, aie aie aie ...
  if (IsFireCel (m_blupi[rank].cel)) // blupi se brule les ailes ?
  {
    if (
      m_blupi[rank].perso == 0 && m_blupi[rank].vehicule != 3 && // pas armure ?
      !m_bInvincible && m_blupi[rank].goalAction != EV_ACTION_GRILLE)
    {
      BlupiDeselect (rank);
      GoalStart (rank, EV_ACTION_GRILLE, m_blupi[rank].cel);
      goto goal;
      //?         BlupiInitAction(rank, ACTION_BRULE);
      //?         goto init;
    }
    if (
      m_blupi[rank].perso == 1 || // araignée ?
      m_blupi[rank].perso == 2)   // virus ?
    {
      BlupiDelete (rank); // la bestiole meurt
      return;
    }
  }

  // Si blupi passe trop prêt d'un virus ...
  if (
    m_blupi[rank].perso == 0 &&
    m_blupi[rank].vehicule != 1 && // pas en bateau ?
    m_blupi[rank].vehicule != 3 && // pas armure ?
    !m_blupi[rank].bMalade &&      // en bonne santé ?
    m_blupi[rank].goalAction != EV_ACTION_GRILLE &&
    m_blupi[rank].goalAction != EV_ACTION_ELECTRO &&
    m_blupi[rank].goalAction != EV_ACTION_ELECTROm &&
    m_blupi[rank].goalAction != EV_ACTION_BOATDE &&
    m_blupi[rank].goalAction != EV_ACTION_BOATDS &&
    m_blupi[rank].goalAction != EV_ACTION_BOATDO &&
    m_blupi[rank].goalAction != EV_ACTION_BOATDN &&
    m_blupi[rank].goalAction != EV_ACTION_BOATAE &&
    m_blupi[rank].goalAction != EV_ACTION_BOATAS &&
    m_blupi[rank].goalAction != EV_ACTION_BOATAO &&
    m_blupi[rank].goalAction != EV_ACTION_BOATAN && !m_bInvincible &&
    IsVirusCel (m_blupi[rank].cel)) // blupi chope un virus ?
  {
    m_blupi[rank].bMalade = true;

    if (m_blupi[rank].energy > MAXENERGY / 4)
      m_blupi[rank].energy = MAXENERGY / 4;

    BlupiSound (rank, SOUND_VIRUS, pos);

    if (m_blupi[rank].vehicule == 2) // en jeep ?
    {
      GoalStart (rank, EV_ACTION_DJEEP, m_blupi[rank].cel);
      goto goal;
    }
  }

  // Si blupi est complètement épuisé, il meurt.
  if (m_blupi[rank].perso == 0 && m_blupi[rank].energy == 0)
  {
    BlupiDeselect (rank);
    BlupiInitAction (rank, ACTION_TCHAO);
    goto init;
  }

  // Assigne un but s'il s'agit d'une araignée.
  if (
    m_blupi[rank].perso == 1 && // araignée ?
    m_blupi[rank].goalAction != EV_ACTION_A_MORT)
  {
    cel   = m_blupi[rank].cel;
    cel.x = (cel.x / 2) * 2;
    cel.y = (cel.y / 2) * 2;
    GetObject (cel, channel, icon);
    if (
      channel == CHOBJECT && (icon == 10000 || icon == 10001) && // rayon ?
      MoveIsUsed (cel))                                          // enclenché ?
    {
      BlupiInitAction (rank, ACTION_S_GRILL);
      goto init;
    }

    cel = m_blupi[rank].cel;
    if (cel.x % 2 != 0 && cel.y % 2 != 0)
    {
      cel.x = (cel.x / 2) * 2;
      cel.y = (cel.y / 2) * 2;
      GetObject (cel, channel, icon);
      if (channel == CHOBJECT && icon == 60) // tomates ?
      {
        PutObject (cel, -1, -1); // plus de tomates
        BlupiSound (rank, SOUND_S_HIHI, pos);
      }
      if (channel == CHOBJECT && icon == 92) // poison ?
      {
        PutObject (cel, -1, -1); // plus de poison
        BlupiInitAction (rank, ACTION_STOP);
        GoalStart (rank, EV_ACTION_A_MORT, m_blupi[rank].cel);
        m_blupi[rank].goalCel = m_blupi[rank].cel;
        goto goal;
        //?             BlupiSound(rank, SOUND_A_POISON, pos);
        //?             BlupiInitAction(rank, ACTION_A_POISON);
        //?             goto init;
      }
      if (channel == CHOBJECT && icon == 93) // piège ?
      {
        BlupiSound (rank, SOUND_TRAP, pos);
        PutObject (cel, CHOBJECT, 96); // araignée piégée
        BlupiDelete (rank);            // supprime araignée
        return;
      }
    }

    m_blupi[rank].bExist = false;
    if (
      m_time % 5 == rank % 5 && // pas trop souvent !
      SearchSpiderObject (rank, m_blupi[rank].cel, 100, cel, icon))
    {
      m_blupi[rank].goalCel = cel;
      FlushUsed (rank);
      //          direct = DirectSearch(m_blupi[rank].cel, cel);
      //          if ( direct != -1 )
      //          {
      //              vector = GetVector(direct);
      //              m_blupi[rank].goalCel.x = m_blupi[rank].cel.x + vector.x;
      //              m_blupi[rank].goalCel.y = m_blupi[rank].cel.y + vector.y;
      //              FlushUsed(rank);
      //          }
    }
    m_blupi[rank].bExist = true;
  }

  // Assigne un but s'il s'agit d'un virus.
  if (m_blupi[rank].perso == 2) // virus ?
  {
    cel   = m_blupi[rank].cel;
    cel.x = (cel.x / 2) * 2;
    cel.y = (cel.y / 2) * 2;
    GetObject (cel, channel, icon);
    if (
      channel == CHOBJECT && (icon == 10000 || icon == 10001) && // rayon ?
      MoveIsUsed (cel))                                          // enclenché ?
    {
      BlupiInitAction (rank, ACTION_V_GRILL);
      goto init;
    }

    min   = 50; // ignore si trop loin !
    fRank = -1;
    for (i = 0; i < MAXBLUPI; i++)
    {
      if (m_blupi[i].bExist && m_blupi[i].perso == 0 && !m_blupi[i].bMalade)
      {
        lg = abs (m_blupi[rank].cel.x - m_blupi[i].cel.x) +
             abs (m_blupi[rank].cel.y - m_blupi[i].cel.y);
        if (lg < min)
        {
          min   = lg;
          fRank = i;
        }
      }
    }
    if (fRank != -1)
    {
      direct = DirectSearch (m_blupi[rank].cel, m_blupi[fRank].cel);
      if (direct != -1)
      {
        vector                  = GetVector (direct);
        m_blupi[rank].goalCel.x = m_blupi[rank].cel.x + vector.x;
        m_blupi[rank].goalCel.y = m_blupi[rank].cel.y + vector.y;
        FlushUsed (rank);
      }
    }
  }

  // Assigne un but s'il s'agit d'un tracks.
  if (
    m_blupi[rank].perso == 3 && // tracks ?
    m_blupi[rank].goalAction != EV_ACTION_T_DYNAMITE)
  {
    cel = m_blupi[rank].cel;
    if (cel.x % 2 != 0 && cel.y % 2 != 0)
    {
      cel.x = (cel.x / 2) * 2;
      cel.y = (cel.y / 2) * 2;
      GetObject (cel, channel, icon);
      if (channel == CHOBJECT && IsTracksObject (icon))
      {
        if (icon == 85) // dynamite ?
        {
          BlupiInitAction (rank, ACTION_STOP);
          GoalStart (rank, EV_ACTION_T_DYNAMITE, cel);
          goto goal;
        }
        if (
          icon == 125 || // mine ?
          icon == 127)
        {
          // Supprime le détonnateur.
          BlupiDelete (GetCel (cel.x + 1, cel.y + 1), 6);
        }
        if (icon == 93) // piège ?
        {
          BlupiSound (rank, SOUND_TRAP, pos);
          PutObject (cel, CHOBJECT, 97); // tracks piégé
          BlupiDelete (rank);            // supprime tracks
          return;
        }

        PutObject (cel, -1, -1); // plus d'objet

        BlupiSound (rank, SOUND_T_ECRASE, pos);
        BlupiInitAction (rank, ACTION_T_CRUSHED);
        goto init;
      }
    }
    cel                  = m_blupi[rank].cel;
    m_blupi[rank].bExist = false;
    if (
      IsBlupiHere (cel, false) && m_blupi[m_blupiHere].perso == 0 &&
      m_blupi[m_blupiHere].vehicule == 0) // à pied ?
    {
      m_blupi[rank].bExist = true;
      // Blupi écrasé au sol.
      if (MoveCreate (
            cel, rank, true, CHFLOOR, -1, -1, -1, 100, 1, 100, false, true))
      {
        if (m_blupi[m_blupiHere].bMalade)
          MoveAddIcons (cel, 10);
        else
          MoveAddIcons (cel, 9);
      }
      BlupiDelete (m_blupiHere); // plus de blupi !
      BlupiSound (rank, SOUND_AIE, pos);
      BlupiInitAction (rank, ACTION_T_CRUSHED); // écrase blupi
      goto init;
    }
    m_blupi[rank].bExist = true;

    //      if ( m_blupi[rank].goalCel.x != -1 )
    //      {
    //          GetObject(m_blupi[rank].goalCel, channel, icon);
    //          if ( IsTracksObject(icon) )  goto action;
    //      }
    m_blupi[rank].bExist = false;
    if (
      m_time % 5 == rank % 5 && // pas trop souvent !
      SearchTracksObject (rank, m_blupi[rank].cel, 25, cel, icon))
    {
      m_blupi[rank].goalCel = cel;
      FlushUsed (rank);
    }
    m_blupi[rank].bExist = true;
  }

  // Assigne un but s'il s'agit d'un robot.
  if (
    m_blupi[rank].perso == 4 && // robot ?
    m_blupi[rank].goalAction != EV_ACTION_T_DYNAMITE)
  {
    cel = m_blupi[rank].cel;
    if (cel.x % 2 != 0 && cel.y % 2 != 0)
    {
      cel.x = (cel.x / 2) * 2;
      cel.y = (cel.y / 2) * 2;
      GetObject (cel, channel, icon);
      if (channel == CHOBJECT && IsRobotObject (icon))
      {
        if (icon == 85) // dynamite ?
        {
          BlupiInitAction (rank, ACTION_STOP);
          GoalStart (rank, EV_ACTION_T_DYNAMITE, cel);
          goto goal;
        }
        if (
          icon == 125 || // mine ?
          icon == 127)
        {
          // Supprime le détonnateur.
          BlupiDelete (GetCel (cel.x + 1, cel.y + 1), 6);
        }
        if (icon == 93) // piège ?
        {
          BlupiSound (rank, SOUND_TRAP, pos);
          PutObject (cel, CHOBJECT, 98); // robot piégé
          BlupiDelete (rank);            // supprime robot
          return;
        }

        PutObject (cel, -1, -1); // plus d'objet

        BlupiSound (rank, SOUND_T_ECRASE, pos);
        BlupiInitAction (rank, ACTION_R_CRUSHED);
        goto init;
      }
    }
    cel = m_blupi[rank].cel;
    if (
      m_blupi[rank].goalAction == 0 &&
      m_time % 17 == rank % 17 && // pas trop souvent !
      SearchRobotObject (rank, m_blupi[rank].fix, 50, cel, icon, action))
    {
      if (action == -1)
      {
        m_blupi[rank].goalCel = cel;
        FlushUsed (rank);
      }
      else
      {
        BlupiInitAction (rank, ACTION_STOP);
        GoalStart (rank, action, cel);
        goto goal;
      }
    }
  }

  // Assigne un but s'il s'agit d'une bombe.
  if (
    m_blupi[rank].perso == 5 && // bombe ?
    m_blupi[rank].goalAction != EV_ACTION_T_DYNAMITE)
  {
    cel = m_blupi[rank].cel;
    if (cel.x % 2 != 0 && cel.y % 2 != 0)
    {
      cel.x = (cel.x / 2) * 2;
      cel.y = (cel.y / 2) * 2;
      GetObject (cel, channel, icon);
      if (channel == CHOBJECT && icon == 93) // piège ?
      {
        BlupiSound (rank, SOUND_TRAP, pos);
        PutObject (cel, CHOBJECT, 114); // bombe piégée
        BlupiDelete (rank);             // supprime bombe
        return;
      }
    }
    for (i = 0; i < 4; i++)
    {
      vector = GetVector (i * 2 * 16);
      cel.x  = ((m_blupi[rank].cel.x + vector.x * 2) / 2) * 2;
      cel.y  = ((m_blupi[rank].cel.y + vector.y * 2) / 2) * 2;
      GetObject (cel, channel, icon);
      if (
        channel == CHOBJECT &&
        IsBombeObject (icon) && // cabane, palissade, etc. ?
        icon != 93)             // pas piège ?
      {
        BlupiInitAction (rank, ACTION_STOP);
        GoalStart (rank, EV_ACTION_T_DYNAMITE, m_blupi[rank].cel);
        goto goal;
      }
    }
    cel = m_blupi[rank].cel;
    if (
      m_blupi[rank].goalAction == 0 &&
      m_time % 17 == rank % 17 && // pas trop souvent !
      SearchBombeObject (rank, cel, 100, cel, icon))
    {
      m_blupi[rank].goalCel = cel;
      FlushUsed (rank);
    }
  }

  // Assigne un but s'il s'agit d'un électro.
  if (m_blupi[rank].perso == 7) // électro ?
  {
    cel = m_blupi[rank].cel;
    if (cel.x % 2 != 0 && cel.y % 2 != 0)
    {
      cel.x = (cel.x / 2) * 2;
      cel.y = (cel.y / 2) * 2;
      GetObject (cel, channel, icon);
      if (channel == CHOBJECT && icon == 93) // piège ?
      {
        BlupiSound (rank, SOUND_TRAP, pos);
        PutObject (cel, CHOBJECT, 19); // électro piégée
        BlupiDelete (rank);            // supprime électro
        return;
      }
    }
    cel = m_blupi[rank].cel;
    if (
      m_blupi[rank].goalAction == 0 && m_blupi[rank].goalCel.x == -1 &&
      m_time % 37 == rank % 37 && // pas trop souvent !
      SearchElectroObject (rank, cel, 100, cel, icon))
    {
      if (icon == -1) // sur un blupi ?
      {
        BlupiInitAction (rank, ACTION_STOP);
        GoalStart (rank, EV_ACTION_E_RAYON, cel);
        m_blupi[rank].fix = cel;
        goto goal;
      }
      m_blupi[rank].goalCel = cel;
      FlushUsed (rank);
    }
  }

  BlupiInitAction (rank, ACTION_STOP);

goal:
  if (m_blupi[rank].goalCel.x != -1) // y a-t-il un but ?
  {
    direct = DirectSearch (m_blupi[rank].cel, m_blupi[rank].goalCel);

    if (direct == -1) // but atteint ?
    {
      m_blupi[rank].goalCel.x = -1;
      FlushUsed (rank);
    }
    else
    {
      // Si blupi a peu d'énergie et qu'il transporte
      // qq chose, il doit stopper !
      if (
        m_blupi[rank].energy <= MAXENERGY / 4 &&
        m_blupi[rank].takeChannel != -1 &&
        (m_blupi[rank].vehicule == 0 || // à pied ?
         m_blupi[rank].vehicule == 3))  // armure ?
      {
        // Si blupi est en train de descendre de la jeep
        // et qu'il est malade tout en transportant qq
        // chose, il ne faut pas stopper !!!
        cel = m_blupi[rank].cel;
        if (
          cel.x % 2 != 0 && cel.y % 2 != 0 &&
          m_decor[cel.x / 2][cel.y / 2].objectIcon == 118 && // jeep
          m_blupi[rank].bMalade && m_blupi[rank].takeChannel != -1)
          goto search;
        GoalStop (rank, true);
      }
      else
      {
      search:
        //-             BlupiInitAction(rank, ACTION_MARCHE, direct);
        if (SearchBestPass (rank, action))
        {
          //-                 if ( (action >= ACTION_SAUTE2 &&
          //-                       action <= ACTION_SAUTE5) ||
          //-                      action == ACTION_GLISSE )
          //-                 {
          BlupiInitAction (rank, action);
          //-                 }
        }
        else
        {
          BlupiInitAction (rank, ACTION_STOP);
          if (
            m_blupi[rank].perso == 0 || // blupi ?
            m_blupi[rank].perso == 8)   // disciple ?
          {
            if (m_blupi[rank].busyCount == 0) // dernière tentative ?
            {
              GoalStop (rank, true);
              m_blupi[rank].goalCel.x = -1;
              m_blupi[rank].goalPhase = 0;
              m_blupi[rank].interrupt = 1;
            }
          }
          else // perso ennemi ?
          {
            // On cherchera un autre but !
            GoalStop (rank, true);
            //?                     m_blupi[rank].goalCel.x = -1;
            //?                     m_blupi[rank].goalPhase = 0;
            //?                     m_blupi[rank].interrupt = 1;
          }
        }
      }
    }
  }
  else
  {
    GoalNextPhase (rank); // méta opération suivante
  }

init:
  BlupiDestCel (rank);

  m_blupi[rank].phase = 0;
  m_blupi[rank].pos.x = 0;
  m_blupi[rank].pos.y = 0;

  BlupiNextAction (rank);
}

// Calcule la cellule de destination.

void
CDecor::BlupiDestCel (Sint32 rank)
{
  Sint32 a;
  Point  vector;

  m_blupi[rank].destCel = m_blupi[rank].cel;

  if (
    m_blupi[rank].action == ACTION_WALK ||
    m_blupi[rank].action == ACTION_WALKTIRED ||
    m_blupi[rank].action == ACTION_MARCHEb ||
    m_blupi[rank].action == ACTION_WALKJEEP ||
    m_blupi[rank].action == ACTION_WALKARMOR ||
    m_blupi[rank].action == ACTION_S_WALK ||
    m_blupi[rank].action == ACTION_V_WALK ||
    m_blupi[rank].action == ACTION_T_WALK ||
    m_blupi[rank].action == ACTION_R_WALK ||
    m_blupi[rank].action == ACTION_B_WALK ||
    m_blupi[rank].action == ACTION_E_WALK ||
    m_blupi[rank].action == ACTION_D_WALK)
  {
    vector = GetVector (m_blupi[rank].sDirect);

    m_blupi[rank].destCel.x += vector.x;
    m_blupi[rank].destCel.y += vector.y;
  }

  a = GetAmplitude (m_blupi[rank].action);
  if (a > 1)
  {
    vector = GetVector (m_blupi[rank].sDirect);

    m_blupi[rank].destCel.x += vector.x * a;
    m_blupi[rank].destCel.y += vector.y * a;
  }
}

// Avance tous les blupis.

void
CDecor::BlupiStep (bool bFirst)
{
  Sint32 rank;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (m_blupi[rank].bExist)
    {
      if (!BlupiNextAction (rank))
        BlupiNextGoal (rank);
    }
  }

  if (bFirst)
  {
    m_timeConst++; // avance le temps absolu global constant

    if (m_timeConst == m_timeFlipOutline)
    {
      m_bOutline = false; // supprime le mode "outline"
    }
  }
  m_time++; // avance le temps absolu global
}

// Retourne le rectangle occupé par un blupi,
// pour les sélections (pas exact).

void
CDecor::BlupiGetRect (Sint32 rank, Rect & rect)
{
  Point pos;

  pos = ConvCelToPos (m_blupi[rank].cel);
  pos.x += m_blupi[rank].pos.x;
  pos.y += m_blupi[rank].pos.y - (DIMBLUPIY - DIMCELY) - SHIFTBLUPIY;

  rect.left   = pos.x + 16;
  rect.top    = pos.y + 10;
  rect.right  = pos.x + DIMBLUPIX - 16;
  rect.bottom = pos.y + DIMBLUPIY;
}

// Retourne le blupi visé par la souris.

Sint32
CDecor::GetTargetBlupi (Point pos)
{
  Sint32 rank, found, prof;
  Point  test, rel, cel;

  cel = ConvPosToCel (pos);

  found = -1;
  prof  = 0;
  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist && (m_blupi[rank].perso == 0 || // blupi ?
                               m_blupi[rank].perso == 8))  // disciple ?
    {
      test = ConvCelToPos (m_blupi[rank].cel);
      test.x += m_blupi[rank].pos.x;
      test.y += m_blupi[rank].pos.y - (DIMBLUPIY - DIMCELY) - SHIFTBLUPIY;

      if (
        pos.x >= test.x && pos.x <= test.x + DIMBLUPIX && pos.y >= test.y &&
        pos.y <= test.y + DIMBLUPIY)
      {
        rel.x = pos.x - test.x;
        rel.y = pos.y - test.y;
        if (
          (cel.x == m_blupi[rank].cel.x && cel.y == m_blupi[rank].cel.y) ||
          (cel.x == m_blupi[rank].destCel.x &&
           cel.y == m_blupi[rank].destCel.y) ||
          m_pPixmap->IsIconPixel (
            m_blupi[rank].channel, m_blupi[rank].icon, rel))
        {
          if (found != -1 && test.y < prof)
            continue;

          found = rank;
          prof  = test.y;
        }
      }
    }
  }

  return found;
}

// Déslectionne tous les blupi.

void
CDecor::BlupiDeselect ()
{
  Sint32 rank;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    m_blupi[rank].bHili  = false;
    m_blupi[rank].bArrow = false;
  }

  m_nbBlupiHili   = 0;
  m_rankBlupiHili = -1;
}

// Déslectionne un blupi.

void
CDecor::BlupiDeselect (Sint32 rank)
{
  m_blupi[rank].bHili  = false;
  m_blupi[rank].bArrow = false;

  if (m_nbBlupiHili > 0 && m_rankBlupiHili == rank) // est-ce le blupi
                                                    // sélectionné ?
  {
    m_nbBlupiHili   = 0;
    m_rankBlupiHili = -1;
  }
}

// Met ou enlève une flèche au blupi sélectionné blupi.

void
CDecor::BlupiSetArrow (Sint32 rank, bool bArrow)
{
  m_celArrow.x = -1;

  if (bArrow)
    m_blupi[rank].bArrow = true;
  else
  {
    for (rank = 0; rank < MAXBLUPI; rank++)
      m_blupi[rank].bArrow = false;
  }
}

// Initialise la zone outline en fonction du rectangle de sélection.

void
CDecor::InitOutlineRect ()
{
  if (!m_bOutline && m_bHiliRect)
  {
    m_celOutline1.x = m_p1Hili.x < m_p2Hili.x ? m_p1Hili.x : m_p2Hili.x;
    m_celOutline1.y = m_p1Hili.y < m_p2Hili.y ? m_p1Hili.y : m_p2Hili.y;
    m_celOutline2.x = m_p1Hili.x > m_p2Hili.x ? m_p1Hili.x : m_p2Hili.x;
    m_celOutline2.y = m_p1Hili.y > m_p2Hili.y ? m_p1Hili.y : m_p2Hili.y;
    m_celOutline1.x = (m_celOutline1.x / 2) * 2;
    m_celOutline1.y = (m_celOutline1.y / 2) * 2;
    m_celOutline2.x += 2;
    m_celOutline2.y += 2;
  }
  else
  {
    m_celOutline1.x = -1;
    m_celOutline2.x = -1;
  }
}

// Sélectionne un blupi lorsque le bouton est pressé.

void
CDecor::BlupiHiliDown (Point pos, bool bAdd)
{
  if (MapMove (pos))
    return;

  if (!bAdd)
    BlupiDeselect ();

  m_p1Hili    = ConvPosToCel (pos);
  m_p2Hili    = ConvPosToCel (pos);
  m_bHiliRect = true;
  m_celHili.x = -1;

  InitOutlineRect ();
}

// Sélectionne un blupi lorsque la souris est déplacée.

void
CDecor::BlupiHiliMove (Point pos)
{
  if (m_bHiliRect) // rectangle de sélection existe ?
  {
    m_p2Hili = ConvPosToCel (pos);
    InitOutlineRect ();
  }
}

// Sélectionne un blupi lorsque le bouton est relâché.
// Retourne false si la sélection n'a pas changé !

void
CDecor::BlupiHiliUp (Point pos)
{
  Sint32 rank, r, nb;
  Sounds sound;
  bool   bEnerve = false;
  Point  c1, c2;

  static Sounds table_sound_ok[] = {
    SOUND_OK1, SOUND_OK2, SOUND_OK3, SOUND_OK4, SOUND_OK5, SOUND_OK6,
  };

  static Sounds table_sound_okf[] = // si fatigué
    {
      SOUND_OK1f,
      SOUND_OK2f,
      SOUND_OK3f,
    };

  static Sounds table_sound_oke[] = // si énervé
    {
      SOUND_OK1e,
      SOUND_OK2e,
      SOUND_OK3e,
    };

  if (m_bHiliRect) // rectangle de sélection existe ?
  {
    nb = 0;

    if (m_p1Hili.x == m_p2Hili.x && m_p1Hili.y == m_p2Hili.y)
    {
      rank = GetTargetBlupi (pos); // rank <- blupi visé par la souris
      if (rank != -1)
      {
        m_blupi[rank].bHili = !m_blupi[rank].bHili;
        if (m_blupi[rank].bHili)
        {
          if (m_blupi[rank].clicDelay > 0)
          {
            m_blupi[rank].clicDelay = 80;
            m_blupi[rank].clicCount++;
            if (m_blupi[rank].clicCount > 4)
              bEnerve = true;
          }
          else
            m_blupi[rank].clicDelay = 40;
          nb = 1;
        }
      }
    }
    else
    {
      if (m_p1Hili.x < m_p2Hili.x)
      {
        c1.x = m_p1Hili.x;
        c2.x = m_p2Hili.x + 1;
      }
      else
      {
        c1.x = m_p2Hili.x;
        c2.x = m_p1Hili.x + 1;
      }

      if (m_p1Hili.y < m_p2Hili.y)
      {
        c1.y = m_p1Hili.y;
        c2.y = m_p2Hili.y + 1;
      }
      else
      {
        c1.y = m_p2Hili.y;
        c2.y = m_p1Hili.y + 1;
      }

      for (r = 0; r < MAXBLUPI; r++)
      {
        if (
          m_blupi[r].bExist && (m_blupi[r].perso == 0 || // blupi ?
                                m_blupi[r].perso == 8))  // disciple ?
        {
          if (
            m_blupi[r].cel.x >= c1.x && m_blupi[r].cel.x < c2.x &&
            m_blupi[r].cel.y >= c1.y && m_blupi[r].cel.y < c2.y)
          {
            m_blupi[r].bHili = true;
            nb++;
            rank = r;
          }
        }
      }
    }

    m_bHiliRect = false; // plus de rectangle
    InitOutlineRect ();

    if (nb > 0)
    {
      if (nb > 1) // sélection multiple ?
        sound = table_sound_ok[Random (0, countof (table_sound_ok) - 1)];
      else
      {
        if (m_blupi[rank].energy <= MAXENERGY / 4)
          sound = table_sound_okf[Random (0, countof (table_sound_okf) - 1)];
        else
          sound = table_sound_ok[Random (0, countof (table_sound_ok) - 1)];
        if (bEnerve) // déjà sélectionné y'a peu ?
          sound = table_sound_oke[Random (0, countof (table_sound_oke) - 1)];
      }
      BlupiSound (rank, sound, pos, true);
    }
  }

  m_nbBlupiHili   = 0;
  m_rankBlupiHili = -1;
  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    m_blupi[rank].bArrow = false;

    if (m_blupi[rank].bExist && m_blupi[rank].bHili)
    {
      m_nbBlupiHili++;
      m_rankBlupiHili = rank;
    }
  }
}

// Dessine le rectangle de sélection, si nécessaire.

void
CDecor::BlupiDrawHili ()
{
  Point  c1, c2, cc;
  Point  p1, p2, p3, p4;
  Point  start, pos;
  Rect   rect;
  Sint32 shift;

  if (!m_bHiliRect)
    return;

  if (m_p1Hili.x < m_p2Hili.x)
  {
    c1.x = m_p1Hili.x;
    c2.x = m_p2Hili.x + 1;
  }
  else
  {
    c1.x = m_p2Hili.x;
    c2.x = m_p1Hili.x + 1;
  }

  if (m_p1Hili.y < m_p2Hili.y)
  {
    c1.y = m_p1Hili.y;
    c2.y = m_p2Hili.y + 1;
  }
  else
  {
    c1.y = m_p2Hili.y;
    c2.y = m_p1Hili.y + 1;
  }

  p1 = ConvCelToPos (c1); // p1 en haut
  p2 = ConvCelToPos (c2); // p2 en bas

  cc.x = c1.x;
  cc.y = c2.y;
  p3   = ConvCelToPos (cc); // p3 à gauche

  cc.x = c2.x;
  cc.y = c1.y;
  p4   = ConvCelToPos (cc); // p4 à droite

  p1.x += DIMCELX / 2;
  p2.x += DIMCELX / 2;
  p3.x += DIMCELX / 2;
  p4.x += DIMCELX / 2;

  shift = m_shiftHili % (64 / 2);

  start.x = p1.x - shift * 2;
  start.y = p1.y - shift - 1;
  while (start.x < p4.x)
  {
    pos         = start;
    rect.left   = 0;
    rect.right  = 64;
    rect.top    = 0;
    rect.bottom = 66 / 2;
    if (pos.x + rect.right > p4.x)
      rect.right = p4.x - pos.x;
    if (pos.x < p1.x)
    {
      rect.left += p1.x - pos.x;
      rect.top += (p1.x - pos.x) / 2;
      pos.x = p1.x;
      pos.y = p1.y - 1;
    }
    m_pPixmap->DrawPart (-1, CHHILI, pos, rect); // ligne p1-p4

    start.x += 64;
    start.y += 64 / 2;
  }

  start.x = p3.x - shift * 2;
  start.y = p3.y - shift - 1;
  while (start.x < p2.x)
  {
    pos         = start;
    rect.left   = 0;
    rect.right  = 64;
    rect.top    = 0;
    rect.bottom = 66 / 2;
    if (pos.x + rect.right > p2.x)
      rect.right = p2.x - pos.x;
    if (pos.x < p3.x)
    {
      rect.left += p3.x - pos.x;
      rect.top += (p3.x - pos.x) / 2;
      pos.x = p3.x;
      pos.y = p3.y - 1;
    }
    m_pPixmap->DrawPart (-1, CHHILI, pos, rect); // ligne p3-p2

    start.x += 64;
    start.y += 64 / 2;
  }

  start.x = p3.x - shift * 2;
  start.y = p3.y + shift - 66 / 2;
  while (start.x < p1.x)
  {
    pos         = start;
    rect.left   = 0;
    rect.right  = 64;
    rect.top    = 66 / 2;
    rect.bottom = 66;
    if (pos.x + rect.right > p1.x)
      rect.right = p1.x - pos.x;
    if (pos.x < p3.x)
    {
      rect.left += p3.x - pos.x;
      rect.bottom -= (p3.x - pos.x) / 2;
      pos.x = p3.x;
    }
    m_pPixmap->DrawPart (-1, CHHILI, pos, rect); // ligne p3-p1

    start.x += 64;
    start.y -= 64 / 2;
  }

  start.x = p2.x - shift * 2;
  start.y = p2.y + shift - 66 / 2;
  while (start.x < p4.x)
  {
    pos         = start;
    rect.left   = 0;
    rect.right  = 64;
    rect.top    = 66 / 2;
    rect.bottom = 66;
    if (pos.x + rect.right > p4.x)
      rect.right = p4.x - pos.x;
    if (pos.x < p2.x)
    {
      rect.left += p2.x - pos.x;
      rect.bottom -= (p2.x - pos.x) / 2;
      pos.x = p2.x;
    }
    m_pPixmap->DrawPart (-1, CHHILI, pos, rect); // ligne p2-p4

    start.x += 64;
    start.y -= 64 / 2;
  }

  m_shiftHili += 3;
}

// Retourne le bouton par défaut à un endroit donné.
// Est utilisé pour trouver que faire lors d'un clic
// avec le bouton de droite.

Buttons
CDecor::GetDefButton (Point cel)
{
  Buttons button;
  Sint32  rank, channel, icon;
  Point   iCel;

  iCel  = cel;
  cel.x = (cel.x / 2) * 2;
  cel.y = (cel.y / 2) * 2;
  GetObject (cel, channel, icon);

  if (m_nbBlupiHili == 0)
    return BUTTON_NONE;
  if (m_nbBlupiHili > 1)
    return BUTTON_GO;

  rank = m_rankBlupiHili;

  button = BUTTON_GO;

  if (channel == CHOBJECT)
  {
    if (icon >= 6 && icon <= 11)
      button = BUTTON_ABAT;
    if (icon >= 37 && icon <= 43)
      button = BUTTON_ROC;

    if (icon == 61)
      button = BUTTON_CULTIVE; // cabane
    if (icon == 81 || icon == 83 || icon == 94)
      button = BUTTON_FLOWER;
    if (icon == 122)
      button = BUTTON_EXTRAIT; // extrait

    if (iCel.x % 2 == 1 && iCel.y % 2 == 1)
    {
      if (icon == 14)
        button = BUTTON_CARRY; // métal
      if (icon == 36)
        button = BUTTON_CARRY; // planches
      if (icon == 44)
        button = BUTTON_CARRY; // pierres
      if (icon == 60)
        button = BUTTON_EAT; // tomates
      if (icon == 63)
        button = BUTTON_CARRY; // oeufs
      if (icon == 80)
        button = BUTTON_BOIT; // bouteille
      if (icon == 82)
        button = BUTTON_CARRY; // fleurs
      if (icon == 84)
        button = BUTTON_CARRY; // fleurs
      if (icon == 95)
        button = BUTTON_CARRY; // fleurs
      if (icon == 85)
        button = BUTTON_CARRY; // dynamite
      if (icon == 92)
        button = BUTTON_CARRY; // poison
      if (icon == 93)
        button = BUTTON_CARRY; // piège
      if (icon == 123)
        button = BUTTON_CARRY; // fer
      if (icon == 125)
        button = BUTTON_CARRY; // mine
    }

    if (
      icon == 28 && // laboratoire ?
      m_blupi[rank].energy > MAXENERGY / 4 &&
      m_blupi[rank].takeChannel == CHOBJECT &&
      (m_blupi[rank].takeIcon == 82 || // porte fleurs ?
       m_blupi[rank].takeIcon == 84 || m_blupi[rank].takeIcon == 95 ||
       m_blupi[rank].takeIcon == 60)) // porte tomates ?
    {
      button = BUTTON_LABO; // transforme
    }
  }

  if (!m_blupi[rank].bMalade && button == BUTTON_BOIT)
  {
    button = BUTTON_CARRY; // porte la bouteille si pas malade
  }

  if (
    (m_blupi[rank].energy <= MAXENERGY / 4 ||
     m_blupi[rank].takeChannel != -1) &&
    (button == BUTTON_ABAT || button == BUTTON_CARRY || button == BUTTON_ROC ||
     button == BUTTON_CULTIVE))
    return BUTTON_NONE;

  if (m_blupi[rank].energy > (MAXENERGY / 4) * 3 && button == BUTTON_EAT)
    button = BUTTON_CARRY;

  if (m_buttonExist[button] == 0) // bouton existe ?
    return BUTTON_NONE;

  return button;
}

// Indique un but visé à Sint32 terme, pour un blupi donné.

bool
CDecor::BlupiGoal (Sint32 rank, Buttons button, Point cel, Point cMem)
{
  Point  goalHili, goalHili2, goal, test;
  Sint32 i, action, channel, icon, error, direct, step;
  bool   bRepeat = false;

  // Si plusieurs blupi sont sélectionnés, ils ne vont pas
  // tous à la même destination.
  if (button == BUTTON_GO)
  {
    step = 0;
    for (i = 0; i < rank; i++)
    {
      if (m_blupi[i].bExist && m_blupi[i].bHili)
        step++;
    }
    if (step > 15)
      step = 15;
    cel.x += table_multi_goal[step * 2 + 0];
    cel.y += table_multi_goal[step * 2 + 1];
    cMem.x += table_multi_goal[step * 2 + 0];
    cMem.y += table_multi_goal[step * 2 + 1];
  }

  if (!IsCheminFree (rank, cel, button))
    return false;

  goal        = cel;
  goalHili    = cel;
  goalHili2.x = (cel.x / 2) * 2;
  goalHili2.y = (cel.y / 2) * 2;

  if (
    button == BUTTON_GO &&
    m_decor[goalHili.x / 2][goalHili.y / 2].objectIcon == 113) // maison ?
  {
    goalHili.x = (goalHili.x / 2) * 2 + 1;
    goalHili.y = (goalHili.y / 2) * 2 + 1;
  }

  if (button == BUTTON_ABATn)
  {
    button  = BUTTON_ABAT;
    bRepeat = true;
  }
  if (button == BUTTON_ROCn)
  {
    button  = BUTTON_ROC;
    bRepeat = true;
  }
  if (button == BUTTON_FLOWERn)
  {
    button  = BUTTON_FLOWER;
    bRepeat = true;
  }
  action = table_actions[button];

  if (action == EV_ACTION_STOP)
  {
    if (m_blupi[rank].goalAction != 0 && m_blupi[rank].interrupt <= 0)
    {
      m_blupi[rank].stop = 1; // faudra stopper
    }
    else
    {
      m_blupi[rank].goalCel    = m_blupi[rank].destCel;
      m_blupi[rank].goalAction = 0;
    }
    m_blupi[rank].repeatLevel = -1; // stoppe la répétition
    return false;
  }

  // Action prioritaire en cours ?
  if (m_blupi[rank].goalAction != 0 && m_blupi[rank].interrupt <= 0)
    return false;

  error = CelOkForAction (goalHili, action, rank);
  if (error != 0 && error != Errors::TOURISOL)
    return false;

  if (
    action == EV_ACTION_GO && m_blupi[rank].energy <= MAXENERGY / 4 &&
    m_blupi[rank].takeChannel != -1)
    return false;

  if (action == EV_ACTION_GO)
  {
    GetObject (goalHili2, channel, icon);
    if (
      channel == CHOBJECT && icon == 120 && // usine ?
      goalHili.x % 2 == 0 &&                // au fond ?
      goalHili.y % 2 == 1)
    {
      return false; // action refusée
    }
    if (
      m_blupi[rank].perso != 8 &&           // pas disciple ?
      channel == CHOBJECT && icon == 118 && // jeep ?
      goalHili.x % 2 == 1 &&                // sur la jeep ?
      goalHili.y % 2 == 1)
      action = EV_ACTION_MJEEP;
    if (
      m_blupi[rank].perso != 8 &&          // pas disciple ?
      m_blupi[rank].takeChannel == -1 &&   // ne porte rien ?
      channel == CHOBJECT && icon == 16 && // armure ?
      goalHili.x % 2 == 1 &&               // sur l'armure ?
      goalHili.y % 2 == 1)
      action = EV_ACTION_MARMURE;
    if (
      m_blupi[rank].perso != 8 &&         // pas disciple ?
      channel == CHOBJECT && icon == 113) // maison ?
      action = EV_ACTION_HOUSE;
    GetFloor (goalHili2, channel, icon);
    if (
      m_blupi[rank].perso == 0 && m_blupi[rank].vehicule == 0 && // à pied ?
      m_blupi[rank].takeChannel == -1 && // ne porte rien ?
      channel == CHFLOOR && icon == 80)  // téléporteur ?
    {
      if (cel.x % 2 == 0 && cel.y % 2 == 0)
        action = EV_ACTION_TELEPORTE00;
      if (cel.x % 2 == 1 && cel.y % 2 == 0)
        action = EV_ACTION_TELEPORTE10;
      if (cel.x % 2 == 0 && cel.y % 2 == 1)
        action = EV_ACTION_TELEPORTE01;
      if (cel.x % 2 == 1 && cel.y % 2 == 1)
        action = EV_ACTION_TELEPORTE11;
    }
    IsFreeCelEmbarque (goalHili, rank, action, goal);
    IsFreeCelDebarque (goalHili, rank, action, goal);
  }

  if (action == EV_ACTION_DROP && m_blupi[rank].energy <= MAXENERGY / 4)
  {
    // Energie juste pour déposer l'objet transporté.
    m_blupi[rank].energy = MAXENERGY / 4 + 20;
  }

  if (action == EV_ACTION_ABAT1)
  {
    GetObject (goalHili2, channel, icon);
    if (channel == CHOBJECT && icon >= 6 && icon <= 11) // arbre ?
    {
      action += icon - 6; // EV_ACTION_ABAT1..6
    }
  }

  if (action == EV_ACTION_ROC1)
  {
    GetObject (goalHili2, channel, icon);
    if (channel == CHOBJECT && icon >= 37 && icon <= 43) // rochers ?
    {
      action += icon - 37; // EV_ACTION_ROC1..7
    }
  }

  if (action == EV_ACTION_FLOWER1)
  {
    GetObject (goalHili2, channel, icon);
    if (channel == CHOBJECT && icon == 83) // fleurs foncées ?
      action = EV_ACTION_FLOWER2;
    if (channel == CHOBJECT && icon == 94) // fleurs vertes ?
      action = EV_ACTION_FLOWER3;
  }

  if (action == EV_ACTION_BRIDGEE)
  {
    cel  = goalHili2;
    test = goalHili2;
    if (IsBuildPont (test, icon) != 0)
      return false;

    m_blupi[rank].nLoop =
      static_cast<Sint16> (abs ((test.x - cel.x) + (test.y - cel.y)) / 2);
    m_blupi[rank].cLoop = 0;
    m_blupi[rank].vIcon = icon;
    m_blupi[rank].fix   = cel;

    if (test.x - cel.x < 0)
      action = EV_ACTION_BRIDGEO;
    if (test.y - cel.y > 0)
      action = EV_ACTION_BRIDGES;
    if (test.y - cel.y < 0)
      action = EV_ACTION_BRIDGEN;
  }

  if (action == EV_ACTION_BOATE)
  {
    if (!IsBuildBateau (goalHili2, direct))
      return false;

    if (direct == DIRECT_S)
      action = EV_ACTION_BOATS;
    if (direct == DIRECT_W)
      action = EV_ACTION_BOATO;
    if (direct == DIRECT_N)
      action = EV_ACTION_BOATN;
  }

  if (action == EV_ACTION_CARRY)
  {
    if (IsBlupiHereEx (GetCel (goalHili2, 0, 1), rank, true))
      action = EV_ACTION_CARRY2;
  }

  if (action == EV_ACTION_DROP)
  {
    GetFloor (goalHili2, channel, icon);
    if (
      channel == CHFLOOR && icon == 52 && // nurserie ?
      m_blupi[rank].takeChannel == CHOBJECT &&
      m_blupi[rank].takeIcon == 63) // oeufs ?
      action = EV_ACTION_NEWBLUPI;
    if (
      !IsFreeCelDepose (GetCel (goalHili2, 0, 1), rank) ||
      IsBlupiHereEx (GetCel (goalHili2, 0, 1), rank, true))
      action = EV_ACTION_DROP2;
  }

  if (action == EV_ACTION_EAT)
  {
    if (IsBlupiHereEx (GetCel (goalHili2, 0, 1), rank, true))
      action = EV_ACTION_EAT2;
  }

  if (action == EV_ACTION_DRINK)
  {
    if (IsBlupiHereEx (GetCel (goalHili2, 0, 1), rank, true))
      action = EV_ACTION_DRINK2;
  }

  if (action == EV_ACTION_DYNAMITE)
  {
    GetObject (goalHili2, channel, icon);
    if (channel == CHOBJECT && icon == 125) // mine ?
      action = EV_ACTION_MINE;
    else
    {
      if (
        m_blupi[rank].takeChannel == CHOBJECT &&
        m_blupi[rank].takeIcon == 85) // porte dynamite ?
        action = EV_ACTION_DYNAMITE2;
    }
  }

  GoalStart (rank, action, goal);
  m_blupi[rank].bRepeat   = bRepeat;
  m_blupi[rank].busyCount = 5; // 5 tentatives au maximum
  m_blupi[rank].busyDelay = 0;

  if (action == EV_ACTION_REPEAT)
  {
    m_blupi[rank].repeatLevel = m_blupi[rank].repeatLevelHope;
    m_blupi[rank].listCel[m_blupi[rank].repeatLevel] = goal;
  }
  else
    ListPut (rank, button, goal, cMem);

  return true;
}

// Indique un but visé à Sint32 terme, pour tous les blupi
// sélectionnés.

void
CDecor::BlupiGoal (Point cel, Buttons button)
{
  Point  bPos, avg;
  Sint32 rank, nb, nbHili;

  static Sounds table_sound_go[] = {
    SOUND_GO1, SOUND_GO2, SOUND_GO3, SOUND_GO4, SOUND_GO5, SOUND_GO6,
  };

  static Sounds table_sound_gom[] = {
    SOUND_GO4,
    SOUND_GO5,
    SOUND_GO6,
  };

  static Sounds table_sound_boing[] = {
    SOUND_BOING1,
    SOUND_BOING2,
    SOUND_BOING3,
  };

  if (button == -1)
  {
    avg = ConvCelToPos (cel);
    m_pSound->PlayImage (
      table_sound_boing[Random (0, countof (table_sound_boing) - 1)], avg);
    return;
  }

  avg.x  = 0;
  avg.y  = 0;
  nb     = 0;
  nbHili = 0;
  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (m_blupi[rank].bExist && m_blupi[rank].bHili)
    {
      bPos = ConvCelToPos (m_blupi[rank].cel);
      avg.x += bPos.x;
      avg.y += bPos.y;
      nbHili++;

      if (BlupiGoal (rank, button, cel, cel))
        nb++;
    }
  }

  if (button == BUTTON_STOP)
    return;

  if (nbHili > 0)
  {
    avg.x /= nbHili;
    avg.y /= nbHili;
  }
  if (avg.x < 0)
    avg.x = 0;
  if (avg.x > LXIMAGE ())
    avg.x = LXIMAGE ();
  avg.y = LYIMAGE () / 2;

  if (nb == 0 && nbHili > 0)
  {
    if (nbHili == 1)
      BlupiSound (m_rankBlupiHili, table_sound_boing[Random (0, 2)], avg, true);
    else
      m_pSound->PlayImage (table_sound_boing[Random (0, 2)], avg);
  }

  if (nb > 0)
  {
    if (nbHili == 1)
      BlupiSound (m_rankBlupiHili, table_sound_go[Random (0, 5)], avg, true);
    else
      m_pSound->PlayImage (table_sound_gom[Random (0, 2)], avg);
  }
}

// Indique si une cellule est occupée pour un tracks.
// La cellule est considérée libre uniquement si elle
// contient un blupi à pied ou un détonnateur de mine
// (personnage invisible).

bool
CDecor::IsTracksHere (Point cel, bool bSkipInMove)
{
  Sint32 rank;

  if (!IsValid (cel))
    return false;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist &&
      (m_blupi[rank].perso != 0 ||    // blupi ?
       m_blupi[rank].vehicule != 0 || // à pied ?
       m_bInvincible) &&
      m_blupi[rank].perso != 6) // détonnateur ?
    {
      if (bSkipInMove && m_blupi[rank].goalCel.x != -1)
        continue;

      if (cel.x == m_blupi[rank].cel.x && cel.y == m_blupi[rank].cel.y)
      {
        m_blupiHere = rank;
        return true;
      }

      if (cel.x == m_blupi[rank].destCel.x && cel.y == m_blupi[rank].destCel.y)
      {
        m_blupiHere = rank;
        return true;
      }
    }
  }

  return false;
}

// Indique si une cellule est occupée par un blupi.
// Le blupi donné dans exRank est ignoré !

bool
CDecor::IsBlupiHereEx (Point cel1, Point cel2, Sint32 exRank, bool bSkipInMove)
{
  Sint32 rank;

  if (!IsValid (cel1))
    return false;
  if (!IsValid (cel2))
    return false;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist &&
      m_blupi[rank].perso != 6 && // pas le détonnateur de mine
      rank != exRank)
    {
      if (bSkipInMove && m_blupi[rank].goalCel.x != -1)
        continue;

      if (
        cel1.x <= m_blupi[rank].cel.x && cel2.x >= m_blupi[rank].cel.x &&
        cel1.y <= m_blupi[rank].cel.y && cel2.y >= m_blupi[rank].cel.y)
      {
        m_blupiHere = rank;
        return true;
      }

      if (
        cel1.x <= m_blupi[rank].destCel.x &&
        cel2.x >= m_blupi[rank].destCel.x &&
        cel1.y <= m_blupi[rank].destCel.y && cel2.y >= m_blupi[rank].destCel.y)
      {
        m_blupiHere = rank;
        return true;
      }
    }
  }

  return false;
}

// Indique si une cellule est occupée par un blupi.
// Le blupi donné dans exRank est ignoré !

bool
CDecor::IsBlupiHereEx (Point cel, Sint32 exRank, bool bSkipInMove)
{
  Sint32 rank;

  if (!IsValid (cel))
    return false;

  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist &&
      m_blupi[rank].perso != 6 && // pas le détonnateur de mine
      rank != exRank)
    {
      if (bSkipInMove && m_blupi[rank].goalCel.x != -1)
        continue;

      if (cel.x == m_blupi[rank].cel.x && cel.y == m_blupi[rank].cel.y)
      {
        m_blupiHere = rank;
        return true;
      }

      if (cel.x == m_blupi[rank].destCel.x && cel.y == m_blupi[rank].destCel.y)
      {
        m_blupiHere = rank;
        return true;
      }
    }
  }

  return false;
}

// Indique si une cellule est occupée par un blupi.

bool
CDecor::IsBlupiHere (Point cel, bool bSkipInMove)
{
  return IsBlupiHereEx (cel, -1, bSkipInMove);
}

// Indique si une cellule future (dans une direction donnée)
// est déjà occupée par un blupi.

bool
CDecor::IsBlupiHere (Point cel, Sint32 direct, bool bSkipInMove)
{
  Point vector;

  vector = GetVector (direct);

  cel.x += vector.x;
  cel.y += vector.y;

  return IsBlupiHereEx (cel, -1, bSkipInMove);
}

// Retourne les niveaux des jauges.

void
CDecor::GetLevelJauge (Sint32 * pLevels, Sint32 * pTypes)
{
  Sint32 rank;

  pLevels[0] = -1;
  pLevels[1] = -1;

  rank = m_rankBlupiHili;

  if (m_nbBlupiHili == 1) // un seul blupi sélectionné ?
  {
    pLevels[0] = (m_blupi[rank].energy * 100) / MAXENERGY;

    pTypes[0] = 1; // rouge
    if (m_blupi[rank].energy > MAXENERGY / 4)
    {
      pTypes[0] = 2; // bleu
    }
  }

  if (m_blupi[rank].interrupt == 0 && m_blupi[rank].jaugeMax > 0)
  {
    pLevels[1] = (m_blupi[rank].jaugePhase * 100) / m_blupi[rank].jaugeMax;
    pTypes[1]  = 3; // jaune
  }
}

// Retourne true si un blupi est déjà sélectionné et qu'il
// effectue une action prioritaire. Dans ce cas, il faut tout
// de suite mettre le menu "stoppe" s'il est cliqué.

bool
CDecor::IsWorkBlupi (Sint32 rank)
{
  if (
    m_blupi[rank].bHili && m_blupi[m_rankBlupiHili].goalAction != 0 &&
    m_blupi[m_rankBlupiHili].interrupt <= 0)
    return true;

  return false;
}

// Retourne les boutons possibles à un endroit donné,
// pour le blupi sélectionné.

void
CDecor::BlupiGetButtons (
  Point pos, Sint32 & nb, Buttons * pButtons, Errors * pErrors,
  std::unordered_map<Sint32, const char *> & texts, Sint32 & perso)
{
  Buttons *    pB = pButtons;
  Errors *     pE = pErrors;
  Point        cel, cel2;
  Sint32       i, rank, channel, icon;
  Errors       error;
  Buttons      button;
  bool         bBuild = false;
  bool         bPut;
  const char * textForButton;

  static struct {
    Buttons button;
    Sint32  icon;
  } table_buttons[] = {
    //
    {BUTTON_GO, 0},        //
    {BUTTON_DJEEP, 0},     //
    {BUTTON_DARMOR, 0},    //
    {BUTTON_EAT, 0},       //
    {BUTTON_BOIT, 0},      //
    {BUTTON_CARRY, 0},     //
    {BUTTON_DEPOSE, 0},    //
    {BUTTON_LABO, 0},      //
    {BUTTON_ABAT, 0},      //
    {BUTTON_ABATn, 0},     //
    {BUTTON_ROC, 0},       //
    {BUTTON_ROCn, 0},      //
    {BUTTON_CULTIVE, 0},   //
    {BUTTON_FLOWER, 0},    //
    {BUTTON_FLOWERn, 0},   //
    {BUTTON_DYNAMITE, 0},  //
    {BUTTON_FLAG, 0},      //
    {BUTTON_EXTRAIT, 0},   //
    {BUTTON_FABJEEP, 0},   //
    {BUTTON_FABMINE, 0},   //
    {BUTTON_FABDISC, 0},   //
    {BUTTON_MAKEARMOR, 0}, //
    {BUTTON_BUILD1, 36},   // si planches (cabane)
    {BUTTON_BUILD2, 36},   // si planches (nurserie)
    {BUTTON_BUILD4, 36},   // si planches (mine)
    {BUTTON_PALIS, 36},    // si planches
    {BUTTON_BRIDGE, 36},   // si planches
    {BUTTON_BOAT, 36},     // si planches
    {BUTTON_BUILD6, 36},   // si planches (téléporteur)
    {BUTTON_BUILD3, 44},   // si pierres (laboratoire)
    {BUTTON_BUILD5, 44},   // si pierres (usine)
    {BUTTON_WALL, 44},     // si pierres
    {BUTTON_TOWER, 44},    // si pierres
    {BUTTON_STOP, 0},      //
    {BUTTON_NONE, 0}       //
  };

  nb    = 0;
  perso = 0;

  cel  = ConvPosToCel (pos);
  cel2 = ConvPosToCel2 (pos);

  if (m_nbBlupiHili == 0)
    return;

  if (m_nbBlupiHili > 1) // sélection multiple ?
  {
    error = CelOkForAction (cel, table_actions[BUTTON_GO], m_rankBlupiHili);
    if (error == 0)
    {
      *pB++ = BUTTON_GO;
      *pE++ = Errors::NONE;
      nb++;
    }

    for (rank = 0; rank < MAXBLUPI; rank++)
    {
      if (m_blupi[rank].bExist && m_blupi[rank].goalAction != 0)
      {
        *pB++ = BUTTON_STOP;
        *pE++ = Errors::NONE;
        nb++;
        break;
      }
    }

    return;
  }

  if (m_nbBlupiHili != 1)
    return;

  perso = m_blupi[m_rankBlupiHili].perso;

  // Si action prioritaire en cours -> seulement stoppe.
  if (
    m_blupi[m_rankBlupiHili].goalAction != 0 &&
    m_blupi[m_rankBlupiHili].interrupt <= 0)
  {
    if (
      abs (m_blupi[m_rankBlupiHili].cel.x - cel.x) <= 3 &&
      abs (m_blupi[m_rankBlupiHili].cel.y - cel.y) <= 3 &&
      CelOkForAction (cel, table_actions[BUTTON_STOP], m_rankBlupiHili) == 0)
    {
      *pB++ = BUTTON_STOP;
      *pE++ = Errors::NONE;
      nb++;
    }
    return;
  }

  // Vérifie si le blupi sélectionné peut construire.
  if (m_rankBlupiHili >= 0)
  {
    if (
      m_blupi[m_rankBlupiHili].energy > MAXENERGY / 4 &&
      m_blupi[m_rankBlupiHili].takeChannel == -1 &&
      m_blupi[m_rankBlupiHili].vehicule == 0) // à pied ?
      bBuild = true;
  }

  // Met les différentes actions.
  for (size_t i = 0; i < countof (table_buttons); ++i)
  {
    button = table_buttons[i].button;

    if (m_buttonExist[button] == 0)
      continue;

    error = CelOkForAction (cel, table_actions[button], m_rankBlupiHili);

    if (error == 0)
      bPut = true;
    else
      bPut = false;

    if (
      bBuild && table_buttons[i].icon != 0 && // toujours présent si matière ?
      (m_rankBlupiHili < 0 ||
       m_blupi[m_rankBlupiHili].perso != 8 || // pas disciple ?
       table_buttons[i].icon != 44))          // ni pierres ?
    {
      GetObject (cel2, channel, icon);
      if (
        channel == CHOBJECT && icon == table_buttons[i].icon && // matière ?
        cel.x % 2 == 1 && cel.y % 2 == 1)
      {
        bPut = true; // bouton présent, mais disable !
      }
    }

    if (bPut)
    {
      *pB++ = button;
      *pE++ = error;
      nb++;
    }
  }

  // Si le premier bouton est "abat", ajoute "va" devant !
  if (pButtons[0] == BUTTON_ABAT)
  {
    for (i = nb; i > 0; i--)
    {
      pButtons[i] = pButtons[i - 1];
      pErrors[i]  = pErrors[i - 1];
    }

    pButtons[0] = BUTTON_GO;
    pErrors[0]  = Errors::MISC;
    nb++;
  }

  // Regarde s'il faut ajouter le bouton "répète".
  if (
    m_blupi[m_rankBlupiHili].repeatLevel != -1 ||
    m_blupi[m_rankBlupiHili].energy <= MAXENERGY / 4 ||
    m_buttonExist[BUTTON_REPEAT] == 0)
    return;

  for (i = 0; i < nb; i++)
  {
    rank = ListSearch (m_rankBlupiHili, pButtons[i], cel, textForButton);
    if (rank > 0) // au moins 2 actions à répéter ?
    {
      m_blupi[m_rankBlupiHili].repeatLevelHope = rank;

      pButtons[nb] = BUTTON_REPEAT;
      pErrors[nb]  = Errors::REPEAT;
      texts[nb]    = textForButton;
      nb++;
      return;
    }
  }
}

// Initialise les conditions de fin.

void
CDecor::TerminatedInit ()
{
  m_winCount = 50;

  m_winLastHachBlupi   = 0;
  m_winLastHachPlanche = 0;
  m_winLastHachTomate  = 0;
  m_winLastHachMetal   = 0;
  m_winLastHachRobot   = 0;
  m_winLastHome        = 0;
  m_winLastHomeBlupi   = 0;
  m_winLastRobots      = 0;
}

// Vérifie si la partie est terminée.
// Retourne 0 si la partie n'est pas terminée.
// Retourne 1 si la partie est perdue.
// Retourne 2 si la partie est gagnée.

Sint32
CDecor::IsTerminated ()
{
  Sint32 nb, count, out;
  Point  pos;

  pos.x = LXIMAGE () / 2;
  pos.y = LYIMAGE () / 2;

  count      = m_winCount;
  m_winCount = 50;

  if (m_winLastHome > m_nbStatHome) // une maison en moins ?
  {
    out = 1; // perdu
    goto delay;
  }
  m_winLastHome = m_nbStatHome;

  nb = StatisticGetBlupi ();
  if (nb < m_term.nbMinBlupi)
  {
    out = 1; // perdu
    goto delay;
  }
  if (nb < m_term.nbMaxBlupi)
    return 0; // continue

  if (m_term.bStopFire)
  {
    nb = StatisticGetFire ();
    if (nb > 0)
      return 0; // continue;
  }

  if (m_term.bHachBlupi)
  {
    if (m_winLastHachBlupi < m_nbStatHachBlupi)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHachBlupi = m_nbStatHachBlupi;

    if (m_nbStatHachBlupi < m_nbStatHach * m_term.nbMinBlupi)
      return 0; // continue;
  }

  if (m_term.bHachPlanche)
  {
    if (m_winLastHachPlanche < m_nbStatHachPlanche)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHachPlanche = m_nbStatHachPlanche;

    if (m_nbStatHachPlanche < m_nbStatHach)
      return 0; // continue;
  }

  if (m_term.bHachTomate)
  {
    if (m_winLastHachTomate < m_nbStatHachTomate)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHachTomate = m_nbStatHachTomate;

    if (m_nbStatHachTomate < m_nbStatHach)
      return 0; // continue;
  }

  if (m_term.bHachMetal)
  {
    if (m_winLastHachMetal < m_nbStatHachMetal)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHachMetal = m_nbStatHachMetal;

    if (m_nbStatHachMetal < m_nbStatHach)
      return 0; // continue;
  }

  if (m_term.bHachRobot)
  {
    if (m_winLastRobots > m_nbStatRobots)
    {
      out = 1; // perdu
      goto delay;
    }
    m_winLastRobots = m_nbStatRobots;

    if (m_winLastHachRobot < m_nbStatHachRobot)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHachRobot = m_nbStatHachRobot;

    if (m_nbStatHachRobot < m_nbStatHach)
      return 0; // continue;
  }

  if (m_term.bHomeBlupi)
  {
    if (m_winLastHomeBlupi < m_nbStatHomeBlupi)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastHomeBlupi = m_nbStatHomeBlupi;

    if (m_nbStatHomeBlupi < m_nbStatHome)
      return 0; // continue;
  }

  if (m_term.bKillRobots)
  {
    if (m_winLastRobots > m_nbStatRobots)
      m_pSound->PlayImage (SOUND_GOAL, pos);
    m_winLastRobots = m_nbStatRobots;

    if (m_nbStatRobots > 0)
      return 0; // continue;
  }

  out = 2; // gagné

delay:
  m_winCount = count;
  if (m_winCount == 0)
    return out; // perdu/gagné

  m_winCount--;
  return 0; // continue
}

// Retourne la structure pour terminer une partie.

Term *
CDecor::GetTerminated ()
{
  return &m_term;
}
