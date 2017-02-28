// Def.h
//

#pragma once

#include <SDL2/SDL_stdinc.h>

#define _INTRO          true    // true si images d'introduction


#define LXIMAGE         640     // dimensions de la fenêtre de jeu
#define LYIMAGE         480

#define POSDRAWX        144     // surface de dessin
#define POSDRAWY        15
#define DIMDRAWX        480
#define DIMDRAWY        450

#define POSMAPX         8       // surface pour la carte
#define POSMAPY         15
#define DIMMAPX         128
#define DIMMAPY         128

#define MAXCELX         200     // nb max de cellules d'un monde
#define MAXCELY         200

#define DIMCELX         60      // dimensions d'une cellule (décor)
#define DIMCELY         30

#define DIMOBJX         120     // dimensions d'un objet
#define DIMOBJY         120

#define DIMBLUPIX       60      // dimensions de blupi
#define DIMBLUPIY       60
#define SHIFTBLUPIY     5       // petit décalage vers le haut

#define DIMBUTTONX      40      // dimensions d'un button
#define DIMBUTTONY      40

#define DIMJAUGEX       124     // dimensions de la jauge
#define DIMJAUGEY       22

#define POSSTATX        12      // statistiques
#define POSSTATY        220
#define DIMSTATX        60
#define DIMSTATY        30

#define DIMTEXTX        16      // dimensions max d'un caractère
#define DIMTEXTY        16

#define DIMLITTLEX      16      // dimensions max d'un petit caractère
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

#define FOGHIDE         4


// Directions :

#define DIRECT_E        (0*16)      // est
#define DIRECT_SE       (1*16)      // sud-est
#define DIRECT_S        (2*16)      // sud
#define DIRECT_SO       (3*16)      // sud-ouest
#define DIRECT_O        (4*16)      // ouest
#define DIRECT_NO       (5*16)      // nord-ouest
#define DIRECT_N        (6*16)      // nord
#define DIRECT_NE       (7*16)      // nord-est

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


// Actions :

#define ACTION_STOP         0       // arrêt
#define ACTION_STOPf        1       // arrêt fatigué
#define ACTION_MARCHE       2       // marche
#define ACTION_MARCHEf      3       // marche fatigué
#define ACTION_BUILD        4       // construit
#define ACTION_PIOCHE       5       // pioche
#define ACTION_ENERGY       6       // prend de l'énergie
#define ACTION_TAKE         8       // fait sauter un objet sur la tête (est)
#define ACTION_DEPOSE       9       // repose l'objet sur la tête (est)
#define ACTION_SCIE         10      // scie du bois
#define ACTION_BRULE        11      // blupi crame !
#define ACTION_TCHAO        12      // blupi disparait !
#define ACTION_MANGE        13      // blupi mange
#define ACTION_NAISSANCE    14      // naissance
#define ACTION_SAUTE2       15      // saute par-dessus un obstacle
#define ACTION_SAUTE3       16      // saute par-dessus un obstacle
#define ACTION_SAUTE4       17      // saute par-dessus un obstacle
#define ACTION_SAUTE5       18      // saute par-dessus un obstacle
#define ACTION_PONT         19      // pousse un pont
#define ACTION_MISC1        20      // divers 1 (hausse les épaules)
#define ACTION_MISC2        21      // divers 2 (grat-grat)
#define ACTION_MISC3        22      // divers 3 (yoyo)
#define ACTION_MISC1f       23      // divers 1 fatigué (bof-bof)
#define ACTION_GLISSE       24      // glisse en marchant
#define ACTION_BOIT         25      // blupi boit
#define ACTION_LABO         26      // blupi travaille dans son laboratoire
#define ACTION_DYNAMITE     27      // blupi fait péter la dynamite
#define ACTION_DELAY        28      // blupi attend un frame
#define ACTION_CUEILLE1     29      // blupi cueille des fleurs
#define ACTION_CUEILLE2     30      // blupi cueille des fleurs
#define ACTION_MECHE        31      // blupi se bouche les oreilles
#define ACTION_STOPb        32      // arrêt en bateau
#define ACTION_MARCHEb      33      // avance en bateau
#define ACTION_STOPj        34      // arrêt en jeep
#define ACTION_MARCHEj      35      // avance en jeep
#define ACTION_ELECTRO      36      // blupi électrocuté
#define ACTION_GRILLE1      37      // blupi grille (phase 1)
#define ACTION_GRILLE2      38      // blupi grille (phase 2)
#define ACTION_GRILLE3      39      // blupi grille (phase 3)
#define ACTION_MISC4        40      // divers 4 (ferme les yeux)
#define ACTION_CONTENT      41      // blupi est content
#define ACTION_ARROSE       42      // blupi arrose
#define ACTION_BECHE        43      // blupi bèche
#define ACTION_CUEILLE3     44      // blupi cueille des fleurs
#define ACTION_BUILDBREF    45      // construit
#define ACTION_BUILDSEC     46      // construit
#define ACTION_BUILDSOURD   47      // construit
#define ACTION_BUILDPIERRE  48      // construit
#define ACTION_PIOCHEPIERRE 49      // pioche
#define ACTION_PIOCHESOURD  50      // pioche
#define ACTION_MISC5        51      // divers 5 (ohé)
#define ACTION_TELEPORTE1   52      // téléporte
#define ACTION_TELEPORTE2   53      // téléporte
#define ACTION_TELEPORTE3   54      // téléporte
#define ACTION_STOPa        55      // arrêt armure
#define ACTION_MARCHEa      56      // marche armure
#define ACTION_ARMUREOPEN   57      // ouvre armure
#define ACTION_ARMURECLOSE  58      // ferme armure
#define ACTION_SAUTE1       59      // saute dans la jeep
#define ACTION_MISC6        60      // divers 6 (diabolo)

