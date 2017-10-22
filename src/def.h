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

#include <SDL2/SDL_stdinc.h>

// clang-format off
#define _INTRO          true    // true for init screen

#define SCRFACTOR       4 / 3
#define LXIMAGE         (480 * SCRFACTOR + (480 * SCRFACTOR) % 2) // window size
#define LYIMAGE         480

#define POSDRAWX        144     // draw surface
#define POSDRAWY        15
#define DIMDRAWX        (LXIMAGE - (640 - 480))
#define DIMDRAWY        450

#define POSMAPX         8       // map surface
#define POSMAPY         15
#define DIMMAPX         128
#define DIMMAPY         128

#define MAXCELX         200     // max cells for a world
#define MAXCELY         200

#define DIMCELX         60      // cell size (decor)
#define DIMCELY         30

#define DIMOBJX         120     // object size
#define DIMOBJY         120

#define DIMBLUPIX       60      // Blupi size
#define DIMBLUPIY       60
#define SHIFTBLUPIY     5       // shift on top

#define DIMBUTTONX      40      // button size
#define DIMBUTTONY      40

#define DIMJAUGEX       124     // progress size
#define DIMJAUGEY       22

#define POSSTATX        12      // statistics
#define POSSTATY        220
#define DIMSTATX        60
#define DIMSTATY        30

#define DIMTEXTX        16      // max char size
#define DIMTEXTY        16

#define DIMLITTLEX      16      // max small char size
#define DIMLITTLEY      12

#define CHBACK          0
#define CHFLOOR         1
#define CHOBJECT        2
#define CHOBJECTo       3
#define CHBLUPI         4
#define CHHILI          5
#define CHFOG           6
#define CHMASK1         7
#define CHLITTLE        8
#define CHMAP           9
#define CHBUTTON        10
#define CHGROUND        11
#define CHJAUGE         12
#define CHTEXT          13
#define CHBIGNUM        14
#define CHMASK2         15

#define FOGHIDE         4
// clang-format on

// Directions :
enum Directions {
  DIRECT_E  = (0 * 16), // east
  DIRECT_SE = (1 * 16), // south-east
  DIRECT_S  = (2 * 16), // south
  DIRECT_SW = (3 * 16), // south-west
  DIRECT_W  = (4 * 16), // west
  DIRECT_NW = (5 * 16), // north-west
  DIRECT_N  = (6 * 16), // north
  DIRECT_NE = (7 * 16), // north-east
};
/*                  NO
 *            O     |     N
 *             \    |    /
 *               \  |  /
 *                 \|/
 *        SO -------o------- NE
 *                 /|\
 *               /  |  \
 *             /    |    \
 *            S     |     E
 *          (y)     SE    (x)
 */

