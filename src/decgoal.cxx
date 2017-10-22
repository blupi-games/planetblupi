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

#include "decgoal.h"
#include "decor.h"
#include "def.h"

// clang-format off
Sint16 table_goal_nbop[] =
{
    0,      // term
    3,      // gohili
    3,      // gohili2
    3,      // goblupi
    4,      // putfloor
    4,      // putobject
    9,      // buildfloor
    9,      // buildobject
    2,      // action
    1,      // interrupt
    1,      // energy
    3,      // addmoves
    6,      // other
    0,      // finishmove
    2,      // take
    0,      // depose
    1,      // group
    2,      // work
    4,      // testobject
    2,      // fix
    6,      // otherfix
    3,      // addicons
    2,      // newblupi
    1,      // sound
    1,      // repeat
    1,      // otherloop
    0,      // nextloop
    2,      // arrangeobject
    0,      // labo
    2,      // cache
    0,      // delete
    3,      // electro
    3,      // newperso
    2,      // usinebuild
    2,      // usinefree
    2,      // explose1
    2,      // explose2
    1,      // vehicule
    4,      // takeobject
    3,      // floorjump
    2,      // adddrapeau
    2,      // amorce
    1,      // malade
    2,      // ifterm
    2,      // ifdebarque
    0,      // isnomalade
    2,      // skipskill
    2,      // teleporte
    0,      // actualise
    2,      // waitfree
};

// Va, mon petit.
static Sint16 table_goal_go[] =
{
    EV_ACTION_GO,
    GOAL_GOHILI,        0, 0, false,
    GOAL_TERM,
    0
};

