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
#include "misc.h"
#include "decmove.h"


/**
 * Movements to shake a tree
 * (synchronized with ACTION_PIOCHE).
 */
static Sint16 table_move1[] =
{
    9 * 4,  // nb
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
    -1,  0,
    +1,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
    -1,  0,
    +1,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
    -1,  0,
    +1,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
    -1,  0,
    +1,  0,
};

/**
 * Movements to jump an object (east) on Blupi
 * (synchronized with ACTION_TAKE).
 */
static Sint16 table_move2[] =
{
    9 + 19,     // nb
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0,   0,
      0, -10,
      0, -20,
      0, -30,
      0, -40,
     -1, -50,
     -3, -60,
     -7, -70,
    -15, -80,
    -22, -70,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
    -30, -56,
};

/**
 * Movements to jump an object (south) on Blupi
 * (synchronized with ACTION_TAKE2).
 */
static Sint16 table_move3[] =
{
    9 + 19,     // nb
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0, -10,
     0, -20,
     0, -30,
     0, -40,
     1, -50,
     3, -60,
     7, -70,
    15, -80,
    22, -70,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
    30, -56,
};

/**
 * Movements to drop and object (east) on Blupi
 * (synchronized with ACTION_DEPOSE).
 */
static Sint16 table_move4[] =
{
    10,     // nb
    -30, -60,
    -22, -70,
    -15, -80,
     -7, -70,
     -3, -60,
     -1, -50,
      0, -40,
      0, -30,
      0, -20,
      0, -10,
};

/**
 * Movements to drop an object (south) on Blupi
 * (synchronized with ACTION_DEPOSE2).
 */
static Sint16 table_move5[] =
{
    10,     // nb
    30, -60,
    22, -70,
    15, -80,
     7, -70,
     3, -60,
     1, -50,
     0, -40,
     0, -30,
     0, -20,
     0, -10,
};

/**
 * Movements to push a bridge to the east.
 */
static Sint16 table_move6[] =
{
    40,     // nb
     +2,  +1,
     +4,  +2,
     +6,  +3,
     +8,  +4,
    +10,  +5,
    +12,  +6,
    +14,  +7,
    +16,  +8,
    +18,  +9,
    +20, +10,
    +22, +11,
    +24, +12,
    +26, +13,
    +28, +14,
    +30, +15,
    +32, +16,
    +34, +17,
    +36, +18,
    +38, +19,
    +40, +20,
    +42, +21,
    +44, +22,
    +46, +23,
    +48, +24,
    +50, +25,
    +52, +26,
    +54, +27,
    +56, +28,
    +58, +29,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
    +60, +30,
};

/**
 * Movements to push a bridge to the west.
 */
static Sint16 table_move7[] =
{
    40,     // nb
     -2,  -1,
     -4,  -2,
     -6,  -3,
     -8,  -4,
    -10,  -5,
    -12,  -6,
    -14,  -7,
    -16,  -8,
    -18,  -9,
    -20, -10,
    -22, -11,
    -24, -12,
    -26, -13,
    -28, -14,
    -30, -15,
    -32, -16,
    -34, -17,
    -36, -18,
    -38, -19,
    -40, -20,
    -42, -21,
    -44, -22,
    -46, -23,
    -48, -24,
    -50, -25,
    -52, -26,
    -54, -27,
    -56, -28,
    -58, -29,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
    -60, -30,
};

/**
 * Movements to push a bridge to the south.
 */
static Sint16 table_move8[] =
{
    40,     // nb
     -2,  +1,
     -4,  +2,
     -6,  +3,
     -8,  +4,
    -10,  +5,
    -12,  +6,
    -14,  +7,
    -16,  +8,
    -18,  +9,
    -20, +10,
    -22, +11,
    -24, +12,
    -26, +13,
    -28, +14,
    -30, +15,
    -32, +16,
    -34, +17,
    -36, +18,
    -38, +19,
    -40, +20,
    -42, +21,
    -44, +22,
    -46, +23,
    -48, +24,
    -50, +25,
    -52, +26,
    -54, +27,
    -56, +28,
    -58, +29,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
    -60, +30,
};