// Actions:
enum Actions {
  ACTION_STOP         = 0,   // stop
  ACTION_STOPTIRED    = 1,   // stop tiredness
  ACTION_WALK         = 2,   // walk
  ACTION_WALKTIRED    = 3,   // walk tiredness
  ACTION_BUILD        = 4,   // build
  ACTION_PICKAXE      = 5,   // pickaxe
  ACTION_ENERGY       = 6,   // prend de l'énergie
  ACTION_CARRY        = 8,   // take with a jump the object on the head (est)
  ACTION_DROP         = 9,   // drop the object which is on the head (est)
  ACTION_SAW          = 10,  // saw wood
  ACTION_BURN         = 11,  // blupi is burning !
  ACTION_TCHAO        = 12,  // blupi disappeard !
  ACTION_EAT          = 13,  // blupi eats
  ACTION_BORN         = 14,  // born
  ACTION_JUMP2        = 15,  // jump over an obstacle
  ACTION_JUMP3        = 16,  // jump over an obstacle
  ACTION_JUMP4        = 17,  // jump over an obstacle
  ACTION_JUMP5        = 18,  // jump over an obstacle
  ACTION_BRIDGE       = 19,  // push a bridge
  ACTION_MISC1        = 20,  // divers 1 (hausse les épaules)
  ACTION_MISC2        = 21,  // divers 2 (grat-grat)
  ACTION_MISC3        = 22,  // divers 3 (yoyo)
  ACTION_MISC1f       = 23,  // divers 1 fatigué (bof-bof)
  ACTION_SLIDE        = 24,  // slide when walking
  ACTION_DRINK        = 25,  // blupi is drinking
  ACTION_LABO         = 26,  // blupi travaille dans son laboratoire
  ACTION_DYNAMITE     = 27,  // blupi fait péter la dynamite
  ACTION_DELAY        = 28,  // blupi attend un frame
  ACTION_CUEILLE1     = 29,  // blupi cueille des fleurs
  ACTION_CUEILLE2     = 30,  // blupi cueille des fleurs
  ACTION_MECHE        = 31,  // blupi se bouche les oreilles
  ACTION_STOPb        = 32,  // arrêt en bateau
  ACTION_MARCHEb      = 33,  // avance en bateau
  ACTION_STOPJEEP     = 34,  // stop when using a jeep
  ACTION_WALKJEEP     = 35,  // going in jeep
  ACTION_ELECTRO      = 36,  // blupi électrocuté
  ACTION_GRILL1       = 37,  // blupi grills (phase 1)
  ACTION_GRILL2       = 38,  // blupi grills (phase 2)
  ACTION_GRILL3       = 39,  // blupi grills (phase 3)
  ACTION_MISC4        = 40,  // divers 4 (ferme les yeux)
  ACTION_HAPPY        = 41,  // blupi is happy
  ACTION_ARROSE       = 42,  // blupi arrose
  ACTION_BECHE        = 43,  // blupi bèche
  ACTION_CUEILLE3     = 44,  // blupi cueille des fleurs
  ACTION_BUILDBREF    = 45,  // construit
  ACTION_BUILDSEC     = 46,  // construit
  ACTION_BUILDSOURD   = 47,  // construit
  ACTION_BUILDPIERRE  = 48,  // construit
  ACTION_PIOCHEPIERRE = 49,  // pioche
  ACTION_PIOCHESOURD  = 50,  // pioche
  ACTION_MISC5        = 51,  // divers 5 (ohé)
  ACTION_TELEPORTE1   = 52,  // téléporte
  ACTION_TELEPORTE2   = 53,  // téléporte
  ACTION_TELEPORTE3   = 54,  // téléporte
  ACTION_STOPARMOR    = 55,  // stop armor
  ACTION_WALKARMOR    = 56,  // walk armor
  ACTION_ARMOROPEN    = 57,  // open armor
  ACTION_ARMORCLOSE   = 58,  // close armor
  ACTION_JUMPJEEP     = 59,  // jump in the jeep
  ACTION_MISC6        = 60,  // divers 6 (diabolo)
  ACTION_S_STOP       = 100, // spider: stop
  ACTION_S_WALK       = 101, // spider: walk
  ACTION_S_JUMP       = 102, // spider: jump
  ACTION_S_GRILL      = 103, // spider: grill in rays
  ACTION_S_POISON     = 105, // spider: poisoned
  ACTION_S_DEAD1      = 106, // spider: dead
  ACTION_S_DEAD2      = 107, // spider: dead
  ACTION_S_DEAD3      = 108, // spider: dead
  ACTION_V_STOP       = 200, // virus: stop
  ACTION_V_WALK       = 201, // virus: walk
  ACTION_V_GRILL      = 202, // virus: grill in rays
  ACTION_T_STOP       = 300, // tracks: stop
  ACTION_T_WALK       = 301, // tracks: walk
  ACTION_T_CRUSHED    = 302, // tracks: crushed an object
  ACTION_R_STOP       = 400, // robot: stop
  ACTION_R_WALK       = 401, // robot: walk
  ACTION_R_APLAT      = 402, // robot: applatit
  ACTION_R_BUILD      = 403, // robot: construit
  ACTION_R_DELAY      = 404, // robot: construit
  ACTION_R_LOAD       = 405, // robot: reload
  ACTION_R_CRUSHED    = 406, // robot: crushed an object
  ACTION_B_STOP       = 500, // bomb: stop
  ACTION_B_WALK       = 501, // bomb: walk
  ACTION_D_DELAY      = 600, // detonator: wait
  ACTION_E_STOP       = 700, // electro: stop
  ACTION_E_WALK       = 701, // electro: walk
  ACTION_E_BEGIN      = 702, // electro: begin
  ACTION_E_RAYON      = 703, // electro: rayon
  ACTION_D_STOP       = 800, // disciple: stop
  ACTION_D_WALK       = 801, // disciple: walk
  ACTION_D_BUILD      = 802, // disciple: build
  ACTION_D_PICKAXE    = 803, // disciple: pickaxe
  ACTION_D_SAW        = 804, // disciple: saw wood
  ACTION_D_TCHAO      = 805, // disciple: disappeard !
  ACTION_D_CUEILLE1   = 806, // disciple: cueille des fleurs
  ACTION_D_CUEILLE2   = 807, // disciple: cueille des fleurs
  ACTION_D_MECHE      = 808, // disciple: se bouche les oreilles
  ACTION_D_ARROSE     = 809, // disciple: arrose
  ACTION_D_BECHE      = 810, // disciple: bèche
};

