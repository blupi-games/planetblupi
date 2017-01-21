/*==========================================================================
 *
 *  Copyright (C) 1995-1996 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include "ddutil.h"
#include "misc.h"


// Le message :
// First-chance exception in Blupi.exe (GDI32.DLL): 0xC0000005: Access Violation.
// apparaît au endroits marqués par (@) ...

BOOL g_bDebug = TRUE;

void DDSetDebug(BOOL bDebug)
{
	g_bDebug = bDebug;
}

/*
 *  DDConnectBitmap
 *
 *  create a DirectDrawSurface from a bitmap resource.
 *
 */
extern "C" IDirectDrawSurface * DDConnectBitmap(IDirectDraw *pdd, HBITMAP hbm)
{
    BITMAP              bm;
    DDSURFACEDESC       ddsd;
    IDirectDrawSurface *pdds;
	HRESULT				ddrval;

    //
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap

    //
    // create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
//?    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth        = bm.bmWidth;
    ddsd.dwHeight       = bm.bmHeight;
 
    ddrval = pdd->CreateSurface(&ddsd, &pdds, NULL);
	if ( ddrval != DD_OK )
	{
		TraceErrorDD(ddrval, "", 0);
		return NULL;
	}

    DDCopyBitmap(pdds, hbm, 0, 0, bm.bmWidth, bm.bmHeight);

    return pdds;
}

/*
 *  DDLoadBitmap
 *
 *  create a DirectDrawSurface from a bitmap resource.
 *
 */
extern "C" IDirectDrawSurface * DDLoadBitmap(IDirectDraw *pdd, LPCSTR szBitmap, int dx, int dy)
{
    HBITMAP             hbm;
    BITMAP              bm;
    DDSURFACEDESC       ddsd;
    IDirectDrawSurface *pdds;
	HRESULT				ddrval;

	if ( g_bDebug )  OutputDebug("DDLoadBitmap\n");
    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

	if ( g_bDebug )  OutputDebug("DDLoadBitmap\n");
    if (hbm == NULL)
	{
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);  // (@)
	}

    if ( hbm == NULL )  return NULL;

    //
    // get size of the bitmap
    //
	if ( g_bDebug )  OutputDebug("DDLoadBitmap: GetObject\n");
    GetObject(hbm, sizeof(bm), &bm);      // get size of bitmap

    //
    // create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT |DDSD_WIDTH;
//?    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth        = bm.bmWidth;
    ddsd.dwHeight       = bm.bmHeight;
 
	if ( g_bDebug )  OutputDebug("DDLoadBitmap: CreateSurface\n");
    ddrval = pdd->CreateSurface(&ddsd, &pdds, NULL);
	if ( ddrval != DD_OK )
	{
		TraceErrorDD(ddrval, "", 0);
		return NULL;
	}

    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
	if ( g_bDebug )  OutputDebug("DDLoadBitmap: DeleteObject\n");
    DeleteObject(hbm);  // (@)

	if ( g_bDebug )  OutputDebug("DDLoadBitmap: return\n");
    return pdds;
}

/*
 *  DDReLoadBitmap
 *
 *  load a bitmap from a file or resource into a directdraw surface.
 *  normaly used to re-load a surface after a restore.
 *
 */
HRESULT DDReLoadBitmap(IDirectDrawSurface *pdds, LPCSTR szBitmap)
{
    HBITMAP             hbm;
    HRESULT             hr;

    //
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    if (hbm == NULL)
        hbm = (HBITMAP)LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
    {
        if ( g_bDebug )  OutputDebug("handle is null\n");
        return E_FAIL;
    }

    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    {
        if ( g_bDebug )  OutputDebug("ddcopybitmap failed\n");
    }


    DeleteObject(hbm);
    return hr;
}

/*
 *  DDCopyBitmap
 *
 *  draw a bitmap into a DirectDrawSurface
 *
 */
extern "C" HRESULT DDCopyBitmap(IDirectDrawSurface *pdds, HBITMAP hbm, int x, int y, int dx, int dy)
{
    HDC                 hdcImage;
    HDC                 hdc;
    BITMAP              bm;
    DDSURFACEDESC       ddsd;
    HRESULT             hr;

	if ( g_bDebug )  OutputDebug("DDCopyBitmap\n");
    if (hbm == NULL || pdds == NULL)
        return E_FAIL;

    //
    // make sure this surface is restored.
    //
	if ( g_bDebug )  OutputDebug("DDCopyBitmap: Restore\n");
    pdds->Restore();

    //
    //  select bitmap into a memoryDC so we can use it.
    //
	if ( g_bDebug )  OutputDebug("DDCopyBitmap: CreateCompatibleDC\n");
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
        OutputDebug("createcompatible dc failed\n");
	if ( g_bDebug )  OutputDebug("DDCopyBitmap: SelectObject\n");
    SelectObject(hdcImage, hbm);

    //
    // get size of the bitmap
    //
	if ( g_bDebug )  OutputDebug("DDCopyBitmap: GetObject\n");
    GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap
    dx = dx == 0 ? bm.bmWidth  : dx;    // use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;

    //
    // get size of surface.
    //
	if ( g_bDebug )  OutputDebug("DDCopyBitmap: GetSurfaceDesc\n");
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

	if ( g_bDebug )  OutputDebug("DDCopyBitmap: StretchBlt\n");
    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
		if ( g_bDebug )  OutputDebug("DDCopyBitmap: StretchBlt-go\n");
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y, dx, dy, SRCCOPY);
		if ( g_bDebug )  OutputDebug("DDCopyBitmap: ReleaseDC\n");
        pdds->ReleaseDC(hdc);  // (@)
    }

	if ( g_bDebug )  OutputDebug("DDCopyBitmap: DeleteDC\n");
    DeleteDC(hdcImage);

    return hr;
}

