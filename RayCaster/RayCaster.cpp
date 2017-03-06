#include "Window.h"
#include <sstream>

#include "Vector.h"
#include "Color.h"
#include "Matrix.h"
#include "GraphicsDevice.h"

#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <assert.h>

#define SCREEN_SCALE 1.0

#define SCREEN_W (int(800 * SCREEN_SCALE))
#define SCREEN_H (int(480 * SCREEN_SCALE))

#define PI 3.14159265359f
#define TWOPI (PI * 2.0f)

#define DEG2RAD(a) (a * (PI / 180.0f))
#define RAD2DEG(x) (x * (180.0f / PI))

#define MAP_W 24
#define MAP_H 24

#define TEXTURE_W 256
#define TEXTURE_H 256

#define MINIMAP_SCALE 7

int map[MAP_W][MAP_H] = 
{
	{8,8,8,8,8,8,8,8,8,8,8,4,4,6,4,4,6,4,6,4,4,4,6,4},
	{8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
	{8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,6},
	{8,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
	{8,0,3,3,0,0,0,0,0,8,8,4,0,0,0,0,0,0,0,0,0,0,0,4},
	{8,0,0,0,0,0,0,0,0,0,8,4,0,0,0,0,0,6,6,6,0,6,4,6},
	{8,8,8,8,0,8,8,8,8,8,8,4,4,4,4,4,4,6,0,0,0,0,0,6},
	{7,7,7,7,0,7,7,7,7,0,8,0,8,0,8,0,8,4,0,4,0,6,0,6},
	{7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,0,0,0,0,0,6},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,0,0,0,0,4},
	{7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,6,0,6,0,6,0,6},
	{7,7,0,0,0,0,0,0,7,8,0,8,0,8,0,8,8,6,4,6,0,6,6,6},
	{7,7,7,7,0,7,7,7,7,8,8,4,0,6,8,4,8,3,3,3,0,3,3,3},
	{2,2,2,2,0,2,2,2,2,4,6,4,0,0,6,0,6,3,0,0,0,0,0,3},
	{2,2,0,0,0,0,0,2,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
	{2,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,4,3,0,0,0,0,0,3},
	{1,0,0,0,0,0,0,0,1,4,4,4,4,4,6,0,6,3,3,0,0,0,3,3},
	{2,0,0,0,0,0,0,0,2,2,2,1,2,2,2,6,6,0,0,5,0,5,0,5},
	{2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
	{2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
	{1,0,0,0,0,0,0,0,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5},
	{2,0,0,0,0,0,0,0,2,0,0,0,0,0,2,5,0,5,0,5,0,5,0,5},
	{2,2,0,0,0,0,0,2,2,2,0,0,0,2,2,0,5,0,5,0,0,0,5,5},
	{2,2,2,2,1,2,2,2,2,2,2,1,2,2,2,5,5,5,5,5,5,5,5,5}
};

bool Init();
void Update(float);
void Draw();
void Cleanup();

WND_CONTEXT context = {0};
GraphicsDevice *pDevice = NULL;

void KeyUp(int);
void KeyDown(int);
void ButtonUp(int);
void ButtonDown(int);
void MouseWheel(int);
void MouseMove(int, int);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	freopen("CON", "w", stdout);

	context.szTitle = L"RayCast";
	context.nWidth	= SCREEN_W;
	context.nHeight = SCREEN_H;

	context.cbInit		= Init;
	context.cbUpdate	= Update;
	context.cbDraw		= Draw;
	context.cbCleanup	= Cleanup;

	context.cbKeyDown	= KeyDown;
	context.cbKeyUp		= KeyUp;
	context.cbMouseDown = ButtonDown;
	context.cbMouseUp	= ButtonUp;
	context.cbMouseWheel= MouseWheel;
	context.cbMouseMove	= MouseMove;

	if(OpenWindow(&context, nShowCmd, hInstance) != 0)
		return -1;

	return StartLoop(&context);
}

BYTE* LoadTexture(LPCTSTR imagePath, LONG *width, LONG *height, DWORD *size)
{
	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfo;
	DWORD bytesRead;

	if(!imagePath)
		return NULL;

	HANDLE file = CreateFile(imagePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(!file)
		return NULL;

	if(!ReadFile(file, &bmpHeader, sizeof(BITMAPFILEHEADER), &bytesRead, NULL))
	{
		CloseHandle(file);
		return NULL;
	}

	if(!ReadFile(file, &bmpInfo, sizeof(BITMAPINFOHEADER), &bytesRead, NULL))
	{
		CloseHandle(file);
		return NULL;
	}

	if(bmpHeader.bfType != 'MB')
	{
		CloseHandle(file);
		return NULL;
	}

	if(bmpInfo.biCompression != BI_RGB)
	{
		CloseHandle(file);
		return NULL;
	}

	if(bmpInfo.biBitCount != 24)
	{
		CloseHandle(file);
		return NULL;
	}

	*width = bmpInfo.biWidth;
	*height = abs(bmpInfo.biHeight);
	*size = bmpHeader.bfSize - bmpHeader.bfOffBits;
	BYTE* buffer = new BYTE[*size];

	SetFilePointer(file, bmpHeader.bfOffBits, NULL, FILE_BEGIN);

	if(!ReadFile(file, buffer, *size, &bytesRead, NULL))
	{
		delete[] buffer;
		CloseHandle(file);
		return NULL;
	}

	CloseHandle(file);
	return buffer;
}

// BMP:			 BGR  BGR  BGR  BGR  BGR
// XRGB:		XRGB XRGB XRGB XRGB XRGB

BYTE* BMPtoXRGB(BYTE *buffer, int width, int height)
{
	if(!buffer || width == 0 || height == 0)
		return NULL;

	BYTE *newBuffer = new BYTE[width * height * 4];

	int i = 0;
	int j = width * height * 3 - 1;
	for(int y = 0; y < height; ++y)
		for(int x = 0; x < width; ++x)
		{
			newBuffer[i+3] = 0xFF;
			newBuffer[i+2] = buffer[j-0];
			newBuffer[i+1] = buffer[j-1];
			newBuffer[i+0] = buffer[j-2];

			i+=4;
			j-=3;
		}

	return newBuffer;
}

//////////////////////////////////////////////////////////////////////////

inline float sqrt_f(const float x)
{
	union
	{
		int i;
		float x;
	} u;
	u.x = x;
	u.i = (1<<29) + (u.i >> 1) - (1<<22); 
	return u.x;
}

#if 0
#define sqrt sqrt_f
#else
#define sqrt std::sqrtf
#endif

vec2_t playerPos;
float playerAngle;

float zBuffer[SCREEN_W];

uint32_t texture[9][TEXTURE_W * TEXTURE_H];

float fov = DEG2RAD(60);
float fovHalf = fov / 2.0f;
float viewDist = ((SCREEN_W / 2) / std::tanf(fovHalf));

bool keys[256] = {0};
bool buttons[3] = {0};
int wheel = 0;
int mx = 0, my = 0;

vec2_t hits[SCREEN_W];

int centerX, centerY;

void CalcFOV(float newfov)
{
	fov = newfov;
	fovHalf = fov / 2.0f;
	viewDist = ((SCREEN_W / 2) / std::tanf(fovHalf));
}

bool Init()
{
	pDevice = CreateGD(&context);
	if(!pDevice) return false;

	playerPos = vec2_t(22, 11.5f);
	playerAngle = 0;

	for(int x = 0; x < TEXTURE_W; ++x)
		for(int y = 0; y < TEXTURE_H; ++y)
		{
			int xorcolor = (x * 256 / TEXTURE_W) ^ (y * 256 / TEXTURE_H);
			int ycolor = y * 256 / TEXTURE_H;
			int xycolor = y * 128 / TEXTURE_H + x * 128 / TEXTURE_W;
			texture[0][TEXTURE_W * y + x] = 65536 * 254 * (x != y && x != TEXTURE_W - y); //flat red texture with black cross
			texture[1][TEXTURE_W * y + x] = xycolor + 256 * xycolor + 65536 * xycolor; //sloped greyscale
			texture[2][TEXTURE_W * y + x] = 256 * xycolor + 65536 * xycolor; //sloped yellow gradient
			texture[3][TEXTURE_W * y + x] = xorcolor + 256 * xorcolor + 65536 * xorcolor; //xor greyscale
			texture[4][TEXTURE_W * y + x] = 256 * xorcolor; //xor green
			texture[5][TEXTURE_W * y + x] = 65536 * 192 * (x % 16 && y % 16); //red bricks
			texture[6][TEXTURE_W * y + x] = 65536 * ycolor; //red gradient
			texture[7][TEXTURE_W * y + x] = 128 + 256 * 128 + 65536 * 128; //flat grey texture
		}

	LONG width, height;
	DWORD unused;
	BYTE* tmp = LoadTexture(L"brick_24.bmp", &width, &height, &unused);
	BYTE* buffer = BMPtoXRGB(tmp, width, height);
	memcpy(texture[8], buffer, TEXTURE_W * TEXTURE_H * 4);
	delete[] tmp;
	delete[] buffer;

	centerX = GetSystemMetrics(SM_CXSCREEN) / 2;
	centerY = GetSystemMetrics(SM_CYSCREEN) / 2;

	SetCursorPos(centerX, centerY);
	ShowCursor(FALSE);

	return true;
}

bool IsBlocking(float x, float y)
{
	return (map[int(x)][int(y)] != 0);
}

vec2_t CheckCollision(const vec2_t from, const vec2_t to, const float radius)
{
	vec2_t pos = from;

	float blockX = std::floorf(to.x);
	float blockY = std::floorf(to.y);

	if(IsBlocking(blockX, blockY))
		return pos;

	pos = to;

	bool blockTop = IsBlocking(blockX, blockY - 1);
	bool blockBottom = IsBlocking(blockX, blockY + 1);
	bool blockLeft = IsBlocking(blockX - 1, blockY);
	bool blockRight = IsBlocking(blockX + 1, blockY);

	if (blockTop && to.y - blockY < radius)
		pos.y = blockY + radius;
	if (blockBottom && blockY + 1 - to.y < radius)
		pos.y = blockY + 1 - radius;
	if (blockLeft && to.x - blockX < radius)
		pos.x = blockX + radius;
	if (blockRight && blockX + 1 - to.x < radius)
		pos.x = blockX + 1 - radius;

	// is tile to the top-left a wall
	if (IsBlocking(blockX - 1, blockY - 1) && !(blockTop && blockLeft)) 
	{
		float dx = to.x - blockX;
		float dy = to.y - blockY;
		if (dx*dx + dy*dy < radius*radius) {
			if (dx*dx > dy*dy)
				pos.x = blockX + radius;
			else
				pos.y = blockY + radius;
		}
	}
	// is tile to the top-right a wall
	if (IsBlocking(blockX + 1, blockY - 1) && !(blockTop && blockRight)) 
	{
		float dx = to.x - (blockX + 1);
		float dy = to.y - blockY;
		if (dx*dx+dy*dy < radius*radius) 
		{
			if (dx*dx > dy*dy)
				pos.x = blockX + 1 - radius;
			else
				pos.y = blockY + radius;
		}
	}
	// is tile to the bottom-left a wall
	if (IsBlocking(blockX - 1, blockY + 1) && !(blockBottom && blockBottom)) 
	{
		float dx = to.x - blockX;
		float dy = to.y - (blockY + 1);
		if (dx*dx + dy*dy < radius*radius) 
		{
			if (dx*dx > dy*dy)
				pos.x = blockX + radius;
			else
				pos.y = blockY + 1 - radius;
		}
	}
	// is tile to the bottom-right a wall
	if (IsBlocking(blockX + 1, blockY + 1) && !(blockBottom && blockRight)) 
	{
		float dx = to.x - (blockX + 1);
		float dy = to.y - (blockY + 1);
		if (dx*dx + dy*dy < radius*radius) 
		{
			if (dx*dx > dy*dy)
				pos.x = blockX + 1 - radius;
			else
				pos.y = blockY + 1 - radius;
		}
	}

	return pos;
}

int counter = 0;
float timePassed = 0;

int pmx = 400;

void Update(float d)
{
	if(keys[VK_ESCAPE])
		PostQuitMessage(0);

	int md = mx - pmx;

	float moveSpeed = d * 3.0f;
	float rotSpeed = d * md * 0.5f;

	vec2_t newPos = playerPos;

	if(keys['W'])
	{
		newPos.x += std::cosf(playerAngle) * moveSpeed;
		newPos.y += std::sinf(playerAngle) * moveSpeed;

		playerPos = CheckCollision(playerPos, newPos, 0.1f);
	}

	if(keys['S'])
	{
		newPos.x -= std::cosf(playerAngle) * moveSpeed;
		newPos.y -= std::sinf(playerAngle) * moveSpeed;

		playerPos = CheckCollision(playerPos, newPos, 0.1f);
	}

	if(keys['D'])
	{
		newPos.x += -std::sinf(playerAngle) * moveSpeed;
		newPos.y += std::cosf(playerAngle) * moveSpeed;

		playerPos = CheckCollision(playerPos, newPos, 0.1f);
	}

	if(keys['A'])
	{
		newPos.x -= -std::sinf(playerAngle) * moveSpeed;
		newPos.y -= std::cosf(playerAngle) * moveSpeed;

		playerPos = CheckCollision(playerPos, newPos, 0.1f);
	}

	playerAngle += rotSpeed;
	if(playerAngle >= TWOPI)
		playerAngle -= TWOPI;

	if(playerAngle < 0)
		playerAngle += TWOPI;

	if(keys['H'])
		CalcFOV(DEG2RAD(60));

	if(keys['G'])
		CalcFOV(DEG2RAD(179));

	printf("Delta: %.3f\t", d);
	timePassed += d;
	if(timePassed >= 1.0f)
	{
		timePassed -= 1.0f;
		printf("FPS: %3i", counter);
		counter = 0;
	}
	counter++;

	printf("\r");

	pmx = 400;
	wheel = 0;

	SetCursorPos(centerX, centerY);
}

void CastRay(float rayAngle, int x)
{
	rayAngle = std::fmod(rayAngle, TWOPI);
	if(rayAngle < 0) rayAngle += TWOPI;

	zBuffer[x] = FLT_MAX;

	int mapX;
	int mapY;

	float xHit;
	float yHit;

	int wallType;

	float textureX;

	bool right = (rayAngle > TWOPI * 0.75f || rayAngle < TWOPI * 0.25f);
	bool up = (rayAngle < 0 || rayAngle > PI);

	float angleSin = std::sinf(rayAngle);
	float angleCos = std::cosf(rayAngle);

	float dist = FLT_MAX;

	bool wallIsHorizontal = false;

	float slope = angleSin / angleCos;
	float dx = right ? 1.f : -1.f;
	float dy = dx * slope;

	float ax = right ? std::ceilf(playerPos.x) : std::floorf(playerPos.x);
	float ay = playerPos.y + (ax - playerPos.x) * slope;

	while(ax >= 0 && ax < MAP_W && ay >= 0 && ay < MAP_H)
	{
		mapX = int(ax + (right ? 0 : -1));
		mapY = int(ay);

		if(mapX < MAP_W && mapY < MAP_H && mapX >= 0 && mapY >= 0)
		if(map[mapX][mapY] > 0)
		{
			float distX = ax - playerPos.x;
			float distY = ay - playerPos.y;

			if(map[mapX][mapY] == 9)
			{
				distX += dx / 2;
				distY += dy / 2;
			}

			xHit = ax;
			yHit = ay;

			wallType = map[mapX][mapY] - 1;
			textureX = std::fmod(ay, 1);
			if(!right) textureX = 1 - textureX;

			dist = distX*distX + distY*distY;

			wallIsHorizontal = true;

			break;
		}

		ax += dx;
		ay += dy;
	}

	slope = angleCos / angleSin;
	dy = up ? -1.f : 1.f;
	dx = dy * slope;

	ay = up ? std::floorf(playerPos.y) : std::ceilf(playerPos.y);
	ax = playerPos.x + (ay - playerPos.y) * slope;

	while(ax >= 0 && ax < MAP_W && ay >= 0 && ay < MAP_H)
	{
		mapY = int(ay + (up ? -1 : 0));
		mapX = int(ax);

		if(mapX < MAP_W && mapY < MAP_H && mapX >= 0 && mapY >= 0)
		if(map[mapX][mapY] > 0)
		{
			float distX = ax - playerPos.x;
			float distY = ay - playerPos.y;

			if(map[mapX][mapY] == 9)
			{
				distX += dx / 2;
				distY += dy / 2;
			}

			float blockDist = distX*distX + distY*distY;
			if(blockDist < dist) 
			{
				dist = blockDist;
				xHit = ax;
				yHit = ay;
				wallIsHorizontal = false;
				wallType = map[mapX][mapY] - 1;
				textureX = std::fmod(ax, 1);
				if(up) textureX = 1 - textureX;
			}

			break;
		}

		ax += dx;
		ay += dy;
	}

	if(dist < 10000.0f)
	{
		hits[x] = vec2_t(xHit, yHit);

		dist = sqrt(dist); // sqrt_f(dist);

		dist *= std::cosf(playerAngle - rayAngle);

		int height = int(viewDist / dist);
		int drawStart = -height / 2 + SCREEN_H / 2;
		if(drawStart < 0) drawStart = 0;
		int drawEnd = height / 2 + SCREEN_H / 2;
		if(drawEnd >= SCREEN_H) drawEnd = SCREEN_H - 1;

		int texX = int(textureX * TEXTURE_W);

		for(int y = drawStart; y < drawEnd; ++y)
		{
			int d = y * 256 - SCREEN_H * 128 + height * 128;
			int texY = ((d * TEXTURE_H) / height) / 256;
			color_t color = texture[wallType][TEXTURE_W * texY + texX];
			if(wallIsHorizontal) color = color / 2.0f;
			((uint32_t*)pDevice->pBuffer)[SCREEN_W * y + x] = color;
		}
	}
}

void DrawRay(int rayX, int rayY)
{
	vec2_t pos = playerPos * MINIMAP_SCALE;
	draw_line((int)pos.x, (int)pos.y, rayX, rayY, RGB_GREEN, pDevice);
}

void DrawMiniMap()
{
	fill_rect(0, 0, MAP_W * MINIMAP_SCALE, MAP_H * MINIMAP_SCALE, RGB_BLACK, pDevice);

	for(int y = 0; y < MAP_H; y++)
		for(int x = 0; x < MAP_W; x++)
			if(map[x][y])
				fill_rect(x * MINIMAP_SCALE, y * MINIMAP_SCALE, MINIMAP_SCALE, MINIMAP_SCALE, RGB_WHITE, pDevice);

	vec2_t pos = playerPos * MINIMAP_SCALE;

	for(int i = 0; i < SCREEN_W; ++i)
		DrawRay((int)(hits[i].x * MINIMAP_SCALE), (int)(hits[i].y * MINIMAP_SCALE));

	fill_circle((int)pos.x, (int)pos.y, 3, RGB_RED, pDevice);
	draw_line((int)pos.x, (int)pos.y, (int)pos.x + int(std::cosf(playerAngle) * 10), (int)pos.y + int(std::sinf(playerAngle) * 10), RGB_BLUE, pDevice);
}

void Draw()
{
	//clear_screen(RGB_BLACK, pDevice);
	fill_rect(0, 0, SCREEN_W, SCREEN_H / 2, RGB_BLACK, pDevice);
	fill_rect(0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2, RGB_GRAY, pDevice);

	for(int x = 0; x < SCREEN_W; ++x)
	{
		int rayScreenPos = (-SCREEN_W / 2 + x);
		float rayViewDist = sqrt(float(rayScreenPos * rayScreenPos + viewDist * viewDist)); // sqrt_f(float(rayScreenPos * rayScreenPos + viewDist * viewDist));
		float rayAngle = std::asinf(float(rayScreenPos / rayViewDist));

		CastRay(playerAngle + rayAngle, x);
	}

	DrawMiniMap();

	Flip(pDevice, &context);
}

void Cleanup()
{
	FreeGD(pDevice);
	ShowCursor(TRUE);
}

void KeyUp(int keycode)
{
	keys[keycode] = false;
}

void KeyDown(int keycode)
{
	keys[keycode] = true;
}

void ButtonUp(int mousecode)
{
	buttons[mousecode] = false;
}

void ButtonDown(int mousecode)
{
	buttons[mousecode] = true;
}

void MouseWheel(int steps)
{
	wheel = steps;
}

void MouseMove(int x, int y)
{
	mx = x;
	my = y;
}