#define ACTION_A_STOP       100     // araignée: arrêt
#define ACTION_A_MARCHE     101     // araignée: marche
#define ACTION_A_SAUT       102     // araignée: saute
#define ACTION_A_GRILLE     103     // araignée: grille dans rayon
#define ACTION_A_POISON     105     // araignée: empoisonée
#define ACTION_A_MORT1      106     // araignée: meurt
#define ACTION_A_MORT2      107     // araignée: meurt
#define ACTION_A_MORT3      108     // araignée: meurt

#define ACTION_V_STOP       200     // virus: arrêt
#define ACTION_V_MARCHE     201     // virus: marche
#define ACTION_V_GRILLE     202     // virus: grille dans rayon

#define ACTION_T_STOP       300     // tracks: arrêt
#define ACTION_T_MARCHE     301     // tracks: marche
#define ACTION_T_ECRASE     302     // tracks: écrase un objet

#define ACTION_R_STOP       400     // robot: arrêt
#define ACTION_R_MARCHE     401     // robot: marche
#define ACTION_R_APLAT      402     // robot: applatit
#define ACTION_R_BUILD      403     // robot: construit
#define ACTION_R_DELAY      404     // robot: construit
#define ACTION_R_CHARGE     405     // robot: recharge
#define ACTION_R_ECRASE     406     // robot: écrase un objet

#define ACTION_B_STOP       500     // bombe: arrêt
#define ACTION_B_MARCHE     501     // bombe: marche

#define ACTION_D_DELAY      600     // détonnateur: attend

#define ACTION_E_STOP       700     // électro: arrêt
#define ACTION_E_MARCHE     701     // électro: marche
#define ACTION_E_DEBUT      702     // électro: débute
#define ACTION_E_RAYON      703     // électro: rayonne

#define ACTION_D_STOP       800     // disciple: arrêt
#define ACTION_D_MARCHE     801     // disciple: marche
#define ACTION_D_BUILD      802     // disciple: construit
#define ACTION_D_PIOCHE     803     // disciple: pioche
#define ACTION_D_SCIE       804     // disciple: scie du bois
#define ACTION_D_TCHAO      805     // disciple: disparait !
#define ACTION_D_CUEILLE1   806     // disciple: cueille des fleurs
#define ACTION_D_CUEILLE2   807     // disciple: cueille des fleurs
#define ACTION_D_MECHE      808     // disciple: se bouche les oreilles
#define ACTION_D_ARROSE     809     // disciple: arrose
#define ACTION_D_BECHE      810     // disciple: bèche