// Sounds:
enum Sounds {
  SOUND_NONE        = -1,
  SOUND_CLICK       = 0,
  SOUND_BOING       = 1,
  SOUND_OK1         = 2,
  SOUND_OK2         = 3,
  SOUND_OK3         = 4,
  SOUND_GO1         = 5,
  SOUND_GO2         = 6,
  SOUND_GO3         = 7,
  SOUND_TERM1       = 8,
  SOUND_TERM2       = 9,
  SOUND_TERM3       = 10,
  SOUND_COUPTERRE   = 11,
  SOUND_COUPTOC     = 12,
  SOUND_JUMP        = 13,
  SOUND_HOP         = 14,
  SOUND_SAW         = 15,
  SOUND_FIRE        = 16,
  SOUND_BURN        = 17,
  SOUND_TCHAO       = 18,
  SOUND_EAT         = 19,
  SOUND_BORN        = 20,
  SOUND_S_JUMP      = 21,
  SOUND_S_HIHI      = 22,
  SOUND_PLOUF       = 23,
  SOUND_GOAL        = 24,
  SOUND_RAYON1      = 25,
  SOUND_RAYON2      = 26,
  SOUND_VIRUS       = 27,
  SOUND_SLIDE       = 28,
  SOUND_DRINK       = 29,
  SOUND_LABO        = 30,
  SOUND_DYNAMITE    = 31,
  SOUND_DOOR        = 32,
  SOUND_FLOWER      = 33,
  SOUND_T_ENGINE    = 34,
  SOUND_T_ECRASE    = 35,
  SOUND_TRAP        = 36,
  SOUND_AIE         = 37,
  SOUND_A_POISON    = 38,
  SOUND_R_ENGINE    = 39,
  SOUND_R_APLAT     = 40,
  SOUND_R_ROTATE    = 41,
  SOUND_R_LOAD      = 42,
  SOUND_B_JUMP      = 43,
  SOUND_BOAT        = 44,
  SOUND_JEEP        = 45,
  SOUND_MINE        = 46,
  SOUND_USINE       = 47,
  SOUND_E_RAYON     = 48,
  SOUND_E_TOURNE    = 49,
  SOUND_ARROSE      = 50,
  SOUND_BECHE       = 51,
  SOUND_D_BOING     = 52,
  SOUND_D_OK        = 53,
  SOUND_D_GO        = 54,
  SOUND_D_TERM      = 55,
  SOUND_BOING1      = 56,
  SOUND_BOING2      = 57,
  SOUND_BOING3      = 58,
  SOUND_OK4         = 59,
  SOUND_OK5         = 60,
  SOUND_OK6         = 61,
  SOUND_OK1f        = 62,
  SOUND_OK2f        = 63,
  SOUND_OK3f        = 64,
  SOUND_OK1e        = 65,
  SOUND_OK2e        = 66,
  SOUND_OK3e        = 67,
  SOUND_GO4         = 68,
  SOUND_GO5         = 69,
  SOUND_GO6         = 70,
  SOUND_TERM4       = 71,
  SOUND_TERM5       = 72,
  SOUND_TERM6       = 73,
  SOUND_COUPSEC     = 74,
  SOUND_COUPPIERRE  = 75,
  SOUND_COUPSOURD   = 76,
  SOUND_COUPBREF    = 77,
  SOUND_OPEN        = 78,
  SOUND_CLOSE       = 79,
  SOUND_TELEPORTE   = 80,
  SOUND_ARMUREOPEN  = 81,
  SOUND_ARMURECLOSE = 82,
  SOUND_WIN         = 83,
  SOUND_LOST        = 84,
};