/**
 * Movements to push a bridge to the north.
 */
static Sint16 table_move9[] =
{
    40,     // nb
     +2,  -1,
     +4,  -2,
     +6,  -3,
     +8,  -4,
    +10,  -5,
    +12,  -6,
    +14,  -7,
    +16,  -8,
    +18,  -9,
    +20, -10,
    +22, -11,
    +24, -12,
    +26, -13,
    +28, -14,
    +30, -15,
    +32, -16,
    +34, -17,
    +36, -18,
    +38, -19,
    +40, -20,
    +42, -21,
    +44, -22,
    +46, -23,
    +48, -24,
    +50, -25,
    +52, -26,
    +54, -27,
    +56, -28,
    +58, -29,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
    +60, -30,
};

/**
 * Movements to shake a laboratory.
 */
static Sint16 table_move10[] =
{
    16 * 4, // nb
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
     0, 0,
     0, 0,
     0, 0,
     0, 0,
     0, 0,
     0, 0,
     0, 0,
     0, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -2, 0,
    +2, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
    -1, 0,
    +1, 0,
};

/**
 * Movements to shake a mine.
 */
static Sint16 table_move11[] =
{
    20 * 4, // nb
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0,  0,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
    0, -1,
    0, +1,
};

/**
 * Movements to shake a factory.
 */
static Sint16 table_move12[] =
{
    20 * 4, // nb
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
     0, -1,
     0, +1,
     0, -1,
     0, +1,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
     0, -1,
     0, +1,
     0, -2,
     0, +2,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -1,  0,
    +1,  0,
    -1,  0,
    +1,  0,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
     0,  0,
     0,  0,
     0,  0,
     0,  0,
    -2,  0,
    +2,  0,
     0, +1,
     0, +1,
};

Sint16 *GetListMoves (Sint32 rank)
{
    switch (rank)
    {
    case 1:
        return table_move1;
    case 2:
        return table_move2;
    case 3:
        return table_move3;
    case 4:
        return table_move4;
    case 5:
        return table_move5;
    case 6:
        return table_move6;
    case 7:
        return table_move7;
    case 8:
        return table_move8;
    case 9:
        return table_move9;
    case 10:
        return table_move10;
    case 11:
        return table_move11;
    case 12:
        return table_move12;
    }

    return nullptr;
}

/**
 * Small fire.
 */
static Sint16 table_icon1[] =
{
    8 * 3,
    49, 50, 51, 52, 50, 49, 51, 52,
    49, 52, 51, 50, 49, 52, 50, 49,
    52, 51, 50, 49, 51, 52, 49, 51,
};

/**
 * Large fire.
 */
static Sint16 table_icon2[] =
{
    8 * 3,
    45, 46, 47, 48, 46, 45, 48, 47,
    46, 45, 48, 47, 45, 48, 47, 48,
    46, 45, 48, 47, 45, 48, 47, 46,
};

// Eclairs de la dalle nurserie.
static Sint16 table_icon3[] =
{
    4,      // nb
    53, 54, 55, 56,
};

// Eclairs n-s entre les tours.
static Sint16 table_icon4[] =
{
    8 * 4,      // nb
    74, 76, 78, 76, 74, 78, 74, 76,
    78, 76, 74, 76, 74, 76, 78, 76,
    74, 78, 76, 74, 76, 78, 74, 76,
    78, 74, 76, 78, 74, 76, 74, 78,
};

// Eclairs e-o entre les tours.
static Sint16 table_icon5[] =
{
    8 * 4,      // nb
    75, 77, 75, 79, 75, 77, 79, 75,
    79, 77, 75, 77, 75, 79, 75, 79,
    75, 77, 79, 75, 75, 77, 79, 75,
    79, 75, 77, 79, 75, 79, 75, 79,
};