// Sons :

#define SOUND_CLICK         0
#define SOUND_BOING         1
#define SOUND_OK1           2
#define SOUND_OK2           3
#define SOUND_OK3           4
#define SOUND_GO1           5
#define SOUND_GO2           6
#define SOUND_GO3           7
#define SOUND_TERM1         8
#define SOUND_TERM2         9
#define SOUND_TERM3         10
#define SOUND_COUPTERRE     11
#define SOUND_COUPTOC       12
#define SOUND_SAUT          13
#define SOUND_HOP           14
#define SOUND_SCIE          15
#define SOUND_FEU           16
#define SOUND_BRULE         17
#define SOUND_TCHAO         18
#define SOUND_MANGE         19
#define SOUND_NAISSANCE     20
#define SOUND_A_SAUT        21
#define SOUND_A_HIHI        22
#define SOUND_PLOUF         23
#define SOUND_BUT           24
#define SOUND_RAYON1        25
#define SOUND_RAYON2        26
#define SOUND_VIRUS         27
#define SOUND_GLISSE        28
#define SOUND_BOIT          29
#define SOUND_LABO          30
#define SOUND_DYNAMITE      31
#define SOUND_PORTE         32
#define SOUND_FLEUR         33
#define SOUND_T_MOTEUR      34
#define SOUND_T_ECRASE      35
#define SOUND_PIEGE         36
#define SOUND_AIE           37
#define SOUND_A_POISON      38
#define SOUND_R_MOTEUR      39
#define SOUND_R_APLAT       40
#define SOUND_R_ROTATE      41
#define SOUND_R_CHARGE      42
#define SOUND_B_SAUT        43
#define SOUND_BATEAU        44
#define SOUND_JEEP          45
#define SOUND_MINE          46
#define SOUND_USINE         47
#define SOUND_E_RAYON       48
#define SOUND_E_TOURNE      49
#define SOUND_ARROSE        50
#define SOUND_BECHE         51
#define SOUND_D_BOING       52
#define SOUND_D_OK          53
#define SOUND_D_GO          54
#define SOUND_D_TERM        55
#define SOUND_BOING1        56
#define SOUND_BOING2        57
#define SOUND_BOING3        58
#define SOUND_OK4           59
#define SOUND_OK5           60
#define SOUND_OK6           61
#define SOUND_OK1f          62
#define SOUND_OK2f          63
#define SOUND_OK3f          64
#define SOUND_OK1e          65
#define SOUND_OK2e          66
#define SOUND_OK3e          67
#define SOUND_GO4           68
#define SOUND_GO5           69
#define SOUND_GO6           70
#define SOUND_TERM4         71
#define SOUND_TERM5         72
#define SOUND_TERM6         73
#define SOUND_COUPSEC       74
#define SOUND_COUPPIERRE    75
#define SOUND_COUPSOURD     76
#define SOUND_COUPBREF      77
#define SOUND_OPEN          78
#define SOUND_CLOSE         79
#define SOUND_TELEPORTE     80
#define SOUND_ARMUREOPEN    81
#define SOUND_ARMURECLOSE   82
#define SOUND_WIN           83
#define SOUND_LOST          84


// Boutons (play) :

#define MAXBUTTON           40