// Blupi va dans sa maison.
static Sint16 table_goal_maison[] =
{
    EV_ACTION_HOUSE,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_HAPPY, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi bucheron.
static Sint16 table_goal_abat1[] =
{
    EV_ACTION_ABAT1,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 30, -1, -1, DIMOBJY + 20, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, -1, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      -1, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_abat2[] =
{
    EV_ACTION_ABAT2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 7,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 31, -1, -1, DIMOBJY + 20, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, -1, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      -1, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_abat3[] =
{
    EV_ACTION_ABAT3,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   -1, 0, CHOBJECT, 8,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 32, -1, -1, DIMOBJY + 20, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      -1, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      -1, -1, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      -1, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_abat4[] =
{
    EV_ACTION_ABAT4,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, 0, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, 0, CHOBJECT, 9,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, 0,
    GOAL_BUILDOBJECT,  0, 0, CHOBJECT, 33, -1, -1, DIMOBJY, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   0, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_abat5[] =
{
    EV_ACTION_ABAT5,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, 0, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, 0, CHOBJECT, 10,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, 0,
    GOAL_BUILDOBJECT,  0, 0, CHOBJECT, 34, -1, -1, DIMOBJY, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   0, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_abat6[] =
{
    EV_ACTION_ABAT6,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, 0, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, 0, CHOBJECT, 11,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, 0,
    GOAL_BUILDOBJECT,  0, 0, CHOBJECT, 35, -1, -1, DIMOBJY, 1, -1 * 100,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ADDMOVES,      0, 0, 1,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,

    GOAL_BUILDOBJECT,   0, 0, CHOBJECT, 36, -1, -1, 80 / 10, 10, -10 * 100,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_OTHER,         CHOBJECT, 6, 11, -1, -1, EV_ACTION_ABAT1,
    GOAL_TERM,
    0
};

// Blupi tailleur de pierre.
static Sint16 table_goal_roc1[] =
{
    EV_ACTION_ROC1,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 38, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 39, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 40, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 41, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 42, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc2[] =
{
    EV_ACTION_ROC2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 39, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 40, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 41, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 42, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc3[] =
{
    EV_ACTION_ROC3,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 40, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 41, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 42, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc4[] =
{
    EV_ACTION_ROC4,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 41, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 42, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc5[] =
{
    EV_ACTION_ROC5,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 42, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc6[] =
{
    EV_ACTION_ROC6,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 43, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

static Sint16 table_goal_roc7[] =
{
    EV_ACTION_ROC7,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, true,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 44, -1, -1, 120 / 10, 10, -10 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,

    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 37, 43, -1, -1, EV_ACTION_ROC1,
    GOAL_TERM,
    0
};

// Construction d'une cabane.
static Sint16 table_goal_build1[] =
{
    EV_ACTION_BUILD1,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, 16, CHMASK1, 0, 16, 14, 1 * 100, // briques
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    // échaffaudage
    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 62, -1, -1, DIMOBJY / 10, 18, 10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, +1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -2, true,
    GOAL_GOBLUPI,       -2, 0, true,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       +2, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    // maison
    GOAL_BUILDOBJECT,   -2, 0, CHOBJECT, 61, -1, -1, DIMOBJY / 10, 20, 10 * 100,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'une nurserie.
static Sint16 table_goal_build2[] =
{
    EV_ACTION_BUILD2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, 52, CHMASK1, 0, 16, 14, 1 * 100, // nurserie
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_PUTOBJECT,     -1, 0, -1, -1, // enlève les planches
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, +1, true,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'un laboratoire.
static Sint16 table_goal_build3[] =
{
    EV_ACTION_BUILD3,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, 16, CHMASK1, 0, 16, 14, 1 * 100, // briques
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       +1, 0, true,
    // laboratoire
    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 28, -1, -1, DIMOBJY / 10, 20, 10 * 100,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'une mine.
static Sint16 table_goal_build4[] =
{
    EV_ACTION_BUILD4,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         2,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    // échaffaudage
    GOAL_BUILDOBJECT,   0, -1, CHOBJECT, 126, -1, -1, DIMOBJY / 10, 18, 10 * 100,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -2, true,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    // mine
    GOAL_BUILDOBJECT,   -2, 0, CHOBJECT, 122, -1, -1, DIMOBJY / 10, 20, 10 * 100,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'une usine.
static Sint16 table_goal_build5[] =
{
    EV_ACTION_BUILD5,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, 16, CHMASK1, 0, 16, 14, 1 * 100, // briques
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       +1, 0, true,
    // usine
    GOAL_BUILDOBJECT,   -1, 0, CHOBJECT, 120, -1, -1, DIMOBJY / 10, 20, 10 * 100,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDSOURD, DIRECT_W,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'un téléporteur.
static Sint16 table_goal_build6[] =
{
    EV_ACTION_BUILD6,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, 80, CHMASK1, 0, 16, 14, 1 * 100, // téléporteur
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_PUTOBJECT,     -1, 0, -1, -1, // enlève les planches
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_BUILDSEC, DIRECT_S,
    GOAL_GOBLUPI,       +1, +1, true,
    GOAL_FINISHMOVE,
    GOAL_GOBLUPI,       0, -2, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_TERM,
    0
};

// Construction d'un mur.
static Sint16 table_goal_mur[] =
{
    EV_ACTION_WALL,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 26, -1, -1, DIMOBJY / 5, 40, 5 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_PUTOBJECT,     0, 0, -1, -1, // enlève les pierres
    GOAL_GOBLUPI,       +1, +1, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_TCHAO, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Construction d'une tour.
static Sint16 table_goal_tour[] =
{
    EV_ACTION_TOWER,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 27, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_PUTOBJECT,     -1, 0, -1, -1, // enlève les pierres
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_PIOCHEPIERRE, DIRECT_E,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_BUILDBREF, DIRECT_W,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_ARRANGEOBJECT, -1, -1,
    GOAL_ACTION,        ACTION_TCHAO, DIRECT_E,
    GOAL_TERM,
    0
};

// Construction d'une palissade.
static Sint16 table_goal_palis[] =
{
    EV_ACTION_PALIS,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 71, -1, -1, DIMOBJY / 10, 20, 10 * 100,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_PICKAXE, DIRECT_E,
    GOAL_PUTOBJECT,     0, 0, -1, -1, // enlève les planches
    GOAL_GOBLUPI,       +1, +1, true,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Transporte (est).
static Sint16 table_goal_carry[] =
{
    EV_ACTION_CARRY,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, -2, -2, -1, -1, 9 + 18, 1, 1 * 100,
    GOAL_ADDMOVES,     0, -1, 2,
    GOAL_ACTION,        ACTION_CARRY, DIRECT_E,
    GOAL_TAKE,          0, -1,
    GOAL_TERM,
    0
};

// Transporte (sud).
static Sint16 table_goal_carry2[] =
{
    EV_ACTION_CARRY2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, -2, -2, -1, -1, 9 + 18, 1, 1 * 100,
    GOAL_ADDMOVES,     -1, 0, 3,
    GOAL_ACTION,        ACTION_CARRY, DIRECT_S,
    GOAL_TAKE,          -1, 0,
    GOAL_TERM,
    0
};

// Repose (est).
static Sint16 table_goal_depose[] =
{
    EV_ACTION_DROP,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         6,
    GOAL_IFTERM,       +1, 0, // emplacement libre ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_PUTOBJECT,    0, -1, -3, -3, // l'objet transporté
    GOAL_BUILDOBJECT,  0, -1, -2, -2, -1, -1, 11, 1, 1 * 100,
    GOAL_ADDMOVES,     0, -1, 4,
    GOAL_DEPOSE,
    GOAL_ACTION,        ACTION_DROP, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Repose (sud).
static Sint16 table_goal_depose2[] =
{
    EV_ACTION_DROP2,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_GROUP,         6,
    GOAL_IFTERM,       0, +1, // emplacement libre ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_PUTOBJECT,    -1, 0, -3, -3, // l'objet transporté
    GOAL_BUILDOBJECT,  -1, 0, -2, -2, -1, -1, 11, 1, 1 * 100,
    GOAL_ADDMOVES,     -1, 0, 5,
    GOAL_DEPOSE,
    GOAL_ACTION,        ACTION_DROP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Repose des oeufs sur la nurserie.
static Sint16 table_goal_newblupi[] =
{
    EV_ACTION_NEWBLUPI,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         5,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_PUTOBJECT,    0, -1, -3, -3, // l'objet transporté
    GOAL_BUILDOBJECT,  0, -1, -2, -2, -1, -1, 11, 1, 1 * 100,
    GOAL_ADDMOVES,     0, -1, 4,
    GOAL_DEPOSE,
    GOAL_ACTION,        ACTION_DROP, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         3,
    GOAL_PUTOBJECT,    +1, -1, CHOBJECT, 64, // 4 oeufs
    GOAL_BUILDFLOOR,   +1, -1, CHFLOOR, -1, -1, -1, 100, 1, 1 * 100,
    GOAL_ADDICONS,     +1, -1, 3,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_NEWBLUPI,      +1, -1,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_TERM,
    0
};

// Cultive des tomates.
static Sint16 table_goal_cultive[] =
{
    EV_ACTION_CULTIVE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +2, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_FIX,           -2, 0,
    GOAL_REPEAT,        true,
    GOAL_OTHERFIX,      CHFLOOR, 1, 1, 19, 32, EV_ACTION_CULTIVE2,
    GOAL_TERM,
    0
};

static Sint16 table_goal_cultive2[] =
{
    EV_ACTION_CULTIVE2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,   -1, -1, CHFLOOR, 57, CHMASK1, 0, 16, 6, 1 * 100, // terre
    GOAL_ACTION,        ACTION_BECHE, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_BECHE, DIRECT_E,
    GOAL_FINISHMOVE,

    GOAL_BUILDOBJECT,   0, -1, CHOBJECT, 57, -1, -1, DIMOBJY, 1, 1 * 100,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_FINISHMOVE,

    GOAL_BUILDOBJECT,   0, -1, CHOBJECT, 58, -1, -1, DIMOBJY, 1, 1 * 100,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_FINISHMOVE,

    GOAL_BUILDOBJECT,   0, 0, CHOBJECT, 59, -1, -1, DIMOBJY, 1, 1 * 100,
    GOAL_ACTION,        ACTION_BECHE, DIRECT_E,
    GOAL_ACTION,        ACTION_BECHE, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_FINISHMOVE,

    GOAL_BUILDOBJECT,   0, -1, CHOBJECT, 60, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_ARROSE, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_FINISHMOVE,

    GOAL_BUILDFLOOR,    0, -1, CHFLOOR, 1, CHMASK1, 0, 16, 1, 1 * 100, // herbe
    GOAL_FINISHMOVE,

    GOAL_OTHERFIX,      CHFLOOR, 1, 1, 19, 32, EV_ACTION_CULTIVE2,
    GOAL_TERM,
    0
};

// Bouffe des tomates (est).
static Sint16 table_goal_mange[] =
{
    EV_ACTION_EAT,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 60, // tomates ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, -1, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,

    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Bouffe des tomates (sud).
static Sint16 table_goal_mange2[] =
{
    EV_ACTION_EAT2,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   -1, 0, CHOBJECT, 60, // tomates ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, -1, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,
    GOAL_ACTION,        ACTION_EAT, DIRECT_E,

    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, 0, -1, -1,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_TERM,
    0
};

// Boit à la bouteille (est).
static Sint16 table_goal_boit[] =
{
    EV_ACTION_DRINK,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         5,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 80, // bouteille ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_ACTION,       ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Boit à la bouteille (sud).
static Sint16 table_goal_boit2[] =
{
    EV_ACTION_DRINK2,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_GROUP,         5,
    GOAL_TESTOBJECT,   -1, 0, CHOBJECT, 80, // bouteille ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_PUTOBJECT,    -1, 0, -1, -1,
    GOAL_ACTION,       ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_DRINK, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_TERM,
    0
};

// Cueille des fleurs.
static Sint16 table_goal_fleur1[] =
{
    EV_ACTION_FLOWER1,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, true,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 81,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 82, -1, -1, DIMOBJY + 20, 1, -1 * 100,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_E,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE1, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 81, 81, -1, -1, EV_ACTION_FLOWER1,
    GOAL_TERM,
    0
};

// Cueille des fleurs.
static Sint16 table_goal_fleur2[] =
{
    EV_ACTION_FLOWER2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, true,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 83,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 84, -1, -1, DIMOBJY + 20, 1, -1 * 100,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_E,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE2, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 83, 83, -1, -1, EV_ACTION_FLOWER2,
    GOAL_TERM,
    0
};

// Cueille des fleurs.
static Sint16 table_goal_fleur3[] =
{
    EV_ACTION_FLOWER3,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, true,
    GOAL_GROUP,         4,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 94,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 95, -1, -1, DIMOBJY + 20, 1, -1 * 100,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_E,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_FLOWER,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_S,
    GOAL_ACTION,        ACTION_CUEILLE3, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_OTHER,         CHOBJECT, 94, 94, -1, -1, EV_ACTION_FLOWER3,
    GOAL_TERM,
    0
};

// Transforme des fleurs.
static Sint16 table_goal_labo[] =
{
    EV_ACTION_LABO,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 29, // ferme la porte
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 29, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 10, // secoue
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         4,
    GOAL_LABO,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 28, // ouvre la porte
    GOAL_CACHE,        false, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_IFTERM,        0, -1,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi fait péter la dynamite.
static Sint16 table_goal_dynamite[] =
{
    EV_ACTION_DYNAMITE,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GOHILI2,       0, +1, false,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         5,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     0, -1, 7, // mèche
    GOAL_ACTION,        ACTION_MECHE, DIRECT_E,
    GOAL_SOUND,         SOUND_DYNAMITE,
    GOAL_GROUP,         4,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     0, -1, 6, // explosion
    GOAL_CACHE,        true, true,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,      0, -3,
    GOAL_EXPLOSE1,     -2, -1,
    GOAL_EXPLOSE1,     +2, -1,
    GOAL_EXPLOSE1,      0, +1,
    GOAL_EXPLOSE2,      0, -3,
    GOAL_EXPLOSE2,     -2, -1,
    GOAL_EXPLOSE2,     +2, -1,
    GOAL_EXPLOSE2,      0, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -2, -3,
    GOAL_EXPLOSE1,     +2, -3,
    GOAL_EXPLOSE1,     -2, +1,
    GOAL_EXPLOSE1,     +2, +1,
    GOAL_EXPLOSE2,     -2, -3,
    GOAL_EXPLOSE2,     +2, -3,
    GOAL_EXPLOSE2,     -2, +1,
    GOAL_EXPLOSE2,     +2, +1,
    GOAL_ACTION,        ACTION_DYNAMITE, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_FINISHMOVE,
    GOAL_CACHE,        false, true,
    GOAL_DELETE,       // blupi meurt brutalement
    GOAL_TERM,
    0
};

// Blupi fait péter la dynamite très fort.
static Sint16 table_goal_dynamite2[] =
{
    EV_ACTION_DYNAMITE2,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GOHILI2,       0, +1, false,
    //?     GOAL_ENERGY,        MAXENERGY/4,
    GOAL_GROUP,         5,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     0, -1, 7, // mèche
    GOAL_ACTION,        ACTION_MECHE, DIRECT_E,
    GOAL_SOUND,         SOUND_DYNAMITE,
    GOAL_GROUP,         5,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     0, -1, 6, // explosion
    GOAL_CACHE,        true, true,
    GOAL_DEPOSE,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,      0, -3,
    GOAL_EXPLOSE1,     -2, -1,
    GOAL_EXPLOSE1,     +2, -1,
    GOAL_EXPLOSE1,      0, +1,
    GOAL_EXPLOSE2,      0, -3,
    GOAL_EXPLOSE2,     -2, -1,
    GOAL_EXPLOSE2,     +2, -1,
    GOAL_EXPLOSE2,      0, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -2, -3,
    GOAL_EXPLOSE1,     +2, -3,
    GOAL_EXPLOSE1,     -2, +1,
    GOAL_EXPLOSE1,     +2, +1,
    GOAL_EXPLOSE2,     -2, -3,
    GOAL_EXPLOSE2,     +2, -3,
    GOAL_EXPLOSE2,     -2, +1,
    GOAL_EXPLOSE2,     +2, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         12 + 12,
    GOAL_EXPLOSE1,     -4, -3,
    GOAL_EXPLOSE1,     -4, -1,
    GOAL_EXPLOSE1,     -4, +1,
    GOAL_EXPLOSE1,     -2, -5,
    GOAL_EXPLOSE1,     -2, +3,
    GOAL_EXPLOSE1,      0, -5,
    GOAL_EXPLOSE1,      0, +3,
    GOAL_EXPLOSE1,     +2, -5,
    GOAL_EXPLOSE1,     +2, +3,
    GOAL_EXPLOSE1,     +4, -3,
    GOAL_EXPLOSE1,     +4, -1,
    GOAL_EXPLOSE1,     +4, +1,
    GOAL_EXPLOSE2,     -4, -3,
    GOAL_EXPLOSE2,     -4, -1,
    GOAL_EXPLOSE2,     -4, +1,
    GOAL_EXPLOSE2,     -2, -5,
    GOAL_EXPLOSE2,     -2, +3,
    GOAL_EXPLOSE2,      0, -5,
    GOAL_EXPLOSE2,      0, +3,
    GOAL_EXPLOSE2,     +2, -5,
    GOAL_EXPLOSE2,     +2, +3,
    GOAL_EXPLOSE2,     +4, -3,
    GOAL_EXPLOSE2,     +4, -1,
    GOAL_EXPLOSE2,     +4, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         16 + 16,
    GOAL_EXPLOSE1,     -6, -3,
    GOAL_EXPLOSE1,     -6, -1,
    GOAL_EXPLOSE1,     -6, +1,
    GOAL_EXPLOSE1,     -4, -5,
    GOAL_EXPLOSE1,     -4, +3,
    GOAL_EXPLOSE1,     -2, -7,
    GOAL_EXPLOSE1,     -2, +5,
    GOAL_EXPLOSE1,      0, -7,
    GOAL_EXPLOSE1,      0, +5,
    GOAL_EXPLOSE1,     +2, -7,
    GOAL_EXPLOSE1,     +2, +5,
    GOAL_EXPLOSE1,     +4, -5,
    GOAL_EXPLOSE1,     +4, +3,
    GOAL_EXPLOSE1,     +6, -3,
    GOAL_EXPLOSE1,     +6, -1,
    GOAL_EXPLOSE1,     +6, +1,
    GOAL_EXPLOSE2,     -6, -3,
    GOAL_EXPLOSE2,     -6, -1,
    GOAL_EXPLOSE2,     -6, +1,
    GOAL_EXPLOSE2,     -4, -5,
    GOAL_EXPLOSE2,     -4, +3,
    GOAL_EXPLOSE2,     -2, -7,
    GOAL_EXPLOSE2,     -2, +5,
    GOAL_EXPLOSE2,      0, -7,
    GOAL_EXPLOSE2,      0, +5,
    GOAL_EXPLOSE2,     +2, -7,
    GOAL_EXPLOSE2,     +2, +5,
    GOAL_EXPLOSE2,     +4, -5,
    GOAL_EXPLOSE2,     +4, +3,
    GOAL_EXPLOSE2,     +6, -3,
    GOAL_EXPLOSE2,     +6, -1,
    GOAL_EXPLOSE2,     +6, +1,
    GOAL_ACTION,        ACTION_DYNAMITE, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_FINISHMOVE,
    GOAL_CACHE,        false, true,
    GOAL_DELETE,       // blupi meurt brutalement
    GOAL_TERM,
    0
};

// Tracks fait péter la dynamite.
static Sint16 table_goal_t_dynamite[] =
{
    EV_ACTION_T_DYNAMITE,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_SOUND,         SOUND_DYNAMITE,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     -1, -1, 6, // explosion
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -1, -3,
    GOAL_EXPLOSE1,     -3, -1,
    GOAL_EXPLOSE1,     +1, -1,
    GOAL_EXPLOSE1,     -1, +1,
    GOAL_EXPLOSE2,     -1, -3,
    GOAL_EXPLOSE2,     -3, -1,
    GOAL_EXPLOSE2,     +1, -1,
    GOAL_EXPLOSE2,     -1, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -3, -3,
    GOAL_EXPLOSE1,     +1, -3,
    GOAL_EXPLOSE1,     -3, +1,
    GOAL_EXPLOSE1,     +1, +1,
    GOAL_EXPLOSE2,     -3, -3,
    GOAL_EXPLOSE2,     +1, -3,
    GOAL_EXPLOSE2,     -3, +1,
    GOAL_EXPLOSE2,     +1, +1,
    GOAL_ACTION,        ACTION_DYNAMITE, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_FINISHMOVE,
    GOAL_CACHE,        false, false,
    GOAL_DELETE,       // tracks meurt brutalement
    GOAL_TERM,
    0
};

// Blupi amorce une mine.
static Sint16 table_goal_mine[] =
{
    EV_ACTION_MINE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 125, // mine ?
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_ACTION,        ACTION_MECHE, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_TESTOBJECT,   0, -1, CHOBJECT, 125, // mine ?
    GOAL_AMORCE,       +1, 0,
    GOAL_TERM,
    0
};

// Détonnateur de mine créé par GOAL_AMORCE (blupi invisible).
static Sint16 table_goal_mine2[] =
{
    EV_ACTION_MINE2,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 127,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 10, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 125,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 9, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 127,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 8, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 125,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 7, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 127,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 6, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 125,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 5, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 127,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 4, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 125,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 3, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 127,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 2, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,
    GOAL_PUTOBJECT,     -1, -1, CHOBJECT, 125,
    GOAL_BUILDOBJECT,   -1, -1, -1, MOVEICONNB + 1, -1, -1, 100, 1, -1 * 100,
    GOAL_SOUND,         SOUND_CLICK,
    GOAL_ACTION,        ACTION_D_DELAY, DIRECT_E,

    GOAL_SOUND,         SOUND_DYNAMITE,
    GOAL_GROUP,         4,
    GOAL_PUTOBJECT,    -1, -1, -1, -1,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, -1, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDICONS,     -1, -1, 6, // explosion
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -1, -3,
    GOAL_EXPLOSE1,     -3, -1,
    GOAL_EXPLOSE1,     +1, -1,
    GOAL_EXPLOSE1,     -1, +1,
    GOAL_EXPLOSE2,     -1, -3,
    GOAL_EXPLOSE2,     -3, -1,
    GOAL_EXPLOSE2,     +1, -1,
    GOAL_EXPLOSE2,     -1, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         4 + 4,
    GOAL_EXPLOSE1,     -3, -3,
    GOAL_EXPLOSE1,     +1, -3,
    GOAL_EXPLOSE1,     -3, +1,
    GOAL_EXPLOSE1,     +1, +1,
    GOAL_EXPLOSE2,     -3, -3,
    GOAL_EXPLOSE2,     +1, -3,
    GOAL_EXPLOSE2,     -3, +1,
    GOAL_EXPLOSE2,     +1, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         12 + 12,
    GOAL_EXPLOSE1,     -5, -3,
    GOAL_EXPLOSE1,     -5, -1,
    GOAL_EXPLOSE1,     -5, +1,
    GOAL_EXPLOSE1,     -3, -5,
    GOAL_EXPLOSE1,     -3, +3,
    GOAL_EXPLOSE1,     -1, -5,
    GOAL_EXPLOSE1,     -1, +3,
    GOAL_EXPLOSE1,     +1, -5,
    GOAL_EXPLOSE1,     +1, +3,
    GOAL_EXPLOSE1,     +3, -3,
    GOAL_EXPLOSE1,     +3, -1,
    GOAL_EXPLOSE1,     +3, +1,
    GOAL_EXPLOSE2,     -5, -3,
    GOAL_EXPLOSE2,     -5, -1,
    GOAL_EXPLOSE2,     -5, +1,
    GOAL_EXPLOSE2,     -3, -5,
    GOAL_EXPLOSE2,     -3, +3,
    GOAL_EXPLOSE2,     -1, -5,
    GOAL_EXPLOSE2,     -1, +3,
    GOAL_EXPLOSE2,     +1, -5,
    GOAL_EXPLOSE2,     +1, +3,
    GOAL_EXPLOSE2,     +3, -3,
    GOAL_EXPLOSE2,     +3, -1,
    GOAL_EXPLOSE2,     +3, +1,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_DELAY, DIRECT_E,
    GOAL_GROUP,         16 + 16,
    GOAL_EXPLOSE1,     -7, -3,
    GOAL_EXPLOSE1,     -7, -1,
    GOAL_EXPLOSE1,     -7, +1,
    GOAL_EXPLOSE1,     -5, -5,
    GOAL_EXPLOSE1,     -5, +3,
    GOAL_EXPLOSE1,     -3, -7,
    GOAL_EXPLOSE1,     -3, +5,
    GOAL_EXPLOSE1,     -1, -7,
    GOAL_EXPLOSE1,     -1, +5,
    GOAL_EXPLOSE1,     +1, -7,
    GOAL_EXPLOSE1,     +1, +5,
    GOAL_EXPLOSE1,     +3, -5,
    GOAL_EXPLOSE1,     +3, +3,
    GOAL_EXPLOSE1,     +5, -3,
    GOAL_EXPLOSE1,     +5, -1,
    GOAL_EXPLOSE1,     +5, +1,
    GOAL_EXPLOSE2,     -7, -3,
    GOAL_EXPLOSE2,     -7, -1,
    GOAL_EXPLOSE2,     -7, +1,
    GOAL_EXPLOSE2,     -5, -5,
    GOAL_EXPLOSE2,     -5, +3,
    GOAL_EXPLOSE2,     -3, -7,
    GOAL_EXPLOSE2,     -3, +5,
    GOAL_EXPLOSE2,     -1, -7,
    GOAL_EXPLOSE2,     -1, +5,
    GOAL_EXPLOSE2,     +1, -7,
    GOAL_EXPLOSE2,     +1, +5,
    GOAL_EXPLOSE2,     +3, -5,
    GOAL_EXPLOSE2,     +3, +3,
    GOAL_EXPLOSE2,     +5, -3,
    GOAL_EXPLOSE2,     +5, -1,
    GOAL_EXPLOSE2,     +5, +1,
    GOAL_ACTION,        ACTION_DYNAMITE, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_FINISHMOVE,
    GOAL_CACHE,        false, false,
    GOAL_DELETE,       // blupi meurt brutalement
    GOAL_TERM,
    0
};

// Construit un pont en direction de l'est.
static Sint16 table_goal_ponte[] =
{
    EV_ACTION_BRIDGEE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 72, -1, -1, DIMOBJY / 10, 12, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGEEL,
    GOAL_TERM,
    0
};

static Sint16 table_goal_pontel[] =
{
    EV_ACTION_BRIDGEEL,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDOBJECT,  -10, -10, CHOBJECT, 72, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -10, -10, 6, // pont vers l'est
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_E,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_NEXTLOOP,
    GOAL_PUTOBJECT,    -10, -10, CHOBJECT, 72,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGEEL,

    GOAL_GROUP,         3,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_PUTFLOOR,     -10, -10, CHFLOOR, -2, // vIcon
    GOAL_SOUND,        SOUND_PLOUF,
    //?     GOAL_ACTION,        0,-1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Construit un pont en direction de l'ouest.
static Sint16 table_goal_ponto[] =
{
    EV_ACTION_BRIDGEO,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 72, -1, -1, DIMOBJY / 10, 12, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGEOL,
    GOAL_TERM,
    0
};

static Sint16 table_goal_pontol[] =
{
    EV_ACTION_BRIDGEOL,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDOBJECT,  -10, -10, CHOBJECT, 72, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -10, -10, 7, // pont vers l'ouest
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_W,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_NEXTLOOP,
    GOAL_PUTOBJECT,    -10, -10, CHOBJECT, 72,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGEOL,

    GOAL_GROUP,         3,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_PUTFLOOR,     -10, -10, CHFLOOR, -2, // vIcon
    GOAL_SOUND,        SOUND_PLOUF,
    //?     GOAL_GOBLUPI,       0,-1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_TERM,
    0
};

// Construit un pont en direction du sud.
static Sint16 table_goal_ponts[] =
{
    EV_ACTION_BRIDGES,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 73, -1, -1, DIMOBJY / 10, 12, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGESL,
    GOAL_TERM,
    0
};

static Sint16 table_goal_pontsl[] =
{
    EV_ACTION_BRIDGESL,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDOBJECT,  -10, -10, CHOBJECT, 73, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -10, -10, 8, // pont vers le sud
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_S,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_NEXTLOOP,
    GOAL_PUTOBJECT,    -10, -10, CHOBJECT, 73,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGESL,

    GOAL_GROUP,         3,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_PUTFLOOR,     -10, -10, CHFLOOR, -2, // vIcon
    GOAL_SOUND,        SOUND_PLOUF,
    //?     GOAL_GOBLUPI,       -1,0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_S,
    GOAL_TERM,
    0
};

// Construit un pont en direction du nord.
static Sint16 table_goal_pontn[] =
{
    EV_ACTION_BRIDGEN,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 73, -1, -1, DIMOBJY / 10, 12, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGENL,
    GOAL_TERM,
    0
};

static Sint16 table_goal_pontnl[] =
{
    EV_ACTION_BRIDGENL,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDOBJECT,  -10, -10, CHOBJECT, 73, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -10, -10, 9, // pont vers le nord
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_N,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_NEXTLOOP,
    GOAL_PUTOBJECT,    -10, -10, CHOBJECT, 73,
    GOAL_OTHERLOOP,     EV_ACTION_BRIDGENL,

    GOAL_GROUP,         3,
    GOAL_PUTOBJECT,    -10, -10, -1, -1,
    GOAL_PUTFLOOR,     -10, -10, CHFLOOR, -2, // vIcon
    GOAL_SOUND,        SOUND_PLOUF,
    //?     GOAL_GOBLUPI,       -1,0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Construit un bateau.
static Sint16 table_goal_bateaue[] =
{
    EV_ACTION_BOATE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY / 10, 20, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_FINISHMOVE,

    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, 0, 6, // bateau vers l'est
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_E,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, 0, -1, -1,
    GOAL_PUTOBJECT,    +1, 0, CHOBJECT, 117,
    GOAL_SOUND,        SOUND_PLOUF,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateaus[] =
{
    EV_ACTION_BOATS,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY / 10, 20, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,

    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 117, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 8, // bateau vers le sud
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_S,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_PUTOBJECT,    0, +1, CHOBJECT, 117,
    GOAL_SOUND,        SOUND_PLOUF,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateauo[] =
{
    EV_ACTION_BOATO,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY / 10, 20, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_FINISHMOVE,

    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, 0, 7, // bateau vers l'ouest
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_W,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, 0, -1, -1,
    GOAL_PUTOBJECT,    -3, 0, CHOBJECT, 117,
    GOAL_SOUND,        SOUND_PLOUF,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateaun[] =
{
    EV_ACTION_BOATN,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GROUP,         3,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_BUILDOBJECT,  -1, 0, CHOBJECT, 117, -1, -1, DIMOBJY / 10, 20, -10 * 100,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_SAW, DIRECT_S,
    GOAL_GOBLUPI,       -1, +1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FINISHMOVE,

    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 117, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 9, // bateau vers le nord
    GOAL_ACTION,        ACTION_BRIDGE, DIRECT_N,
    GOAL_GROUP,         4,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, -1, -1,
    GOAL_PUTOBJECT,    0, -3, CHOBJECT, 117,
    GOAL_SOUND,        SOUND_PLOUF,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Départ en bateau.
static Sint16 table_goal_bateaude[] =
{
    EV_ACTION_BOATDE,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       +1, +1, true,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   +1, -1, CHOBJECT, 117,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         +1, -1,
    GOAL_GOBLUPI,       +2, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève le bateau
    GOAL_VEHICULE,     1,  // en bateau
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateauds[] =
{
    EV_ACTION_BOATDS,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   -1, +1, CHOBJECT, 117,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -1, +1,
    GOAL_GOBLUPI,       0, +2, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève le bateau
    GOAL_VEHICULE,     1,  // en bateau
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateaudo[] =
{
    EV_ACTION_BOATDO,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   -2, -1, CHOBJECT, 117,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -2, -1,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève le bateau
    GOAL_VEHICULE,     1,  // en bateau
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateaudn[] =
{
    EV_ACTION_BOATDN,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   -1, -2, CHOBJECT, 117,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -1, -2,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève le bateau
    GOAL_VEHICULE,     1,  // en bateau
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_TERM,
    0
};

// Arrivée en bateau.
static Sint16 table_goal_bateauae[] =
{
    EV_ACTION_BOATAE,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_GROUP,         4,
    GOAL_IFDEBARQUE,   -3, 0,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -2, -1,
    GOAL_WORK,         -4, -1,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 117, // remet le bateau
    GOAL_VEHICULE,     0,  // à pied
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       -2, 0, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateauas[] =
{
    EV_ACTION_BOATAS,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_GROUP,         4,
    GOAL_IFDEBARQUE,   0, -3,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -1, -2,
    GOAL_WORK,         -1, -4,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 117, // remet le bateau
    GOAL_VEHICULE,     0,  // à pied
    GOAL_GOBLUPI,       0, -2, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateauao[] =
{
    EV_ACTION_BOATAO,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_GROUP,         4,
    GOAL_IFDEBARQUE,   +2, 0,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         0, -1,
    GOAL_WORK,         +2, -1,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 117, // remet le bateau
    GOAL_VEHICULE,     0,  // à pied
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_TERM,
    0
};

static Sint16 table_goal_bateauan[] =
{
    EV_ACTION_BOATAN,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_GROUP,         4,
    GOAL_IFDEBARQUE,   0, +2,
    GOAL_INTERRUPT,    -1,  // passe muraille
    GOAL_WORK,         -1, 0,
    GOAL_WORK,         -1, +2,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 117, // remet le bateau
    GOAL_VEHICULE,     0,  // à pied
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_TERM,
    0
};

// Le robot construit une station de recharge.
static Sint16 table_goal_r_build1[] =
{
    EV_ACTION_R_BUILD1,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 99, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_N,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot construit une usine à araignées.
static Sint16 table_goal_r_build2[] =
{
    EV_ACTION_R_BUILD2,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 100, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_N,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot construit une usine à virus.
static Sint16 table_goal_r_build3[] =
{
    EV_ACTION_R_BUILD3,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 102, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_N,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot construit une usine à tracks.
static Sint16 table_goal_r_build4[] =
{
    EV_ACTION_R_BUILD4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 104, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_N,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot construit une usine à bombe.
static Sint16 table_goal_r_build5[] =
{
    EV_ACTION_R_BUILD5,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 115, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_N,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot construit une usine à électro.
static Sint16 table_goal_r_build6[] =
{
    EV_ACTION_R_BUILD6,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         3,
    GOAL_USINEBUILD,   -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_BUILDFLOOR,    -1, -1, CHFLOOR, 67, CHMASK1, 0, 16, 14, 1 * 100,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_E,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_BUILDOBJECT,   -1, -1, CHOBJECT, 17, -1, -1, DIMOBJY / 4, 20, 4 * 100,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_W,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_APLAT, DIRECT_N,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_E,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_S,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Le robot se recharge.
static Sint16 table_goal_r_make1[] =
{
    EV_ACTION_R_MAKE1,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -1, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_LOAD, DIRECT_W,
    GOAL_ACTION,        ACTION_R_BUILD, DIRECT_W,
    GOAL_TERM,
    0
};

// Le robot construit une araignée.
static Sint16 table_goal_r_make2[] =
{
    EV_ACTION_R_MAKE2,
    GOAL_GOHILI2,       +2, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -2, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 101, // ouvre la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 100, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 100, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 100, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      2, 0,
    GOAL_WAITFREE,      3, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 101, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 1, // araignée (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 100, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      1, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 101, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 100, // ferme la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Le robot construit un virus.
static Sint16 table_goal_r_make3[] =
{
    EV_ACTION_R_MAKE3,
    GOAL_GOHILI2,       +2, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -2, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 103, // ouvre la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 102, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 102, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 102, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      2, 0,
    GOAL_WAITFREE,      3, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 103, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 2, // virus (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 102, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      1, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 103, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 102, // ferme la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Le robot construit un tracks.
static Sint16 table_goal_r_make4[] =
{
    EV_ACTION_R_MAKE4,
    GOAL_GOHILI2,       +2, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -2, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 105, // ouvre la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 104, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 104, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 104, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      2, 0,
    GOAL_WAITFREE,      3, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 105, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 3, // tracks (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 104, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      1, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 105, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 104, // ferme la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Le robot construit une bombe.
static Sint16 table_goal_r_make5[] =
{
    EV_ACTION_R_MAKE5,
    GOAL_GOHILI2,       +2, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -2, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 116, // ouvre la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 115, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 115, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 115, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      2, 0,
    GOAL_WAITFREE,      3, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 116, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 5, // bombe (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 115, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      1, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 116, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 115, // ferme la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Le robot construit un électro.
static Sint16 table_goal_r_make6[] =
{
    EV_ACTION_R_MAKE6,
    GOAL_GOHILI2,       +2, +1, false,
    GOAL_GROUP,         2,
    GOAL_USINEFREE,    -2, -1,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 18, // ouvre la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_W,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 17, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 17, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 17, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 10, // secoue
    GOAL_SOUND,         SOUND_LABO,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      2, 0,
    GOAL_WAITFREE,      3, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 18, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 7, // électro (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 17, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_WAITFREE,      1, 0,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 18, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     -2, -1, CHOBJECT, 17, // ferme la porte
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_R_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Blupi monte dans une jeep.
static Sint16 table_goal_mjeep[] =
{
    EV_ACTION_MJEEP,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   -1, 0, CHOBJECT, 118,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_ACTION,        ACTION_JUMPJEEP, DIRECT_S,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève la jeep
    GOAL_VEHICULE,     2,  // en jeep
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi descend de la jeep.
static Sint16 table_goal_djeep[] =
{
    EV_ACTION_DJEEP,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         5,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 118, // remet la jeep
    GOAL_VEHICULE,     0,  // à pied
    GOAL_ACTUALISE,
    GOAL_ACTION,        ACTION_JUMPJEEP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_TERM,
    0
};

// Blupi monte dans une armure.
static Sint16 table_goal_marmure[] =
{
    EV_ACTION_MARMURE,
    GOAL_ISNOMALADE,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_ISNOMALADE,
    GOAL_GROUP,         3,
    GOAL_TESTOBJECT,   -1, 0, CHOBJECT, 16,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, 0,
    GOAL_GOBLUPI,       0, +1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_PUTOBJECT,    -1, -1, -1, -1, // enlève l'armure
    GOAL_ACTION,       ACTION_ARMORCLOSE, DIRECT_E,
    GOAL_VEHICULE,      3,  // en armure
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi descend de l'armure.
static Sint16 table_goal_darmure[] =
{
    EV_ACTION_DARMURE,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_INTERRUPT,     0,  // prioritaire
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_ARMOROPEN, DIRECT_E,
    GOAL_GROUP,         3,
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 16, // remet l'armure
    GOAL_VEHICULE,     0,  // à pied
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_TERM,
    0
};

// Blupi cherche où planter des drapeaux.
static Sint16 table_goal_drapeau[] =
{
    EV_ACTION_FLAG,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_FIX,           0, -1,
    GOAL_REPEAT,        true,
    GOAL_OTHERFIX,      CHFLOOR, 33, 48, 71, 71, EV_ACTION_FLAG2,
    GOAL_TERM,
    0
};

static Sint16 table_goal_drapeau2[] =
{
    EV_ACTION_FLAG2,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_BUILD, DIRECT_S,
    GOAL_FLOORJUMP,     CHFLOOR, 71, EV_ACTION_FLAG3,
    GOAL_ADDDRAPEAU,    -1, 0,

    GOAL_OTHERFIX,      CHFLOOR, 33, 48, 71, 71, EV_ACTION_FLAG2,
    GOAL_TERM,
    0
};

// Plante un drapeau.
static Sint16 table_goal_drapeau3[] =
{
    EV_ACTION_FLAG3,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         0, -1,
    GOAL_BUILDOBJECT,   0, -1, CHOBJECT, 124, -1, -1, DIMOBJY / 10, 8, 10 * 100,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_W,
    GOAL_GOBLUPI,       +1, -1, true,
    GOAL_ACTION,        ACTION_PIOCHESOURD, DIRECT_E,
    GOAL_ADDDRAPEAU,    -1, 0,
    GOAL_FINISHMOVE,

    //?     GOAL_OTHERFIX,      CHFLOOR,33,48,71,71, EV_ACTION_FLAG2,
    GOAL_TERM,
    0
};

// Blupi extrait du fer.
static Sint16 table_goal_extrait[] =
{
    EV_ACTION_EXTRAIT,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 121, // ferme la porte
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 121, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 11, // secoue
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_MINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 121, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 11, // secoue
    GOAL_SOUND,         SOUND_MINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         4,
    GOAL_TAKEOBJECT,   -1, -1, CHOBJECT, 123, // porte du fer
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 122, // ouvre la porte
    GOAL_CACHE,        false, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_IFTERM,        0, -1,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi fabrique une jeep.
static Sint16 table_goal_fabjeep[] =
{
    EV_ACTION_FABJEEP,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 119, // ferme la porte
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         5,
    GOAL_TAKEOBJECT,   -1, -1, -1, -1, // ne porte plus rien
    GOAL_VEHICULE,     2,  // en jeep
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 120, // ouvre la porte
    GOAL_CACHE,        false, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_IFTERM,        +1, 0,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_TERM,
    0
};

// Blupi fabrique une armure.
static Sint16 table_goal_fabarmure[] =
{
    EV_ACTION_FABARMURE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 119, // ferme la porte
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         5,
    GOAL_TAKEOBJECT,   -1, -1, -1, -1, // ne porte plus rien
    GOAL_VEHICULE,     3,  // en armure
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 120, // ouvre la porte
    GOAL_CACHE,        false, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_IFTERM,        +1, 0,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_TERM,
    0
};

// Blupi fabrique une mine.
static Sint16 table_goal_fabmine[] =
{
    EV_ACTION_FABMINE,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         6,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 119, // ferme la porte
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_CACHE,        true, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  -1, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     -1, -1, 12, // secoue
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         4,
    GOAL_TAKEOBJECT,   -1, -1, CHOBJECT, 125, // porte une mine
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 120, // ouvre la porte
    GOAL_CACHE,        false, false,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_IFTERM,        0, -1,
    GOAL_GOBLUPI,       0, -1, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// Blupi fabrique un disciple.
static Sint16 table_goal_fabdisc[] =
{
    EV_ACTION_FABDISC,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_ENERGY,        MAXENERGY / 4,
    GOAL_ACTION,        ACTION_STOP, DIRECT_N,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_WORK,         -1, -1,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 119, // ferme la porte
    GOAL_CACHE,        true, false,
    GOAL_GOBLUPI,       -1, 0, true,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 12, // secoue
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_W,
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_GROUP,         2,
    GOAL_BUILDOBJECT,  0, -1, CHOBJECT, 119, -1, -1, DIMOBJY, 1, -1 * 100,
    GOAL_ADDMOVES,     0, -1, 12, // secoue
    GOAL_SOUND,         SOUND_USINE,
    GOAL_ACTION,        ACTION_LABO, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         2,
    GOAL_FINISHMOVE,
    GOAL_PUTOBJECT,    0, -1, CHOBJECT, 120, // ouvre la porte
    GOAL_NEWPERSO,      +1, 0, 8, // disciple (qui sortira)
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_PUTOBJECT,     0, -1, CHOBJECT, 119, // ferme la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_ACTION,        ACTION_R_DELAY, DIRECT_E,
    GOAL_SOUND,         SOUND_DOOR,
    GOAL_GROUP,         3,
    GOAL_TAKEOBJECT,   -1, -1, -1, -1, // ne porte plus rien
    GOAL_CACHE,        false, false,
    GOAL_PUTOBJECT,    -1, -1, CHOBJECT, 120, // ouvre la porte
    GOAL_GOBLUPI,       +1, 0, true,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_TERM,
    0
};

// L'électro va sur un blupi puis lance ses rayons.
static Sint16 table_goal_e_rayon[] =
{
    EV_ACTION_E_RAYON,
    GOAL_GOHILI2,       0, 0, false,
    GOAL_SOUND,         SOUND_E_TOURNE,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_SKIPSKILL,     1, 7,
    GOAL_ACTION,       ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,       ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,       ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,       ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,       ACTION_E_BEGIN, DIRECT_E,
    GOAL_ACTION,       ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,       ACTION_E_RAYON, DIRECT_E,
    GOAL_SOUND,         SOUND_E_RAYON,
    GOAL_ELECTRO,        0, 0, 8,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ELECTRO,       -2, 0, 8,
    GOAL_ELECTRO,        0, -2, 8,
    GOAL_ELECTRO,        0, +2, 8,
    GOAL_ELECTRO,       +2, 0, 8,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ELECTRO,       -2, -2, 8,
    GOAL_ELECTRO,       +2, -2, 8,
    GOAL_ELECTRO,       -2, +2, 8,
    GOAL_ELECTRO,       +2, +2, 8,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ELECTRO,       -4, -2, 8,
    GOAL_ELECTRO,       -4, 0, 8,
    GOAL_ELECTRO,       -4, +2, 8,
    GOAL_ELECTRO,       -2, -4, 8,
    GOAL_ELECTRO,       -2, +4, 8,
    GOAL_ELECTRO,        0, -4, 8,
    GOAL_ELECTRO,        0, +4, 8,
    GOAL_ELECTRO,       +2, -4, 8,
    GOAL_ELECTRO,       +2, +4, 8,
    GOAL_ELECTRO,       +4, -2, 8,
    GOAL_ELECTRO,       +4, 0, 8,
    GOAL_ELECTRO,       +4, +2, 8,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ELECTRO,       -6, -2, 8,
    GOAL_ELECTRO,       -6, 0, 8,
    GOAL_ELECTRO,       -6, +2, 8,
    GOAL_ELECTRO,       -4, -4, 8,
    GOAL_ELECTRO,       -4, +4, 8,
    GOAL_ELECTRO,       -2, -6, 8,
    GOAL_ELECTRO,       -2, +6, 8,
    GOAL_ELECTRO,        0, -6, 8,
    GOAL_ELECTRO,        0, +6, 8,
    GOAL_ELECTRO,       +2, -6, 8,
    GOAL_ELECTRO,       +2, +6, 8,
    GOAL_ELECTRO,       +4, -4, 8,
    GOAL_ELECTRO,       +4, +4, 8,
    GOAL_ELECTRO,       +6, -2, 8,
    GOAL_ELECTRO,       +6, 0, 8,
    GOAL_ELECTRO,       +6, +2, 8,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_ACTION,        ACTION_E_RAYON, DIRECT_E,
    GOAL_FINISHMOVE,
    GOAL_TERM,
    0
};

// Blupi s'électrocute.
static Sint16 table_goal_electro[] =
{
    EV_ACTION_ELECTRO,
    GOAL_INTERRUPT, 0,  // prioritaire
    GOAL_ACTION,        ACTION_STOP, DIRECT_SE,
    GOAL_MALADE,        false,
    GOAL_ACTION,        ACTION_ELECTRO, DIRECT_SE,
    GOAL_TAKEOBJECT,    -1, -1, -1, -1, // ne porte plus rien
    GOAL_SOUND,         SOUND_TCHAO,
    GOAL_ACTION,        ACTION_GRILL1, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL2, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL3, DIRECT_SE,
    GOAL_DELETE,        // blupi meurt brutalement
    GOAL_TERM,
    0
};

static Sint16 table_goal_electrom[] =
{
    EV_ACTION_ELECTROm,
    GOAL_INTERRUPT, 0,  // prioritaire
    GOAL_ACTION,        ACTION_STOP, DIRECT_SE,
    GOAL_GROUP,         3,
    GOAL_MALADE,       false,
    GOAL_TAKEOBJECT,   -1, -1, -1, -1, // ne porte plus rien
    GOAL_SOUND,        SOUND_TCHAO,
    GOAL_ACTION,        ACTION_GRILL1, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL2, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL3, DIRECT_SE,
    GOAL_DELETE,        // blupi meurt brutalement
    GOAL_TERM,
    0
};

// Blupi grille.
static Sint16 table_goal_grille[] =
{
    EV_ACTION_GRILLE,
    GOAL_INTERRUPT, 0,  // prioritaire
    GOAL_ACTION,        ACTION_STOP, DIRECT_SE,
    GOAL_GROUP,         3,
    GOAL_MALADE,       false,
    GOAL_TAKEOBJECT,   -1, -1, -1, -1, // ne porte plus rien
    GOAL_SOUND,        SOUND_BURN,
    GOAL_ACTION,        ACTION_GRILL1, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL2, DIRECT_SE,
    GOAL_ACTION,        ACTION_GRILL3, DIRECT_SE,
    GOAL_DELETE,        // blupi meurt brutalement
    GOAL_TERM,
    0
};

// L'araignée meurt.
static Sint16 table_goal_a_mort[] =
{
    EV_ACTION_A_MORT,
    GOAL_INTERRUPT, 0,  // prioritaire
    GOAL_ACTION,        ACTION_STOP, DIRECT_E,
    GOAL_SOUND,         SOUND_A_POISON,
    GOAL_ACTION,        ACTION_S_POISON, DIRECT_E,
    GOAL_ACTION,        ACTION_S_DEAD1, DIRECT_E,
    GOAL_ACTION,        ACTION_S_DEAD2, DIRECT_E,
    GOAL_ACTION,        ACTION_S_DEAD3, DIRECT_E,
    GOAL_DELETE,        // l'araignée meurt brutalement
    GOAL_TERM,
    0
};

// Blupi se prépare à répéter une liste.
static Sint16 table_goal_repeat[] =
{
    EV_ACTION_REPEAT,
    GOAL_TERM,
    0
};

// Blupi se téléporte d'un endroit à un autre.
static Sint16 table_goal_teleporte00[] =
{
    EV_ACTION_TELEPORTE00,
    GOAL_GOHILI2,       0, 0, false,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDFLOOR,   0, 0, CHFLOOR, -1, -1, -1, 100, 1, 1 * 100,
    GOAL_ADDICONS,     0, 0, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE3, DIRECT_E,
    GOAL_TELEPORTE,     0, 0,
    GOAL_GROUP,         3,
    GOAL_BUILDFLOOR,   0, 0, CHFLOOR, -1, -1, -1, 60, 1, 1 * 60,
    GOAL_ADDICONS,     0, 0, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_teleporte10[] =
{
    EV_ACTION_TELEPORTE10,
    GOAL_GOHILI2,       +1, 0, false,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDFLOOR,   -1, 0, CHFLOOR, -1, -1, -1, 100, 1, 1 * 100,
    GOAL_ADDICONS,     -1, 0, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE3, DIRECT_E,
    GOAL_TELEPORTE,     +1, 0,
    GOAL_GROUP,         3,
    GOAL_BUILDFLOOR,   -1, 0, CHFLOOR, -1, -1, -1, 60, 1, 1 * 60,
    GOAL_ADDICONS,     -1, 0, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_teleporte01[] =
{
    EV_ACTION_TELEPORTE01,
    GOAL_GOHILI2,       0, +1, false,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, -1, -1, -1, 100, 1, 1 * 100,
    GOAL_ADDICONS,     0, -1, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE3, DIRECT_E,
    GOAL_TELEPORTE,     0, +1,
    GOAL_GROUP,         3,
    GOAL_BUILDFLOOR,   0, -1, CHFLOOR, -1, -1, -1, 60, 1, 1 * 60,
    GOAL_ADDICONS,     0, -1, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_TERM,
    0
};

static Sint16 table_goal_teleporte11[] =
{
    EV_ACTION_TELEPORTE11,
    GOAL_GOHILI2,       +1, +1, false,
    GOAL_GROUP,         4,
    GOAL_INTERRUPT,    0,  // prioritaire
    GOAL_BUILDFLOOR,   -1, -1, CHFLOOR, -1, -1, -1, 100, 1, 1 * 100,
    GOAL_ADDICONS,     -1, -1, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE3, DIRECT_E,
    GOAL_TELEPORTE,     +1, +1,
    GOAL_GROUP,         3,
    GOAL_BUILDFLOOR,   -1, -1, CHFLOOR, -1, -1, -1, 60, 1, 1 * 60,
    GOAL_ADDICONS,     -1, -1, 11,
    GOAL_SOUND,        SOUND_TELEPORTE,
    GOAL_ACTION,        ACTION_TELEPORTE2, DIRECT_E,
    GOAL_ACTION,        ACTION_TELEPORTE1, DIRECT_E,
    GOAL_TERM,
    0
};


static Sint16 *table_pGoal[] =
{
    table_goal_go,
    table_goal_maison,
    table_goal_abat1,
    table_goal_abat2,
    table_goal_abat3,
    table_goal_abat4,
    table_goal_abat5,
    table_goal_abat6,
    table_goal_roc1,
    table_goal_roc2,
    table_goal_roc3,
    table_goal_roc4,
    table_goal_roc5,
    table_goal_roc6,
    table_goal_roc7,
    table_goal_build1,
    table_goal_build2,
    table_goal_build3,
    table_goal_build4,
    table_goal_build5,
    table_goal_build6,
    table_goal_mur,
    table_goal_tour,
    table_goal_palis,
    table_goal_carry,
    table_goal_carry2,
    table_goal_depose,
    table_goal_depose2,
    table_goal_newblupi,
    table_goal_cultive,
    table_goal_cultive2,
    table_goal_mange,
    table_goal_mange2,
    table_goal_boit,
    table_goal_boit2,
    table_goal_fleur1,
    table_goal_fleur2,
    table_goal_fleur3,
    table_goal_labo,
    table_goal_dynamite,
    table_goal_dynamite2,
    table_goal_t_dynamite,
    table_goal_mine,
    table_goal_mine2,
    table_goal_ponte,
    table_goal_pontel,
    table_goal_ponto,
    table_goal_pontol,
    table_goal_ponts,
    table_goal_pontsl,
    table_goal_pontn,
    table_goal_pontnl,
    table_goal_bateaue,
    table_goal_bateaus,
    table_goal_bateauo,
    table_goal_bateaun,
    table_goal_bateaude,
    table_goal_bateauds,
    table_goal_bateaudo,
    table_goal_bateaudn,
    table_goal_bateauae,
    table_goal_bateauas,
    table_goal_bateauao,
    table_goal_bateauan,
    table_goal_r_build1,
    table_goal_r_build2,
    table_goal_r_build3,
    table_goal_r_build4,
    table_goal_r_build5,
    table_goal_r_build6,
    table_goal_r_make1,
    table_goal_r_make2,
    table_goal_r_make3,
    table_goal_r_make4,
    table_goal_r_make5,
    table_goal_r_make6,
    table_goal_mjeep,
    table_goal_djeep,
    table_goal_marmure,
    table_goal_darmure,
    table_goal_drapeau,
    table_goal_drapeau2,
    table_goal_drapeau3,
    table_goal_extrait,
    table_goal_fabjeep,
    table_goal_fabarmure,
    table_goal_fabmine,
    table_goal_fabdisc,
    table_goal_e_rayon,
    table_goal_electro,
    table_goal_electrom,
    table_goal_grille,
    table_goal_a_mort,
    table_goal_repeat,
    table_goal_teleporte00,
    table_goal_teleporte10,
    table_goal_teleporte01,
    table_goal_teleporte11,
    nullptr
};
// clang-format on

// Retourne le pointeur à la table table_goal_*.

Sint16 *
GetTableGoal (Sint32 action)
{
  Sint16 ** ppTable = table_pGoal;

  while (*ppTable != nullptr)
  {
    if (**ppTable == action)
    {
      return *ppTable + 1; // après EV_ACTION_*
    }

    ppTable++;
  }

  return nullptr;
}