// Explosion de la dynamite.
static Sint16 table_icon6[] =
{
    7 + 30, // nb
    91, 90, 89, 88, 89, 90, 91,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

// Mèche de la dynamite.
static Sint16 table_icon7[] =
{
    12 * 4, // nb
    86, 87, 86, 87, 86, 87, 86, 87, 86, 87, 86, 87,
    86, 87, 86, 87, 86, 87, 86, 87, 86, 87, 86, 87,
    86, 87, 86, 87, 86, 87, 86, 87, 86, 87, 86, 87,
    86, 87, 86, 87, 86, 87, 86, 87, 86, 87, 86, 87,
};

// Rayons de  l'électrocuteur.
static Sint16 table_icon8[] =
{
    10 * 10,    // nb
    75, -1, -1, 74, -1, 75, -1, 74, 75, -1,
    74, 73, -1, 75, 74, 73, 72, 74, -1, 72,
    73, 74, 72, 73, 75, 73, 72, 73, 74, 72,
    75, 72, 73, 72, -1, 74, 73, 72, 75, 74,
    -1, 73, 74, 75, -1, 74, 75, -1, 75, -1,
    74, 75, -1, -1, 74, -1, -1, -1, 75, -1,
    -1, -1, -1, -1, -1, 74, -1, -1, -1, 75,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, 75, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, 74,
};

// Blupi écrasé.
static Sint16 table_icon9[] =
{
    10 * 10,    // nb
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
    76, 76, 76, 76, 76, 76, 76, 76, 76, 76,
};

// Blupi malade écrasé.
static Sint16 table_icon10[] =
{
    10 * 10,    // nb
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
    77, 77, 77, 77, 77, 77, 77, 77, 77, 77,
};

// Eclairs de la dalle téléporteur.
static Sint16 table_icon11[] =
{
    4,      // nb
    81, 82, 83, 84,
};

Sint16 *GetListIcons (Sint32 rank)
{
    switch (rank)
    {
    case 1:
        return table_icon1;
    case 2:
        return table_icon2;
    case 3:
        return table_icon3;
    case 4:
        return table_icon4;
    case 5:
        return table_icon5;
    case 6:
        return table_icon6;
    case 7:
        return table_icon7;
    case 8:
        return table_icon8;
    case 9:
        return table_icon9;
    case 10:
        return table_icon10;
    case 11:
        return table_icon11;
    }

    return nullptr;
}

/**
 * \brief Remove all animated environments.
 */
void CDecor::MoveFlush()
{
    Sint32      i, x, y;

    for (i = 0 ; i < MAXMOVE ; i++)
        m_move[i].bExist = false;

    for (x = 0 ; x < MAXCELX / 2 ; x++)
    {
        for (y = 0 ; y < MAXCELY / 2 ; y++)
            m_decor[x][y].rankMove = -1;
    }
}

/**
 * \brief Get the fire duration.
 *
 * \returns the duration.
 */
Sint32 CDecor::MoveMaxFire()
{
    if (m_skill >= 1)
        return (MAXFIRE / 4) * 3;
    return MAXFIRE;
}

/**
 * \brief Initialize all environment perpetual movements.
 */
void CDecor::MoveFixInit()
{
    Sint32      x, y;

    MoveFlush();

    for (x = 0 ; x < MAXCELX ; x += 2)
    {
        for (y = 0 ; y < MAXCELY ; y += 2)
        {
            // Démarre le feu.
            if (m_decor[x / 2][y / 2].fire > 0 &&
                m_decor[x / 2][y / 2].fire < MoveMaxFire())
                MoveStartFire (GetCel (x, y));

            // Démarre les éclairs entre les tours.
            if (m_decor[x / 2][y / 2].objectIcon == 10000) // éclair n-s
            {
                if (MoveCreate (GetCel (x, y), -1, false, CHOBJECT, -1,
                                -1, -1, 9999, 1, 0, true))
                {
                    MoveAddIcons (GetCel (x, y), 4, true); // éclairs n-s
                }
            }

            if (m_decor[x / 2][y / 2].objectIcon == 10001) // éclair e-o
            {
                if (MoveCreate (GetCel (x, y), -1, false, CHOBJECT, -1,
                                -1, -1, 9999, 1, 0, true))
                {
                    MoveAddIcons (GetCel (x, y), 5, true); // éclairs e-o
                }
            }
        }
    }
}

/**
 * \brief Create a new animated environment.
 *
 * If \p bMisc is true, we keep 10 movements back for important
 * actions (\p bMisc == false).
 *
 * \param[in] cel - ?
 * \param[in] rankBlupi - ?
 * \param[in] bFloor - ?
 * \param[in] channel - ?
 * \param[in] icon - ?
 * \param[in] maskChannel - ?
 * \param[in] maskIcon - ?
 * \param[in] total - ?
 * \param[in] delai - ?
 * \param[in] stepY - ?
 * \param[in] bMisc - ?
 * \param[in] bNotIfExist - ?
 * \returns true on success.
 */
bool CDecor::MoveCreate (POINT cel, Sint32 rankBlupi, bool bFloor,
                         Sint32 channel, Sint32 icon,
                         Sint32 maskChannel, Sint32 maskIcon,
                         Sint32 total, Sint32 delai, Sint32 stepY,
                         bool bMisc, bool bNotIfExist)
{
    Sint32      rank, max;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y)
        {
            if (bNotIfExist)
                return false;
            goto create;
        }
    }

    max = 0;
    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (!m_move[rank].bExist)
        {
            if (bMisc && max > MAXMOVE - 10)
                return false;
            goto create;
        }
        max ++;
    }
    return false;

