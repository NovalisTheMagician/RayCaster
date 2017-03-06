#ifndef _WINDOW_H
#define _WINDOW_H

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Winmm.lib")

#define CLASS_NAME L"RayCastEngine"

typedef bool (*InitCallback)(void);
typedef void (*UpdateCallback)(float);
typedef void (*DrawCallback)(void);
typedef void (*CleanupCallback)(void);

typedef void (*KeyUpCallback)(int);
typedef void (*KeyDownCallback)(int);
typedef void (*MouseUpCallback)(int);
typedef void (*MouseDownCallback)(int);
typedef void (*MouseMoveCallback)(int, int);
typedef void (*MouseWheelCallback)(int);

typedef struct tagWINDOW
{
	InitCallback	cbInit;
	UpdateCallback	cbUpdate;
	DrawCallback	cbDraw;
	CleanupCallback cbCleanup;

	KeyDownCallback cbKeyDown;
	KeyUpCallback	cbKeyUp;

	MouseDownCallback	cbMouseDown;
	MouseUpCallback		cbMouseUp;
	MouseMoveCallback	cbMouseMove;
	MouseWheelCallback	cbMouseWheel;

	wchar_t *szTitle;

	int nWidth;
	int nHeight;

	HWND hWnd;

} WND_CONTEXT;

int OpenWindow(WND_CONTEXT *pContext, int nShowCmd, HINSTANCE hInstance);
int StartLoop(WND_CONTEXT *pContext);

#endif