// Buttons (play):
enum Buttons {
  BUTTON_NONE      = -1,
  BUTTON_GO        = 0,
  BUTTON_STOP      = 1,
  BUTTON_EAT       = 2,
  BUTTON_CARRY     = 3,
  BUTTON_DEPOSE    = 4,
  BUTTON_ABAT      = 5,
  BUTTON_ROC       = 6,
  BUTTON_CULTIVE   = 7,
  BUTTON_BUILD1    = 8,
  BUTTON_BUILD2    = 9,
  BUTTON_BUILD3    = 10,
  BUTTON_BUILD4    = 11,
  BUTTON_BUILD5    = 12,
  BUTTON_BUILD6    = 13,
  BUTTON_WALL      = 14,
  BUTTON_PALIS     = 15,
  BUTTON_ABATn     = 16,
  BUTTON_ROCn      = 17,
  BUTTON_BRIDGE    = 18,
  BUTTON_TOWER     = 19,
  BUTTON_BOIT      = 20,
  BUTTON_LABO      = 21,
  BUTTON_FLOWER    = 22,
  BUTTON_FLOWERn   = 23,
  BUTTON_DYNAMITE  = 24,
  BUTTON_BOAT      = 25,
  BUTTON_DJEEP     = 26,
  BUTTON_FLAG      = 27,
  BUTTON_EXTRAIT   = 28,
  BUTTON_FABJEEP   = 29,
  BUTTON_FABMINE   = 30,
  BUTTON_FABDISC   = 31,
  BUTTON_REPEAT    = 32,
  BUTTON_DARMOR    = 33,
  BUTTON_MAKEARMOR = 34,
  MAXBUTTON        = 40,
};

// Errors:
enum Errors {
  NONE     = 0,
  MISC     = 1,
  GROUND   = 2,
  FREE     = 3,
  PONTOP   = 4,
  PONTTERM = 5,
  TOURISOL = 6,
  TOUREAU  = 7,
  TELE2    = 8,
  ENERGY   = 9,
  REPEAT   = 500,
};

// Lutins pour la souris

enum MouseSprites {
  SPRITE_BEGIN   = 1,
  SPRITE_ARROW   = 1,
  SPRITE_POINTER = 2,
  SPRITE_MAP     = 3,
  SPRITE_ARROWU  = 4,
  SPRITE_ARROWD  = 5,
  SPRITE_ARROWL  = 6,
  SPRITE_ARROWR  = 7,
  SPRITE_ARROWUL = 8,
  SPRITE_ARROWUR = 9,
  SPRITE_ARROWDL = 10,
  SPRITE_ARROWDR = 11,
  SPRITE_WAIT    = 12,
  SPRITE_EMPTY   = 13,
  SPRITE_FILL    = 14,
  SPRITE_END     = 14,
};