create:
    m_move[rank].bExist      = true;
    m_move[rank].cel         = cel;
    m_move[rank].rankBlupi   = rankBlupi;
    m_move[rank].bFloor      = bFloor;
    m_move[rank].channel     = channel;
    m_move[rank].icon        = icon;
    m_move[rank].maskChannel = maskChannel;
    m_move[rank].maskIcon    = maskIcon;
    m_move[rank].total       = total;
    m_move[rank].delai       = delai;
    m_move[rank].stepY       = stepY;
    m_move[rank].cTotal      = 0;
    m_move[rank].cDelai      = 0;
    m_move[rank].rankMoves   = 0;
    m_move[rank].rankIcons   = 0;
    m_move[rank].phase       = 0;

    m_decor[cel.x / 2][cel.y / 2].rankMove = rank;
    return true;
}

// Ajoute un mouvement.

bool CDecor::MoveAddMoves (POINT cel, Sint32 rankMoves)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y)
        {
            m_move[rank].rankMoves = rankMoves;
            m_move[rank].phase  = 0;

            return true;
        }
    }

    return false;
}

// Ajoute un mouvement.

bool CDecor::MoveAddIcons (POINT cel, Sint32 rankIcons, bool bContinue)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y)
        {
            m_move[rank].rankIcons = rankIcons;
            if (!bContinue)
                m_move[rank].phase = 0;

            if (rankIcons == 4 || rankIcons == 5)    // éclairs entre tours ?
                m_move[rank].cTotal = Random (0, 10);

            return true;
        }
    }

    return false;
}

/**
 * \brief Start the fire on a cell.
 *
 * \param[in] cel - Position.
 * \returns true if possible.
 */
