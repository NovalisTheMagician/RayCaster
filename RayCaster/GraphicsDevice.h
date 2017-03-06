#ifndef _GRAPHICSDEVICE_H
#define _GRAPHICSDEVICE_H

#include "Window.h"

#ifndef BYTES_PER_PIXEL
#define BYTES_PER_PIXEL 4
#endif

typedef struct tagGRAPHICS
{
	HBITMAP	hBackBuffer;
	HDC		hBackBufferDC;
	BYTE *pBuffer;

	int nMemLn;
	int nLineLength;

	int nWidth;
	int nHeight;
} GraphicsDevice;

typedef struct tagFONT
{
	USHORT nWidth;
	USHORT nHeight;
	BYTE nNumChars;
	BYTE nOffset;
} FontInfo;

void put_pixel(int x, int y, int rgb, GraphicsDevice *gd);
void draw_line(int x1, int y1, int x2, int y2, int rgb, GraphicsDevice *gd);
void draw_verLine(int x, int y, int length, int rgb, GraphicsDevice *gd);
void draw_horLine(int x, int y, int length, int rgb, GraphicsDevice *gd);
void draw_rect(int x, int y, int w, int h, int rgb, GraphicsDevice *gd);
void fill_rect(int x, int y, int w, int h, int rgb, GraphicsDevice *gd);
void draw_circle(int x, int y, int r, int rgb, GraphicsDevice *gd);
void fill_circle(int x, int y, int r, int rgb, GraphicsDevice *gd);

void draw_text(int x, int y, const char *text, FontInfo fontInfo, BYTE **font, GraphicsDevice *gd);

void draw_buffer(BYTE *buffer, GraphicsDevice *gd);
void bit_Blit(int x, int y, int width, int height, BYTE *buffer, GraphicsDevice *gd);

void clear_screen(int rgb, GraphicsDevice *gd);

static GraphicsDevice *InitBackBuffer(WND_CONTEXT *pContext);

GraphicsDevice *CreateGD(WND_CONTEXT *pContext);
void			FreeGD(GraphicsDevice *pDevice);

void Flip(GraphicsDevice *pDevice, WND_CONTEXT *pContext);

#endif