// clang-format off
#define EV_OFFSET                 0x0400

#define EV_UPDATE               (EV_OFFSET+1)
#define EV_WARPMOUSE            (EV_OFFSET+2)
#define EV_CHECKUPDATE          (EV_OFFSET+3)

#define EV_DECOR1               (EV_OFFSET+20)
#define EV_DECOR2               (EV_OFFSET+21)
#define EV_DECOR3               (EV_OFFSET+22)
#define EV_DECOR4               (EV_OFFSET+23)
#define EV_DECOR5               (EV_OFFSET+24)

#define EV_ACTION_GO            (EV_OFFSET+30)
#define EV_ACTION_ABAT1         (EV_OFFSET+31)
#define EV_ACTION_ABAT2         (EV_OFFSET+32)
#define EV_ACTION_ABAT3         (EV_OFFSET+33)
#define EV_ACTION_ABAT4         (EV_OFFSET+34)
#define EV_ACTION_ABAT5         (EV_OFFSET+35)
#define EV_ACTION_ABAT6         (EV_OFFSET+36)
#define EV_ACTION_BUILD1        (EV_OFFSET+37)
#define EV_ACTION_BUILD2        (EV_OFFSET+38)
#define EV_ACTION_BUILD3        (EV_OFFSET+39)
#define EV_ACTION_BUILD4        (EV_OFFSET+40)
#define EV_ACTION_BUILD5        (EV_OFFSET+41)
#define EV_ACTION_BUILD6        (EV_OFFSET+42)
#define EV_ACTION_STOP          (EV_OFFSET+43)
#define EV_ACTION_CARRY         (EV_OFFSET+44)
#define EV_ACTION_DROP          (EV_OFFSET+45)
#define EV_ACTION_ROC1          (EV_OFFSET+46)
#define EV_ACTION_ROC2          (EV_OFFSET+47)
#define EV_ACTION_ROC3          (EV_OFFSET+48)
#define EV_ACTION_ROC4          (EV_OFFSET+49)
#define EV_ACTION_ROC5          (EV_OFFSET+50)
#define EV_ACTION_ROC6          (EV_OFFSET+51)
#define EV_ACTION_ROC7          (EV_OFFSET+52)
#define EV_ACTION_WALL          (EV_OFFSET+53)
#define EV_ACTION_CULTIVE       (EV_OFFSET+54)
#define EV_ACTION_CULTIVE2      (EV_OFFSET+55)
#define EV_ACTION_EAT           (EV_OFFSET+56)
#define EV_ACTION_MAKE          (EV_OFFSET+57)
#define EV_ACTION_BUILD         (EV_OFFSET+58)
#define EV_ACTION_PALIS         (EV_OFFSET+59)
#define EV_ACTION_NEWBLUPI      (EV_OFFSET+60)
#define EV_ACTION_BRIDGEE       (EV_OFFSET+61)
#define EV_ACTION_BRIDGES       (EV_OFFSET+62)
#define EV_ACTION_BRIDGEO       (EV_OFFSET+63)
#define EV_ACTION_BRIDGEN       (EV_OFFSET+64)
#define EV_ACTION_BRIDGEEL      (EV_OFFSET+65)
#define EV_ACTION_BRIDGESL      (EV_OFFSET+66)
#define EV_ACTION_BRIDGEOL      (EV_OFFSET+67)
#define EV_ACTION_BRIDGENL      (EV_OFFSET+68)
#define EV_ACTION_TOWER         (EV_OFFSET+69)
#define EV_ACTION_CARRY2        (EV_OFFSET+70)
#define EV_ACTION_DROP2         (EV_OFFSET+71)
#define EV_ACTION_EAT2          (EV_OFFSET+72)
#define EV_ACTION_DRINK         (EV_OFFSET+73)
#define EV_ACTION_DRINK2        (EV_OFFSET+74)
#define EV_ACTION_LABO          (EV_OFFSET+75)
#define EV_ACTION_FLOWER1       (EV_OFFSET+76)
#define EV_ACTION_FLOWER2       (EV_OFFSET+77)
#define EV_ACTION_DYNAMITE      (EV_OFFSET+78)
#define EV_ACTION_DYNAMITE2     (EV_OFFSET+79)
#define EV_ACTION_T_DYNAMITE    (EV_OFFSET+80)
#define EV_ACTION_FLOWER3       (EV_OFFSET+81)
#define EV_ACTION_R_BUILD1      (EV_OFFSET+82)
#define EV_ACTION_R_BUILD2      (EV_OFFSET+83)
#define EV_ACTION_R_BUILD3      (EV_OFFSET+84)
#define EV_ACTION_R_BUILD4      (EV_OFFSET+85)
#define EV_ACTION_R_MAKE1       (EV_OFFSET+86)
#define EV_ACTION_R_MAKE2       (EV_OFFSET+87)
#define EV_ACTION_R_MAKE3       (EV_OFFSET+88)
#define EV_ACTION_R_MAKE4       (EV_OFFSET+89)
#define EV_ACTION_R_BUILD5      (EV_OFFSET+90)
#define EV_ACTION_R_MAKE5       (EV_OFFSET+91)
#define EV_ACTION_BOATE         (EV_OFFSET+92)
#define EV_ACTION_BOATS         (EV_OFFSET+93)
#define EV_ACTION_BOATO         (EV_OFFSET+94)
#define EV_ACTION_BOATN         (EV_OFFSET+95)
#define EV_ACTION_BOATDE        (EV_OFFSET+96)
#define EV_ACTION_BOATDS        (EV_OFFSET+97)
#define EV_ACTION_BOATDO        (EV_OFFSET+98)
#define EV_ACTION_BOATDN        (EV_OFFSET+99)
#define EV_ACTION_BOATAE        (EV_OFFSET+100)
#define EV_ACTION_BOATAS        (EV_OFFSET+101)
#define EV_ACTION_BOATAO        (EV_OFFSET+102)
#define EV_ACTION_BOATAN        (EV_OFFSET+103)
#define EV_ACTION_MJEEP         (EV_OFFSET+104)
#define EV_ACTION_DJEEP         (EV_OFFSET+105)
#define EV_ACTION_FLAG          (EV_OFFSET+106)
#define EV_ACTION_FLAG2         (EV_OFFSET+107)
#define EV_ACTION_FLAG3         (EV_OFFSET+108)
#define EV_ACTION_EXTRAIT       (EV_OFFSET+109)
#define EV_ACTION_FABJEEP       (EV_OFFSET+110)
#define EV_ACTION_FABMINE       (EV_OFFSET+111)
#define EV_ACTION_MINE          (EV_OFFSET+112)
#define EV_ACTION_MINE2         (EV_OFFSET+113)
#define EV_ACTION_R_BUILD6      (EV_OFFSET+114)
#define EV_ACTION_R_MAKE6       (EV_OFFSET+115)
#define EV_ACTION_E_RAYON       (EV_OFFSET+116)
#define EV_ACTION_ELECTRO       (EV_OFFSET+117)
#define EV_ACTION_ELECTROm      (EV_OFFSET+118)
#define EV_ACTION_GRILLE        (EV_OFFSET+119)
#define EV_ACTION_HOUSE         (EV_OFFSET+120)
#define EV_ACTION_FABDISC       (EV_OFFSET+121)
#define EV_ACTION_A_MORT        (EV_OFFSET+122)
#define EV_ACTION_REPEAT        (EV_OFFSET+123)
#define EV_ACTION_TELEPORTE00   (EV_OFFSET+124)
#define EV_ACTION_TELEPORTE10   (EV_OFFSET+125)
#define EV_ACTION_TELEPORTE01   (EV_OFFSET+126)
#define EV_ACTION_TELEPORTE11   (EV_OFFSET+127)
#define EV_ACTION_FABARMURE     (EV_OFFSET+128)
#define EV_ACTION_MARMURE       (EV_OFFSET+129)
#define EV_ACTION_DARMURE       (EV_OFFSET+130)

