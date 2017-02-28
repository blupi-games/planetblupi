
#include <stdlib.h>
#include <stdio.h>
#include <unordered_map>
#include <assert.h>

#include "gettext.h"
#include "blupi.h"
#include "def.h"
#include "pixmap.h"
#include "sound.h"
#include "decor.h"
#include "movie.h"
#include "button.h"
#include "menu.h"
#include "jauge.h"
#include "event.h"
#include "action.h"
#include "text.h"
#include "misc.h"

#ifdef _WIN32
#define unlink _unlink
#else // _WIN32
#include <unistd.h>
#endif // !_WINE32


#define DEF_TIME_HELP   10000       // ~10 minutes
#define DEF_TIME_DEMO   1000        // ~1 minute
#define MAXDEMO         2000


typedef struct
{
    Sint16          majRev;
    Sint16          minRev;
    Sint16          reserve1[9];
    Sint16          exercice;       // exercice en cours (0..n)
    Sint16          mission;        // mission en cours (0..n)
    Sint16          speed;
    Sint16          bMovie;
    Sint16          maxMission;     // dernière mission effectuée (0..n)
    Sint16          scrollSpeed;
    Sint16          audioVolume;
    Sint16          midiVolume;
    Sint16          bAccessBuild;
    Sint16          prive;
    Sint16          skill;
    Sint16          reserve2[93];
}
DescInfo;



// Toutes les premières lettres doivent
// être différentes !

static char cheat_code[9][20] =
{
    "vision",       // 0
    "power",        // 1
    "lonesome",     // 2
    "allmissions",  // 3
    "quick",        // 4
    "helpme",       // 5
    "invincible",   // 6
    "superblupi",   // 7
    "construire",   // 8 (CPOTUSVJSF)
};



/////////////////////////////////////////////////////////////////////////////