bool CDecor::MoveStartFire (POINT cel)
{
    Sint32      channel, icon;

    cel.x = (cel.x / 2) * 2;
    cel.y = (cel.y / 2) * 2;

    channel = m_decor[cel.x / 2][cel.y / 2].objectChannel;
    icon    = m_decor[cel.x / 2][cel.y / 2].objectIcon;

    if (channel == CHOBJECT
        && (   (icon >=   6 && icon <= 11) // trees ?
            || (icon >=  65 && icon <= 71) // palissade ?
            ||  icon ==  61                // cabane ?
            ||  icon ==  36                // planches ?
            ||  icon ==  60                // tomatoes ?
            ||  icon ==  63                // eggs ?
            ||  icon == 113                // house ?
            ||  icon == 121                // mine de fer ?
            ||  icon == 122                // mine de fer ?
           )
       )
    {
        if (!MoveCreate (cel, -1, false, CHOBJECT, -1,
                         -1, -1, 9999, 1, 0, true))
            return false;

        MoveAddIcons (cel, 1, true); // small fire
        m_decor[cel.x / 2][cel.y / 2].fire = 2;
        return true;
    }

    // If there is an other object, then no fire.
    if (channel >= 0)
        return false;

    channel = m_decor[cel.x / 2][cel.y / 2].floorChannel;
    icon    = m_decor[cel.x / 2][cel.y / 2].floorIcon;

    if (channel == CHFLOOR &&
        (icon == 20 ||    // herbe foncée ?
         (icon >= 59 && icon <= 64)))   // bridge ?
    {
        if (!MoveCreate (cel, -1, false, CHOBJECT, -1,
                         -1, -1, 9999, 1, 0, true))
            return false;

        MoveAddIcons (cel, 1, true); // small fire
        m_decor[cel.x / 2][cel.y / 2].fire = 2;
        return true;
    }

    return false;
}

// Démarre le feu si c'est possible par proximité.

void CDecor::MoveProxiFire (POINT cel)
{
    Sint32      cx, cy, xx, yy, x, y, channel, icon;

    static Sint16 tableInd[5] = {2, 1, 3, 0, 4};
    static Sint16 tablePos[5 * 5 * 2] =
    {
        0, 0, -1, -2,  0, -2, +1, -2,  0, 0,
        -2, -1, -1, -1,  0, -1, +1, -1, +2, -1,
        -2, 0, -1, 0,  0, 0, +1, 0, +2, 0,
        -2, +1, -1, +1,  0, +1, +1, +1, +2, +1,
        0, 0, -1, +2,  0, +2, +1, +2,  0, 0,
    };

    for (cx = 0 ; cx < 5 ; cx++)
    {
        for (cy = 0 ; cy < 5 ; cy++)
        {
            xx = tablePos[ (tableInd[cx] + tableInd[cy] * 5) * 2 + 0];
            yy = tablePos[ (tableInd[cx] + tableInd[cy] * 5) * 2 + 1];

            if (xx == 0 && yy == 0)
                continue;

            x = cel.x + xx * 2;
            y = cel.y + yy * 2;

            if (x < 0 || x >= MAXCELX ||
                y < 0 || y >= MAXCELX)
                continue;

            if (m_decor[x / 2][y / 2].fire != 0)
                continue;  // brule déjà ?

            x = ((cel.x + xx) / 2) * 2;
            y = ((cel.y + yy) / 2) * 2;

            // Mur ou rochers entre le feu et l'objet ?
            channel = m_decor[x / 2][y / 2].objectChannel;
            icon    = m_decor[x / 2][y / 2].objectIcon;
            if (channel == CHOBJECT &&
                ((icon >= 20 && icon <= 26) ||
                 (icon >= 37 && icon <= 43)))
                continue;

            x = cel.x + xx * 2;
            y = cel.y + yy * 2;

            // Mur ou rochers entre le feu et l'objet ?
            channel = m_decor[x / 2][y / 2].objectChannel;
            icon    = m_decor[x / 2][y / 2].objectIcon;
            if (channel == CHOBJECT &&
                ((icon >= 20 && icon <= 26) ||
                 (icon >= 37 && icon <= 43)))
                continue;

            // Démarre éventuellement un seul foyer.
            if (MoveStartFire (GetCel (x, y)))
                return;
        }
    }
}

// Fait évoluer le feu.

