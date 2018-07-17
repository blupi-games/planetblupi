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

#pragma once

#include <SDL_stdinc.h>

// Méta opérations (goal).

// clang-format off
#define GOAL_TERM           0   //
#define GOAL_GOHILI         1   // dx,dy, bPass
#define GOAL_GOHILI2        2   // dx,dy, bPass
#define GOAL_GOBLUPI        3   // dx,dy, bPass
#define GOAL_PUTFLOOR       4   // dx,dy, channel,icon
#define GOAL_PUTOBJECT      5   // dx,dy, channel,icon
#define GOAL_BUILDFLOOR     6   // dx,dy, ch,i,mch, mi,total,delai,step
#define GOAL_BUILDOBJECT    7   // dx,dy, ch,i,mch, mi,total,delai,step
#define GOAL_ACTION         8   // action,direction
#define GOAL_INTERRUPT      9   // niveau (0..2)
#define GOAL_ENERGY         10  // niveau minimum requis
#define GOAL_ADDMOVES       11  // dx,dy,no
#define GOAL_OTHER          12  // channel,first,last,fitst,last,action
#define GOAL_FINISHMOVE     13  //
#define GOAL_TAKE           14  // dx,dy
#define GOAL_DEPOSE         15  //
#define GOAL_GROUP          16  // nb
#define GOAL_WORK           17  // dx,dy
#define GOAL_TESTOBJECT     18  // dx,dy, channel,icon
#define GOAL_FIX            19  // dx,dy
#define GOAL_OTHERFIX       20  // channel,first,last,first,last,action
#define GOAL_ADDICONS       21  // dx,dy,no
#define GOAL_NEWBLUPI       22  // dx,dy
#define GOAL_SOUND          23  // sound
#define GOAL_REPEAT         24  // true/false
#define GOAL_OTHERLOOP      25  // action
#define GOAL_NEXTLOOP       26  //
#define GOAL_ARRANGEOBJECT  27  // dx,dy
#define GOAL_LABO           28  //
#define GOAL_CACHE          29  // true/false, bDynamite
#define GOAL_DELETE         30  //
#define GOAL_ELECTRO        31  // dx,dy,no
#define GOAL_NEWPERSO       32  // dx,dy, perso
#define GOAL_USINEBUILD     33  // dx,dy,
#define GOAL_USINEFREE      34  // dx,dy,
#define GOAL_EXPLOSE1       35  // dx,dy
#define GOAL_EXPLOSE2       36  // dx,dy
#define GOAL_VEHICULE       37  // type
#define GOAL_TAKEOBJECT     38  // dx,dy, channel,icon
#define GOAL_FLOORJUMP      39  // channel,icon,action
#define GOAL_ADDDRAPEAU     40  // dx,dy
#define GOAL_AMORCE         41  // dx,dy
#define GOAL_MALADE         42  // bMalade
#define GOAL_IFTERM         43  // dx,dy
#define GOAL_IFDEBARQUE     44  // dx,dy
#define GOAL_ISNOMALADE     45  //
#define GOAL_SKIPSKILL      46  // skill,d
#define GOAL_TELEPORTE      47  // dx,dy
#define GOAL_ACTUALISE      48  //
#define GOAL_WAITFREE       49  // dx,dy
// clang-format on

Sint16 *      GetTableGoal (Sint32 action);
extern Sint16 table_goal_nbop[];
