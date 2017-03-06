#include "GraphicsDevice.h"

void put_pixel(int x, int y, int rgb, GraphicsDevice *gd)
{
	int offs = (y * gd->nLineLength) + (x * BYTES_PER_PIXEL);

	if((offs + 2) > gd->nMemLn)
		return;

	gd->pBuffer[offs + 2] = (rgb >> 16) & 0xFF;
	gd->pBuffer[offs + 1] = (rgb >>  8) & 0xFF;
	gd->pBuffer[offs + 0] = (rgb >>  0) & 0xFF;
}

void draw_line(int x1, int y1, int x2, int y2, int rgb, GraphicsDevice *gd)
{
	int dx = x2 - x1;
	dx = (dx >= 0) ? dx : -dx;
	int dy = y2 - y1;
	dy = (dy >= 0) ? dy : -dy;

	int sx;
	int sy;

	if(x1 < x2)
		sx = 1;
	else
		sx = -1;

	if(y1 < y2)
		sy = 1;
	else
		sy = -1;

	int err = dx - dy;
	int e2;
	int done = 0;
	while(!done)
	{
		put_pixel(x1, y1, rgb, gd);
		if((x1 == x2) && (y1 == y2))
			done = 1;
		else
		{
			e2 = 2 * err;
			if(e2 > -dy)
			{
				err -= dy;
				x1 += sx;
			}
			if(e2 < dx)
			{
				err += dx;
				y1 += sy;
			}
		}
	}
}

void draw_verLine(int x, int y, int length, int rgb, GraphicsDevice *gd)
{
	int i;
	for(i = 0; i < length; ++i)
		put_pixel(x, y + i, rgb, gd);
}

void draw_horLine(int x, int y, int length, int rgb, GraphicsDevice *gd)
{
	int i;
	for(i = 0; i < length; ++i)
		put_pixel(x + i, y, rgb, gd);
}

void draw_rect(int x, int y, int w, int h, int rgb, GraphicsDevice *gd)
{
	draw_line(x, y, x + w, y, rgb, gd);
	draw_line(x, y, x, y + h, rgb, gd);
	draw_line(x, y + h, x + w, y + h, rgb, gd);
	draw_line(x + w, y, x + w, y + h, rgb, gd);
}

void fill_rect(int x, int y, int w, int h, int rgb, GraphicsDevice *gd)
{
	int cy;
	for(cy = 0; cy < h; ++cy)
		draw_line(x, y + cy, x + w, y + cy, rgb, gd);
}

void draw_circle(int x, int y, int r, int rgb, GraphicsDevice *gd)
{
	int cx = r;
	int cy = 0;
	int radiusError = 1 - x;

	while(cx >= cy)
	{
		put_pixel(-cy + x, -cx + y, rgb, gd);
		put_pixel(cy + x, -cx + y, rgb, gd);
		put_pixel(-cx + x, -cy + y, rgb, gd);
		put_pixel(cx + x, -cy + y, rgb, gd);
		put_pixel(-cx + x, cy + y, rgb, gd);
		put_pixel(cx + x, cy + y, rgb, gd);
		put_pixel(-cy + x, cx + y, rgb, gd);
		put_pixel(cy + x, cx + y, rgb, gd);

		cy++;
		if(radiusError < 0)
			radiusError += 2 * cy + 1;
		else
		{
			cx--;
			radiusError += 2 * (cy - cx + 1);
		}
	}
}

void fill_circle(int x, int y, int r, int rgb, GraphicsDevice *gd)
{
	int cx = r;
	int cy = 0;
	int radiusError = 1 - cx;

	while(cx >= cy)
	{
		draw_line(-cy + x, -cx + y, cy + x, -cx + y, rgb, gd);
		draw_line(-cx + x, -cy + y, cx + x, -cy + y, rgb, gd);
		draw_line(-cx + x, cy + y, cx + x, cy + y, rgb, gd);
		draw_line(-cy + x, cx + y, cy + x, cx + y, rgb, gd);

		cy++;
		if(radiusError < 0)
			radiusError += 2 * cy + 1;
		else
		{
			cx--;
			radiusError += 2 * (cy - cx + 1);
		}
	}
}