void CDecor::MoveFire (Sint32 rank)
{
    Sint32      x, y, icon, newIcon;
    POINT   pos;

    x = (m_move[rank].cel.x / 2) * 2;
    y = (m_move[rank].cel.y / 2) * 2;

    if (m_decor[x / 2][y / 2].fire == 0 ||
        m_decor[x / 2][y / 2].fire >= MoveMaxFire())
        return;

    m_decor[x / 2][y / 2].fire ++;

    pos = ConvCelToPos (GetCel (x, y));
    m_pSound->PlayImage (SOUND_FEU, pos);

    if (m_decor[x / 2][y / 2].objectIcon >= 0) // objet qui brule ?
    {
        if (m_decor[x / 2][y / 2].fire > MoveMaxFire() / 2 &&
            m_decor[x / 2][y / 2].fire % 50 == 0) // pas trop souvent !
        {
            MoveProxiFire (GetCel (x, y)); // boutte le feu
        }

        // Début petites flammes.
        if (m_decor[x / 2][y / 2].fire == 2)
            MoveStartFire (GetCel (x, y));

        // Début grandes flammes.
        if (m_decor[x / 2][y / 2].fire == (MoveMaxFire() - DIMOBJY * 2) / 2)
        {
            MoveAddIcons (GetCel (x, y), 2, true); // grandes flammes
        }

        // Début objet squelette.
        if (m_decor[x / 2][y / 2].fire == MoveMaxFire() - DIMOBJY * 2)
        {
            icon = m_decor[x / 2][y / 2].floorIcon;
            if (icon == 20)    // herbe foncée ?
            {
                PutFloor (GetCel (x, y), CHFLOOR, 19); // herbe brulée
            }

            icon = m_decor[x / 2][y / 2].objectIcon;
            newIcon = -1;
            if (icon >= 6 && icon <= 11)
                newIcon = icon - 6 + 30; // arbres ?
            if (icon ==  61)
                newIcon =  62;  // cabane ?
            if (icon == 113)
                newIcon =  15;  // maison ?
            if (icon == 121)
                newIcon = 126;  // mine de fer ?
            if (icon == 122)
                newIcon = 126;  // mine de fer ?
            MoveCreate (GetCel (x, y), -1, false, CHOBJECT, newIcon,
                        -1, -1, DIMOBJY * 2, 1, -1 * 50);
            MoveAddIcons (GetCel (x, y), 2, true); // grandes flammes
        }

        // Fin grandes flammes.
        if (m_decor[x / 2][y / 2].fire == MoveMaxFire() - DIMOBJY)
        {
            MoveAddIcons (GetCel (x, y), 1, true); // petites flammes
        }

        // Fin feu.
        if (m_decor[x / 2][y / 2].fire == MoveMaxFire() - 1)
        {
            MoveFinish (GetCel (x, y));
            icon = m_decor[x / 2][y / 2].objectIcon;
            if (icon == 36               ||    // planches ?
                icon == 60               ||  // tomates ?
                icon == 63               ||  // oeufs ?
                icon == 113              ||  // maison ?
                (icon >= 65 && icon <= 71))   // palissade ?
            {
                m_decor[x / 2][y / 2].objectChannel = -1;
                m_decor[x / 2][y / 2].objectIcon    = -1;
            }
            m_decor[x / 2][y / 2].fire = MoveMaxFire(); // déjà brulé
        }
    }
    else    // sol qui brule ?
    {
        if (m_decor[x / 2][y / 2].fire > DIMOBJY &&
            m_decor[x / 2][y / 2].fire % 50 == 0) // pas trop souvent !
        {
            MoveProxiFire (GetCel (x, y)); // boutte le feu
        }

        // Début petites flammes.
        if (m_decor[x / 2][y / 2].fire == 2)
            MoveStartFire (GetCel (x, y));

        // Milieu feu.
        if (m_decor[x / 2][y / 2].fire == DIMOBJY)
        {
            icon = m_decor[x / 2][y / 2].floorIcon;
            switch (icon)
            {
            case 20:    // herbe foncée ?
                PutFloor (GetCel (x, y), CHFLOOR, 19); // herbe brulée
                break;
            case 59:    // pont ?
                PutFloor (GetCel (x, y), CHFLOOR, 2); // rivage
                break;
            case 61:    // pont ?
                PutFloor (GetCel (x, y), CHFLOOR, 4); // rivage
                break;
            case 62:    // pont ?
                PutFloor (GetCel (x, y), CHFLOOR, 3); // rivage
                break;
            case 64:    // pont ?
                PutFloor (GetCel (x, y), CHFLOOR, 5); // rivage
                break;
            case 60:
            case 63:    // pont ?
                PutFloor (GetCel (x, y), CHFLOOR, 14); // eau
                break;
            }
        }

        // Fin feu.
        if (m_decor[x / 2][y / 2].fire == DIMOBJY * 2 - 1)
        {
            MoveFinish (GetCel (x, y));
            m_decor[x / 2][y / 2].objectChannel = -1;
            m_decor[x / 2][y / 2].objectIcon    = -1;
            m_decor[x / 2][y / 2].fire = MoveMaxFire(); // déjà brulé
        }
    }
}