static Phase table[] =
{
    {
        WM_PHASE_TESTCD,
        "image/init.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_INTRO1,
        "image/intro1.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_INIT,
        "image/init.png",
        false,
        {
            {
                WM_PHASE_DEMO,
                0, {1, 108},
                16, 424 - 60 - 42 * 3 - 18,
                { translate ("Demo") },
            },
            {
                WM_PHASE_SCHOOL,
                0, {1, 79},
                16, 424 - 60 - 42 * 2,
                { translate ("Training") },
            },
            {
                WM_PHASE_MISSION,
                0, {1, 80},
                16, 424 - 60 - 42 * 1,
                { translate ("Missions") },
            },
            {
                WM_PHASE_PRIVATE,
                0, {1, 49},
                16, 424 - 60 - 42 * 0,
                { translate ("Construction") },
            },
            {
                WM_PHASE_BYE,
                0, {1, 36},
                16, 424,
                { translate ("Quit Planet Blupi") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_HISTORY0,
        "image/history0.png",
        true,
        {
            {
                WM_PHASE_INIT,
                0, {1, 50},
                42 + 42 * 0, 433,
                { translate ("Previous page") },
            },
            {
                WM_PHASE_H1MOVIE,
                0, {1, 51},
                558 - 42 * 0, 433,
                { translate ("Next page") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_HISTORY1,
        "image/history1.png",
        true,
        {
            {
                WM_PHASE_HISTORY0,
                0, {1, 50},
                42 + 42 * 0, 433,
                { translate ("Previous page") },
            },
            {
                WM_PHASE_H2MOVIE,
                0, {1, 51},
                558 - 42 * 0, 433,
                { translate ("Next page") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_INFO,
        "image/info%.3d.png",
        false,
        {
            {
                WM_PREV,
                0, {1, 50},
                558 - 42 * 2, 433,
                { translate ("Previous game") },
            },
            {
                WM_PHASE_PLAYMOVIE,
                0, {1, 48},
                558 - 42 * 1, 433,
                { translate ("Play this game") },
            },
            {
                WM_NEXT,
                0, {1, 51},
                558 - 42 * 0, 433,
                { translate ("Next game") },
            },
            {
                WM_PHASE_READ,
                0, {1, 52},
                42 + 42 * 4, 433,
                { translate ("Open another game") },
            },
            {
                WM_PHASE_SETUP,
                0, {1, 47},
                42 + 42 * 7, 433,
                { translate ("Settings") },
            },
            {
                WM_PHASE_BUILD,
                0, {1, 49},
                42 + 42 * 8, 433,
                { translate ("Construct this game") },
            },
            {
                WM_PHASE_SKILL1,
                0, {1, 94},
                150, 230,
                { translate ("Skill level") },
            },
            {
                WM_PHASE_SKILL2,
                0, {1, 95},
                150, 230 + 42,
                { translate ("Skill level") },
            },
            {
                WM_PHASE_INIT,
                0, {1, 40},
                42 + 42 * 0, 433,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_PLAY,
        "image/play.png",
        false,
        {
            {
                WM_PHASE_STOP,
                0, {1, 40},
                10 + 42 * 0, 422,
                {},
            },
            {
                WM_PHASE_SETUPp,
                0, {1, 47},
                10 + 42 * 1, 422,
                {},
            },
            {
                WM_PHASE_WRITEp,
                0, {1, 53},
                10 + 42 * 2, 422,
                {},
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_STOP,
        "image/stop%.3d.png",
        false,
        {
            {
                WM_PHASE_PLAY,
                0, {1, 77},
                558 - 42 * 1, 433,
                { translate ("Continue this game") },
            },
            {
                WM_PHASE_READ,
                0, {1, 52},
                42 + 42 * 4, 433,
                { translate ("Open another game") },
            },
            {
                WM_PHASE_WRITE,
                0, {1, 53},
                42 + 42 * 5, 433,
                { translate ("Save this game") },
            },
            {
                WM_PHASE_SETUP,
                0, {1, 47},
                42 + 42 * 7, 433,
                { translate ("Settings") },
            },
            {
                WM_PHASE_INFO,
                0, {1, 78},
                42 + 42 * 0, 433,
                { translate ("Quit this game") },
            },
            {
                WM_PHASE_HELP,
                0, {1, 86},
                42 + 42 * 9, 433,
                { translate ("Help") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_HELP,
        "image/help.png",
        true,
        {
            {
                WM_PHASE_PLAY,
                0, {1, 77},
                558 - 42 * 1, 433,
                { translate ("Continue this game") },
            },
            {
                WM_PHASE_READ,
                0, {1, 52},
                42 + 42 * 4, 433,
                { translate ("Open another game") },
            },
            {
                WM_PHASE_WRITE,
                0, {1, 53},
                42 + 42 * 5, 433,
                { translate ("Save this game") },
            },
            {
                WM_PHASE_SETUP,
                0, {1, 47},
                42 + 42 * 7, 433,
                { translate ("Settings") },
            },
            {
                WM_PHASE_STOP,
                0, {1, 50},
                42 + 42 * 0, 433,
                { translate ("Previous page") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_SETUP,
        "image/setup.png",
        false,
        {
            {
                WM_BUTTON1,
                0, {1, 50},
                54, 330,
                { translate ("Slower") },
            },
            {
                WM_BUTTON2,
                0, {1, 51},
                54 + 40, 330,
                { translate ("Faster") },
            },
            {
                WM_BUTTON3,
                0, {1, 50},
                284, 330,
                { translate ("Reduce volume") },
            },
            {
                WM_BUTTON4,
                0, {1, 51},
                284 + 40, 330,
                { translate ("Increase volume") },
            },
            {
                WM_BUTTON5,
                0, {1, 50},
                399, 330,
                { translate ("Reduce volume") },
            },
            {
                WM_BUTTON6,
                0, {1, 51},
                399 + 40, 330,
                { translate ("Increase volume") },
            },
            {
                WM_BUTTON7,
                0, {1, 50},
                514, 330,
                { translate ("No video") },
            },
            {
                WM_BUTTON8,
                0, {1, 51},
                514 + 40, 330,
                { translate ("Show videos") },
            },
            {
                WM_BUTTON9,
                0, {1, 50},
                169, 330,
                { translate ("Slower") },
            },
            {
                WM_BUTTON10,
                0, {1, 51},
                169 + 40, 330,
                { translate ("Faster") },
            },
            {
                WM_PHASE_STOP,
                0, {1, 40},
                11, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_SETUPp,
        "image/setup.png",
        false,
        {
            {
                WM_BUTTON1,
                0, {1, 50},
                54, 330,
                { translate ("Slower") },
            },
            {
                WM_BUTTON2,
                0, {1, 51},
                54 + 40, 330,
                { translate ("Faster") },
            },
            {
                WM_BUTTON3,
                0, {1, 50},
                284, 330,
                { translate ("Reduce volume") },
            },
            {
                WM_BUTTON4,
                0, {1, 51},
                284 + 40, 330,
                { translate ("Increase volume") },
            },
            {
                WM_BUTTON5,
                0, {1, 50},
                399, 330,
                { translate ("Reduce volume") },
            },
            {
                WM_BUTTON6,
                0, {1, 51},
                399 + 40, 330,
                { translate ("Increase volume") },
            },
            {
                WM_BUTTON7,
                0, {1, 50},
                514, 330,
                { translate ("No video") },
            },
            {
                WM_BUTTON8,
                0, {1, 51},
                514 + 40, 330,
                { translate ("Show videos") },
            },
            {
                WM_BUTTON9,
                0, {1, 50},
                169, 330,
                { translate ("Slower") },
            },
            {
                WM_BUTTON10,
                0, {1, 51},
                169 + 40, 330,
                { translate ("Faster") },
            },
            {
                WM_PHASE_PLAY,
                0, {1, 77},
                11, 424,
                { translate ("Continue this game") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_READ,
        "image/read.png",
        false,
        {
            {
                WM_READ0,
                0, {0},
                420, 30 + 42 * 0,
                {},
            },
            {
                WM_READ1,
                0, {0},
                420, 30 + 42 * 1,
                {},
            },
            {
                WM_READ2,
                0, {0},
                420, 30 + 42 * 2,
                {},
            },
            {
                WM_READ3,
                0, {0},
                420, 30 + 42 * 3,
                {},
            },
            {
                WM_READ4,
                0, {0},
                420, 30 + 42 * 4,
                {},
            },
            {
                WM_READ5,
                0, {0},
                420, 30 + 42 * 5,
                {},
            },
            {
                WM_READ6,
                0, {0},
                420, 30 + 42 * 6,
                {},
            },
            {
                WM_READ7,
                0, {0},
                420, 30 + 42 * 7,
                {},
            },
            {
                WM_READ8,
                0, {0},
                420, 30 + 42 * 8,
                {},
            },
            {
                WM_READ9,
                0, {0},
                420, 30 + 42 * 9,
                {},
            },
            {
                WM_PHASE_STOP,
                0, {1, 40},
                16, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_WRITE,
        "image/write.png",
        false,
        {
            {
                WM_WRITE0,
                0, {0},
                420, 30 + 42 * 0,
                {},
            },
            {
                WM_WRITE1,
                0, {0},
                420, 30 + 42 * 1,
                {},
            },
            {
                WM_WRITE2,
                0, {0},
                420, 30 + 42 * 2,
                {},
            },
            {
                WM_WRITE3,
                0, {0},
                420, 30 + 42 * 3,
                {},
            },
            {
                WM_WRITE4,
                0, {0},
                420, 30 + 42 * 4,
                {},
            },
            {
                WM_WRITE5,
                0, {0},
                420, 30 + 42 * 5,
                {},
            },
            {
                WM_WRITE6,
                0, {0},
                420, 30 + 42 * 6,
                {},
            },
            {
                WM_WRITE7,
                0, {0},
                420, 30 + 42 * 7,
                {},
            },
            {
                WM_WRITE8,
                0, {0},
                420, 30 + 42 * 8,
                {},
            },
            {
                WM_WRITE9,
                0, {0},
                420, 30 + 42 * 9,
                {},
            },
            {
                WM_PHASE_STOP,
                0, {1, 40},
                16, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_WRITEp,
        "image/write.png",
        false,
        {
            {
                WM_WRITE0,
                0, {0},
                420, 30 + 42 * 0,
                {},
            },
            {
                WM_WRITE1,
                0, {0},
                420, 30 + 42 * 1,
                {},
            },
            {
                WM_WRITE2,
                0, {0},
                420, 30 + 42 * 2,
                {},
            },
            {
                WM_WRITE3,
                0, {0},
                420, 30 + 42 * 3,
                {},
            },
            {
                WM_WRITE4,
                0, {0},
                420, 30 + 42 * 4,
                {},
            },
            {
                WM_WRITE5,
                0, {0},
                420, 30 + 42 * 5,
                {},
            },
            {
                WM_WRITE6,
                0, {0},
                420, 30 + 42 * 6,
                {},
            },
            {
                WM_WRITE7,
                0, {0},
                420, 30 + 42 * 7,
                {},
            },
            {
                WM_WRITE8,
                0, {0},
                420, 30 + 42 * 8,
                {},
            },
            {
                WM_WRITE9,
                0, {0},
                420, 30 + 42 * 9,
                {},
            },
            {
                WM_PHASE_PLAY,
                0, {1, 77},
                16, 424,
                { translate ("Continue this game") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_LOST,
        "image/lost.png",
        true,
        {
            {
                WM_PHASE_INFO,
                0, {1, 50},
                9, 431,
                { translate ("Restart this game") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_WIN,
        "image/win.png",
        true,
        {
            {
                WM_NEXT,
                0, {1, 51},
                9, 431,
                { translate ("Next game") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_LASTWIN,
        "image/last%.3d.png",
        true,
        {
            {
                WM_PHASE_INIT,
                0, {1, 51},
                9, 431,
                { translate ("Next game") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_BUILD,
        "image/build.png",
        true,
        {
            {
                WM_DECOR1,      // pose des sols
                0, {6, 0, 1, 2, 3, 4, 25},
                11 + 42 * 2, 190 + 42 * 0,
                {
                    translate ("Normal ground"),
                    translate ("Inflammable ground"),
                    translate ("Sterile ground"),
                    translate ("Water"),
                    translate ("Special pavings"),
                    translate ("Incubator or teleporter")
                },
            },
            {
                WM_DECOR2,      // pose des plantes
                0, {4, 6, 7, 8, 11},
                11 + 42 * 2, 190 + 42 * 1,
                {
                    translate ("Delete item"),
                    translate ("Decorative plants"),
                    translate ("Tree"),
                    translate ("Flowers")
                },
            },
            {
                WM_DECOR3,      // pose des batiments
                0, {11, 18, 81, 33, 61, 82, 93, 20, 21, 22, 57, 58},
                11 + 42 * 2, 190 + 42 * 2,
                {
                    translate ("Delete item"),
                    translate ("Buildings"),
                    translate ("Protection tower"),
                    translate ("Mine"),
                    translate ("Enemy buildings"),
                    translate ("Enemy barrier"),
                    translate ("Wall or palisade"),
                    translate ("Rocks"),
                    translate ("Items"),
                    translate ("Weapons"),
                    translate ("Transport")
                },
            },
            {
                WM_DECOR4,      // pose des blupi
                0, {10, 12, 13, 14, 85, 15, 16, 17, 38, 75, 56},
                11 + 42 * 2, 190 + 42 * 3,
                {
                    translate ("Delete figure"),
                    translate ("Tired Blupi"),
                    translate ("Blupi"),
                    translate ("Helper robot"),
                    translate ("Spider"),
                    translate ("Virus"),
                    translate ("Bulldozer"),
                    translate ("Bouncing bomb"),
                    translate ("Electrocutor"),
                    translate ("Master robot")
                },
            },
            {
                WM_DECOR5,      // pose les catastrophes
                0, {2, 36, 37},
                11 + 42 * 2, 190 + 42 * 4,
                {
                    translate ("Delete fire"),
                    translate ("Starting fire")
                },
            },
            {
                WM_PHASE_REGION,
                0, {1, 5},
                11 + 42 * 0, 190 + 42 * 1,
                { translate ("Scenery choice") },
            },
            {
                WM_PHASE_MUSIC,
                0, {1, 44},
                11 + 42 * 0, 190 + 42 * 2,
                { translate ("Music choice") },
            },
            {
                WM_PHASE_BUTTON,
                0, {1, 46},
                11 + 42 * 0, 190 + 42 * 3,
                { translate ("Available buttons") },
            },
            {
                WM_PHASE_TERM,
                0, {1, 45},
                11 + 42 * 0, 190 + 42 * 4,
                { translate ("Ending conditions") },
            },
            {
                WM_PHASE_INFO,
                0, {1, 40},
                11 + 42 * 0, 424,
                { translate ("Quit construction") },
            },
            {
                WM_PHASE_UNDO,
                0, {1, 87},
                11 + 42 * 2, 424,
                { translate ("Cancel last operation") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_BUTTON,
        "image/button.png",
        true,
        {
            {
                WM_BUTTON1,     // stop
                0, {1, 40},
                170 + 42 * 0, 30 + 52 * 0,
                { translate ("Stop") },
            },
            {
                WM_BUTTON0,     // go
                0, {1, 24},
                170 + 42 * 1, 30 + 52 * 0,
                { translate ("Go") },
            },
            {
                WM_BUTTON3,     // carry
                0, {1, 30},
                170 + 42 * 3, 30 + 52 * 0,
                { translate ("Take") },
            },
            {
                WM_BUTTON4,     // depose
                0, {1, 31},
                170 + 42 * 4, 30 + 52 * 0,
                { translate ("Drop") },
            },
            {
                WM_BUTTON32,    // répète
                0, {1, 100},
                170 + 42 * 6, 30 + 52 * 0,
                { translate ("Repeat") },
            },

            {
                WM_BUTTON5,     // abat
                0, {1, 22},
                170 + 42 * 0, 30 + 52 * 1,
                { translate ("Cut down a tree") },
            },
            {
                WM_BUTTON16,    // abat n
                0, {1, 42},
                170 + 42 * 1, 30 + 52 * 1,
                { translate ("Cut down trees") },
            },
            {
                WM_BUTTON6,     // roc
                0, {1, 27},
                170 + 42 * 3, 30 + 52 * 1,
                { translate ("Carve a rock") },
            },
            {
                WM_BUTTON17,    // roc n
                0, {1, 43},
                170 + 42 * 4, 30 + 52 * 1,
                { translate ("Carve rocks") },
            },
            {
                WM_BUTTON22,    // fleurs
                0, {1, 54},
                170 + 42 * 6, 30 + 52 * 1,
                { translate ("Make bunch of flowers") },
            },
            {
                WM_BUTTON23,    // fleurs n
                0, {1, 55},
                170 + 42 * 7, 30 + 52 * 1,
                { translate ("Make bunches of flowers") },
            },

            {
                WM_BUTTON9,     // build2 (couveuse)
                0, {1, 25},
                170 + 42 * 0, 30 + 52 * 2,
                { translate ("Incubator") },
            },
            {
                WM_BUTTON15,    // palis
                0, {1, 26},
                170 + 42 * 1, 30 + 52 * 2,
                { translate ("Palisade") },
            },
            {
                WM_BUTTON18,    // pont
                0, {1, 23},
                170 + 42 * 2, 30 + 52 * 2,
                { translate ("Bridge") },
            },
            {
                WM_BUTTON25,    // bateau
                0, {1, 58},
                170 + 42 * 3, 30 + 52 * 2,
                { translate ("Boat") },
            },
            {
                WM_BUTTON13,    // build6 (téléporteur)
                0, {1, 101},
                170 + 42 * 4, 30 + 52 * 2,
                { translate ("Teleporter") },
            },
            {
                WM_BUTTON14,    // mur
                0, {1, 20},
                170 + 42 * 6, 30 + 52 * 2,
                { translate ("Wall") },
            },
            {
                WM_BUTTON19,    // tour
                0, {1, 33},
                170 + 42 * 7, 30 + 52 * 2,
                { translate ("Protection tower") },
            },

            {
                WM_BUTTON8,     // build1 (cabane)
                0, {1, 19},
                170 + 42 * 0, 30 + 52 * 3,
                { translate ("Garden shed") },
            },
            {
                WM_BUTTON7,     // cultive
                0, {1, 28},
                170 + 42 * 1, 30 + 52 * 3,
                { translate ("Grow tomatoes") },
            },
            {
                WM_BUTTON2,     // mange
                0, {1, 32},
                170 + 42 * 2, 30 + 52 * 3,
                { translate ("Eat") },
            },

            {
                WM_BUTTON10,    // build3 (laboratoire)
                0, {1, 35},
                170 + 42 * 0, 30 + 52 * 4,
                { translate ("Laboratory") },
            },
            {
                WM_BUTTON21,    // laboratoire
                0, {1, 39},
                170 + 42 * 1, 30 + 52 * 4,
                { translate ("Transform") },
            },
            {
                WM_BUTTON20,    // boit
                0, {1, 34},
                170 + 42 * 2, 30 + 52 * 4,
                { translate ("Drink") },
            },
            {
                WM_BUTTON24,    // dynamite
                0, {1, 41},
                170 + 42 * 3, 30 + 52 * 4,
                { translate ("Blow up") },
            },

            {
                WM_BUTTON27,    // drapeau
                0, {1, 64},
                170 + 42 * 0, 30 + 52 * 5,
                { translate ("Prospect for iron") },
            },
            {
                WM_BUTTON11,    // build4 (mine)
                0, {1, 61},
                170 + 42 * 1, 30 + 52 * 5,
                { translate ("Mine") },
            },
            {
                WM_BUTTON28,    // extrait
                0, {1, 62},
                170 + 42 * 2, 30 + 52 * 5,
                { translate ("Extract iron") },
            },
            {
                WM_BUTTON12,    // build5 (usine)
                0, {1, 59},
                170 + 42 * 4, 30 + 52 * 5,
                { translate ("Workshop") },
            },
            {
                WM_BUTTON29,    // fabrique jeep
                0, {1, 65},
                170 + 42 * 5, 30 + 52 * 5,
                { translate ("Make a Jeep") },
            },
            {
                WM_BUTTON30,    // fabrique mine
                0, {1, 63},
                170 + 42 * 6, 30 + 52 * 5,
                { translate ("Make a time bomb") },
            },
            {
                WM_BUTTON34,    // fabrique armure
                0, {1, 106},
                170 + 42 * 7, 30 + 52 * 5,
                { translate ("Make armour") },
            },
            {
                WM_BUTTON31,    // fabrique disciple
                0, {1, 83},
                170 + 42 * 8, 30 + 52 * 5,
                { translate ("Make a helper robot") },
            },
            {
                WM_PHASE_BUILD,
                0, {1, 50},
                11, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_TERM,
        "image/term.png",
        true,
        {
            {
                WM_BUTTON8,     // home blupi
                0, {1, 81},
                170 + 42 * 0, 30 + 42 * 0,
                { translate ("Blupi in house") },
            },
            {
                WM_BUTTON9,     // kill robots
                0, {1, 57},
                170 + 42 * 1, 30 + 42 * 0,
                { translate ("No more enemies") },
            },
            {
                WM_BUTTON3,     // stop fire
                0, {1, 37},
                170 + 42 * 2, 30 + 42 * 0,
                { translate ("Fire out") },
            },
            {
                WM_BUTTON1,     // hach blupi
                0, {1, 14},
                170 + 42 * 0, 30 + 42 * 2,
                { translate ("Blupi on striped paving stones") },
            },
            {
                WM_BUTTON2,     // hach planche
                0, {1, 22},
                170 + 42 * 1, 30 + 42 * 2,
                { translate ("Planks on striped paving stones") },
            },
            {
                WM_BUTTON10,    // hach tomate
                0, {1, 28},
                170 + 42 * 2, 30 + 42 * 2,
                { translate ("Tomatoes on striped paving stones") },
            },
            {
                WM_BUTTON11,    // hach métal
                0, {1, 84},
                170 + 42 * 3, 30 + 42 * 2,
                { translate ("Platinium on striped paving stones") },
            },
            {
                WM_BUTTON12,    // hach robot
                0, {1, 94},
                170 + 42 * 4, 30 + 42 * 2,
                { translate ("Robot on striped paving stones") },
            },
            {
                WM_BUTTON4,     // - min blupi
                0, {1, 50},
                170 + 42 * 0, 30 + 42 * 4,
                { "(-)" },
            },
            {
                WM_BUTTON5,     // + min blupi
                0, {1, 51},
                170 + 42 * 1, 30 + 42 * 4,
                { "(+)" },
            },
            {
                WM_BUTTON6,     // - max blupi
                0, {1, 50},
                170 + 42 * 0, 30 + 42 * 5,
                { "(-)" },
            },
            {
                WM_BUTTON7,     // + max blupi
                0, {1, 51},
                170 + 42 * 1, 30 + 42 * 5,
                { "(+)" },
            },
            {
                WM_PHASE_BUILD,
                0, {1, 50},
                11, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_MUSIC,
        "image/music.png",
        true,
        {
            {
                WM_BUTTON1,
                0, {1, 40},
                170 + 42 * 0, 30 + 42 * 0,
                { translate ("No music") },
            },
            {
                WM_BUTTON2,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 1,
                { translate ("Music number 1") },
            },
            {
                WM_BUTTON3,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 2,
                { translate ("Music number 2") },
            },
            {
                WM_BUTTON4,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 3,
                { translate ("Music number 3") },
            },
            {
                WM_BUTTON5,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 4,
                { translate ("Music number 4") },
            },
            {
                WM_BUTTON6,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 5,
                { translate ("Music number 5") },
            },
            {
                WM_BUTTON7,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 6,
                { translate ("Music number 6") },
            },
            {
                WM_BUTTON8,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 7,
                { translate ("Music number 7") },
            },
            {
                WM_BUTTON9,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 8,
                { translate ("Music number 8") },
            },
            {
                WM_BUTTON10,
                0, {1, 44},
                170 + 42 * 0, 30 + 42 * 9,
                { translate ("Music number 9") },
            },
            {
                WM_PHASE_BUILD,
                0, {1, 50},
                11, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_REGION,
        "image/region.png",
        true,
        {
            {
                WM_BUTTON1,     // normal
                0, {0},
                220, 60,
                { translate ("Prairie") },
            },
            {
                WM_BUTTON4,     // sapins
                0, {0},
                220, 170,
                { translate ("Forest") },
            },
            {
                WM_BUTTON2,     // palmiers
                0, {0},
                220, 280,
                { translate ("Desert") },
            },
            {
                WM_BUTTON3,     // hivers
                0, {0},
                220, 390,
                { translate ("Forest under snow") },
            },
            {
                WM_PHASE_BUILD,
                0, {1, 50},
                11, 424,
                { translate ("Finish") },
            },
            {
                0
            },
        },
    },

    {
        WM_PHASE_PLAYMOVIE,
        "image/movie.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_WINMOVIE,
        "image/movie.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_H0MOVIE,
        "image/movie.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_H1MOVIE,
        "image/movie.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_H2MOVIE,
        "image/movie.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_BYE,
        "image/bye.png",
        false,
        {
            {
                0
            },
        },
    },

    {
        WM_PHASE_INSERT,
        "image/insert.png",
        false,
        {
            {
                WM_PHASE_INIT,
                0, {1, 40},
                16, 424,
                { translate ("Quit Planet Blupi") },
            },
            {
                0
            },
        },
    },

    {
        0
    }
};


/////////////////////////////////////////////////////////////////////////////


// Constructeur.

CEvent::CEvent()
{
    Sint32      i;

    m_bFullScreen   = true;
    m_mouseType     = MOUSETYPEGRA;
    m_exercice      = 0;
    m_mission       = 0;
    m_private       = 0;
    m_maxMission    = 0;
    m_phase         = 0;
    m_index         = -1;
    m_bSchool       = false;
    m_bPrivate      = false;
    m_bAccessBuild  = false;
    m_bRunMovie     = false;
    m_bBuildModify  = false;
    m_bMouseDown    = false;
    m_oldMousePos.x = 0;
    m_oldMousePos.y = 0;
    m_mouseSprite   = SPRITE_ARROW;
    m_bFillMouse    = false;
    m_bWaitMouse    = false;
    m_bHideMouse    = false;
    m_rankCheat     = -1;
    m_posCheat      = 0;
    m_speed         = 1;
    m_bMovie        = true;
    m_bSpeed        = false;
    m_bHelp         = false;
    m_bAllMissions  = false;
    m_bChangeCheat  = false;
    m_scrollSpeed   = 1;
    m_bPause        = false;
    m_bShift        = false;
    m_shiftPhase    = 0;
    m_movieToStart[0] = 0;
    m_bInfoHelp     = false;
    m_bDemoRec      = false;
    m_bDemoPlay     = false;
    m_pDemoBuffer   = nullptr;
    m_demoTime      = 0;
    m_keymod        = 0;
    m_posHelpButton = {-1, -1};
    m_pPixmap       = nullptr;
    m_pDecor        = nullptr;
    m_pSound        = nullptr;
    m_pMovie        = nullptr;
    m_bMenu         = false;
    m_bHili         = false;
    m_demoIndex     = 0;
    m_demoEnd       = 0;
    m_bHiliInfoButton = false;
    m_bHiliHelpButton = false;

    memset (m_textToolTips, 0, sizeof (m_textToolTips));
    memset (m_libelle, 0, sizeof (m_libelle));

    for (i = 0 ; i < MAXBUTTON ; i++)
        m_lastFloor[i] = 0;

    for (i = 0 ; i < MAXBUTTON ; i++)
        m_lastObject[i] = 0;

    for (i = 0 ; i < MAXBUTTON ; i++)
        m_lastHome[i] = 0;
}

// Destructeur.

CEvent::~CEvent()
{
    WriteInfo();  // lit le fichier "info.blp"
}


// Retourne la position de la souris.

POINT CEvent::GetMousePos()
{
    POINT       pos;
    Sint32 x, y;

    SDL_GetMouseState (&x, &y);
    pos.x = x;
    pos.y = y;

    return pos;
}


// Initialise le mode full screen ou non.

void CEvent::SetFullScreen (bool bFullScreen)
{
    m_bFullScreen = bFullScreen;
}

// Initialise le type de souris.

void CEvent::SetMouseType (Sint32 mouseType)
{
    m_mouseType = mouseType;
}

// Crée le gestionnaire d'événements.

void CEvent::Create (CPixmap *pPixmap, CDecor *pDecor,
                     CSound *pSound, CMovie *pMovie)
{
    POINT   pos;

    m_pPixmap = pPixmap;
    m_pDecor  = pDecor;
    m_pSound  = pSound;
    m_pMovie  = pMovie;

    ReadInfo();  // lit le fichier "info.blp"

    pos.x = 10;
    pos.y = 158;
    m_jauges[0].Create (m_pPixmap, m_pSound, pos, 1);

    pos.y += DIMJAUGEY + 2;
    m_jauges[1].Create (m_pPixmap, m_pSound, pos, 3);
}


// Retourne l'index d'un bouton.

Sint32 CEvent::GetButtonIndex (Sint32 button)
{
    int i = 0;

    if (m_index < 0)
        return -1;

    while (table[m_index].buttons[i].message != 0)
    {
        if ((Uint32) button == table[m_index].buttons[i].message)
            return i;
        i ++;
    }

    return -1;
}

Sint32 CEvent::GetState (Sint32 button)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return 0;

    return m_buttons[index].GetState();
}

void CEvent::SetState (Sint32 button, Sint32 state)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return;

    m_buttons[index].SetState (state);
}

bool CEvent::GetEnable (Sint32 button)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return 0;

    return m_buttons[index].GetEnable();
}

void CEvent::SetEnable (Sint32 button, bool bEnable)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return;

    m_buttons[index].SetEnable (bEnable);
}

bool CEvent::GetHide (Sint32 button)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return 0;

    return m_buttons[index].GetHide();
}

void CEvent::SetHide (Sint32 button, bool bHide)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return;

    m_buttons[index].SetHide (bHide);
}

Sint32 CEvent::GetMenu (Sint32 button)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return 0;

    return m_buttons[index].GetMenu();
}

void CEvent::SetMenu (Sint32 button, Sint32 menu)
{
    Sint32      index;

    index = GetButtonIndex (button);
    if (index < 0)
        return;

    m_buttons[index].SetMenu (menu);
}

// Crée tous les boutons nécessaires à la phase en cours.

bool CEvent::CreateButtons ()
{
    Sint32          i = 0, message;
    POINT       pos;

    if (m_index < 0)
        return false;

    while (table[m_index].buttons[i].message != 0)
    {
        pos.x = table[m_index].buttons[i].x;
        pos.y = table[m_index].buttons[i].y;
        message = table[m_index].buttons[i].message;

        if (m_bPrivate)
        {
            if (message == WM_PHASE_SKILL1)
            {
                pos.x = 117;
                pos.y = 115;
            }
            if (message == WM_PHASE_SKILL2)
            {
                pos.x = 117;
                pos.y = 115 + 42;
            }
        }

        m_buttons[i].Create (m_pPixmap, m_pSound, pos,
                             table[m_index].buttons[i].type,
                             table[m_index].buttons[i].iconMenu + 1,
                             table[m_index].buttons[i].iconMenu[0],
                             table[m_index].buttons[i].toolTips,
                             m_pDecor->GetRegion(),
                             message);
        i ++;
    }

    return true;

}

// Ajoute un cheat-code dans un buffer.

void AddCheatCode (char *pDst, char *pSrc)
{
    Sint32      i;
    size_t  j;

    if (pDst[0] != 0)
        strcat (pDst, " / ");

    i = 0;
    j = strlen (pDst);
    while (pSrc[i] != 0)
        pDst[j++] = pSrc[i++];
    pDst[j] = 0;
}

// Dessine un texte multi-lignes centré.

void CEvent::DrawTextCenter (const char *text, Sint32 x, Sint32 y, Sint32 font)
{
    POINT   pos;
    pos.x = x;
    pos.y = y;
    ::DrawTextCenter (m_pPixmap, pos, text, font);
}

// Dessine tous les boutons de la phase en cours.

bool CEvent::DrawButtons()
{
    Sint32          i;
    Sint32          levels[2];
    Sint32          types[2];
    Sint32          world, time, lg, button, volume, pente, icon;
    char        res[100];
    char        text[100];
    POINT       pos;
    RECT        rect;
    bool        bEnable;

    //?
#if 0
    pos.x = 1;
    pos.y = 1;
    rect.left   = pos.x;
    rect.right  = pos.x + 50;
    rect.top    = pos.y;
    rect.bottom = pos.y + DIMLITTLEY;
    m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1); // dessine le fond
    time = m_pDecor->GetTotalTime();
    sprintf (text, "time=%d", time);
    DrawText (m_pPixmap, pos, text, FONTLITTLE);
#endif
    //?

    if ((m_phase == WM_PHASE_PLAY && m_bChangeCheat) ||
        (m_phase != WM_PHASE_PLAY   &&
         m_phase != WM_PHASE_INSERT &&
         m_phase != WM_PHASE_INTRO1 &&
         m_phase != WM_PHASE_BYE))
    {
        m_bChangeCheat = false;

        text[0] = 0;
        if (m_bAllMissions)
            AddCheatCode (text, cheat_code[3]);
        if (m_bSpeed)
            AddCheatCode (text, cheat_code[4]);
        if (m_bHelp)
            AddCheatCode (text, cheat_code[5]);
        if (m_pDecor->GetInvincible())
            AddCheatCode (text, cheat_code[6]);
        if (m_pDecor->GetSuper())
            AddCheatCode (text, cheat_code[7]);
        pos.x = 2;
        pos.y = 2;
        rect.left   = pos.x;
        rect.right  = pos.x + 300;
        rect.top    = pos.y;
        rect.bottom = pos.y + DIMLITTLEY;
        m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1); // dessine le fond
        DrawText (m_pPixmap, pos, text, FONTLITTLE);
    }

    if (m_phase == WM_PHASE_INIT)
    {
        pos.x = 580;
        pos.y = 465;
        DrawText (m_pPixmap, pos, "Version 1.7", FONTLITTLE);
    }

    if (m_phase == WM_PHASE_SETUP  ||
        m_phase == WM_PHASE_SETUPp)
    {
        bEnable = true;
        if (m_speed == 1)
            bEnable = false;
        SetEnable (WM_BUTTON1, bEnable);
        bEnable = true;
        if (m_speed >= (m_bSpeed ? 8 : 2))
            bEnable = false;
        SetEnable (WM_BUTTON2, bEnable);

        volume = m_pSound->GetAudioVolume();
        bEnable = true;
        if (volume == 0 ||
            !m_pSound->GetEnable())
            bEnable = false;
        SetEnable (WM_BUTTON3, bEnable);
        bEnable = true;
        if (volume >= MAXVOLUME ||
            !m_pSound->GetEnable())
            bEnable = false;
        SetEnable (WM_BUTTON4, bEnable);

        volume = m_pSound->GetMidiVolume();
        bEnable = true;
        if (volume == 0 ||
            !m_pSound->GetEnable())
            bEnable = false;
        SetEnable (WM_BUTTON5, bEnable);
        bEnable = true;
        if (volume >= MAXVOLUME ||
            !m_pSound->GetEnable())
            bEnable = false;
        SetEnable (WM_BUTTON6, bEnable);

        if (m_pMovie->GetEnable())
        {
            SetEnable (WM_BUTTON7,  m_bMovie);
            SetEnable (WM_BUTTON8, !m_bMovie);
        }
        else
        {
            SetEnable (WM_BUTTON7, false);
            SetEnable (WM_BUTTON8, false);
        }

        bEnable = true;
        if (m_scrollSpeed == 0)
            bEnable = false;
        SetEnable (WM_BUTTON9, bEnable);
        bEnable = true;
        if (m_scrollSpeed >= 3)
            bEnable = false;
        SetEnable (WM_BUTTON10, bEnable);
    }

    assert (m_index >= 0);

    // Dessine les boutons.
    i = 0;
    while (table[m_index].buttons[i].message != 0)
    {
        m_buttons[i].Draw ();
        ++i;
    }

    if (m_phase == WM_PHASE_PLAY)
    {
        // Dessine les jauges.
        m_pDecor->GetLevelJauge (levels, types);

        for (i = 0 ; i < 2 ; i++)
        {
            if (levels[i] < 0)
                m_jauges[i].SetHide (true);
            else
            {
                m_jauges[i].SetHide (false);
                m_jauges[i].SetLevel (levels[i]);
                m_jauges[i].SetType (types[i]);
            }
            m_jauges[i].Draw();
        }

        // Dessine le menu.
        if (m_menu.IsExist())
        {
            m_pDecor->BlupiGetButtons (m_menuPos, m_menuNb,
                                       m_menuButtons, m_menuErrors,
                                       m_menuTexts,
                                       m_menuPerso);
            m_menu.Update (m_menuNb, m_menuButtons, m_menuErrors, m_menuTexts);

            button = m_menu.GetSel();
            m_pDecor->CelHiliButton (m_menuCel, button);
        }
        m_menu.Draw();

        // Dessine la pause.
        if (m_bPause)
            DrawTextCenter (gettext ("Game paused"), (10 + 134) / 2, 20);
        else
        {
            if (m_bDemoRec)    // recording demo ?
                DrawTextCenter (gettext ("REC"), (10 + 134) / 2, 20, FONTRED);
            if (m_bDemoPlay)    // playing demo ?
                DrawTextCenter (gettext ("Demo"), (10 + 134) / 2, 20, FONTRED);
        }

        // Dessine la rose des vents.
        if (!m_bPause && !m_bDemoRec && !m_bDemoPlay)
        {
            DrawTextCenter (gettext ("N"), (10 + 134) / 2, 17);
            DrawTextCenter (gettext ("S"), (10 + 134) / 2, 126);
            DrawTextCenter (gettext ("W"), 14, 70);
            DrawTextCenter (gettext ("E"), 129, 70);
        }

        // Dessine la vitesse.
        pos.x = 64;
        pos.y = LYIMAGE - 15;
        rect.left   = pos.x;
        rect.right  = pos.x + 20;
        rect.top    = pos.y;
        rect.bottom = pos.y + 15;
        m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1); // dessine le fond
        if (m_speed > 1)
        {
            sprintf (res, "x%d", m_speed);
            DrawText (m_pPixmap, pos, res);
        }

        // Dessine le bouton pour les infos.
        if (m_pDecor->GetInfoMode())    // infos visibles ?
        {
            lg = m_pDecor->GetInfoHeight();

            pos.x = POSDRAWX;
            pos.y = 0;
            rect.left   = POSDRAWX;
            rect.right  = POSDRAWX + DIMDRAWX;
            rect.top    = 0;
            rect.bottom = lg;
            m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1);

            pos.x = POSDRAWX;
            pos.y = lg;
            rect.left   = POSDRAWX;
            rect.right  = POSDRAWX + DIMDRAWX;
            rect.top    = 0;
            rect.bottom = POSDRAWY;
            m_pPixmap->DrawPart (-1, CHBACK, pos, rect, 1);

            pos.x = POSDRAWX + 20;
            pos.y = POSDRAWY + 4;
            DrawTextRect (m_pPixmap, pos, m_libelle, 0, FONTLITTLE, 1);
            pos.x = POSDRAWX + DIMDRAWX / 2 + 20;
            DrawTextRect (m_pPixmap, pos, m_libelle, 0, FONTLITTLE, 2);

            pos.x = POSDRAWX + DIMDRAWX / 2 - DIMBUTTONX / 2;
            pos.y = lg - 14;
            m_pPixmap->DrawIcon (-1, CHBUTTON, m_bHiliInfoButton ? 73 : 72, pos);

            if (IsHelpHide())    // bouton pour aide ?
                m_posHelpButton.x = -1;
            else
            {
                m_posHelpButton.x = POSDRAWX + DIMDRAWX - DIMBUTTONX - 2;
                m_posHelpButton.y = lg - DIMBUTTONY - 2;
                m_pPixmap->DrawIcon (-1, CHBUTTON, m_bHiliHelpButton ? 2 : 0, m_posHelpButton);
                if (m_bInfoHelp)
                    icon = 86;  // livre
                else
                    icon = 92;  // aide
                m_pPixmap->DrawIcon (-1, CHBUTTON, icon, m_posHelpButton);
            }
        }
        else        // infos cachées ?
        {
            pos.x = POSDRAWX + DIMDRAWX / 2 - DIMBUTTONX / 2;
            pos.y = -12;
            m_pPixmap->DrawIcon (-1, CHBUTTON, m_bHiliInfoButton ? 75 : 74, pos);
        }
        m_posInfoButton = pos;
    }

    // Dessine les noms des fichiers.
    if (m_phase == WM_PHASE_READ   ||
        m_phase == WM_PHASE_WRITE  ||
        m_phase == WM_PHASE_WRITEp)
    {
        if (m_phase == WM_PHASE_READ)
            snprintf (res, sizeof (res), "%s", gettext ("Open another game"));
        else
            snprintf (res, sizeof (res), "%s", gettext ("Save this game"));

        pos.x = 420;
        pos.y = 8;
        DrawText (m_pPixmap, pos, res);

        for (i = 0 ; i < 10 ; i++)
        {
            world = m_fileWorld[i];
            time  = m_fileTime[i];

            sprintf (text, "%d", i + 1);
            lg = GetTextWidth (text);
            pos.x = (420 + 460) / 2 - lg / 2;
            pos.y = 30 + 12 + 42 * i;
            DrawText (m_pPixmap, pos, text, FONTSLIM);

            pos.x = 420 + 50;

            if (world >= 0)
            {
                if (world >= 200)
                    snprintf (text, sizeof (text), gettext ("construction %d, time %d"),
                              (world - 200) + 1, time / 100);
                else if (world >= 100)
                    snprintf (text, sizeof (text), gettext ("mission %d, time %d"),
                              (world - 100) + 1, time / 100);
                else
                    snprintf (text, sizeof (text), gettext ("training %d, time %d"), world + 1,
                              time / 100);

                DrawText (m_pPixmap, pos, text); // partie x, temps t
            }
            else
                DrawText (m_pPixmap, pos, gettext ("free slot"), FONTRED); // libre
        }
    }

    // Dessine les réglages pour la fin de la partie.
    if (m_phase == WM_PHASE_TERM)
    {
        Term   *pTerm = m_pDecor->GetTerminated();

        DrawTextCenter (gettext ("Ending conditions"), (10 + 134) / 2, 20);

        pos.x = 170 + 42 * 2 + 4;
        pos.y = 30 + 12 + 42 * 4;
        snprintf (text, sizeof (text), gettext ("Lost if less than %d Blupi"),
                  pTerm->nbMinBlupi);
        DrawText (m_pPixmap, pos, text);

        pos.x = 170 + 42 * 2 + 4;
        pos.y = 30 + 12 + 42 * 5;
        snprintf (text, sizeof (text),
                  gettext ("Impossible to win if less than %d Blupi"), pTerm->nbMaxBlupi);
        DrawText (m_pPixmap, pos, text);
    }

    // Dessine les textes pour les choix des boutons.
    if (m_phase == WM_PHASE_BUTTON)
        DrawTextCenter (gettext ("Available buttons"), (10 + 134) / 2, 20);

    // Dessine les textes pour le choix des musiques.
    if (m_phase == WM_PHASE_MUSIC)
        DrawTextCenter (gettext ("Music choice"), (10 + 134) / 2, 20);

    // Dessine les textes pour le choix de la région.
    if (m_phase == WM_PHASE_REGION)
        DrawTextCenter (gettext ("Scenery choice"), (10 + 134) / 2, 20);

    // Ajoute "Mission numéro".
    if (m_phase == WM_PHASE_INFO)
    {
        if (m_bSchool)
            snprintf (res, sizeof (res), gettext ("Training number"));
        else
            snprintf (res, sizeof (res), gettext ("Mission number"));

        if (m_bPrivate)
            snprintf (res, sizeof (res), gettext ("Construction number"));

        lg = GetTextWidth (res);
        pos.x = (140 + 270) / 2 - lg / 2;
        pos.y = 70;
        if (m_bSchool)
            pos.x -= 40;
        if (m_bPrivate)
            pos.x -= 100;
        if (m_bPrivate)
            pos.y += 14;
        DrawText (m_pPixmap, pos, res, FONTSLIM);
    }

    // Ajoute le texte "Partie interrompue".
    if (m_phase == WM_PHASE_STOP)
    {
        char *text = gettext ("Game paused");
        lg = GetTextWidth (text);
        pos.x = (140 + 270) / 2 - lg / 2;
        pos.y = 70;
        if (m_bSchool)
            pos.x -= 40;
        if (m_bPrivate)
            pos.x -= 100;
        if (m_bPrivate)
            pos.y += 14;
        DrawText (m_pPixmap, pos, text, FONTRED);
    }

    // Ajoute le texte "Informations complémentaires".
    if (m_phase == WM_PHASE_HELP)
    {
        char *text = gettext ("Help number");
        lg = GetTextWidth (text);
        pos.x = (140 + 270) / 2 - lg / 2;
        pos.y = 70;
        if (m_bSchool)
            pos.x -= 40;
        if (m_bPrivate)
            pos.x -= 100;
        if (m_bPrivate)
            pos.y += 14;
        DrawText (m_pPixmap, pos, text, FONTRED);
    }

    // Ajoute le numéro du monde.
    if (m_phase == WM_PHASE_INFO ||
        m_phase == WM_PHASE_STOP ||
        m_phase == WM_PHASE_HELP)
    {
        if (m_bSchool)
            world = m_exercice;
        else
            world = m_mission;
        if (m_bPrivate)
            world = m_private;

        lg = GetBignumWidth (world + 1);
        pos.x = (140 + 270) / 2 - lg / 2;
        pos.y = 100;
        if (m_bSchool)
            pos.x -= 40;
        if (m_bPrivate)
            pos.x -= 135;
        if (m_bPrivate)
            pos.y = 115;
        DrawBignum (m_pPixmap, pos, world + 1);
    }

    // Affiche facile/difficile.
    if (m_phase == WM_PHASE_INFO)
    {
        if (!m_bSchool)
        {
            if (m_pDecor->GetSkill() == 0)
            {
                if (m_bPrivate)
                {
                    pos.x = 117 + 50;
                    pos.y = 115 + 13;
                }
                else
                {
                    pos.x = 150 + 50;
                    pos.y = 230 + 13;
                }
                DrawText (m_pPixmap, pos, gettext ("Easy"), FONTSLIM);
            }

            if (m_pDecor->GetSkill() == 1)
            {
                if (m_bPrivate)
                {
                    pos.x = 117 + 50;
                    pos.y = 115 + 42 + 13;
                }
                else
                {
                    pos.x = 150 + 50;
                    pos.y = 230 + 42 + 13;
                }
                DrawText (m_pPixmap, pos, gettext ("Difficult"), FONTSLIM);
            }
        }
    }

    // Affiche le libellé de l'énigme.
    if (m_phase == WM_PHASE_INFO     ||
        m_phase == WM_PHASE_STOP     ||
        m_phase == WM_PHASE_HELP     ||
        m_phase == WM_PHASE_HISTORY0 ||
        m_phase == WM_PHASE_HISTORY1)
    {
        pos.x = 355;
        pos.y = 70;
        if (m_bSchool)
            pos.x -= 20;
        if (m_bPrivate)
        {
            pos.x = 460;
            pos.y = 260;
        }
        if (m_bSchool || m_bPrivate)
            pente = 0;
        else
            pente = 19;
        DrawTextRect (m_pPixmap, pos, m_libelle, pente, FONTSLIM);
    }

    // Affiche le texte lorsque c'est raté.
    if (m_phase == WM_PHASE_LOST)
    {
        static char *list[] = {
            gettext ("You have failed, try again..."),
            gettext ("No, wrong way ..."),
            gettext ("Bang, failed again !"),
            gettext ("Another mistake..."),
            gettext ("No, not that way !"),
        };

        pos.x = 60;
        pos.y = 443;
        DrawText (m_pPixmap, pos, list[GetWorld() % 5]);
    }

    // Affiche le texte lorsque c'est réussi.
    if (m_phase == WM_PHASE_WIN)
    {
        static char *list[] = {
            gettext ("Well done !"),
            gettext ("Yes, great ..."),
            gettext ("Very good."),
            gettext ("Excellent..."),
            gettext ("Mission over..."),
        };

        pos.x = 60;
        pos.y = 443;
        DrawText (m_pPixmap, pos, list[GetWorld() % 5]);
    }

    // Affiche le texte lorsque c'est fini.
    if (m_phase == WM_PHASE_LASTWIN)
    {
        char *text;
        if (m_bSchool)
            text = gettext ("Now go on mission.");
        else
            text = gettext ("Very good, success on all missions !");

        if (m_bPrivate)
            text = gettext ("Last construction resolved !");

        pos.x = 60;
        pos.y = 443;
        DrawText (m_pPixmap, pos, text);
    }

    // Dessine les réglages.
    if (m_phase == WM_PHASE_SETUP  ||
        m_phase == WM_PHASE_SETUPp)
    {
        DrawTextCenter (gettext ("Global game\nspeed"),  54 + 40, 80);
        DrawTextCenter (gettext ("Scroll speed\nwith mouse"), 169 + 40, 80);
        DrawTextCenter (gettext ("Sound effect\nvolume"), 284 + 40, 80);
        DrawTextCenter (gettext ("Music\nvolume"), 399 + 40, 80);
        DrawTextCenter (gettext ("Video\nsequences"), 514 + 40, 80);

        sprintf (res, "x%d", m_speed);
        lg = GetTextWidth (res);
        pos.x = (54 + 40) - lg / 2;
        pos.y = 330 - 20;
        DrawText (m_pPixmap, pos, res);

        sprintf (res, "%d", m_pSound->GetAudioVolume());
        lg = GetTextWidth (res);
        pos.x = (284 + 40) - lg / 2;
        pos.y = 330 - 20;
        DrawText (m_pPixmap, pos, res);

        sprintf (res, "%d", m_pSound->GetMidiVolume());
        lg = GetTextWidth (res);
        pos.x = (399 + 40) - lg / 2;
        pos.y = 330 - 20;
        DrawText (m_pPixmap, pos, res);

        char *text = gettext ("No");
        if (m_pMovie->GetEnable() && m_bMovie)
            text = gettext ("Yes");
        lg = GetTextWidth (text);
        pos.x = (514 + 40) - lg / 2;
        pos.y = 330 - 20;
        DrawText (m_pPixmap, pos, text);

        if (!m_scrollSpeed)
            snprintf (res, sizeof (res), gettext ("None"));
        else
            sprintf (res, "%d", m_scrollSpeed);
        lg = GetTextWidth (res);
        pos.x = (169 + 40) - lg / 2;
        pos.y = 330 - 20;
        DrawText (m_pPixmap, pos, res);
    }

    // Affiche le texte de fin de la version demo.
    if (m_phase == WM_PHASE_BYE)
    {
        char *text;

        text = gettext ("You have played Planet Blupi.");

        lg = GetTextWidth (text);
        pos.x = LXIMAGE / 2 - lg / 2;
        pos.y = 20;
        DrawText (m_pPixmap, pos, text);

        text = gettext ("We hope you have had as much fun playing the game as we had making it !");

        lg = GetTextWidth (text);
        pos.x = LXIMAGE / 2 - lg / 2;
        pos.y = 40;
        DrawText (m_pPixmap, pos, text);

        text = gettext ("This game is an original creation of EPSITEC SA, CH-1092 Belmont");

        lg = GetTextWidth (text);
        pos.x = LXIMAGE / 2 - lg / 2;
        pos.y = 430;
        DrawText (m_pPixmap, pos, text);

        text = gettext ("http://www.blupi.com            blupi@epsitec.ch");

        lg = GetTextWidth (text);
        pos.x = LXIMAGE / 2 - lg / 2;
        pos.y = 450;
        DrawText (m_pPixmap, pos, text);
    }

    // Affiche le texte lorsqu'il faut insérer le CD-Rom.
    if (m_phase == WM_PHASE_INSERT)
        DrawTextCenter (
            gettext ("Insert CD-Rom Planet Blupi and wait a few seconds..."), LXIMAGE / 2,
            20);

    if (m_phase == WM_PHASE_BUILD)
        SetEnable (WM_PHASE_UNDO, m_pDecor->IsUndo());

    // Dessine les tool tips (info bulle).
    if (m_textToolTips[0] != 0)
        DrawText (m_pPixmap, m_posToolTips, m_textToolTips);

    return true;
}

// Retourne le lutin à utiliser à une position donnée.

MouseSprites CEvent::MousePosToSprite (POINT pos)
{
    MouseSprites sprite;
    bool    bUp = false, bDown = false, bLeft = false, bRight = false;

    sprite = SPRITE_POINTER;

    if (m_phase == WM_PHASE_PLAY  ||
        m_phase == WM_PHASE_BUILD)
    {
        if (pos.x >= POSMAPX && pos.x <= POSMAPX + DIMMAPX &&
            pos.y >= POSMAPY && pos.y <= POSMAPY + DIMMAPY)
            sprite = SPRITE_MAP;

        if (m_bFullScreen &&
            !m_bDemoRec   &&
            !m_bDemoPlay  &&
            m_scrollSpeed != 0)
        {
            if (pos.x <= 5 && pos.x >= -2)
                bLeft = true;

            if (pos.x >= LXIMAGE - 5 && pos.x <= LXIMAGE + 2)
                bRight = true;

            if (pos.y <= 5 && pos.y >= -2)
                bUp = true;

            if (pos.y >= LYIMAGE - 5 && pos.y <= LYIMAGE + 2)
                bDown = true;

            if (bLeft)
                sprite = SPRITE_ARROWL;
            if (bRight)
                sprite = SPRITE_ARROWR;
            if (bUp)
                sprite = SPRITE_ARROWU;
            if (bDown)
                sprite = SPRITE_ARROWD;
            if (bLeft  && bUp)
                sprite = SPRITE_ARROWUL;
            if (bLeft  && bDown)
                sprite = SPRITE_ARROWDL;
            if (bRight && bUp)
                sprite = SPRITE_ARROWUR;
            if (bRight && bDown)
                sprite = SPRITE_ARROWDR;
        }
    }
    else if (m_phase == WM_PHASE_INTRO1 )
        sprite = SPRITE_POINTER;
    else if (m_phase == WM_PHASE_BYE)
        sprite = SPRITE_POINTER;
    else
    {
        if (!MouseOnButton (pos))
            sprite = SPRITE_ARROW;
    }

    if (m_bFillMouse &&   // bidon présent ?
        pos.x >= POSDRAWX && pos.x <= POSDRAWX + DIMDRAWX &&
        pos.y >= POSDRAWY && pos.y <= POSDRAWY + DIMDRAWY)
        sprite = SPRITE_FILL;
    if (m_bWaitMouse)    // sablier présent ?
        sprite = SPRITE_WAIT;
    if (m_bHideMouse)    // souris cachée ?
        sprite = SPRITE_EMPTY;

    return sprite;
}

// Gère le lutin de la souris.

void CEvent::MouseSprite (POINT pos)
{
    m_mouseSprite = MousePosToSprite (pos);
    m_pPixmap->ChangeSprite (m_mouseSprite);
}

// Met ou enlève le sablier de la souris.

void CEvent::WaitMouse (bool bWait)
{
    m_bWaitMouse = bWait;

    if (bWait)
        m_mouseSprite = SPRITE_WAIT;
    else
        m_mouseSprite = MousePosToSprite (GetMousePos ());
    m_pPixmap->SetMouseSprite (m_mouseSprite, m_bDemoPlay);
    m_pPixmap->ChangeSprite (m_mouseSprite);
}

// Cache ou montre la souris.

void CEvent::HideMouse (bool bHide)
{
    m_bHideMouse = bHide;

    if (bHide)
    {
        m_mouseSprite = SPRITE_EMPTY;
        SDL_ShowCursor (SDL_FALSE);
    }
    else
    {
        m_mouseSprite = MousePosToSprite (GetMousePos ());
        SDL_ShowCursor (SDL_TRUE);
    }

    m_pPixmap->SetMouseSprite (m_mouseSprite, m_bDemoPlay);
    m_pPixmap->ChangeSprite (m_mouseSprite);
}

// Traite les événements pour tous les boutons.

bool CEvent::EventButtons (const SDL_Event &event, POINT pos)
{
    POINT  test;
    Sint32 lg, sound;

    // Cherche le tool tips à utiliser pour la souris.
    m_textToolTips[0] = 0;
    m_posToolTips.x = -1;

    const auto progress = [&] (CJauge &prog, const char *text) -> bool
    {
        if (prog.GetHide())
            return false;

        test = prog.GetPos();
        if (pos.x >= test.x
        && pos.x <= test.x + DIMJAUGEX
        && pos.y >= test.y
        && pos.y <= test.y + DIMJAUGEY)
        {
            snprintf (m_textToolTips, sizeof (m_textToolTips), "%s", text);
            lg = GetTextWidth (m_textToolTips);
            test.x += (DIMJAUGEX - lg) / 2;
            test.y += 4;
            m_posToolTips = test;
            return true;
        }
        return false;
    };

    if (m_phase == WM_PHASE_PLAY)
    {
        const auto spotted = progress (m_jauges[0], gettext ("Blupi's energy"));
        if (!spotted)
            progress (m_jauges[1], gettext ("Work done"));
    }
    else
    {
        int i = 0;

        assert (m_index >= 0);

        while (table[m_index].buttons[i].message != 0)
        {
            const auto text = m_buttons[i].GetToolTips (pos);
            if (text)
            {
                snprintf (m_textToolTips, sizeof (m_textToolTips), "%s", text);
                lg = GetTextWidth (m_textToolTips);
                pos.x += 10;
                pos.y += 20;
                if (pos.x > LXIMAGE - lg)
                    pos.x = LXIMAGE - lg;
                if (pos.y > LYIMAGE - 14)
                    pos.y = LYIMAGE - 14;
                m_posToolTips = pos;
                break;
            }
            i ++;
        }
    }

    if (m_phase == WM_PHASE_PLAY)
    {
        m_bHiliInfoButton = false;
        if (pos.x > m_posInfoButton.x + 6            &&
            pos.x < m_posInfoButton.x + DIMBUTTONX - 6 &&
            pos.y > m_posInfoButton.y + 8            &&
            pos.y < m_posInfoButton.y + DIMBUTTONY - 8)
        {
            m_bHiliInfoButton = true;

            if (event.type == SDL_MOUSEBUTTONDOWN
                && (event.button.button == SDL_BUTTON_LEFT
                    || event.button.button == SDL_BUTTON_RIGHT))
            {
                if (m_pDecor->GetInfoMode())
                    sound = SOUND_CLOSE;
                else
                    sound = SOUND_OPEN;
                m_pSound->PlayImage (sound, pos);
            }
            if (event.type == SDL_MOUSEBUTTONUP
                && (event.button.button == SDL_BUTTON_LEFT
                    || event.button.button == SDL_BUTTON_RIGHT))
            {
                // Montre ou cache les infos tout en haut.
                m_pDecor->SetInfoMode (!m_pDecor->GetInfoMode());
            }
        }

        m_bHiliHelpButton = false;
        if (m_posHelpButton.x != -1              &&
            pos.x > m_posHelpButton.x            &&
            pos.x < m_posHelpButton.x + DIMBUTTONX &&
            pos.y > m_posHelpButton.y            &&
            pos.y < m_posHelpButton.y + DIMBUTTONY)
        {
            m_bHiliHelpButton = true;

            if (event.type == SDL_MOUSEBUTTONDOWN
                && (event.button.button == SDL_BUTTON_LEFT
                    || event.button.button == SDL_BUTTON_RIGHT))
                m_pSound->PlayImage (SOUND_CLICK, pos);
            if (event.type == SDL_MOUSEBUTTONUP
                && (event.button.button == SDL_BUTTON_LEFT
                    || event.button.button == SDL_BUTTON_RIGHT))
            {
                // Inverse le mode aide dans les infos.
                m_bInfoHelp = !m_bInfoHelp;

                if (m_bInfoHelp)
                    ReadLibelle (GetWorld(), false, true);
                else
                    ReadLibelle (GetWorld() + 2, false, false);
            }
        }
    }

    if (m_phase == WM_PHASE_BUILD)
    {
        if (event.type == SDL_MOUSEBUTTONDOWN
            && (event.button.button == SDL_BUTTON_LEFT
                || event.button.button == SDL_BUTTON_RIGHT))
        {
            m_pDecor->HideTooltips (true); // plus de tooltips pour décor
        }
        if (event.type == SDL_MOUSEBUTTONUP
            && (event.button.button == SDL_BUTTON_LEFT
                || event.button.button == SDL_BUTTON_RIGHT))
            m_pDecor->HideTooltips (false);
    }


    int i = 0;

    assert (m_index >= 0);

    while (table[m_index].buttons[i].message != 0)
    {
        if (m_buttons[i].TreatEvent (event))
            return true;
        i ++;
    }

    if (m_phase == WM_PHASE_PLAY)
    {
        if (m_menu.TreatEvent (event))
            return true;
    }

    return false;
}


// Indique si la souris est sur un bouton.

bool CEvent::MouseOnButton (POINT pos)
{
    Sint32      i;

    if (m_index < 0)
        return false;

    i = 0;
    while (table[m_index].buttons[i].message != 0)
    {
        if (m_buttons[i].MouseOnButton (pos))
            return true;
        i ++;
    }

    return false;
}


// Retourne l'index dans table pour une phase donnée.

Sint32 CEvent::SearchPhase (Uint32 phase)
{
    Sint32      i = 0;

    while (table[i].phase != 0)
    {
        if (table[i].phase == phase)
            return i;
        i ++;
    }

    return -1;
}

// Donne le numéro du monde.

Sint32 CEvent::GetWorld()
{
    if (m_bPrivate)
        return m_private;
    if (m_bSchool)
        return m_exercice;
    else
        return m_mission;
}

// Donne le numéro physique du monde.

Sint32 CEvent::GetPhysicalWorld()
{
    if (m_bPrivate)
        return m_private + 200;
    if (m_bSchool)
        return m_exercice;
    else
        return m_mission + 100;
}

Sint32 CEvent::GetImageWorld()
{
    if (m_bPrivate)
        return 2;
    if (m_bSchool)
        return 0;
    else
        return 1;
}

// Indique si l'aide est disponible.

bool CEvent::IsHelpHide()
{
    bool    bHide = true;

    if (m_bHelp || m_pDecor->GetTotalTime() > DEF_TIME_HELP)
        bHide = false;
    if (m_bSchool || m_bPrivate)
    {
        bHide = true;  // pas d'aide pour les exercices
    }

    return bHide;
}

// Change de phase.

bool CEvent::ChangePhase (Uint32 phase)
{
    Sint32      index, world, time, total, music, i, max;
    POINT   totalDim, iconDim;
    char    filename[MAX_PATH];
    char   *pButtonExist;
    bool    bEnable, bHide;
    Term   *pTerm;

    if (phase != WM_PHASE_SETUPp &&
        phase != WM_PHASE_WRITEp &&
        phase != WM_PHASE_PLAY)
        m_pSound->StopMusic();
    if (phase == WM_PHASE_SETUPp && m_bPause)
        m_pSound->StopMusic();

    m_textToolTips[0] = 0;
    m_posToolTips.x = -1;
    m_bPause = false;
    m_keymod = 0;
    m_bMouseDown = false;
    m_debugPos.x = 0;
    m_debugPos.y = 0;

    m_pDecor->SetInfoMode (false);
    m_bInfoHelp = false;
    m_bHiliInfoButton = false;
    m_bHiliHelpButton = false;

    if (phase == WM_PHASE_INTRO1 )
        m_introTime = 0;

    if (phase == WM_PHASE_INIT)
        m_demoTime = 0;
    if (phase == WM_PHASE_PLAY &&
        !m_bDemoPlay &&
        GetPhysicalWorld() >= 150 &&  // mission spéciale démo ?
        GetPhysicalWorld() <  200)
    {
        DemoRecStart();  // début enregistrement
    }
    if (phase != WM_PHASE_PLAY)
    {
        DemoRecStop();  // stoppe l'enregistrement d'une démo
    }

    m_pDecor->UndoClose();  // libère le buffer undo

    index = SearchPhase (phase);
    if (index < 0)
        return false;

    HideMouse (false); // montre la souris
    WaitMouse (true);  // met le sablier

    if (m_bBuildModify)
    {
        m_pDecor->InitAfterBuild();
        m_bBuildModify = false;
    }

    if (m_phase == WM_PHASE_BUILD &&
        phase == WM_PHASE_INFO)    // quitte construction ?
    {
        m_pDecor->Write (GetPhysicalWorld(), false,
                         GetPhysicalWorld(), 0, 0);  // écrit le monde
    }

    // FIXME: pause is better if the game is not stop but just interrupted
    if (m_phase == WM_PHASE_PLAY && m_phase != phase)
        m_pSound->StopAllSounds();

    m_phase = phase;  // change de phase
    m_index = index;

    strcpy (filename, table[m_index].backName);
    if (strstr (filename, "%.3d") != nullptr)   // "%.3d" dans le nom ?
        sprintf (filename, table[m_index].backName, GetImageWorld());
    totalDim.x = LXIMAGE;
    totalDim.y = LYIMAGE;
    iconDim.x  = 0;
    iconDim.y  = 0;
    if (!m_pPixmap->Cache (CHBACK, filename, totalDim, iconDim))
    {
        WaitMouse (false); // enlève le sablier
        m_tryInsertCount = 40;
        m_tryPhase = m_phase;
        return ChangePhase (WM_PHASE_INSERT); // insérez le CD-Rom ...
    }

    if (m_phase == WM_PHASE_READ   ||
        m_phase == WM_PHASE_WRITE  ||
        m_phase == WM_PHASE_WRITEp)
    {
        for (i = 0 ; i < 10 ; i++)
        {
            if (m_pDecor->FileExist (i, true, world, time, total))
            {
                m_fileWorld[i] = world;
                m_fileTime[i]  = time;
            }
            else
                m_fileWorld[i] = -1;
        }
    }

    if (m_phase == WM_PHASE_INFO     ||
        m_phase == WM_PHASE_HISTORY0 ||
        m_phase == WM_PHASE_HISTORY1)
    {
        if (!m_pDecor->Read (GetPhysicalWorld(), false,
                             world, time, total) &&  // lit le monde
            !m_bAccessBuild &&
            !m_bPrivate)
        {
            m_tryInsertCount = 40;
            m_tryPhase = m_phase;
            return ChangePhase (WM_PHASE_INSERT); // insérez le CD-Rom ...
        }
        m_pDecor->SetTime (0);
        m_pDecor->SetTotalTime (0);
        m_pDecor->SetInvincible (false);
        m_pDecor->SetSuper (false);
    }

    if (m_phase == WM_PHASE_INFO     ||
        m_phase == WM_PHASE_STOP     ||
        m_phase == WM_PHASE_HELP     ||
        m_phase == WM_PHASE_HISTORY0 ||
        m_phase == WM_PHASE_HISTORY1)
    {
        if (m_bPrivate)
            PrivateLibelle();
        else if (m_phase == WM_PHASE_INFO ||
                 m_phase == WM_PHASE_STOP)
        {
            if (m_bSchool)
                ReadLibelle (GetWorld(), m_bSchool, false);
            else
                ReadLibelle (GetWorld() + 2, m_bSchool, false);
        }
        else if (m_phase == WM_PHASE_HELP)
            ReadLibelle (GetWorld(), false, true);
        else
        {
            if (m_phase == WM_PHASE_HISTORY0)
                world = 0;
            else
                world = 1;
            ReadLibelle (world, false, false);
        }
    }

    if (m_phase == WM_PHASE_TESTCD)
    {
        if (m_pDecor->Read (0, false, world, time, total))   // lit un monde
        {
            return ChangePhase (WM_PHASE_INIT); // ok
        }
        else
        {
            m_tryInsertCount = 40;
            m_tryPhase = m_phase;
            return ChangePhase (WM_PHASE_INSERT); // insérez le CD-Rom ...
        }
    }

    m_jauges[0].SetHide (true); // cache les jauges
    m_jauges[1].SetHide (true);
    CreateButtons();  // crée les boutons selon la phase
    m_bMenu = false;
    m_pDecor->HideTooltips (false);
    m_menu.Delete();
    m_pDecor->BlupiSetArrow (0, false); // enlève toutes les flèches
    m_pDecor->ResetHili();  // enlève les mises en évidence

    if (m_phase == WM_PHASE_PLAY)
    {
        m_pDecor->LoadImages();
        m_pDecor->SetBuild (false);
        m_pDecor->EnableFog (true);
        m_pDecor->NextPhase (0); // refait la carte tout de suite
        m_pDecor->StatisticInit();
        m_pDecor->TerminatedInit();
        m_bChangeCheat = true;  // affiche les cheat-codes
    }

    if (m_phase == WM_PHASE_BUILD)
    {
        m_bBuildModify = true;
        SetState (WM_DECOR1, 1);
        SetMenu (WM_DECOR1, 0); // herbe
        SetMenu (WM_DECOR2, 2); // arbre
        SetMenu (WM_DECOR3, 1); // maison
        SetMenu (WM_DECOR4, 2); // blupi fort
        SetMenu (WM_DECOR5, 1); // feu
        m_pDecor->LoadImages();
        m_pDecor->SetBuild (true);
        m_pDecor->EnableFog (false);
        m_pDecor->BlupiDeselect();
        m_pDecor->NextPhase (0); // refait la carte tout de suite
    }

    if (m_phase == WM_PHASE_INFO)
    {
        bEnable = true;
        if (GetWorld() == 0)
            bEnable = false;
        SetEnable (WM_PREV, bEnable);

        bEnable = true;
        if (m_bAllMissions)
            max = 99;
        else
            max = m_maxMission;
        if (!m_bSchool &&
            GetWorld() >= max)
            bEnable = false;
        if (!m_pDecor->FileExist (GetPhysicalWorld() + 1, false,
                                  world, time, total))
            bEnable = false;

        if (m_bAccessBuild ||
            m_pDecor->GetTotalTime() > DEF_TIME_HELP * 6)
            bEnable = true;

        if (GetWorld() >= 99)
            bEnable = false;

        if (m_bPrivate)
            bEnable = GetWorld() < 20 - 1;
        SetEnable (WM_NEXT, bEnable);

        bHide = true;
        if (m_bAccessBuild || m_bPrivate)
            bHide = false;
        SetHide (WM_PHASE_BUILD, bHide);

        if (m_bSchool)
        {
            SetHide (WM_PHASE_SKILL1, true);
            SetHide (WM_PHASE_SKILL2, true);
        }
        else
        {
            SetState (WM_PHASE_SKILL1, m_pDecor->GetSkill() == 0 ? 1 : 0);
            SetState (WM_PHASE_SKILL2, m_pDecor->GetSkill() == 1 ? 1 : 0);
        }
    }

    if (m_phase == WM_PHASE_STOP)
        SetHide (WM_PHASE_HELP, IsHelpHide());

    if (m_phase == WM_PHASE_READ)
    {
        for (i = 0 ; i < 10 ; i++)
        {
            if (m_fileWorld[i] == -1)
                SetEnable (WM_READ0 + i, false);
        }
    }

    if (m_phase == WM_PHASE_BUTTON)
    {
        pButtonExist = m_pDecor->GetButtonExist();

        for (i = 0 ; i < MAXBUTTON ; i++)
            SetState (WM_BUTTON0 + i, pButtonExist[i]);
    }

    if (m_phase == WM_PHASE_TERM)
    {
        pTerm = m_pDecor->GetTerminated();

        SetState (WM_BUTTON1,  pTerm->bHachBlupi ? 1 : 0);
        SetState (WM_BUTTON2,  pTerm->bHachPlanche ? 1 : 0);
        SetState (WM_BUTTON3,  pTerm->bStopFire ? 1 : 0);
        SetState (WM_BUTTON8,  pTerm->bHomeBlupi ? 1 : 0);
        SetState (WM_BUTTON9,  pTerm->bKillRobots ? 1 : 0);
        SetState (WM_BUTTON10, pTerm->bHachTomate ? 1 : 0);
        SetState (WM_BUTTON11, pTerm->bHachMetal ? 1 : 0);
        SetState (WM_BUTTON12, pTerm->bHachRobot ? 1 : 0);
    }

    if (m_phase == WM_PHASE_MUSIC)
    {
        music = m_pDecor->GetMusic();

        for (i = 0 ; i < 10 ; i++)
            SetState (WM_BUTTON1 + i, music == i ? 1 : 0);
    }

    if (m_phase == WM_PHASE_REGION)
    {
        music = m_pDecor->GetRegion();

        for (i = 0 ; i < 4 ; i++)
            SetState (WM_BUTTON1 + i, music == i ? 1 : 0);
    }

    if (m_phase == WM_PHASE_PLAY  ||
        m_phase == WM_PHASE_MUSIC)
    {
        if (m_pSound->IsPlayingMusic())
        {
            m_pSound->AdaptVolumeMusic();  // adapte le volume
        }
        else
        {
            music = m_pDecor->GetMusic();
            if (music > 0)
            {
                sprintf (filename, "music/music%.3d.mid", music - 1);
                m_pSound->StopMusic();
                m_pSound->PlayMusic (filename);
            }
        }
    }

    if (phase == WM_PHASE_H0MOVIE)
    {
        strcpy (m_movieToStart, "movie/history0.mkv");
        m_phaseAfterMovie = WM_PHASE_HISTORY0;
    }

    if (phase == WM_PHASE_H1MOVIE)
    {
        strcpy (m_movieToStart, "movie/history1.mkv");
        m_phaseAfterMovie = WM_PHASE_HISTORY1;
    }

    if (phase == WM_PHASE_H2MOVIE)
    {
        strcpy (m_movieToStart, "movie/history2.mkv");
        m_phaseAfterMovie = WM_PHASE_INFO;
    }

    if (phase == WM_PHASE_PLAYMOVIE)
    {
        sprintf (m_movieToStart, "movie/play%.3d.mkv", GetPhysicalWorld());
        m_phaseAfterMovie = WM_PHASE_PLAY;
    }

    if (phase == WM_PHASE_WINMOVIE)
    {
        sprintf (m_movieToStart, "movie/win%.3d.mkv", GetPhysicalWorld());
        m_phaseAfterMovie = WM_PHASE_WIN;

        if (!m_bPrivate &&
            m_pDecor->FileExist (GetPhysicalWorld(),
                                 false, world, time, total) &&
            !m_pDecor->FileExist (GetPhysicalWorld() + 1,
                                  false, world, time, total))
            m_phaseAfterMovie = WM_PHASE_LASTWIN;
    }

    WaitMouse (false); // enlève le sablier
    return true;
}

// Retourne la phase en cours.

Uint32 CEvent::GetPhase()
{
    return m_phase;
}

// Essaye de lire le CD-Rom.

void CEvent::TryInsert()
{
    if (m_tryInsertCount == 0)
        ChangePhase (m_tryPhase);
    else
        m_tryInsertCount --;
}

// Fait démarrer un film si nécessaire.

void CEvent::MovieToStart()
{
    if (m_movieToStart[0] != 0)    // y a-t-il un film à démarrer ?
    {
        HideMouse (true); // cache la souris

        if (StartMovie (m_movieToStart))
        {
            m_phase = m_phaseAfterMovie;  // prochaine phase normale
        }
        else
            ChangePhase (m_phaseAfterMovie);

        m_movieToStart[0] = 0;
    }
}


// Décale le décor.

void CEvent::DecorShift (Sint32 dx, Sint32 dy)
{
    POINT       coin;

    if (m_phase != WM_PHASE_PLAY  &&
        m_phase != WM_PHASE_BUILD)
        return;

    coin = m_pDecor->GetCoin();

    coin.x += dx;
    coin.y += dy;

    m_pDecor->SetCoin (coin);
    //? m_pDecor->NextPhase(0);  // faudra refaire la carte tout de suite
}

// Décale le décor lorsque la souris touche un bord.

void CEvent::DecorAutoShift (POINT pos)
{
    Sint32          max;
    POINT       offset;

    m_bShift = false;

    if (!m_bFullScreen ||
        m_bDemoRec     ||
        m_bDemoPlay    ||
        m_scrollSpeed == 0)
        return;

    max = 4 - m_scrollSpeed; // max <- 3..1

    if (m_phase == WM_PHASE_PLAY  ||
        m_phase == WM_PHASE_BUILD)
    {
        if (m_shiftPhase == 0)    // début du shift ?
        {
            m_shiftOffset.x = 0;
            m_shiftOffset.y = 0;
            m_shiftVector.x = 0;
            m_shiftVector.y = 0;

            if (m_mouseSprite == SPRITE_ARROWL)
            {
                m_shiftOffset.x = +2;
                m_shiftOffset.y = 0;
                m_shiftVector.x = -1;
                m_shiftVector.y = +1;
            }

            if (m_mouseSprite == SPRITE_ARROWR)
            {
                m_shiftOffset.x = -2;
                m_shiftOffset.y = 0;
                m_shiftVector.x = +1;
                m_shiftVector.y = -1;
            }

            if (m_mouseSprite == SPRITE_ARROWU)
            {
                m_shiftOffset.x = 0;
                m_shiftOffset.y = +2;
                m_shiftVector.x = -1;
                m_shiftVector.y = -1;
            }

            if (m_mouseSprite == SPRITE_ARROWD)
            {
                m_shiftOffset.x = 0;
                m_shiftOffset.y = -2;
                m_shiftVector.x = +1;
                m_shiftVector.y = +1;
            }

            if (m_mouseSprite == SPRITE_ARROWUL)
            {
                m_shiftOffset.x = +1;
                m_shiftOffset.y = +1;
                m_shiftVector.x = -1;
                m_shiftVector.y = 0;
            }

            if (m_mouseSprite == SPRITE_ARROWUR)
            {
                m_shiftOffset.x = -1;
                m_shiftOffset.y = +1;
                m_shiftVector.x = 0;
                m_shiftVector.y = -1;
            }

            if (m_mouseSprite == SPRITE_ARROWDL)
            {
                m_shiftOffset.x = +1;
                m_shiftOffset.y = -1;
                m_shiftVector.x = 0;
                m_shiftVector.y = +1;
            }

            if (m_mouseSprite == SPRITE_ARROWDR)
            {
                m_shiftOffset.x = -1;
                m_shiftOffset.y = -1;
                m_shiftVector.x = +1;
                m_shiftVector.y = 0;
            }

            if (m_shiftVector.x != 0 ||
                m_shiftVector.y != 0)
                m_shiftPhase = max;
        }

        if (m_shiftPhase > 0)
        {
            m_bShift = true;
            m_shiftPhase --;

            offset.x = m_shiftOffset.x * (max - m_shiftPhase) * (DIMCELX / 2 / max);
            offset.y = m_shiftOffset.y * (max - m_shiftPhase) * (DIMCELY / 2 / max);
            m_pDecor->SetShiftOffset (offset);

            if (m_shiftPhase == 0)    // dernière phase ?
            {
                offset.x = 0;
                offset.y = 0;
                m_pDecor->SetShiftOffset (offset);
                DecorShift (m_shiftVector.x, m_shiftVector.y);
            }
        }
    }
}

// Indique su un shift est en cours.

bool CEvent::IsShift()
{
    return m_bShift;
}


// Modifie le décor lorsque le bouton de la souris est pressé.

bool CEvent::PlayDown (POINT pos, const SDL_Event &event)
{
    bool    bDecor = false;
    bool    bMap   = false;
    Sint32      rank, button, h;
    POINT   cel;

    m_pDecor->BlupiSetArrow (0, false); // enlève toutes les flèches

    m_bMouseDown = false;

    if (m_bMenu)
    {
        m_menu.Message();
        m_bMenu = false;
        m_pDecor->HideTooltips (false);
        m_menu.Delete();
        return true;
    }

    m_pDecor->StatisticDown (pos);

    if (pos.x >= POSMAPX && pos.x <= POSMAPX + DIMMAPX &&
        pos.y >= POSMAPY && pos.y <= POSMAPY + DIMMAPY)
        bMap = true;
    h = m_pDecor->GetInfoHeight() + POSDRAWY;
    if (pos.x >= POSDRAWX && pos.x <= POSDRAWX + DIMDRAWX &&
        pos.y >= h        && pos.y <= h       + DIMDRAWY)
        bDecor = true;

    if (!bDecor && !bMap)
        return false;

    cel = m_pDecor->ConvPosToCel (pos, true);
    if (event.button.button == SDL_BUTTON_RIGHT)
    {
        if (bMap)
            button = BUTTON_GO;
        else
            button = m_pDecor->GetDefButton (cel);
        m_pDecor->BlupiGoal (cel, button);
        return true;
    }

    if (bMap)
    {
        m_pDecor->SetCoin (cel, true);
        m_pDecor->NextPhase (0); // faudra refaire la carte tout de suite
        return true;
    }

    rank = m_pDecor->GetTargetBlupi (pos);
    if (rank >= 0 &&
        !m_pDecor->IsWorkBlupi (rank))
    {
        m_bHili = true;
        m_bMouseDown = true;
        m_pDecor->BlupiHiliDown (pos, !! (m_keymod & KMOD_SHIFT));
    }
    else
    {
        m_bHili = false;
        m_bMouseDown = true;
    }

    return true;
}

// Modifie le décor lorsque la souris est déplacée.

bool CEvent::PlayMove (POINT pos, Uint16 mod)
{
    if (m_bMenu)
    {
        if (!m_menu.IsExist())
        {
            m_bMenu = false;
            m_pDecor->HideTooltips (false);
            m_menu.Delete();
        }
        return true;
    }

    m_pDecor->StatisticMove (pos);

    if (m_bMouseDown)    // bouton souris pressé ?
    {
        if (m_bHili)
            m_pDecor->BlupiHiliMove (pos);
        else
            m_pDecor->CelHili (pos, 0);
    }
    else
        m_pDecor->CelHili (pos, 0);

    return true;
}

// Modifie le décor lorsque le bouton de la souris est relâché.

bool CEvent::PlayUp (POINT pos, Uint16 mod)
{
    static Sint32 table_sound_boing[3] =
    {
        SOUND_BOING1,
        SOUND_BOING2,
        SOUND_BOING3,
    };

    m_pDecor->StatisticUp (pos);

    if (m_bMouseDown)    // bouton souris pressé ?
    {
        if (m_bHili)
            m_pDecor->BlupiHiliUp (pos);
        else
        {
            m_pDecor->BlupiGetButtons (pos, m_menuNb, m_menuButtons,
                                       m_menuErrors, m_menuTexts, m_menuPerso);
            if (m_menuNb == 0)
                m_pDecor->BlupiSound (-1, table_sound_boing[Random (0, 2)], pos);
            else
            {
                m_menuCel = m_pDecor->ConvPosToCel (pos);
                m_menuPos = pos;
                m_menu.Create (m_pPixmap, m_pSound,
                               pos, m_menuNb, m_menuButtons, m_menuErrors,
                               m_menuTexts, m_menuPerso);
                m_bMenu = true;
                m_pDecor->HideTooltips (true); // plus de tooltips pour décor
            }
        }
    }

    m_bMouseDown = false;

    return true;
}


// Clic dans un bouton.
// Message = WM_BUTTON0..WM_BUTTON39

void CEvent::ChangeButtons (Sint32 message)
{
    Sint32      button, state, volume, max;
    char   *pButtonExist;
    Term   *pTerm;

    if (m_phase == WM_PHASE_PLAY)
    {
        button = m_menuButtons[message - WM_BUTTON0];
        m_pDecor->BlupiGoal (m_menuCel, button);
    }

    if (m_phase == WM_PHASE_BUTTON)
    {
        pButtonExist = m_pDecor->GetButtonExist();

        state = GetState (message);
        if (state == 0)
            state = 1;
        else
            state = 0;
        SetState (message, state); // pressé <-> relâché

        pButtonExist[message - WM_BUTTON0] = state;
        pButtonExist[BUTTON_DJEEP]   = true;
        pButtonExist[BUTTON_DARMURE] = true;
    }

    if (m_phase == WM_PHASE_TERM)
    {
        pTerm = m_pDecor->GetTerminated();

        if (message == WM_BUTTON1  ||
            message == WM_BUTTON2  ||
            message == WM_BUTTON3  ||
            message == WM_BUTTON8  ||
            message == WM_BUTTON9  ||
            message == WM_BUTTON10 ||
            message == WM_BUTTON11 ||
            message == WM_BUTTON12)
        {
            state = GetState (message);
            if (state == 0)
                state = 1;
            else
                state = 0;
            SetState (message, state); // pressé <-> relâché

            if (message == WM_BUTTON1)
                pTerm->bHachBlupi   = state;
            if (message == WM_BUTTON2)
                pTerm->bHachPlanche = state;
            if (message == WM_BUTTON3)
                pTerm->bStopFire    = state;
            if (message == WM_BUTTON8)
                pTerm->bHomeBlupi   = state;
            if (message == WM_BUTTON9)
                pTerm->bKillRobots  = state;
            if (message == WM_BUTTON10)
                pTerm->bHachTomate  = state;
            if (message == WM_BUTTON11)
                pTerm->bHachMetal   = state;
            if (message == WM_BUTTON12)
                pTerm->bHachRobot   = state;
        }

        if (message == WM_BUTTON4)
        {
            if (pTerm->nbMinBlupi > 1)
                pTerm->nbMinBlupi --;
        }
        if (message == WM_BUTTON5)
        {
            if (pTerm->nbMinBlupi < 100)
                pTerm->nbMinBlupi ++;
        }

        if (message == WM_BUTTON6)
        {
            if (pTerm->nbMaxBlupi > 1)
                pTerm->nbMaxBlupi --;
        }
        if (message == WM_BUTTON7)
        {
            if (pTerm->nbMaxBlupi < 100)
                pTerm->nbMaxBlupi ++;
        }
    }

    if (m_phase == WM_PHASE_MUSIC)
    {
        m_pDecor->SetMusic (message - WM_BUTTON1);
        ChangePhase (m_phase);
    }

    if (m_phase == WM_PHASE_REGION)
    {
        m_pDecor->SetRegion (message - WM_BUTTON1);
        ChangePhase (WM_PHASE_BUILD);
    }

    if (m_phase == WM_PHASE_SETUP  ||
        m_phase == WM_PHASE_SETUPp)
    {
        if (message == WM_BUTTON1)
        {
            if (m_speed > 1)
                m_speed = m_speed >> 1;
        }
        if (message == WM_BUTTON2)
        {
            if (m_bSpeed)
                max = 8;
            else
                max = 2;
            if (m_speed < max)
                m_speed = m_speed << 1;
        }

        if (message == WM_BUTTON3)
        {
            volume = m_pSound->GetAudioVolume();
            if (volume > 0)
                m_pSound->SetAudioVolume (volume - 1);
        }
        if (message == WM_BUTTON4)
        {
            volume = m_pSound->GetAudioVolume();
            if (volume < MAXVOLUME)
                m_pSound->SetAudioVolume (volume + 1);
        }

        if (message == WM_BUTTON5)
        {
            volume = m_pSound->GetMidiVolume();
            if (volume > 0)
            {
                m_pSound->SetMidiVolume (volume - 1);
                m_pSound->AdaptVolumeMusic();
            }
        }
        if (message == WM_BUTTON6)
        {
            volume = m_pSound->GetMidiVolume();
            if (volume < MAXVOLUME)
            {
                m_pSound->SetMidiVolume (volume + 1);
                m_pSound->AdaptVolumeMusic();
            }
        }

        if (message == WM_BUTTON7)
            m_bMovie = false;
        if (message == WM_BUTTON8)
            m_bMovie = true;

        if (message == WM_BUTTON9)
        {
            if (m_scrollSpeed > 0)
                m_scrollSpeed --;
        }
        if (message == WM_BUTTON10)
        {
            if (m_scrollSpeed < 3)
                m_scrollSpeed ++;
        }
    }
}



// Met un sol si nécessaire sous un objet.

void CEvent::BuildFloor (POINT cel, Sint32 insIcon)
{
    Sint32      iFloor, channel, icon;

    if (insIcon == -1)
        return;  // supprime ?

    if (insIcon <    6 ||   // petite plante ?
        insIcon == 117)    // bateau ?
        return;

    iFloor = 1;  // herbe

    if (insIcon ==  28 ||   // laboratoire ?
        insIcon ==  61 ||  // cabane de jardin ?
        insIcon == 113 ||  // maison ?
        insIcon == 120)    // usine ?
    {
        iFloor = 16;  // sol brun foncé
    }

    if (insIcon == 122)    // mine de fer ?
    {
        iFloor = 71;  // sol avec minerai
    }

    if (insIcon ==  99 ||   // station de recharge ?
        insIcon == 100 ||  // usine ennemie ?
        insIcon == 102 ||  // usine ennemie ?
        insIcon == 104 ||  // usine ennemie ?
        (insIcon >= 106 && insIcon <= 112) ||  // barrière ?
        insIcon == 115 ||  // usine ennemie ?
        insIcon ==  17 ||  // usine ennemie ?
        insIcon ==  12)    // fusée ?
    {
        iFloor = 67;  // sol bleu ennemi
    }

    m_pDecor->GetFloor (cel, channel, icon);
    if ((channel == CHFLOOR &&
         icon >= 2 && icon <= 14) ||  // eau ou rive ?
        iFloor != 1)
    {
        m_pDecor->PutFloor (cel, CHFLOOR, iFloor); // met un sol
        m_pDecor->ArrangeFloor (cel);
    }
}

// Enlève si nécessaire un objet sur l'eau.

void CEvent::BuildWater (POINT cel, Sint32 insIcon)
{
    Sint32      channel, icon;

    if (insIcon != 14)
        return;  // rien à faire si pas eau

    m_pDecor->GetObject (cel, channel, icon);
    if (channel == CHOBJECT &&
        icon >=   6 &&  // objet (pas petite plante) ?
        icon != 117)    // pas bateau ?
    {
        m_pDecor->PutObject (cel, -1, -1); // enlève l'objet
        m_pDecor->ArrangeObject (cel);
    }
}

// Cette table donne les objets à construire en fonction
// du choix dans le menu.

static Sint32 tableFloor[] =
{
    1, 49, 50, 51, 0, 0, 0, 0, 0, 0,    // 0 herbe
    20, 66, 79, 0, 0, 0, 0, 0, 0, 0,    // 1 foncé
    33, 46, 47, 48, 71, 0, 0, 0, 0, 0,  // 2 terre
    14, 0, 0, 0, 0, 0, 0, 0, 0, 0,      // 3 mer
    15, 16, 17, 18, 19, 65, 67, 0, 0, 0, // 4 dalles
    52, 80, 0, 0, 0, 0, 0, 0, 0, 0,     // 5 couveuse
};

static Sint32 tableObject[] =
{
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,      // 0 détruit
    0, 4, 1, 2, 3, 5, 0, 0, 0, 0,       // 1 plantes
    6, 7, 8, 9, 10, 11, 0, 0, 0, 0,     // 2 arbres
    81, 83, 94, 0, 0, 0, 0, 0, 0, 0,    // 5 fleurs
};

static Sint32 tableHome[] =
{
    -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,      //  0 détruit
    113, 61, 28, 120, 0, 0, 0, 0, 0, 0, //  1 maison
    27, 0, 0, 0, 0, 0, 0, 0, 0, 0,      //  2 tour de protection
    122, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //  3 mine de fer
    99, 100, 102, 104, 115, 17, 12, 0, 0, 0, //  4 ennemi
    112, 0, 0, 0, 0, 0, 0, 0, 0, 0,     //  5 barrière
    26, 71, 0, 0, 0, 0, 0, 0, 0, 0,     //  6 palissade
    37, 38, 39, 40, 41, 42, 43, 0, 0, 0, //  7 rochers
    36, 44, 60, 63, 80, 123, 14, 0, 0, 0, //  8 matières
    85, 125, 93, 92, 0, 0, 0, 0, 0, 0,  //  9 pièges
    117, 118, 16, 0, 0, 0, 0, 0, 0, 0,  // 10 véhicules
};

// Modifie le décor lorsque le bouton de la souris est pressé.

bool CEvent::BuildDown (POINT pos, Uint16 mod, bool bMix)
{
    POINT       cel;
    Sint32          menu, channel, icon;

    if (bMix && m_pDecor->MapMove (pos))
        return true;

    if (pos.x < POSDRAWX || pos.x > POSDRAWX + DIMDRAWX ||
        pos.y < POSDRAWY || pos.y > POSDRAWY + DIMDRAWY)
        return false;

    if (bMix)
    {
        m_pDecor->UndoCopy();  // copie le décor pour undo év.
    }

    if (GetState (WM_DECOR1) == 1)   // pose d'un sol
    {
        cel = m_pDecor->ConvPosToCel2 (pos);
        menu = GetMenu (WM_DECOR1);

        if (!m_pDecor->GetFloor (cel, channel, icon))
            return false;

        if (bMix && tableFloor[menu * 10 + m_lastFloor[menu]] == icon)
        {
            m_lastFloor[menu] ++;
            if (tableFloor[menu * 10 + m_lastFloor[menu]] == 0)
                m_lastFloor[menu] = 0;
        }

        if (mod & KMOD_CTRL)  // touche Ctrl enfoncée ?
        {
            WaitMouse (true);
            m_pDecor->ArrangeFill (cel, CHFLOOR, tableFloor[menu * 10 + m_lastFloor[menu]],
                                   true);
            WaitMouse (false);
        }
        else
        {
            icon = tableFloor[menu * 10 + m_lastFloor[menu]];
            if (menu >= 1)    // met un sol ?
            {
                BuildWater (cel, icon); // enlève les objets
            }
            m_pDecor->PutFloor (cel, CHFLOOR, icon);
            m_pDecor->ArrangeFloor (cel);
        }
    }

    if (GetState (WM_DECOR2) == 1)   // pose d'un objet
    {
        cel = m_pDecor->ConvPosToCel2 (pos);
        menu = GetMenu (WM_DECOR2);

        if (!m_pDecor->GetObject (cel, channel, icon))
            return false;

        if (bMix && tableObject[menu * 10 + m_lastObject[menu]] == icon)
        {
            m_lastObject[menu] ++;
            if (tableObject[menu * 10 + m_lastObject[menu]] == 0)
                m_lastObject[menu] = 0;
        }

        if (mod & KMOD_CTRL)  // touche Ctrl enfoncée ?
        {
            WaitMouse (true);
            m_pDecor->ArrangeFill (cel, CHOBJECT,
                                   tableObject[menu * 10 + m_lastObject[menu]], false);
            WaitMouse (false);
        }
        else
        {
            icon = tableObject[menu * 10 + m_lastObject[menu]];
            BuildFloor (cel, icon); // met un sol si nécessaire
            m_pDecor->PutObject (cel, CHOBJECT, icon);
            m_pDecor->ArrangeObject (cel);
        }
    }

    if (GetState (WM_DECOR3) == 1)   // pose d'un batiment
    {
        cel = m_pDecor->ConvPosToCel2 (pos);
        menu = GetMenu (WM_DECOR3);

        if (!m_pDecor->GetObject (cel, channel, icon))
            return false;

        if (bMix && tableHome[menu * 10 + m_lastHome[menu]] == icon)
        {
            m_lastHome[menu] ++;
            if (tableHome[menu * 10 + m_lastHome[menu]] == 0)
                m_lastHome[menu] = 0;
        }

        if (mod & KMOD_CTRL)  // touche Ctrl enfoncée ?
        {
            WaitMouse (true);
            m_pDecor->ArrangeFill (cel, CHOBJECT, tableHome[menu * 10 + m_lastHome[menu]],
                                   false);
            WaitMouse (false);
        }
        else
        {
            icon = tableHome[menu * 10 + m_lastHome[menu]];
            BuildFloor (cel, icon); // met un sol si nécessaire
            m_pDecor->PutObject (cel, CHOBJECT, icon);
            m_pDecor->ArrangeObject (cel);
        }
    }

    if (GetState (WM_DECOR4) == 1)   // pose d'un blupi
    {
        cel = m_pDecor->ConvPosToCel (pos);
        menu = GetMenu (WM_DECOR4);

        if (menu == 0)    // supprime ?
            m_pDecor->BlupiDelete (cel);
        if (menu == 1)    // ajoute blupi-fatigué ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 0, MAXENERGY / 4);
        if (menu == 2)    // ajoute blupi-énergique ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 0, MAXENERGY);
        if (menu == 3)    // ajoute assistant ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 8, MAXENERGY);
        if (menu == 4)    // ajoute araignée ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 1, MAXENERGY);
        if (menu == 5)    // ajoute virus ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 2, MAXENERGY);
        if (menu == 6)    // ajoute tracks ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 3, MAXENERGY);
        if (menu == 7)    // ajoute bombe ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 5, MAXENERGY);
        if (menu == 8)    // ajoute électro ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 7, MAXENERGY);
        if (menu == 9)    // ajoute robot ?
            m_pDecor->BlupiCreate (cel, ACTION_STOP, DIRECT_S, 4, MAXENERGY);
    }

    if (GetState (WM_DECOR5) == 1)   // pose d'une cata
    {
        cel = m_pDecor->ConvPosToCel2 (pos);
        menu = GetMenu (WM_DECOR5);

        if (menu == 0)    // supprime ?
            m_pDecor->SetFire (cel, false);
        if (menu == 1)    // ajoute ?
            m_pDecor->SetFire (cel, true);
    }

    m_pDecor->ArrangeBlupi();  // supprime les blupi bloqués

    return true;
}

// Modifie le décor lorsque la souris est déplacée.

bool CEvent::BuildMove (POINT pos, Uint16 mod, const SDL_Event &event)
{
    if (event.motion.state & SDL_BUTTON (
            SDL_BUTTON_LEFT)) // bouton souris pressé ?
        BuildDown (pos, mod, false);

    if (GetState (WM_DECOR4) == 1)   // pose d'un blupi
        m_pDecor->CelHili (pos, 1);
    else
        m_pDecor->CelHili (pos, 2);

    return true;
}

// Modifie le décor lorsque le bouton de la souris est relâché.

bool CEvent::BuildUp (POINT pos)
{
    return true;
}


// Démarre un film non interractif.

bool CEvent::StartMovie (const char *pFilename)
{
    RECT        rect;

    if (!m_pMovie->GetEnable())
        return false;
    if (!m_bMovie)
        return false;

    if (!m_pMovie->IsExist (pFilename))
        return false;

    rect.left   = 1;  // mystère: plante avec 0,0,LXIMAGE,LYIMAGE !!!
    rect.top    = 1;
    rect.right  = LXIMAGE - 2;
    rect.bottom = LYIMAGE - 2;

    m_pSound->StopMusic();

    if (!m_pMovie->Play (rect, pFilename))
        return false;
    m_bRunMovie = true;
    return true;
}

// Stoppe un film non interractif.

void CEvent::StopMovie()
{
    m_pMovie->Stop();
    ChangePhase (m_phase);
    m_bRunMovie = false;
}

// Indique s'il y a un film en cours.

bool CEvent::IsMovie()
{
    return m_bRunMovie;
}


// Lit une partie (user000.blp).

void CEvent::Read (Sint32 message)
{
    Sint32      world, time, total;

    m_pDecor->Read (message - WM_READ0, true, world, time, total);
    m_pDecor->SetTime (time);
    m_pDecor->SetTotalTime (total);

    if (world >= 200)
    {
        m_private  = world - 200;
        m_bSchool  = false;
        m_bPrivate = true;
    }
    else if (world >= 100)
    {
        m_mission  = world - 100;
        m_bSchool  = false;
        m_bPrivate = false;
    }
    else
    {
        m_exercice = world;
        m_bSchool  = true;
        m_bPrivate = false;
    }
}

// Ecrit une partie (user000.blp).

void CEvent::Write (Sint32 message)
{
    Sint32      time, total;

    time  = m_pDecor->GetTime();
    total = m_pDecor->GetTotalTime();

    m_pDecor->Write (message - WM_WRITE0, true,
                     GetPhysicalWorld(), time, total);
}


// Initialise le libellé d'une mission privée.

void CEvent::PrivateLibelle()
{
    Sint32          i, nb, h1, h2;
    Term        term;
    char        string[100];
    char        buffer[100];
    const char *text = nullptr;

    snprintf (m_libelle, sizeof (m_libelle), "%s", gettext ("1|Goal :"));

    memcpy (&term, m_pDecor->GetTerminated(), sizeof (Term));
    nb = 0;
    for (i = 0 ; i < 2 ; i++) // 2 objectifs au maximum !
    {
        text = nullptr;

        if (term.bKillRobots)
        {
            term.bKillRobots = false;
            text = gettext ("1|Kill all\n1|enemies !");
        }
        else if (term.bHachBlupi)
        {
            term.bHachBlupi = false;
            text = gettext ("1|Go on striped\n1|paving stones.");
        }
        else if (term.bHachPlanche)
        {
            term.bHachPlanche = false;
            text = gettext ("1|Drop planks on striped \n1|paving stones.");
        }
        else if (term.bHachTomate)
        {
            term.bHachTomate = false;
            text = gettext ("1|Drop tomatoes on striped \n1|paving stones.");
        }
        else if (term.bHachMetal)
        {
            term.bHachMetal = false;
            text = gettext ("1|Drop platinium on striped \n1|paving stones.");
        }
        else if (term.bHachRobot)
        {
            term.bHachRobot = false;
            text = gettext ("1|The robot must reach\n1|the striped paving stones.");
        }
        else if (term.bHomeBlupi)
        {
            term.bHomeBlupi = false;
            text = gettext ("1|Each Blupi in\n1|his house.");
        }
        else if (term.bStopFire)
        {
            term.bStopFire = false;
            text = gettext ("1|Resist until\n1|fire extinction ...");
        }

        if (!text)
            break;

        strcat (m_libelle, "\n1|\n");
        strcat (m_libelle, text);
        nb ++;
    }

    if (nb == 0 || term.nbMaxBlupi > 1)
    {
        snprintf (buffer, sizeof (buffer),
                  "%s", gettext ("1|The Blupi population must\n1|be of at least %d Blupi."));
        sprintf (string, buffer, term.nbMaxBlupi);
        strcat (m_libelle, "\n1|\n");
        strcat (m_libelle, string);
    }

    h1 = GetTextHeight (m_libelle, FONTLITTLE, 1);
    h2 = GetTextHeight (m_libelle, FONTLITTLE, 2);
    if (h2 > h1)
        h1 = h2;
    m_pDecor->SetInfoHeight (POSDRAWY + h1 + 10);
}

// Lit le libellé d'un monde.

bool CEvent::ReadLibelle (Sint32 world, bool bSchool, bool bHelp)
{
    FILE       *file    = nullptr;
    char       *pBuffer = nullptr;
    char       *pText;
    char       *pDest;
    char        indic;
    Sint32          h1, h2;
    size_t      nb;

    if (bSchool)
        indic = '$';
    else
        indic = '#';
    if (bHelp)
        indic = '@';

    auto stories = GetBaseDir() + "data/" + GetLocale () + "/stories.blp";

    pBuffer = (char *)malloc (sizeof (char) * 50000);
    if (pBuffer == nullptr)
        goto error;

    memset (pBuffer, 0, sizeof (char) * 50000);

    file = fopen (stories.c_str (), "rb");
    if (file == nullptr)
    {
        /* Try with the fallback locale */
        stories = GetBaseDir() + "data/en/stories.blp";
        file = fopen (stories.c_str (), "rb");
        if (!file)
            goto error;
    }

    nb = fread (pBuffer, sizeof (char), 50000 - 1, file);
    pBuffer[nb] = 0;

    pText = pBuffer;
    while (world >= 0)
    {
        while (*pText != 0 && *pText != indic)
            pText++;
        if (*pText == indic)
            pText++;
        world --;
    }
    while (*pText != 0 && *pText != '\n')
        pText++;
    if (*pText == '\n')
        pText++;
    pDest = m_libelle;
    while (*pText != 0 && *pText != indic &&
           *pText != '$' && *pText != '#' && *pText != '@')
        *pDest++ = *pText++;
    *pDest = 0;

    h1 = GetTextHeight (m_libelle, FONTLITTLE, 1);
    h2 = GetTextHeight (m_libelle, FONTLITTLE, 2);
    if (h2 > h1)
        h1 = h2;
    m_pDecor->SetInfoHeight (POSDRAWY + h1 + 10);

    free (pBuffer);
    fclose (file);
    return true;

error:
    if (pBuffer != nullptr)
        free (pBuffer);
    if (file    != nullptr)
        fclose (file);
    return false;
}


// Sauve les informations sur disque.

bool CEvent::WriteInfo()
{
    char        filename[MAX_PATH];
    FILE       *file = nullptr;
    DescInfo    info = { 0 };
    size_t      nb;

    strcpy (filename, "data/info.blp");
    AddUserPath (filename);

    file = fopen (filename, "wb");
    if (file == nullptr)
        goto error;

    info.majRev       = 1;
    info.minRev       = 0;
    info.prive        = m_private;
    info.exercice     = m_exercice;
    info.mission      = m_mission;
    info.maxMission   = m_maxMission;
    info.speed        = m_speed;
    info.bMovie       = m_bMovie;
    info.scrollSpeed  = m_scrollSpeed;
    info.bAccessBuild = m_bAccessBuild;

    info.skill        = m_pDecor->GetSkill ();

    info.audioVolume = m_pSound->GetAudioVolume ();
    info.midiVolume  = m_pSound->GetMidiVolume ();

    nb = fwrite (&info, sizeof (info), 1, file);
    if (nb < 1)
        goto error;

    fclose (file);
    return true;

error:
    if (file != nullptr)
        fclose (file);
    return false;
}

// Lit les informations sur disque.

bool CEvent::ReadInfo()
{
    char        filename[MAX_PATH];
    FILE       *file = nullptr;
    DescInfo    info;
    size_t      nb;

    strcpy (filename, "data/info.blp");
    AddUserPath (filename);

    file = fopen (filename, "rb");
    if (file == nullptr)
        goto error;

    nb = fread (&info, sizeof (DescInfo), 1, file);
    if (nb < 1)
        goto error;

    m_private      = info.prive;
    m_exercice     = info.exercice;
    m_mission      = info.mission;
    m_maxMission   = info.maxMission;
    m_speed        = info.speed;
    m_bMovie       = !!info.bMovie;
    m_scrollSpeed  = info.scrollSpeed;
    m_bAccessBuild = !!info.bAccessBuild;

    m_pDecor->SetSkill (info.skill);

    m_pSound->SetAudioVolume (info.audioVolume);
    m_pSound->SetMidiVolume (info.midiVolume);

    fclose (file);
    return true;

error:
    if (file != nullptr)
        fclose (file);
    return false;
}


// Modifie la vitesse du jeu.

void CEvent::SetSpeed (Sint32 speed)
{
    Sint32      max;

    if (m_bSpeed)
        max = 8;
    else
        max = 2;

    if (speed > max)
        speed = max;

    m_speed = speed;
}

Sint32 CEvent::GetSpeed()
{
    return m_speed;
}

bool CEvent::GetPause()
{
    return m_bPause;
}


// Début de l'enregistrement d'une démo.

void CEvent::DemoRecStart()
{
    m_pDemoBuffer = (DemoEvent *)malloc (MAXDEMO * sizeof (DemoEvent));
    if (m_pDemoBuffer == nullptr)
        return;
    memset (m_pDemoBuffer, 0, MAXDEMO * sizeof (DemoEvent));

    m_demoTime  = 0;
    m_demoIndex = 0;
    m_bDemoRec  = true;
    m_bDemoPlay = false;

    InitRandom();
    m_pDecor->SetTime (0);
    m_speed = 1;
}

// Fin de l'enregistrement d'une démo.
// Sauve le fichier sur disque.

void CEvent::DemoRecStop()
{
    FILE       *file = nullptr;
    DemoHeader  header;

    if (m_bDemoPlay)
        return;

    if (m_pDemoBuffer != nullptr)
    {
        const auto demo = GetBaseDir () + "data/demo.blp";
        unlink (demo.c_str ());
        file = fopen (demo.c_str (), "wb");
        if (file != nullptr)
        {
            memset (&header, 0, sizeof (DemoHeader));
            header.majRev   = 1;
            header.minRev   = 0;
            header.bSchool  = m_bSchool;
            header.bPrivate = m_bPrivate;
            header.world    = GetPhysicalWorld();
            header.skill    = m_pDecor->GetSkill();
            fwrite (&header, sizeof (DemoHeader), 1, file);
            fwrite (m_pDemoBuffer, sizeof (DemoEvent), m_demoIndex, file);
            fclose (file);
        }
        free (m_pDemoBuffer);
        m_pDemoBuffer = nullptr;
    }

    m_bDemoRec = false;
    m_demoTime = 0;
}

// Début de la reproduction d'une démo.
// Lit le fichier sur disque.

bool CEvent::DemoPlayStart()
{
    char        filename[MAX_PATH];
    FILE       *file = nullptr;
    DemoHeader  header;
    Sint32          world, time, total;
    size_t      nb;

    m_pDemoBuffer = (DemoEvent *)malloc (MAXDEMO * sizeof (DemoEvent));
    if (m_pDemoBuffer == nullptr)
        return false;
    memset (m_pDemoBuffer, 0, MAXDEMO * sizeof (DemoEvent));

    sprintf (filename, (GetBaseDir() + "data/demo%.3d.blp").c_str(), m_demoNumber);
    file = fopen (filename, "rb");
    if (file == nullptr)
    {
        DemoPlayStop();
        return false;
    }

    nb = fread (&header, sizeof (DemoHeader), 1, file);
    if (nb < 1)
    {
        fclose (file);
        DemoPlayStop();
        return false;
    }
    m_bSchool  = !!header.bSchool;
    m_bPrivate = !!header.bPrivate;
    m_pDecor->SetSkill (header.skill);

    m_demoEnd = fread (m_pDemoBuffer, sizeof (DemoEvent), MAXDEMO, file);
    fclose (file);

    m_demoTime  = 0;
    m_demoIndex = 0;
    m_bDemoPlay = true;
    m_bDemoRec  = false;

    if (!m_pDecor->Read (header.world, false, world, time, total))
    {
        DemoPlayStop();
        return false;
    }
    ChangePhase (WM_PHASE_PLAY);
    InitRandom();
    m_pDecor->SetTime (0);
    m_speed = 1;

    return true;
}

// Fin de la reproduction d'une démo.

void CEvent::DemoPlayStop()
{
    if (m_pDemoBuffer != nullptr)
    {
        free (m_pDemoBuffer);
        m_pDemoBuffer = nullptr;
    }
    m_bDemoPlay = false;
    m_bDemoRec  = false;
    m_demoTime  = 0;

    ChangePhase (WM_PHASE_INIT);
}

void CEvent::WinToSDLEvent (Uint32 msg, WPARAM wParam, LPARAM lParam,
                            SDL_Event &event)
{
#define GET_X_LPARAM(lp) ((Sint32) (Sint16) LOWORD (lp))
#define GET_Y_LPARAM(lp) ((Sint32) (Sint16) HIWORD (lp))

    static const std::unordered_map<Uint32, SDL_Keysym> keycodes = {
        { '0',      { SDL_SCANCODE_0,     SDLK_0,     0, 0 } },
        { '1',      { SDL_SCANCODE_1,     SDLK_1,     0, 0 } },
        { '2',      { SDL_SCANCODE_2,     SDLK_2,     0, 0 } },
        { '3',      { SDL_SCANCODE_3,     SDLK_3,     0, 0 } },
        { '4',      { SDL_SCANCODE_4,     SDLK_4,     0, 0 } },
        { '5',      { SDL_SCANCODE_5,     SDLK_5,     0, 0 } },
        { '6',      { SDL_SCANCODE_6,     SDLK_6,     0, 0 } },
        { '7',      { SDL_SCANCODE_7,     SDLK_7,     0, 0 } },
        { '8',      { SDL_SCANCODE_8,     SDLK_8,     0, 0 } },
        { '9',      { SDL_SCANCODE_9,     SDLK_9,     0, 0 } },
        { 'A',      { SDL_SCANCODE_A,     SDLK_a,     0, 0 } },
        { 'B',      { SDL_SCANCODE_B,     SDLK_b,     0, 0 } },
        { 'C',      { SDL_SCANCODE_C,     SDLK_c,     0, 0 } },
        { 'D',      { SDL_SCANCODE_D,     SDLK_d,     0, 0 } },
        { 'E',      { SDL_SCANCODE_E,     SDLK_e,     0, 0 } },
        { 'F',      { SDL_SCANCODE_F,     SDLK_f,     0, 0 } },
        { 'G',      { SDL_SCANCODE_G,     SDLK_g,     0, 0 } },
        { 'H',      { SDL_SCANCODE_H,     SDLK_h,     0, 0 } },
        { 'I',      { SDL_SCANCODE_I,     SDLK_i,     0, 0 } },
        { 'J',      { SDL_SCANCODE_J,     SDLK_j,     0, 0 } },
        { 'K',      { SDL_SCANCODE_K,     SDLK_k,     0, 0 } },
        { 'L',      { SDL_SCANCODE_L,     SDLK_l,     0, 0 } },
        { 'M',      { SDL_SCANCODE_M,     SDLK_m,     0, 0 } },
        { 'N',      { SDL_SCANCODE_N,     SDLK_n,     0, 0 } },
        { 'O',      { SDL_SCANCODE_O,     SDLK_o,     0, 0 } },
        { 'P',      { SDL_SCANCODE_P,     SDLK_p,     0, 0 } },
        { 'Q',      { SDL_SCANCODE_Q,     SDLK_q,     0, 0 } },
        { 'R',      { SDL_SCANCODE_R,     SDLK_r,     0, 0 } },
        { 'S',      { SDL_SCANCODE_S,     SDLK_0,     0, 0 } },
        { 'T',      { SDL_SCANCODE_T,     SDLK_t,     0, 0 } },
        { 'U',      { SDL_SCANCODE_U,     SDLK_u,     0, 0 } },
        { 'V',      { SDL_SCANCODE_V,     SDLK_v,     0, 0 } },
        { 'W',      { SDL_SCANCODE_W,     SDLK_w,     0, 0 } },
        { 'X',      { SDL_SCANCODE_X,     SDLK_x,     0, 0 } },
        { 'Y',      { SDL_SCANCODE_Y,     SDLK_y,     0, 0 } },
        { 'Z',      { SDL_SCANCODE_Z,     SDLK_z,     0, 0 } },
        { VK_LEFT,  { SDL_SCANCODE_LEFT,  SDLK_LEFT,  0, 0 } },
        { VK_UP,    { SDL_SCANCODE_UP,    SDLK_UP,    0, 0 } },
        { VK_RIGHT, { SDL_SCANCODE_RIGHT, SDLK_RIGHT, 0, 0 } },
        { VK_DOWN,  { SDL_SCANCODE_DOWN,  SDLK_DOWN,  0, 0 } },
        { VK_END,   { SDL_SCANCODE_END,   SDLK_END,   0, 0 } },
    };

    try
    {
        switch (msg)
        {
        case WM_KEYUP:
        case WM_KEYDOWN:
            event.type = msg == WM_KEYDOWN ? SDL_KEYDOWN : SDL_KEYUP;
            event.key.keysym = keycodes.at (wParam);
            // TODO: lParam
            break;

        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
            event.type = msg == WM_LBUTTONDOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
            event.button.button = SDL_BUTTON_LEFT;
            // TODO: wParam CTRL or SHIFT
            event.button.x = GET_X_LPARAM (lParam);
            event.button.y = GET_Y_LPARAM (lParam);
            break;

        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
            event.type = msg == WM_RBUTTONDOWN ? SDL_MOUSEBUTTONDOWN : SDL_MOUSEBUTTONUP;
            event.button.button = SDL_BUTTON_RIGHT;
            // TODO: wParam CTRL or SHIFT
            event.button.x = GET_X_LPARAM (lParam);
            event.button.y = GET_Y_LPARAM (lParam);
            break;

        case WM_MOUSEMOVE:
            event.type = SDL_MOUSEMOTION;
            // TODO: wParam CTRL or SHIFT
            event.motion.x = GET_X_LPARAM (lParam);
            event.motion.y = GET_Y_LPARAM (lParam);
            break;
        }
    }
    catch (...)
    {
        SDL_LogError (SDL_LOG_CATEGORY_APPLICATION, "unsupported keycode");
    }
}

// Avance l'index d'enregistrement ou de reproduction.

void CEvent::DemoStep()
{
    Sint32          time;
    Uint32      message;
    WPARAM      wParam;
    LPARAM      lParam;

    if (m_phase == WM_PHASE_INIT)
    {
        if (m_demoTime > DEF_TIME_DEMO)    // ~30 secondes écoulées ?
        {
            m_demoNumber = 0;
            DemoPlayStart();  // démarre une démo automatique
        }
    }

    if (m_bDemoPlay &&   // démo en lecture ?
        m_pDemoBuffer != nullptr)
    {
        while (true)
        {
            time = m_pDemoBuffer[m_demoIndex].time;
            if (time > m_demoTime)
                break;

            message = m_pDemoBuffer[m_demoIndex].message;
            wParam  = m_pDemoBuffer[m_demoIndex].wParam;
            lParam  = m_pDemoBuffer[m_demoIndex].lParam;
            m_demoIndex ++;

            if (message == WM_MOUSEMOVE)
            {
                POINT pos = ConvLongToPos (lParam);
                SDL_WarpMouseInWindow (g_window, pos.x, pos.y);
            }

            SDL_Event event = { 0 };
            CEvent::WinToSDLEvent (message, wParam, lParam, event);
            TreatEventBase (event);

            if (m_demoIndex >= m_demoEnd)
            {
                m_demoNumber ++;  // démo suivante
                if (!DemoPlayStart())    // démarre la démo suivante
                {
                    m_demoNumber = 0;  // première démo
                    DemoPlayStart();   // démarre la démo
                }
                return;
            }
        }
    }

    m_demoTime ++;
}

// Mémorise un événement.

void CEvent::DemoRecEvent (Uint32 message, WPARAM wParam, LPARAM lParam)
{
    if (m_bDemoRec && m_pDemoBuffer != nullptr &&
        (message == WM_KEYDOWN     ||
         message == WM_KEYUP       ||
         message == WM_LBUTTONDOWN ||
         message == WM_RBUTTONDOWN ||
         message == WM_MOUSEMOVE   ||
         message == WM_LBUTTONUP   ||
         message == WM_RBUTTONUP))
    {
        if (m_demoIndex > 0 &&
            message == WM_MOUSEMOVE &&
            m_pDemoBuffer[m_demoIndex - 1].time    == m_demoTime &&
            m_pDemoBuffer[m_demoIndex - 1].message == message)
        {
            m_pDemoBuffer[m_demoIndex - 1].wParam  = static_cast<Uint32> (wParam);
            m_pDemoBuffer[m_demoIndex - 1].lParam  = static_cast<Uint32> (lParam);
        }
        else
        {
            m_pDemoBuffer[m_demoIndex].time    = m_demoTime;
            m_pDemoBuffer[m_demoIndex].message = message;
            m_pDemoBuffer[m_demoIndex].wParam  = static_cast<Uint32> (wParam);
            m_pDemoBuffer[m_demoIndex].lParam  = static_cast<Uint32> (lParam);

            m_demoIndex ++;
            if (m_demoIndex >= MAXDEMO)
                DemoRecStop();
        }
    }
}


// Retourne la dernière position de la souris.

POINT CEvent::GetLastMousePos()
{
    return m_oldMousePos;
}

// Traitement d'un événement.

bool CEvent::TreatEvent (const SDL_Event &event)
{
    if (m_bDemoPlay)
    {
        if (event.type == SDL_KEYDOWN ||
            event.type == SDL_KEYUP ||
            event.type == SDL_MOUSEBUTTONUP) // is the user clicking?
        {
            DemoPlayStop();
            return true;
        }

        if (event.type == SDL_MOUSEMOTION) // is the user moving?
            return true;
    }

    return TreatEventBase (event);
}

// Traitement d'un événement.

bool CEvent::TreatEventBase (const SDL_Event &event)
{
    POINT       pos;
    Sint32          i, sound;
    char        c;
    bool        bEnable;

    //DemoRecEvent(message, wParam, lParam); XXX: use SDL_Event

    switch (event.type)
    {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym >= SDLK_a && event.key.keysym.sym <= SDLK_z)
        {
            if (m_posCheat == 0)    // première lettre ?
            {
                m_rankCheat = -1;
                for (i = 0 ; i < 9 ; i++)
                {
                    if ((char) event.key.keysym.sym == cheat_code[i][0])
                    {
                        m_rankCheat = i;
                        break;
                    }
                }
            }
            if (m_rankCheat != -1)
            {
                c = cheat_code[m_rankCheat][m_posCheat];
                if (m_posCheat != 0 && m_rankCheat == 8)
                    c++;  // CONSTRUIRE ?
                if ((char) event.key.keysym.sym == c)
                {
                    m_posCheat ++;
                    if (cheat_code[m_rankCheat][m_posCheat] == 0)
                    {
                        bEnable = true;
                        if (m_phase == WM_PHASE_PLAY)
                        {
                            if (m_rankCheat == 0)    // vision ?
                                m_pDecor->EnableFog (false);
                            if (m_rankCheat == 1 ||   // power ?
                                m_rankCheat == 2)    // lonesome ?
                                m_pDecor->BlupiCheat (m_rankCheat);
                        }

                        if (m_rankCheat == 3)    // allmissions ?
                        {
                            m_bAllMissions = !m_bAllMissions;
                            bEnable = m_bAllMissions;
                            m_bChangeCheat = true;
                        }
                        if (m_rankCheat == 4)    // quick ?
                        {
                            m_bSpeed = !m_bSpeed;
                            bEnable = m_bSpeed;
                            m_bChangeCheat = true;
                        }

                        if (m_rankCheat == 5)    // helpme ?
                        {
                            m_bHelp = !m_bHelp;
                            bEnable = m_bHelp;
                            m_bChangeCheat = true;
                        }

                        if (m_rankCheat == 6)    // invincible ?
                        {
                            m_pDecor->SetInvincible (!m_pDecor->GetInvincible());
                            bEnable = m_pDecor->GetInvincible();
                            m_bChangeCheat = true;
                        }

                        if (m_rankCheat == 7)    // superblupi ?
                        {
                            m_pDecor->SetSuper (!m_pDecor->GetSuper());
                            bEnable = m_pDecor->GetSuper();
                            m_bChangeCheat = true;
                        }

                        if (m_rankCheat == 8)    // construire ?
                        {
                            m_bAccessBuild = !m_bAccessBuild;
                            bEnable = m_bAccessBuild;
                            m_bChangeCheat = true;
                        }

                        if (m_phase != WM_PHASE_PLAY)
                            ChangePhase (m_phase);

                        pos.x = LXIMAGE / 2;
                        pos.y = LYIMAGE / 2;
                        if (bEnable)
                            m_pSound->PlayImage (SOUND_BUT, pos);
                        else
                            m_pSound->PlayImage (SOUND_BOING, pos);

                        m_rankCheat = -1;
                        m_posCheat = 0;
                    }
                    return true;
                }
            }
        }
        m_rankCheat = -1;
        m_posCheat = 0;

        if (m_phase == WM_PHASE_INTRO1 )
        {
            ChangePhase (WM_PHASE_INIT);
            return true;
        }

        if (m_phase == WM_PHASE_BYE)
        {
            SDL_Event ev;
            ev.type = SDL_QUIT;
            SDL_PushEvent (&ev);
        }

        switch (event.key.keysym.sym)
        {
        case SDLK_END:
            DemoRecStop();
            return true;
        case SDLK_ESCAPE:
            if (m_bRunMovie)
            {
                StopMovie();
                m_pSound->SetSuspendSkip (1);
                return true;
            }
            if (m_phase == WM_PHASE_PLAY   ||
                m_phase == WM_PHASE_SETUP  ||
                m_phase == WM_PHASE_SETUPp ||
                m_phase == WM_PHASE_READ   ||
                m_phase == WM_PHASE_WRITE  ||
                m_phase == WM_PHASE_WRITEp ||
                m_phase == WM_PHASE_HELP)
            {
                ChangePhase (WM_PHASE_STOP);
                return true;
            }
            if (m_phase == WM_PHASE_STOP  ||
                m_phase == WM_PHASE_LOST  ||
                m_phase == WM_PHASE_BUILD)
            {
                ChangePhase (WM_PHASE_INFO);
                return true;
            }
            if (m_phase == WM_PHASE_INFO)
            {
                ChangePhase (WM_PHASE_INIT);
                return true;
            }
            if (m_phase == WM_PHASE_BUTTON ||
                m_phase == WM_PHASE_TERM   ||
                m_phase == WM_PHASE_MUSIC  ||
                m_phase == WM_PHASE_REGION)
            {
                ChangePhase (WM_PHASE_BUILD);
                return true;
            }
            if (m_phase == WM_PHASE_INIT)
            {
                ChangePhase (WM_PHASE_BYE);
                return true;
            }
            if (m_phase == WM_PHASE_BYE)
            {
                SDL_Event ev;
                ev.type = SDL_QUIT;
                SDL_PushEvent (&ev);
                break;
            }
            return true;
        case SDLK_RETURN:
            if (m_phase == WM_PHASE_PLAY  ||
                m_phase == WM_PHASE_READ  ||
                m_phase == WM_PHASE_WRITE ||
                m_phase == WM_PHASE_SETUP)
            {
                ChangePhase (WM_PHASE_STOP);
                return true;
            }
            if (m_phase == WM_PHASE_INIT  ||
                m_phase == WM_PHASE_LOST  ||
                m_phase == WM_PHASE_BUILD)
            {
                ChangePhase (WM_PHASE_INFO);
                return true;
            }
            if (m_phase == WM_PHASE_INFO   ||
                m_phase == WM_PHASE_STOP   ||
                m_phase == WM_PHASE_HELP   ||
                m_phase == WM_PHASE_SETUPp ||
                m_phase == WM_PHASE_WRITEp)
            {
                ChangePhase (WM_PHASE_PLAY);
                return true;
            }
            if (m_phase == WM_PHASE_BUTTON ||
                m_phase == WM_PHASE_TERM   ||
                m_phase == WM_PHASE_MUSIC  ||
                m_phase == WM_PHASE_REGION)
            {
                ChangePhase (WM_PHASE_BUILD);
                return true;
            }
            return true;
        case SDLK_LEFT:
            DecorShift (-2, 2);
            return true;
        case SDLK_RIGHT:
            DecorShift (2, -2);
            return true;
        case SDLK_UP:
            DecorShift (-3, -3);
            return true;
        case SDLK_DOWN:
            DecorShift (3, 3);
            return true;
        case SDLK_HOME:
            pos = m_pDecor->GetHome();
            m_pDecor->SetCoin (pos);
            return true;
        case SDLK_SPACE:
            if (m_bRunMovie)
            {
                StopMovie();
                m_pSound->SetSuspendSkip (1);
                return true;
            }
            m_pDecor->FlipOutline();
            return true;
        case SDLK_PAUSE:
            m_bPause = !m_bPause;
            if (m_phase == WM_PHASE_PLAY)
            {
                if (m_bPause)
                    m_pSound->SuspendMusic();
                else
                    m_pSound->RestartMusic();
            }
            return true;

        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            m_keymod |= KMOD_SHIFT;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            m_keymod |= KMOD_CTRL;
            if (m_phase == WM_PHASE_BUILD)
            {
                m_bFillMouse = true;
                MouseSprite (GetMousePos());
            }
            else
                m_bFillMouse = false;
            return true;
        case SDLK_F1:
            if (m_phase == WM_PHASE_PLAY)
            {
                // Montre ou cache les infos tout en haut.
                if (m_pDecor->GetInfoMode())
                    sound = SOUND_CLOSE;
                else
                    sound = SOUND_OPEN;
                pos.x = LXIMAGE / 2;
                pos.y = LYIMAGE / 2;
                m_pSound->PlayImage (sound, pos);
                m_pDecor->SetInfoMode (!m_pDecor->GetInfoMode());
            }
            return true;
        case SDLK_F8:
            if (m_phase == WM_PHASE_PLAY)
                m_pDecor->MemoPos (0, !! (m_keymod & KMOD_CTRL));
            return true;
        case SDLK_F10:
            if (m_phase == WM_PHASE_PLAY)
                m_pDecor->MemoPos (1, !! (m_keymod & KMOD_CTRL));
            return true;
        case SDLK_F11:
            if (m_phase == WM_PHASE_PLAY)
                m_pDecor->MemoPos (2, !! (m_keymod & KMOD_CTRL));
            return true;
        case SDLK_F12:
            if (m_phase == WM_PHASE_PLAY)
                m_pDecor->MemoPos (3, !! (m_keymod & KMOD_CTRL));
            return true;
        }
        break;

    case SDL_KEYUP:
        switch (event.key.keysym.sym)
        {
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
            m_keymod &= ~KMOD_SHIFT;
            break;

        case SDLK_LCTRL:
        case SDLK_RCTRL:
            m_keymod &= ~KMOD_CTRL;
            m_bFillMouse = false;
            MouseSprite (GetMousePos());
            return true;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button != SDL_BUTTON_LEFT
            && event.button.button != SDL_BUTTON_RIGHT)
            break;

        pos.x = event.button.x;
        pos.y = event.button.y;

        MouseSprite (pos);
        //?         DecorAutoShift(pos);
        if (EventButtons (event, pos))
            return true;
        if (m_phase == WM_PHASE_BUILD)
        {
            if (BuildDown (pos, m_keymod))
                return true;
        }
        if (m_phase == WM_PHASE_PLAY)
        {
            if (PlayDown (pos, event))
                return true;
        }
        break;

    case SDL_MOUSEMOTION:
        pos.x = event.motion.x;
        pos.y = event.motion.y;

        m_oldMousePos = pos;

        MouseSprite (pos);
        if (EventButtons (event, pos))
            return true;
        if (m_phase == WM_PHASE_BUILD)
        {
            if (BuildMove (pos, m_keymod, event))
                return true;
        }
        if (m_phase == WM_PHASE_PLAY)
        {
            if (PlayMove (pos, m_keymod))
                return true;
        }
        break;

    case SDL_MOUSEBUTTONUP:
        if (event.button.button != SDL_BUTTON_LEFT
            && event.button.button != SDL_BUTTON_RIGHT)
            break;

        pos.x = event.button.x;
        pos.y = event.button.y;

        if (EventButtons (event, pos))
            return true;
        if (m_phase == WM_PHASE_BUILD)
        {
            if (BuildUp (pos))
                return true;
        }
        if (m_phase == WM_PHASE_PLAY)
        {
            if (PlayUp (pos, m_keymod))
                return true;
        }
        if (m_phase == WM_PHASE_BYE)
        {
            SDL_Event ev;
            ev.type = SDL_QUIT;
            SDL_PushEvent (&ev);
        }
        break;

    case SDL_USEREVENT:
        switch (event.user.code)
        {
        case WM_PHASE_DEMO:
            m_demoNumber = 0;
            DemoPlayStart();
            break;

        case WM_PHASE_SCHOOL:
            m_bSchool  = true;
            m_bPrivate = false;
            if (ChangePhase (WM_PHASE_INFO))
                return true;
            break;

        case WM_PHASE_MISSION:
            m_bSchool  = false;
            m_bPrivate = false;
            if (m_mission == 0)    // première mission ?
            {
                if (ChangePhase (WM_PHASE_H0MOVIE))
                    return true;
            }
            else
            {
                if (ChangePhase (WM_PHASE_INFO))
                    return true;
            }
            break;

        case WM_PHASE_PRIVATE:
            m_bSchool  = false;
            m_bPrivate = true;
            if (ChangePhase (WM_PHASE_INFO))
                return true;
            break;

        case WM_PHASE_INTRO1:
        case WM_PHASE_INIT:
        case WM_PHASE_HISTORY0:
        case WM_PHASE_HISTORY1:
        case WM_PHASE_INFO:
        case WM_PHASE_PLAY:
        case WM_PHASE_READ:
        case WM_PHASE_WRITE:
        case WM_PHASE_WRITEp:
        case WM_PHASE_BUILD:
        case WM_PHASE_BUTTON:
        case WM_PHASE_TERM:
        case WM_PHASE_STOP:
        case WM_PHASE_HELP:
        case WM_PHASE_MUSIC:
        case WM_PHASE_REGION:
        case WM_PHASE_SETUP:
        case WM_PHASE_SETUPp:
        case WM_PHASE_PLAYMOVIE:
        case WM_PHASE_H0MOVIE:
        case WM_PHASE_H1MOVIE:
        case WM_PHASE_H2MOVIE:
        case WM_PHASE_WINMOVIE:
        case WM_PHASE_BYE:
            if (ChangePhase (event.user.code))
                return true;
            break;

        case WM_PHASE_UNDO:
            m_pDecor->UndoBack();  // revient en arrière
            break;

        case WM_PREV:
            m_pDecor->SetInvincible (false);
            m_pDecor->SetSuper (false);
            if (m_bPrivate)
            {
                if (m_private > 0)
                {
                    m_private --;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            else if (m_bSchool)
            {
                if (m_exercice > 0)
                {
                    m_exercice --;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            else
            {
                if (m_mission > 0)
                {
                    m_mission --;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            break;

        case WM_NEXT:
            m_pDecor->SetInvincible (false);
            m_pDecor->SetSuper (false);
            if (m_bPrivate)
            {
                if (m_private < 20 - 1)
                {
                    m_private ++;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            else if (m_bSchool)
            {
                if (m_exercice < 99)
                {
                    m_exercice ++;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            else
            {
                if (m_mission < 99)
                {
                    m_mission ++;
                    if (m_maxMission < m_mission)
                        m_maxMission = m_mission;
                    if (ChangePhase (WM_PHASE_INFO))
                        return true;
                }
            }
            break;

        case WM_DECOR1:
            SetState (WM_DECOR1, 1);
            SetState (WM_DECOR2, 0);
            SetState (WM_DECOR3, 0);
            SetState (WM_DECOR4, 0);
            SetState (WM_DECOR5, 0);
            break;

        case WM_DECOR2:
            SetState (WM_DECOR1, 0);
            SetState (WM_DECOR2, 1);
            SetState (WM_DECOR3, 0);
            SetState (WM_DECOR4, 0);
            SetState (WM_DECOR5, 0);
            break;

        case WM_DECOR3:
            SetState (WM_DECOR1, 0);
            SetState (WM_DECOR2, 0);
            SetState (WM_DECOR3, 1);
            SetState (WM_DECOR4, 0);
            SetState (WM_DECOR5, 0);
            break;

        case WM_DECOR4:
            SetState (WM_DECOR1, 0);
            SetState (WM_DECOR2, 0);
            SetState (WM_DECOR3, 0);
            SetState (WM_DECOR4, 1);
            SetState (WM_DECOR5, 0);
            break;

        case WM_DECOR5:
            SetState (WM_DECOR1, 0);
            SetState (WM_DECOR2, 0);
            SetState (WM_DECOR3, 0);
            SetState (WM_DECOR4, 0);
            SetState (WM_DECOR5, 1);
            break;

        case WM_PHASE_SKILL1:
            m_pDecor->SetSkill (0);
            SetState (WM_PHASE_SKILL1, true);
            SetState (WM_PHASE_SKILL2, false);
            break;
        case WM_PHASE_SKILL2:
            m_pDecor->SetSkill (1);
            SetState (WM_PHASE_SKILL1, false);
            SetState (WM_PHASE_SKILL2, true);
            break;

        case WM_BUTTON0:
        case WM_BUTTON1:
        case WM_BUTTON2:
        case WM_BUTTON3:
        case WM_BUTTON4:
        case WM_BUTTON5:
        case WM_BUTTON6:
        case WM_BUTTON7:
        case WM_BUTTON8:
        case WM_BUTTON9:
        case WM_BUTTON10:
        case WM_BUTTON11:
        case WM_BUTTON12:
        case WM_BUTTON13:
        case WM_BUTTON14:
        case WM_BUTTON15:
        case WM_BUTTON16:
        case WM_BUTTON17:
        case WM_BUTTON18:
        case WM_BUTTON19:
        case WM_BUTTON20:
        case WM_BUTTON21:
        case WM_BUTTON22:
        case WM_BUTTON23:
        case WM_BUTTON24:
        case WM_BUTTON25:
        case WM_BUTTON26:
        case WM_BUTTON27:
        case WM_BUTTON28:
        case WM_BUTTON29:
        case WM_BUTTON30:
        case WM_BUTTON31:
        case WM_BUTTON32:
        case WM_BUTTON33:
        case WM_BUTTON34:
        case WM_BUTTON35:
        case WM_BUTTON36:
        case WM_BUTTON37:
        case WM_BUTTON38:
        case WM_BUTTON39:
            ChangeButtons (event.user.code);
            break;

        case WM_READ0:
        case WM_READ1:
        case WM_READ2:
        case WM_READ3:
        case WM_READ4:
        case WM_READ5:
        case WM_READ6:
        case WM_READ7:
        case WM_READ8:
        case WM_READ9:
            Read (event.user.code);
            ChangePhase (WM_PHASE_PLAY); // joue
            break;

        case WM_WRITE0:
        case WM_WRITE1:
        case WM_WRITE2:
        case WM_WRITE3:
        case WM_WRITE4:
        case WM_WRITE5:
        case WM_WRITE6:
        case WM_WRITE7:
        case WM_WRITE8:
        case WM_WRITE9:
            Write (event.user.code);
            if (m_phase == WM_PHASE_WRITEp)
            {
                ChangePhase (WM_PHASE_PLAY); // joue
            }
            else
            {
                ChangePhase (WM_PHASE_STOP); // pause
            }
            break;

        case WM_MOVIE:
            StartMovie ("movie/essai.avi");
            ChangePhase (WM_PHASE_INIT);
            break;
        }
    }

    return false;
}

// Passe les images d'introduction.

void CEvent::IntroStep()
{
    m_introTime ++;

    if (m_introTime > 20 * 1)
    {
        if (m_phase == WM_PHASE_INTRO1 )
        {
            ChangePhase (WM_PHASE_INIT);
            return;
        }
    }
}

void CEvent::PushUserEvent (Sint32 code, void *data)
{
    SDL_Event event;

    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = data;
    event.user.data2 = nullptr;

    SDL_PushEvent (&event);

}