#define EV_BUTTON0              (EV_OFFSET+200)
#define EV_BUTTON1              (EV_OFFSET+201)
#define EV_BUTTON2              (EV_OFFSET+202)
#define EV_BUTTON3              (EV_OFFSET+203)
#define EV_BUTTON4              (EV_OFFSET+204)
#define EV_BUTTON5              (EV_OFFSET+205)
#define EV_BUTTON6              (EV_OFFSET+206)
#define EV_BUTTON7              (EV_OFFSET+207)
#define EV_BUTTON8              (EV_OFFSET+208)
#define EV_BUTTON9              (EV_OFFSET+209)
#define EV_BUTTON10             (EV_OFFSET+210)
#define EV_BUTTON11             (EV_OFFSET+211)
#define EV_BUTTON12             (EV_OFFSET+212)
#define EV_BUTTON13             (EV_OFFSET+213)
#define EV_BUTTON14             (EV_OFFSET+214)
#define EV_BUTTON15             (EV_OFFSET+215)
#define EV_BUTTON16             (EV_OFFSET+216)
#define EV_BUTTON17             (EV_OFFSET+217)
#define EV_BUTTON18             (EV_OFFSET+218)
#define EV_BUTTON19             (EV_OFFSET+219)
#define EV_BUTTON20             (EV_OFFSET+220)
#define EV_BUTTON21             (EV_OFFSET+221)
#define EV_BUTTON22             (EV_OFFSET+222)
#define EV_BUTTON23             (EV_OFFSET+223)
#define EV_BUTTON24             (EV_OFFSET+224)
#define EV_BUTTON25             (EV_OFFSET+225)
#define EV_BUTTON26             (EV_OFFSET+226)
#define EV_BUTTON27             (EV_OFFSET+227)
#define EV_BUTTON28             (EV_OFFSET+228)
#define EV_BUTTON29             (EV_OFFSET+229)
#define EV_BUTTON30             (EV_OFFSET+230)
#define EV_BUTTON31             (EV_OFFSET+231)
#define EV_BUTTON32             (EV_OFFSET+232)
#define EV_BUTTON33             (EV_OFFSET+233)
#define EV_BUTTON34             (EV_OFFSET+234)
#define EV_BUTTON35             (EV_OFFSET+235)
#define EV_BUTTON36             (EV_OFFSET+236)
#define EV_BUTTON37             (EV_OFFSET+237)
#define EV_BUTTON38             (EV_OFFSET+238)
#define EV_BUTTON39             (EV_OFFSET+239)

