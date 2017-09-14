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
#include "def.h"
#include "misc.h"

typedef struct {
  Sint16 majRev;
  Sint16 minRev;
  Sint32 nbDecor;
  Sint32 lgDecor;
  Sint32 nbBlupi;
  Sint32 lgBlupi;
  Sint32 nbMove;
  Sint32 lgMove;
  Sint16 reserve1[100];
  Point  celCoin;
  Sint16 world;
  Sint32 time;
  char   buttonExist[MAXBUTTON];
  Term   term;
  Sint16 music;
  Sint16 region;
  Sint32 totalTime;
  Sint16 skill;
  Point  memoPos[4];
  Sint16 reserve2[29];
} DescFile;

typedef struct {
  Sint32 bExist; // true -> utilisé
  Sint32 bHili;  // true -> sélectionné

  Sint16 perso; // personnage, voir (*)

  Sint16 goalAction; // action (Sint32 terme)
  Sint16 goalPhase;  // phase (Sint32 terme)
  Point  goalCel;    // cellule visée (Sint32 terme)
  Point  passCel;    // cellule tranversante

  Sint16 energy; // énergie restante

  Point  cel;     // cellule actuelle
  Point  destCel; // cellule destination
  Sint16 action;  // action en cours
  Sint16 aDirect; // direction actuelle
  Sint16 sDirect; // direction souhaitée

  Point  pos;  // position relative à partir de la cellule
  Sint16 posZ; // déplacement z
  Sint16 channel;
  Sint16 lastIcon;
  Sint16 icon;
  Sint16 phase;     // phase dans l'action
  Sint16 step;      // pas global
  Sint16 interrupt; // 0=prioritaire, 1=normal, 2=misc
  Sint16 clipLeft;

  Sint32 nbUsed; // nb de points déjà visités
  char   nextRankUsed;
  Point  posUsed[MAXUSED];
  char   rankUsed[MAXUSED];

  Sint16 takeChannel; // objet transporté
  Sint16 takeIcon;

  Point fix; // point fixe (cultive, pont)

  Sint16 jaugePhase;
  Sint16 jaugeMax;
  Sint16 stop;     // 1 -> devra stopper
  Sint16 bArrow;   // true -> flèche en dessus de blupi
  Sint16 bRepeat;  // true -> répète l'action
  Sint16 nLoop;    // nb de boucles pour GOAL_OTHERLOOP
  Sint16 cLoop;    // boucle en cours
  Sint16 vIcon;    // icône variable
  Point  goalHili; // but visé
  Sint16 bMalade;  // true -> blupi malade
  Sint16 bCache;   // true -> caché (pas dessiné)
  Sint16 vehicule; // véhicule utilisé par blupi, voir (**)
  char   busyCount;
  char   busyDelay;
  char   clicCount;
  char   clicDelay;
  char   reserve2[2];
} OldBlupi;

// Sauve le décor sur disque.

bool
CDecor::Write (Sint32 rank, bool bUser, Sint32 world, Sint32 time, Sint32 total)
{
  std::string filename;
  FILE *      file    = nullptr;
  DescFile *  pBuffer = nullptr;
  Sint32      i;
  size_t      nb;

  if (bUser)
  {
    filename = string_format ("data/user%.3d.blp", rank);
    AddUserPath (filename);
  }
  else
  {
    filename = string_format ("data/world%.3d.blp", rank);
    AddUserPath (filename);
  }

  file = fopen (filename.c_str (), "wb");
  if (file == nullptr)
    goto error;

  pBuffer = (DescFile *) malloc (sizeof (DescFile));
  if (pBuffer == nullptr)
    goto error;
  memset (pBuffer, 0, sizeof (DescFile));

  pBuffer->majRev    = 1;
  pBuffer->minRev    = 5;
  pBuffer->celCoin   = m_celCoin;
  pBuffer->world     = world;
  pBuffer->time      = time;
  pBuffer->totalTime = total;
  pBuffer->term      = m_term;
  pBuffer->music     = m_music;
  pBuffer->region    = m_region;
  pBuffer->skill     = m_skill;
  pBuffer->nbDecor   = MAXCELX * MAXCELY;
  pBuffer->lgDecor   = sizeof (Cellule);
  pBuffer->nbBlupi   = MAXBLUPI;
  pBuffer->lgBlupi   = sizeof (Blupi);
  pBuffer->nbMove    = MAXMOVE;
  pBuffer->lgMove    = sizeof (Move);

  for (i = 0; i < MAXBUTTON; i++)
    pBuffer->buttonExist[i] = m_buttonExist[i];

  for (i = 0; i < 4; i++)
    pBuffer->memoPos[i] = m_memoPos[i];

  nb = fwrite (pBuffer, sizeof (DescFile), 1, file);
  if (nb < 1)
    goto error;

  nb = fwrite (m_decor, sizeof (Cellule), MAXCELX * MAXCELY / 4, file);
  if (nb < MAXCELX * MAXCELY / 4)
    goto error;

  nb = fwrite (m_blupi, sizeof (Blupi), MAXBLUPI, file);
  if (nb < MAXBLUPI)
    goto error;

  nb = fwrite (m_move, sizeof (Move), MAXMOVE, file);
  if (nb < MAXMOVE)
    goto error;

  nb = fwrite (m_lastDrapeau, sizeof (Point), MAXLASTDRAPEAU, file);
  if (nb < MAXLASTDRAPEAU)
    goto error;

  free (pBuffer);
  fclose (file);
  return true;

error:
  if (pBuffer != nullptr)
    free (pBuffer);
  if (file != nullptr)
    fclose (file);
  return false;
}

