/*
 * This file is part of the planetblupi source code
 * Copyright (C) 1997 Daniel Roux, EPSITEC SA & Denis Dumoulin
 * Copyright (C) 2017 Mathieu Schroeter
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

#include "action.h"
#include "decor.h"
#include "fifo.h"

// Mémorise toutes les positions des blupi.

void
CDecor::CheminMemPos (Sint32 exRank)
{
  Sint32 rank, index;

  m_cheminNbPos = 0;
  index         = 0;
  for (rank = 0; rank < MAXBLUPI; rank++)
  {
    if (
      m_blupi[rank].bExist &&
      m_blupi[rank].perso != 6 && // pas le détonnateur de mine
      rank != exRank)
    {
      m_cheminPos[index]  = m_blupi[rank].cel;
      m_cheminRank[index] = rank;
      m_cheminNbPos++;
      index++;

      if (
        m_blupi[rank].destCel.x != m_blupi[rank].cel.x ||
        m_blupi[rank].destCel.y != m_blupi[rank].cel.y)
      {
        m_cheminPos[index]  = m_blupi[rank].destCel;
        m_cheminRank[index] = rank;
        m_cheminNbPos++;
        index++;
      }
    }
  }
}

// Teste si une positiion est occupée par un blupi.

bool
CDecor::CheminTestPos (Point pos, Sint32 & rank)
{
  Sint32 i;

  for (i = 0; i < m_cheminNbPos; i++)
  {
    if (pos.x == m_cheminPos[i].x && pos.y == m_cheminPos[i].y)
    {
      rank = m_cheminRank[i];
      return true;
    }
  }

  return false;
}

// une fois le but trouvé, reprend en arrière
// à la recherche du chemin le plus court

// retourne la direction à prendre
Sint32
CDecor::CheminARebours (Sint32 rank)
{
  Sint32 pos, rebours, last, dir, set;
  Point  v;

  pos = m_blupi[rank].goalCel.y * MAXCELX + m_blupi[rank].goalCel.x;

  rebours = m_cheminWork[pos];

  if (rebours == 0)
    return -1;

  while (true)
  {
  bis:
    for (set = 0; set < 2; set++)
    {
      for (dir = set; dir < 8; dir += 2)
      {
        v    = GetVector (dir * 16);
        last = pos + v.y * MAXCELX + v.x;

        if (m_cheminWork[last] > 0 && m_cheminWork[last] < rebours)
        {
          pos     = last;
          rebours = m_cheminWork[pos];
          if (rebours == 1)
            return (dir >= 4) ? dir - 4 : dir + 4;
          goto bis; // interrompt le for...
        }
      }
    }

    // ne devrait jamais arriver !
    return -1;
  }
}

// troisième méthode de recherche
// semblable à la précédente,
// mais les points à explorer sont classés selon leur distance à la cible

void
CDecor::CheminFillTerrain (Sint32 rank)
{
  Sint32 pos, last, dest, dist;
  Sint32 step, dir, cout, action, max, next, ampli;
  Sint32 dx, dy;
  Sint32 but = 1000;

  if (
    m_blupi[rank].cel.x == m_blupi[rank].goalCel.x &&
    m_blupi[rank].cel.y == m_blupi[rank].goalCel.y)
    return;

  pos  = m_blupi[rank].cel.y * MAXCELX + m_blupi[rank].cel.x;
  dest = m_blupi[rank].goalCel.y * MAXCELX + m_blupi[rank].goalCel.x;

  CPileTriee fifo (2 * MAXCELX + 2 * MAXCELY); // les variantes possibles

  fifo.put (pos, 0);     // position de départ
  m_cheminWork[pos] = 1; // première position

  // répète jusqu'à trouvé ou plus de possibilités
  max = 500;
  while (max-- > 0)
  {
    // reprend une variante de chemin
    pos = fifo.get ();
    if (pos < 0)
      break;

    step = m_cheminWork[pos];

    // on est arrivé au but ?
    //?     if ( pos == dest )  return;
    if (pos == dest)
    {
      but = step; // hélas trop lent !
      max = 50;
    }

    // est-ce vraiment trop loin ?
    if (step > 200)
      return;

    // marque les cases autour du point
    if (step < but)
      for (dir = 0; dir < 8; dir++)
      {
        if (CheminTestDirection (rank, pos, dir, next, ampli, cout, action))
        {
          last = pos + ampli * next;
          if (last < 0 || last >= MAXCELX * MAXCELY)
            continue;

          if (m_cheminWork[last] == 0 || m_cheminWork[last] > step + cout)
          {
            // marque les cases sautées
            for (Sint32 i = 1; i < ampli; i++)
              m_cheminWork[pos + i * next] = step + cout - ampli + i;

            m_cheminWork[last] = step + cout;

            dx = m_blupi[rank].goalCel.x - last % MAXCELX;
            dy = m_blupi[rank].goalCel.y - last / MAXCELX;

            dist = (Sint32) (dy * dy) + (Sint32) (dx * dx);
            fifo.put (last, dist);
          }
        }
      }
  }
}

// routine déterninant si une direction est possible
// retourne l'incrément pour passer à la nouvelle case
// et le "prix à payer" pour aller dans cette direction
// coût doit être déterminé en sortie

bool
CDecor::CheminTestDirection (
  Sint32 rank, Sint32 pos, Sint32 dir, Sint32 & next, Sint32 & ampli,
  Sint32 & cout, Sint32 & action)
{
  Point  cel, vector, newCel;
  bool   bFree;
  Sint32 tryDirect, workBlupi, rankHere;

  cel.x = pos % MAXCELX;
  cel.y = pos / MAXCELX;

  tryDirect = dir * 16;
  vector    = GetVector (tryDirect);

  // Peut-on glisser dans cette direction ?
  bFree = IsFreeGlisse (cel, tryDirect, rank, action);
  cout  = 5; // coût élevé

  if (!bFree)
  {
    // Peut-on marcher normalement ?
    bFree  = IsFreeDirect (cel, tryDirect, rank);
    action = ACTION_WALK;
    cout   = 1; // coût minimal
  }

  if (!bFree)
  {
    // Peut-on sauter ?
    bFree = IsFreeJump (cel, tryDirect, rank, action);
    cout  = 3; // coût élevé
  }

  ampli = GetAmplitude (action); // a <- amplitude (1..5)
  cout *= ampli;                 // coût plus élevé si grande amplitude

  // Blupi peut aller sur le lieu de la construction.
  if (!bFree && m_blupi[rank].passCel.x != -1)
  {
    newCel    = m_blupi[rank].passCel;
    workBlupi = m_decor[newCel.x / 2][newCel.y / 2].workBlupi;

    if (
      m_blupi[rank].passCel.x / 2 == (cel.x + vector.x * ampli) / 2 &&
      m_blupi[rank].passCel.y / 2 == (cel.y + vector.y * ampli) / 2 &&
      (workBlupi < 0 || workBlupi == rank))
    {
      bFree = true;
      cout  = 1;
    }
  }

  if (bFree) // chemin libre (sans tenir compte des perso) ?
  {
    newCel.x = cel.x + vector.x * ampli;
    newCel.y = cel.y + vector.y * ampli; // newCel <- arrivée suposée

    if (m_blupi[rank].perso == 3) // tracks ?
    {
      // Le tracks peut aller sur les blupi
      // pour les écraser !
      if (IsTracksHere (newCel, false)) // autre perso ici ?
        return false;
    }
    else
    {
      //?         if ( IsBlupiHere(newCel, false) )  // autre perso ici ?
      if (CheminTestPos (newCel, rankHere)) // autre perso ici ?
      {
        // Si blupi immobile, comme si obstacle qq.
        //?             if ( m_blupi[m_blupiHere].goalCel.x == -1 )  return
        // false;
        if (m_blupi[rankHere].goalCel.x == -1)
          return false;

        // Si blupi mobile, possible mais coût élevé.
        cout = 20;
      }
    }
    next = vector.y * MAXCELX + vector.x;
    return true;
  }

  return false;
}

// Retourne true si on a assigné une nouvelle direction à blupi.

bool
CDecor::CheminCherche (Sint32 rank, Sint32 & action)
{
  Sint32 cout; // prix pour aller dans une direction
  Sint32 pos, dir, next, ampli;

  // Déjà à destination ?
  if (
    m_blupi[rank].cel.x == m_blupi[rank].goalCel.x &&
    m_blupi[rank].cel.y == m_blupi[rank].goalCel.y)
    return false;

  // Destination occupée ?
  if (IsBlupiHereEx (m_blupi[rank].goalCel, rank, false))
    return false;

  memset (m_cheminWork, 0, (Uint8) MAXCELX * (Uint8) MAXCELY);
  CheminMemPos (rank);

  // fait un remplissage du map de travail
  // autour du point de départ
  CheminFillTerrain (rank);

  // cherche le chemin à partir de la destination
  dir = CheminARebours (rank);
  if (dir < 0)
    return false;

  pos = m_blupi[rank].cel.y * MAXCELX + m_blupi[rank].cel.x;
  if (
    CheminTestDirection (rank, pos, dir, next, ampli, cout, action) &&
    cout < 20)
  {
    m_blupi[rank].sDirect = 16 * dir;
    return true;
  }

  // ne devrait jamais arriver !
  return false;
}

// Teste s'il est possible de se rendre à un endroit donné.

bool
CDecor::IsCheminFree (Sint32 rank, Point dest, Sint32 button)
{
  Sint32 action, sDirect;
  Point  goalCel, passCel, limit;
  bool   bOK;

  if (button == BUTTON_STOP)
    return true;

  goalCel = m_blupi[rank].goalCel;
  passCel = m_blupi[rank].passCel;
  sDirect = m_blupi[rank].sDirect;

  if (IsFreeCelEmbarque (dest, rank, action, limit))
    dest = limit;
  if (IsFreeCelDebarque (dest, rank, action, limit))
    dest = limit;
  if (
    button == BUTTON_GO &&
    m_decor[dest.x / 2][dest.y / 2].objectChannel == CHOBJECT &&
    (m_decor[dest.x / 2][dest.y / 2].objectIcon == 118 || // jeep ?
     m_decor[dest.x / 2][dest.y / 2].objectIcon == 16) && // armure ?
    dest.x % 2 == 1 &&
    dest.y % 2 == 1)
  {
    dest.y--; // va à côté jeep/armure
  }
  if (
    button == BUTTON_GO &&
    m_decor[dest.x / 2][dest.y / 2].objectChannel == CHOBJECT &&
    m_decor[dest.x / 2][dest.y / 2].objectIcon == 113) // maison ?
  {
    dest.x = (dest.x / 2) * 2 + 1;
    dest.y = (dest.y / 2) * 2 + 1; // sous la porte
  }

  if (m_blupi[rank].cel.x == dest.x && m_blupi[rank].cel.y == dest.y)
    return true;

  m_blupi[rank].goalCel = dest;
  if (
    m_decor[dest.x / 2][dest.y / 2].objectChannel == CHOBJECT &&
    button != BUTTON_GO)
  {
    m_blupi[rank].passCel = dest; // si arbres/fleurs/bateau/etc.
  }

  bOK = CheminCherche (rank, action);

  m_blupi[rank].goalCel = goalCel;
  m_blupi[rank].passCel = passCel;
  m_blupi[rank].sDirect = sDirect;

  return bOK;
}