#define EV_READ0                (EV_OFFSET+300)
#define EV_READ1                (EV_OFFSET+301)
#define EV_READ2                (EV_OFFSET+302)
#define EV_READ3                (EV_OFFSET+303)
#define EV_READ4                (EV_OFFSET+304)
#define EV_READ5                (EV_OFFSET+305)
#define EV_READ6                (EV_OFFSET+306)
#define EV_READ7                (EV_OFFSET+307)
#define EV_READ8                (EV_OFFSET+308)
#define EV_READ9                (EV_OFFSET+309)

#define EV_WRITE0               (EV_OFFSET+310)
#define EV_WRITE1               (EV_OFFSET+311)
#define EV_WRITE2               (EV_OFFSET+312)
#define EV_WRITE3               (EV_OFFSET+313)
#define EV_WRITE4               (EV_OFFSET+314)
#define EV_WRITE5               (EV_OFFSET+315)
#define EV_WRITE6               (EV_OFFSET+316)
#define EV_WRITE7               (EV_OFFSET+317)
#define EV_WRITE8               (EV_OFFSET+318)
#define EV_WRITE9               (EV_OFFSET+319)

#define EV_PHASE_INIT           (EV_OFFSET+500)
#define EV_PHASE_PLAY           (EV_OFFSET+501)
#define EV_PHASE_BUILD          (EV_OFFSET+502)
#define EV_PHASE_READ           (EV_OFFSET+503)
#define EV_PHASE_WRITE          (EV_OFFSET+504)
#define EV_PHASE_INFO           (EV_OFFSET+505)
#define EV_PHASE_BUTTON         (EV_OFFSET+506)
#define EV_PHASE_TERM           (EV_OFFSET+507)
#define EV_PHASE_WIN            (EV_OFFSET+508)
#define EV_PHASE_LOST           (EV_OFFSET+509)
#define EV_PHASE_STOP           (EV_OFFSET+510)
#define EV_PHASE_SETUP          (EV_OFFSET+511)
#define EV_PHASE_MUSIC          (EV_OFFSET+512)
#define EV_PHASE_PLAYMOVIE      (EV_OFFSET+513)
#define EV_PHASE_WINMOVIE       (EV_OFFSET+514)
#define EV_PHASE_SCHOOL         (EV_OFFSET+515)
#define EV_PHASE_MISSION        (EV_OFFSET+516)
#define EV_PHASE_LASTWIN        (EV_OFFSET+517)
#define EV_PHASE_WRITEp         (EV_OFFSET+518)
#define EV_PHASE_SETUPp         (EV_OFFSET+519)
#define EV_PHASE_REGION         (EV_OFFSET+520)
#define EV_PHASE_INSERT         (EV_OFFSET+521)
#define EV_PHASE_HISTORY0       (EV_OFFSET+522)
#define EV_PHASE_HISTORY1       (EV_OFFSET+523)
#define EV_PHASE_HELP           (EV_OFFSET+524)
#define EV_PHASE_H0MOVIE        (EV_OFFSET+525)
#define EV_PHASE_H1MOVIE        (EV_OFFSET+526)
#define EV_PHASE_H2MOVIE        (EV_OFFSET+527)
#define EV_PHASE_TESTCD         (EV_OFFSET+528)
#define EV_PHASE_MANUEL         (EV_OFFSET+529)
#define EV_PHASE_PRIVATE        (EV_OFFSET+530)
#define EV_PHASE_UNDO           (EV_OFFSET+531)
#define EV_PHASE_BYE            (EV_OFFSET+532)
#define EV_PHASE_SKILL1         (EV_OFFSET+533)
#define EV_PHASE_SKILL2         (EV_OFFSET+534)
#define EV_PHASE_DEMO           (EV_OFFSET+535)
#define EV_PHASE_INTRO1         (EV_OFFSET+536)
#define EV_PHASE_SETTINGS       (EV_OFFSET+537)

#define EV_MUSIC_STOP           (EV_OFFSET+550)

#define EV_PREV                 (EV_OFFSET+600)
#define EV_NEXT                 (EV_OFFSET+601)
#define EV_MOVIE                (EV_OFFSET+602)
#define EV_MOVIE_PLAY           (EV_OFFSET+603)
// clang-format on

// Conditions pour gagner.

typedef struct {
  Sint16 bHachBlupi;   // blupi sur dalle hachurée
  Sint16 bHachPlanche; // planches sur dalle hachurée
  Sint16 bStopFire;    // feu éteint
  Sint16 nbMinBlupi;   // nb de blupi nécessaires
  Sint16 nbMaxBlupi;   // nb de blupi nécessaires
  Sint16 bHomeBlupi;   // blupi à la maison
  Sint16 bKillRobots;  // plus d'ennemis
  Sint16 bHachTomate;  // tomates sur dalle hachurée
  Sint16 bHachMetal;   // métal sur dalle hachurée
  Sint16 bHachRobot;   // robot sur dalle hachurée
  Sint16 reserve[14];
} Term;