#define BUTTON_GO           0
#define BUTTON_STOP         1
#define BUTTON_MANGE        2
#define BUTTON_CARRY        3
#define BUTTON_DEPOSE       4
#define BUTTON_ABAT         5
#define BUTTON_ROC          6
#define BUTTON_CULTIVE      7
#define BUTTON_BUILD1       8
#define BUTTON_BUILD2       9
#define BUTTON_BUILD3       10
#define BUTTON_BUILD4       11
#define BUTTON_BUILD5       12
#define BUTTON_BUILD6       13
#define BUTTON_MUR          14
#define BUTTON_PALIS        15
#define BUTTON_ABATn        16
#define BUTTON_ROCn         17
#define BUTTON_PONT         18
#define BUTTON_TOUR         19
#define BUTTON_BOIT         20
#define BUTTON_LABO         21
#define BUTTON_FLEUR        22
#define BUTTON_FLEURn       23
#define BUTTON_DYNAMITE     24
#define BUTTON_BATEAU       25
#define BUTTON_DJEEP        26
#define BUTTON_DRAPEAU      27
#define BUTTON_EXTRAIT      28
#define BUTTON_FABJEEP      29
#define BUTTON_FABMINE      30
#define BUTTON_FABDISC      31
#define BUTTON_REPEAT       32
#define BUTTON_DARMURE      33
#define BUTTON_FABARMURE    34


// Erreurs :

#define ERROR_MISC          1
#define ERROR_GROUND        2
#define ERROR_FREE          3
#define ERROR_PONTOP        4
#define ERROR_PONTTERM      5
#define ERROR_TOURISOL      6
#define ERROR_TOUREAU       7
#define ERROR_TELE2         8


// Lutins pour la souris

enum MouseSprites
{
    SPRITE_BEGIN    = 1,
    SPRITE_ARROW    = 1,
    SPRITE_POINTER  = 2,
    SPRITE_MAP      = 3,
    SPRITE_ARROWU   = 4,
    SPRITE_ARROWD   = 5,
    SPRITE_ARROWL   = 6,
    SPRITE_ARROWR   = 7,
    SPRITE_ARROWUL  = 8,
    SPRITE_ARROWUR  = 9,
    SPRITE_ARROWDL  = 10,
    SPRITE_ARROWDR  = 11,
    SPRITE_WAIT     = 12,
    SPRITE_EMPTY    = 13,
    SPRITE_FILL     = 14,
    SPRITE_END      = 14,
};


#define WM_USER                 0x0400

#define WM_UPDATE               (WM_USER+1)
#define WM_WARPMOUSE            (WM_USER+2)

#define WM_DECOR1               (WM_USER+20)
#define WM_DECOR2               (WM_USER+21)
#define WM_DECOR3               (WM_USER+22)
#define WM_DECOR4               (WM_USER+23)
#define WM_DECOR5               (WM_USER+24)

