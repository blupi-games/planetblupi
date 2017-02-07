// Text.h

#pragma once

#define FONTWHITE	0
#define FONTRED		1
#define FONTSLIM	2
#define FONTLITTLE	10

extern
void DrawText(CPixmap *pPixmap, POINT pos, const char *pText, int font=0);

extern
void DrawTextPente(CPixmap *pPixmap, POINT pos, char *pText,
				   int pente, int font=0);

extern
void DrawTextRect(CPixmap *pPixmap, POINT pos, char *pText,
				  int pente, int font=0, int part=-1);

extern
void DrawTextCenter(CPixmap *pPixmap, POINT pos, char *pText, int font=0);

extern
int GetTextHeight(char *pText, int font=0, int part=-1);

extern
int GetTextWidth(const char *pText, int font=0);

extern
void DrawBignum(CPixmap *pPixmap, POINT pos, int num);

extern
int GetBignumWidth(int num);