//
//  DDLoadPalette
//
//  Create a DirectDraw palette object from a bitmap resoure
//
//  if the resource does not exist or NULL is passed create a
//  default 332 palette.
//
extern "C" IDirectDrawPalette * DDLoadPalette(IDirectDraw *pdd, LPCSTR szBitmap)
{
    IDirectDrawPalette* ddpal;
    int                 i;
    int                 n;
    int                 fh;
    HRSRC               h;
    LPBITMAPINFOHEADER  lpbi;
    PALETTEENTRY        ape[256];
    RGBQUAD *           prgb;

    //
    // build a 332 palette as the default.
    //
    for (i=0; i<256; i++)
    {
        ape[i].peRed   = (BYTE)(((i >> 5) & 0x07) * 255 / 7);
        ape[i].peGreen = (BYTE)(((i >> 2) & 0x07) * 255 / 7);
        ape[i].peBlue  = (BYTE)(((i >> 0) & 0x03) * 255 / 3);
        ape[i].peFlags = (BYTE)0;
    }

    //
    // get a pointer to the bitmap resource.
    //
    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP)))
    {
		if ( g_bDebug )  OutputDebug("DDLoadPalette -a\n");
        lpbi = (LPBITMAPINFOHEADER)LockResource(LoadResource(NULL, h));
        if (!lpbi)
            OutputDebug("lock resource failed\n");
        prgb = (RGBQUAD*)((BYTE*)lpbi + lpbi->biSize);

        if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (lpbi->biBitCount > 8)
            n = 0;
        else if (lpbi->biClrUsed == 0)
            n = 1 << lpbi->biBitCount;
        else
            n = lpbi->biClrUsed;

        //
        //  a DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        //
        for(i=0; i<n; i++ )
        {
            ape[i].peRed   = prgb[i].rgbRed;
            ape[i].peGreen = prgb[i].rgbGreen;
            ape[i].peBlue  = prgb[i].rgbBlue;
            ape[i].peFlags = 0;
        }
    }
    else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1)
    {
		if ( g_bDebug )  OutputDebug("DDLoadPalette -b\n");
        BITMAPFILEHEADER bf;
        BITMAPINFOHEADER bi;

        _lread(fh, &bf, sizeof(bf));
        _lread(fh, &bi, sizeof(bi));
        _lread(fh, ape, sizeof(ape));
        _lclose(fh);

        if (bi.biSize != sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (bi.biBitCount > 8)
            n = 0;
        else if (bi.biClrUsed == 0)
            n = 1 << bi.biBitCount;
        else
            n = bi.biClrUsed;

        //
        //  a DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        //
		//?
		char s[50];
		sprintf(s, "DDLoadPalette n=%d\n", n);
		if ( g_bDebug )  OutputDebug(s);
		//?
        for(i=0; i<n; i++ )
        {
            BYTE r = ape[i].peRed;
            ape[i].peRed  = ape[i].peBlue;
            ape[i].peBlue = r;
        }
    }

    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);

    return ddpal;
}

/*
 * DDColorMatch
 *
 * convert a RGB color to a pysical color.
 *
 * we do this by leting GDI SetPixel() do the color matching
 * then we lock the memory and see what it got mapped to.
 */
extern "C" DWORD DDColorMatch(IDirectDrawSurface *pdds, COLORREF rgb)
{
    COLORREF rgbT;
    HDC hdc;
    DWORD dw = CLR_INVALID;
    DDSURFACEDESC ddsd;
    HRESULT hres;

    //
    //  use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);             // save current pixel value
        SetPixel(hdc, 0, 0, rgb);               // set our value
        pdds->ReleaseDC(hdc);
    }

    //
    // now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;

    if (hres == DD_OK)
    {
        dw  = *(DWORD *)ddsd.lpSurface;                     // get DWORD
        dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount)-1;  // mask it to bpp
        pdds->Unlock(NULL);
    }

    //
    //  now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }

    return dw;
}

/*
 * DDSetColorKey
 *
 * set a color key for a surface, given a RGB.
 * if you pass CLR_INVALID as the color key, the pixel
 * in the upper-left corner will be used.
 */
extern "C" HRESULT DDSetColorKey(IDirectDrawSurface *pdds, COLORREF rgb)
{
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

extern "C" HRESULT DDSetColorKey2(IDirectDrawSurface *pdds, COLORREF rgb1,
															COLORREF rgb2)
{
    DDCOLORKEY          ddck;

    ddck.dwColorSpaceLowValue  = DDColorMatch(pdds, rgb1);
    ddck.dwColorSpaceHighValue = DDColorMatch(pdds, rgb2);
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}