void clear_screen(int rgb, GraphicsDevice *gd)
{
	memset(gd->pBuffer, rgb, gd->nMemLn);
}

void draw_buffer(BYTE *buffer, GraphicsDevice *gd)
{
	memcpy(gd->pBuffer, buffer, gd->nMemLn);
}

void bit_Blit(int x, int y, int width, int height, BYTE *buffer, GraphicsDevice *gd)
{
	for(int i = 0, j = y; j < height; ++j, ++i)
		memcpy(&gd->pBuffer[j * gd->nLineLength + x], &buffer[i * gd->nLineLength], width * gd->nLineLength);
}

void draw_text(int x, int y, const char *text, FontInfo fontInfo, BYTE **font, GraphicsDevice *gd)
{
	int l = strlen(text);
	for(int i = 0; i < l; ++i)
	{
		int index = (i - fontInfo.nOffset) < 0 ? 0 : ((i - fontInfo.nOffset) >= fontInfo.nNumChars ? 0 : i - fontInfo.nOffset);
		BYTE *img = font[index];
		for(int py = 0; py < fontInfo.nHeight; ++py)
			for(int px = 0; px < fontInfo.nWidth; ++px)
			{
				BYTE c = img[py * fontInfo.nWidth + px];
				if(c > 0)
					put_pixel(x + i * fontInfo.nWidth + px, y + i * fontInfo.nHeight + py, 0xffffff, gd);
			}
	}
}

GraphicsDevice *InitBackBuffer(WND_CONTEXT *pContext)
{
	GraphicsDevice *gd = (GraphicsDevice*)malloc(sizeof(GraphicsDevice));

	HDC hDc = GetDC(pContext->hWnd);

	BITMAPINFO info = {0};

	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biBitCount = BYTES_PER_PIXEL * 8;
	info.bmiHeader.biWidth = pContext->nWidth;
	info.bmiHeader.biHeight = -pContext->nHeight;
	info.bmiHeader.biPlanes = 1;

	gd->hBackBuffer = CreateDIBSection(hDc, &info, DIB_RGB_COLORS, (void**)&gd->pBuffer, NULL, 0);
	if(gd->pBuffer == NULL)
	{
		printf("ERROR\n");
		delete gd;
		return NULL;
	}

	gd->hBackBufferDC = CreateCompatibleDC(hDc);

	HBITMAP oldBmp = (HBITMAP)SelectObject(gd->hBackBufferDC, gd->hBackBuffer);

	DeleteObject(oldBmp);

	ReleaseDC(pContext->hWnd, hDc);

	gd->nWidth = pContext->nWidth;
	gd->nHeight = pContext->nHeight;
	gd->nMemLn = gd->nWidth * BYTES_PER_PIXEL * gd->nHeight;
	gd->nLineLength = gd->nWidth * BYTES_PER_PIXEL;

	return gd;
}

GraphicsDevice *CreateGD(WND_CONTEXT *pContext)
{
	if(!pContext)
		return NULL;
	return InitBackBuffer(pContext);
}

void FreeGD(GraphicsDevice *pDevice)
{
	if(!pDevice)
		return;

	DeleteDC(pDevice->hBackBufferDC);
	DeleteObject(pDevice->hBackBuffer);

	pDevice->pBuffer = NULL;

	free(pDevice);
	pDevice = NULL;
}

void Flip(GraphicsDevice *pDevice, WND_CONTEXT *pContext)
{
	HDC hDc = GetDC(pContext->hWnd);
	BitBlt(hDc, 0, 0, pDevice->nWidth, pDevice->nHeight, pDevice->hBackBufferDC, 0, 0, SRCCOPY);
	ReleaseDC(pContext->hWnd, hDc);
}