#define WM_ACTION_GO            (WM_USER+30)
#define WM_ACTION_ABAT1         (WM_USER+31)
#define WM_ACTION_ABAT2         (WM_USER+32)
#define WM_ACTION_ABAT3         (WM_USER+33)
#define WM_ACTION_ABAT4         (WM_USER+34)
#define WM_ACTION_ABAT5         (WM_USER+35)
#define WM_ACTION_ABAT6         (WM_USER+36)
#define WM_ACTION_BUILD1        (WM_USER+37)
#define WM_ACTION_BUILD2        (WM_USER+38)
#define WM_ACTION_BUILD3        (WM_USER+39)
#define WM_ACTION_BUILD4        (WM_USER+40)
#define WM_ACTION_BUILD5        (WM_USER+41)
#define WM_ACTION_BUILD6        (WM_USER+42)
#define WM_ACTION_STOP          (WM_USER+43)
#define WM_ACTION_CARRY         (WM_USER+44)
#define WM_ACTION_DEPOSE        (WM_USER+45)
#define WM_ACTION_ROC1          (WM_USER+46)
#define WM_ACTION_ROC2          (WM_USER+47)
#define WM_ACTION_ROC3          (WM_USER+48)
#define WM_ACTION_ROC4          (WM_USER+49)
#define WM_ACTION_ROC5          (WM_USER+50)
#define WM_ACTION_ROC6          (WM_USER+51)
#define WM_ACTION_ROC7          (WM_USER+52)
#define WM_ACTION_MUR           (WM_USER+53)
#define WM_ACTION_CULTIVE       (WM_USER+54)
#define WM_ACTION_CULTIVE2      (WM_USER+55)
#define WM_ACTION_MANGE         (WM_USER+56)
#define WM_ACTION_MAKE          (WM_USER+57)
#define WM_ACTION_BUILD         (WM_USER+58)
#define WM_ACTION_PALIS         (WM_USER+59)
#define WM_ACTION_NEWBLUPI      (WM_USER+60)
#define WM_ACTION_PONTE         (WM_USER+61)
#define WM_ACTION_PONTS         (WM_USER+62)
#define WM_ACTION_PONTO         (WM_USER+63)
#define WM_ACTION_PONTN         (WM_USER+64)
#define WM_ACTION_PONTEL        (WM_USER+65)
#define WM_ACTION_PONTSL        (WM_USER+66)
#define WM_ACTION_PONTOL        (WM_USER+67)
#define WM_ACTION_PONTNL        (WM_USER+68)
#define WM_ACTION_TOUR          (WM_USER+69)
#define WM_ACTION_CARRY2        (WM_USER+70)
#define WM_ACTION_DEPOSE2       (WM_USER+71)
#define WM_ACTION_MANGE2        (WM_USER+72)
#define WM_ACTION_BOIT          (WM_USER+73)
#define WM_ACTION_BOIT2         (WM_USER+74)
#define WM_ACTION_LABO          (WM_USER+75)
#define WM_ACTION_FLEUR1        (WM_USER+76)
#define WM_ACTION_FLEUR2        (WM_USER+77)
#define WM_ACTION_DYNAMITE      (WM_USER+78)
#define WM_ACTION_DYNAMITE2     (WM_USER+79)
#define WM_ACTION_T_DYNAMITE    (WM_USER+80)
#define WM_ACTION_FLEUR3        (WM_USER+81)
#define WM_ACTION_R_BUILD1      (WM_USER+82)
#define WM_ACTION_R_BUILD2      (WM_USER+83)
#define WM_ACTION_R_BUILD3      (WM_USER+84)
#define WM_ACTION_R_BUILD4      (WM_USER+85)
#define WM_ACTION_R_MAKE1       (WM_USER+86)
#define WM_ACTION_R_MAKE2       (WM_USER+87)
#define WM_ACTION_R_MAKE3       (WM_USER+88)
#define WM_ACTION_R_MAKE4       (WM_USER+89)
#define WM_ACTION_R_BUILD5      (WM_USER+90)
#define WM_ACTION_R_MAKE5       (WM_USER+91)
#define WM_ACTION_BATEAUE       (WM_USER+92)
#define WM_ACTION_BATEAUS       (WM_USER+93)
#define WM_ACTION_BATEAUO       (WM_USER+94)
#define WM_ACTION_BATEAUN       (WM_USER+95)
#define WM_ACTION_BATEAUDE      (WM_USER+96)
#define WM_ACTION_BATEAUDS      (WM_USER+97)
#define WM_ACTION_BATEAUDO      (WM_USER+98)
#define WM_ACTION_BATEAUDN      (WM_USER+99)
#define WM_ACTION_BATEAUAE      (WM_USER+100)
#define WM_ACTION_BATEAUAS      (WM_USER+101)
#define WM_ACTION_BATEAUAO      (WM_USER+102)
#define WM_ACTION_BATEAUAN      (WM_USER+103)
#define WM_ACTION_MJEEP         (WM_USER+104)
#define WM_ACTION_DJEEP         (WM_USER+105)
#define WM_ACTION_DRAPEAU       (WM_USER+106)
#define WM_ACTION_DRAPEAU2      (WM_USER+107)
#define WM_ACTION_DRAPEAU3      (WM_USER+108)
#define WM_ACTION_EXTRAIT       (WM_USER+109)
#define WM_ACTION_FABJEEP       (WM_USER+110)
#define WM_ACTION_FABMINE       (WM_USER+111)
#define WM_ACTION_MINE          (WM_USER+112)
#define WM_ACTION_MINE2         (WM_USER+113)
#define WM_ACTION_R_BUILD6      (WM_USER+114)
#define WM_ACTION_R_MAKE6       (WM_USER+115)
#define WM_ACTION_E_RAYON       (WM_USER+116)
#define WM_ACTION_ELECTRO       (WM_USER+117)
#define WM_ACTION_ELECTROm      (WM_USER+118)
#define WM_ACTION_GRILLE        (WM_USER+119)
#define WM_ACTION_MAISON        (WM_USER+120)
#define WM_ACTION_FABDISC       (WM_USER+121)
#define WM_ACTION_A_MORT        (WM_USER+122)
#define WM_ACTION_REPEAT        (WM_USER+123)
#define WM_ACTION_TELEPORTE00   (WM_USER+124)
#define WM_ACTION_TELEPORTE10   (WM_USER+125)
#define WM_ACTION_TELEPORTE01   (WM_USER+126)
#define WM_ACTION_TELEPORTE11   (WM_USER+127)
#define WM_ACTION_FABARMURE     (WM_USER+128)
#define WM_ACTION_MARMURE       (WM_USER+129)
#define WM_ACTION_DARMURE       (WM_USER+130)