// Lit le décor sur disque.

bool
CDecor::Read (
  Sint32 rank, bool bUser, Sint32 & world, Sint32 & time, Sint32 & total)
{
  std::string filename;
  FILE *      file    = nullptr;
  DescFile *  pBuffer = nullptr;
  Sint32      majRev, minRev;
  Sint32      i, x, y;
  size_t      nb;
  OldBlupi    oldBlupi;

  Init (-1, -1);

  if (bUser)
  {
    filename = string_format ("data/user%.3d.blp", rank);
    AddUserPath (filename);
  }
  else if (rank >= 200)
  {
    filename = string_format ("data/world%.3d.blp", rank);
    AddUserPath (filename);
  }
  else
    filename = string_format (GetBaseDir () + "data/world%.3d.blp", rank);

  file = fopen (filename.c_str (), "rb");
  if (file == nullptr)
    goto error;

  pBuffer = (DescFile *) malloc (sizeof (DescFile));
  if (pBuffer == nullptr)
    goto error;

  nb = fread (pBuffer, sizeof (DescFile), 1, file);
  if (nb < 1)
    goto error;

  majRev = pBuffer->majRev;
  minRev = pBuffer->minRev;

  if (majRev == 1 && minRev == 0)
    goto error;

  if (majRev == 1 && minRev == 3)
  {
    if (
      pBuffer->nbDecor != MAXCELX * MAXCELY ||
      pBuffer->lgDecor != sizeof (Cellule) || pBuffer->nbBlupi != MAXBLUPI ||
      pBuffer->lgBlupi != sizeof (OldBlupi) || pBuffer->nbMove != MAXMOVE ||
      pBuffer->lgMove != sizeof (Move))
      goto error;
  }
  else
  {
    if (
      pBuffer->nbDecor != MAXCELX * MAXCELY ||
      pBuffer->lgDecor != sizeof (Cellule) || pBuffer->nbBlupi != MAXBLUPI ||
      pBuffer->lgBlupi != sizeof (Blupi) || pBuffer->nbMove != MAXMOVE ||
      pBuffer->lgMove != sizeof (Move))
      goto error;
  }

  SetCoin (pBuffer->celCoin);
  if (bUser)
  {
    world = pBuffer->world;
    time  = pBuffer->time;
    total = pBuffer->totalTime;
  }
  m_celHome = pBuffer->celCoin;
  m_term    = pBuffer->term;
  m_music   = pBuffer->music;
  m_region  = pBuffer->region;

  if (bUser)
    m_skill = pBuffer->skill;

  for (i = 0; i < MAXBUTTON; i++)
    m_buttonExist[i] = pBuffer->buttonExist[i];

  for (i = 0; i < 4; i++)
    m_memoPos[i] = pBuffer->memoPos[i];

  nb = fread (m_decor, sizeof (Cellule), MAXCELX * MAXCELY / 4, file);
  if (nb < MAXCELX * MAXCELY / 4)
    goto error;
  if (majRev == 1 && minRev < 5)
  {
    for (x = 0; x < MAXCELX / 2; x++)
    {
      for (y = 0; y < MAXCELY / 2; y++)
      {
        if (m_decor[x][y].objectIcon >= 128 && m_decor[x][y].objectIcon <= 130)
          m_decor[x][y].objectIcon -= 128 - 17;
      }
    }
  }

  if (majRev == 1 && minRev == 3)
  {
    memset (m_blupi, 0, sizeof (Blupi) * MAXBLUPI);
    for (i = 0; i < MAXBLUPI; i++)
    {
      nb = fread (&oldBlupi, sizeof (OldBlupi), 1, file);
      if (nb != 1)
        goto error;
      memcpy (m_blupi + i, &oldBlupi, sizeof (OldBlupi));
      ListFlush (i);
    }
  }
  else
  {
    nb = fread (m_blupi, sizeof (Blupi), MAXBLUPI, file);
    if (nb < MAXBLUPI)
      goto error;
  }

  nb = fread (m_move, sizeof (Move), MAXMOVE, file);
  if (nb < MAXMOVE)
    goto error;

  nb = fread (m_lastDrapeau, sizeof (Point), MAXLASTDRAPEAU, file);
  if (nb < MAXLASTDRAPEAU)
    InitDrapeau ();

  BlupiDeselect (); // désélectionne tous les blupi

  free (pBuffer);
  fclose (file);
  return true;

error:
  if (pBuffer != nullptr)
    free (pBuffer);
  if (file != nullptr)
    fclose (file);

  Flush (); // initialise un décor neutre
  return false;
}

