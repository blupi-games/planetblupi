
#pragma once

#define FONTWHITE   0
#define FONTRED     1
#define FONTSLIM    2
#define FONTLITTLE 10

void DrawText (CPixmap *pPixmap, POINT pos, const char *pText, Sint32 font = 0);
void DrawTextPente (CPixmap *pPixmap, POINT pos, const char *pText,
                    Sint32 pente, Sint32 font = 0);
void DrawTextRect (CPixmap *pPixmap, POINT pos, char *pText,
                   Sint32 pente, Sint32 font = 0, Sint32 part = -1);
void DrawTextCenter (CPixmap *pPixmap, POINT pos, const char *pText,
                     Sint32 font = 0);
Sint32 GetTextHeight (char *pText, Sint32 font = 0, Sint32 part = -1);
Sint32 GetTextWidth (const char *pText, Sint32 font = 0);
void DrawBignum (CPixmap *pPixmap, POINT pos, Sint32 num);
Sint32 GetBignumWidth (Sint32 num);