// Fait évoluer tous les décors animés.

void CDecor::MoveStep (bool bFirst)
{
    Sint32      rank, rankBlupi;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist)
        {
            if (!bFirst &&
                (m_move[rank].rankIcons == 4 ||  // éclair entre tours ?
                 m_move[rank].rankIcons == 5))
                continue;

            MoveFire (rank);

            // Si le blupi travaillant ici est stoppé, on
            // arrête aussi la construction.
            rankBlupi = m_move[rank].rankBlupi;
            if (rankBlupi != -1)
            {
                if (m_blupi[rankBlupi].action == ACTION_STOP  ||
                    m_blupi[rankBlupi].action == ACTION_STOPf)
                    continue;
            }

            m_move[rank].phase ++;
            if (m_move[rank].phase > 32000)
                m_move[rank].phase = 0;

            m_move[rank].cDelai ++;
            if (m_move[rank].cDelai >= m_move[rank].delai)    // délai écoulé ?
            {
                m_move[rank].cDelai = 0;

                m_move[rank].cTotal ++;
                if (m_move[rank].cTotal > 32000)
                    m_move[rank].cTotal = 0;

                if (m_move[rank].total != 9999 &&
                    m_move[rank].cTotal >= m_move[rank].total)
                    MoveFinish (m_move[rank].cel);
            }
        }
    }
}

// Termine un mouvement pour une cellule donnée.

void CDecor::MoveFinish (POINT cel)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y)
        {
            if (m_move[rank].channel >= 0 &&
                m_move[rank].icon    >= 0)
            {
                if (m_move[rank].bFloor)
                    PutFloor (cel, m_move[rank].channel, m_move[rank].icon);
                else
                    PutObject (cel, m_move[rank].channel, m_move[rank].icon);
            }

            m_decor[cel.x / 2][cel.y / 2].rankMove = -1;
            m_move[rank].bExist = false;
        }
    }
}

// Termine un mouvement, d'après le rang du blupi.

void CDecor::MoveFinish (Sint32 rankBlupi)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].rankBlupi == rankBlupi)
            MoveFinish (m_move[rank].cel);
    }
}

// Vérifie si une cellule est déjà utilisée.

bool CDecor::MoveIsUsed (POINT cel)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y)
            return true;
    }

    return false;
}

// Retourne l'objet en construction à un endroit donné.

bool CDecor::MoveGetObject (POINT cel, Sint32 &channel, Sint32 &icon)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y &&
            m_move[rank].bFloor == false)
        {
            channel = m_move[rank].channel;
            icon    = m_move[rank].icon;
            return true;
        }
    }

    return false;
}

// Modifie un objet en construction à un endroit donné.

bool CDecor::MovePutObject (POINT cel, Sint32 channel, Sint32 icon)
{
    Sint32      rank;

    for (rank = 0 ; rank < MAXMOVE ; rank++)
    {
        if (m_move[rank].bExist &&
            m_move[rank].cel.x == cel.x &&
            m_move[rank].cel.y == cel.y &&
            m_move[rank].bFloor == false)
        {
            m_move[rank].channel = channel;
            m_move[rank].icon    = icon;
            return true;
        }
    }

    return false;
}