#define WM_BUTTON0              (WM_USER+200)
#define WM_BUTTON1              (WM_USER+201)
#define WM_BUTTON2              (WM_USER+202)
#define WM_BUTTON3              (WM_USER+203)
#define WM_BUTTON4              (WM_USER+204)
#define WM_BUTTON5              (WM_USER+205)
#define WM_BUTTON6              (WM_USER+206)
#define WM_BUTTON7              (WM_USER+207)
#define WM_BUTTON8              (WM_USER+208)
#define WM_BUTTON9              (WM_USER+209)
#define WM_BUTTON10             (WM_USER+210)
#define WM_BUTTON11             (WM_USER+211)
#define WM_BUTTON12             (WM_USER+212)
#define WM_BUTTON13             (WM_USER+213)
#define WM_BUTTON14             (WM_USER+214)
#define WM_BUTTON15             (WM_USER+215)
#define WM_BUTTON16             (WM_USER+216)
#define WM_BUTTON17             (WM_USER+217)
#define WM_BUTTON18             (WM_USER+218)
#define WM_BUTTON19             (WM_USER+219)
#define WM_BUTTON20             (WM_USER+220)
#define WM_BUTTON21             (WM_USER+221)
#define WM_BUTTON22             (WM_USER+222)
#define WM_BUTTON23             (WM_USER+223)
#define WM_BUTTON24             (WM_USER+224)
#define WM_BUTTON25             (WM_USER+225)
#define WM_BUTTON26             (WM_USER+226)
#define WM_BUTTON27             (WM_USER+227)
#define WM_BUTTON28             (WM_USER+228)
#define WM_BUTTON29             (WM_USER+229)
#define WM_BUTTON30             (WM_USER+230)
#define WM_BUTTON31             (WM_USER+231)
#define WM_BUTTON32             (WM_USER+232)
#define WM_BUTTON33             (WM_USER+233)
#define WM_BUTTON34             (WM_USER+234)
#define WM_BUTTON35             (WM_USER+235)
#define WM_BUTTON36             (WM_USER+236)
#define WM_BUTTON37             (WM_USER+237)
#define WM_BUTTON38             (WM_USER+238)
#define WM_BUTTON39             (WM_USER+239)

#define WM_READ0                (WM_USER+300)
#define WM_READ1                (WM_USER+301)
#define WM_READ2                (WM_USER+302)
#define WM_READ3                (WM_USER+303)
#define WM_READ4                (WM_USER+304)
#define WM_READ5                (WM_USER+305)
#define WM_READ6                (WM_USER+306)
#define WM_READ7                (WM_USER+307)
#define WM_READ8                (WM_USER+308)
#define WM_READ9                (WM_USER+309)