// Indique si un fichier existe sur disque.

bool
CDecor::FileExist (
  Sint32 rank, bool bUser, Sint32 & world, Sint32 & time, Sint32 & total)
{
  std::string filename;
  FILE *      file    = nullptr;
  DescFile *  pBuffer = nullptr;
  Sint32      majRev, minRev;
  size_t      nb;

  if (bUser)
  {
    filename = string_format ("data/user%.3d.blp", rank);
    AddUserPath (filename);
  }
  else if (rank >= 200)
  {
    filename = string_format ("data/world%.3d.blp", rank);
    AddUserPath (filename);
  }
  else
    filename = string_format (GetBaseDir () + "data/world%.3d.blp", rank);

  file = fopen (filename.c_str (), "rb");
  if (file == nullptr)
    goto error;

  pBuffer = (DescFile *) malloc (sizeof (DescFile));
  if (pBuffer == nullptr)
    goto error;

  nb = fread (pBuffer, sizeof (DescFile), 1, file);
  if (nb < 1)
    goto error;

  majRev = pBuffer->majRev;
  minRev = pBuffer->minRev;

  if (majRev == 1 && minRev == 0)
    goto error;

  if (majRev == 1 && minRev == 3)
  {
    if (
      pBuffer->nbDecor != MAXCELX * MAXCELY ||
      pBuffer->lgDecor != sizeof (Cellule) || pBuffer->nbBlupi != MAXBLUPI ||
      pBuffer->lgBlupi != sizeof (OldBlupi) || pBuffer->nbMove != MAXMOVE ||
      pBuffer->lgMove != sizeof (Move))
      goto error;
  }
  else
  {
    if (
      pBuffer->nbDecor != MAXCELX * MAXCELY ||
      pBuffer->lgDecor != sizeof (Cellule) || pBuffer->nbBlupi != MAXBLUPI ||
      pBuffer->lgBlupi != sizeof (Blupi) || pBuffer->nbMove != MAXMOVE ||
      pBuffer->lgMove != sizeof (Move))
      goto error;
  }

  world = pBuffer->world;
  time  = pBuffer->time;
  total = pBuffer->totalTime;

  free (pBuffer);
  fclose (file);
  return true;

error:
  if (pBuffer != nullptr)
    free (pBuffer);
  if (file != nullptr)
    fclose (file);
  return false;
}

#define MARG 18

// Initialise un décor neutre.

void
CDecor::Flush ()
{
  Sint32 x, y, i, icon;

  Init (-1, -1);

  for (x = 0; x < MAXCELX; x += 2)
  {
    for (y = 0; y < MAXCELY; y += 2)
    {
      if (x < MARG || x > MAXCELX - MARG || y < MARG || y > MAXCELY - MARG)
      {
        icon = 14; // eau
        goto put;
      }

      if (x == MARG && y == MARG)
      {
        icon = 12;
        goto put;
      }
      if (x == MAXCELX - MARG && y == MARG)
      {
        icon = 13;
        goto put;
      }
      if (x == MARG && y == MAXCELY - MARG)
      {
        icon = 11;
        goto put;
      }
      if (x == MAXCELX - MARG && y == MAXCELY - MARG)
      {
        icon = 10;
        goto put;
      }

      if (x == MARG)
      {
        icon = 4;
        goto put;
      }
      if (x == MAXCELX - MARG)
      {
        icon = 2;
        goto put;
      }
      if (y == MARG)
      {
        icon = 5;
        goto put;
      }
      if (y == MAXCELY - MARG)
      {
        icon = 3;
        goto put;
      }

      icon = 1; // terre

    put:
      m_decor[x / 2][y / 2].floorChannel = CHFLOOR;
      m_decor[x / 2][y / 2].floorIcon    = icon;
    }
  }

  for (i = 0; i < MAXBLUPI; i++)
    m_blupi[i].bExist = false;

  BlupiCreate (GetCel (102, 100), ACTION_STOP, DIRECT_S, 0, MAXENERGY);
  m_decor[98 / 2][100 / 2].floorChannel  = CHFLOOR;
  m_decor[98 / 2][100 / 2].floorIcon     = 16;
  m_decor[98 / 2][100 / 2].objectChannel = CHOBJECT;
  m_decor[98 / 2][100 / 2].objectIcon    = 113;

  for (i = 0; i < MAXBUTTON; i++)
    m_buttonExist[i] = 1;

  for (i = 0; i < 4; i++)
  {
    m_memoPos[i].x = 0;
    m_memoPos[i].y = 0;
  }

  memset (&m_term, 0, sizeof (Term));
  m_term.bHomeBlupi = true;
  m_term.nbMinBlupi = 1;
  m_term.nbMaxBlupi = 1;

  m_music  = 0;
  m_region = 0;

  m_celHome.x = 90;
  m_celHome.y = 98;
  SetCoin (m_celHome);
  InitAfterBuild ();
  LoadImages ();
}