#define WM_WRITE0               (WM_USER+310)
#define WM_WRITE1               (WM_USER+311)
#define WM_WRITE2               (WM_USER+312)
#define WM_WRITE3               (WM_USER+313)
#define WM_WRITE4               (WM_USER+314)
#define WM_WRITE5               (WM_USER+315)
#define WM_WRITE6               (WM_USER+316)
#define WM_WRITE7               (WM_USER+317)
#define WM_WRITE8               (WM_USER+318)
#define WM_WRITE9               (WM_USER+319)

#define WM_PHASE_INIT           (WM_USER+500)
#define WM_PHASE_PLAY           (WM_USER+501)
#define WM_PHASE_BUILD          (WM_USER+502)
#define WM_PHASE_READ           (WM_USER+503)
#define WM_PHASE_WRITE          (WM_USER+504)
#define WM_PHASE_INFO           (WM_USER+505)
#define WM_PHASE_BUTTON         (WM_USER+506)
#define WM_PHASE_TERM           (WM_USER+507)
#define WM_PHASE_WIN            (WM_USER+508)
#define WM_PHASE_LOST           (WM_USER+509)
#define WM_PHASE_STOP           (WM_USER+510)
#define WM_PHASE_SETUP          (WM_USER+511)
#define WM_PHASE_MUSIC          (WM_USER+512)
#define WM_PHASE_PLAYMOVIE      (WM_USER+513)
#define WM_PHASE_WINMOVIE       (WM_USER+514)
#define WM_PHASE_SCHOOL         (WM_USER+515)
#define WM_PHASE_MISSION        (WM_USER+516)
#define WM_PHASE_LASTWIN        (WM_USER+517)
#define WM_PHASE_WRITEp         (WM_USER+518)
#define WM_PHASE_SETUPp         (WM_USER+519)
#define WM_PHASE_REGION         (WM_USER+520)
#define WM_PHASE_INSERT         (WM_USER+521)
#define WM_PHASE_HISTORY0       (WM_USER+522)
#define WM_PHASE_HISTORY1       (WM_USER+523)
#define WM_PHASE_HELP           (WM_USER+524)
#define WM_PHASE_H0MOVIE        (WM_USER+525)
#define WM_PHASE_H1MOVIE        (WM_USER+526)
#define WM_PHASE_H2MOVIE        (WM_USER+527)
#define WM_PHASE_TESTCD         (WM_USER+528)
#define WM_PHASE_MANUEL         (WM_USER+529)
#define WM_PHASE_PRIVATE        (WM_USER+530)
#define WM_PHASE_UNDO           (WM_USER+531)
#define WM_PHASE_BYE            (WM_USER+532)
#define WM_PHASE_SKILL1         (WM_USER+533)
#define WM_PHASE_SKILL2         (WM_USER+534)
#define WM_PHASE_DEMO           (WM_USER+535)
#define WM_PHASE_INTRO1         (WM_USER+536)
#define WM_PHASE_SETTINGS       (WM_USER+537)

#define WM_MUSIC_STOP           (WM_USER+550)

#define WM_PREV                 (WM_USER+600)
#define WM_NEXT                 (WM_USER+601)
#define WM_MOVIE                (WM_USER+602)
#define WM_MOVIE_PLAY           (WM_USER+603)


// Types de gestion de la souris.

#define MOUSETYPEGRA            1
#define MOUSETYPEWIN            2
#define MOUSETYPEWINPOS         3



// Conditions pour gagner.

typedef struct
{
    Sint16  bHachBlupi;     // blupi sur dalle hachurée
    Sint16  bHachPlanche;   // planches sur dalle hachurée
    Sint16  bStopFire;      // feu éteint
    Sint16  nbMinBlupi;     // nb de blupi nécessaires
    Sint16  nbMaxBlupi;     // nb de blupi nécessaires
    Sint16  bHomeBlupi;     // blupi à la maison
    Sint16  bKillRobots;    // plus d'ennemis
    Sint16  bHachTomate;    // tomates sur dalle hachurée
    Sint16  bHachMetal;     // métal sur dalle hachurée
    Sint16  bHachRobot;     // robot sur dalle hachurée
    Sint16  reserve[14];
}
Term;

