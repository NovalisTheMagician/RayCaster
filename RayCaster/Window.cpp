#include "Window.h"
#include <windowsx.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int OpenWindow(WND_CONTEXT *pContext, int nShowCmd, HINSTANCE hInstance)
{
	if(!pContext)
		return 1;

	WNDCLASSEX wnd = {0};
	wnd.cbSize			= sizeof(WNDCLASSEX);
	wnd.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd.hCursor			= LoadCursor(NULL, IDC_HAND);
	wnd.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wnd.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	wnd.hInstance		= hInstance;
	wnd.lpfnWndProc		= WndProc;
	wnd.lpszClassName	= CLASS_NAME;
	wnd.lpszMenuName	= NULL;
	wnd.style			= CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	if(!RegisterClassEx(&wnd))
		return 3;

	RECT rect = { 0, 0, pContext->nWidth, pContext->nHeight };
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	int windowW = rect.right - rect.left;
	int windowH = rect.bottom - rect.top;

	int displayW = GetSystemMetrics(SM_CXSCREEN);
	int displayH = GetSystemMetrics(SM_CYSCREEN);

	int x = (displayW - windowW) / 2;
	int y = (displayH - windowH) / 2;

	pContext->hWnd = CreateWindow(CLASS_NAME, 
									pContext->szTitle, WS_OVERLAPPEDWINDOW, 
									x, y, 
									windowW, windowH, 
									NULL, NULL, hInstance, NULL);

	if(pContext->hWnd == 0)
		return 2;

	SetWindowLongPtr(pContext->hWnd, GWLP_USERDATA, (LONG)(pContext));

	ShowWindow(pContext->hWnd, nShowCmd);
	UpdateWindow(pContext->hWnd);

	return 0;
}

int StartLoop(WND_CONTEXT *pContext)
{
	if(pContext->cbInit)
		if(!pContext->cbInit())
			return 1;

	timeBeginPeriod(1);

	DWORD curTime = timeGetTime();
	DWORD prevTime = curTime;

	MSG msg = {0};
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			curTime = timeGetTime();
			float delta = (float)(curTime - prevTime) / 1000.0f;
			prevTime = curTime;

			if(pContext->cbUpdate)
				pContext->cbUpdate(delta);
			if(pContext->cbDraw)
				pContext->cbDraw();
		}
	}

	timeEndPeriod(1);

	if(pContext->cbCleanup)
		pContext->cbCleanup();

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WND_CONTEXT *pContext = (WND_CONTEXT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch(msg)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hDc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_KEYDOWN:
		{
			if(pContext->cbKeyDown)
				pContext->cbKeyDown(wParam);
		}
		break;

	case WM_KEYUP:
		{
			if(pContext->cbKeyUp)
				pContext->cbKeyUp(wParam);
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
		{
			if(pContext->cbMouseDown)
				pContext->cbMouseDown(wParam);
		}
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
		{
			if(pContext->cbMouseUp)
				pContext->cbMouseUp(wParam);
		}
		break;

	case WM_MOUSEMOVE:
		{
			if(pContext->cbMouseMove)
				pContext->cbMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		}
		break;

	case WM_MOUSEHWHEEL:
		{
			if(pContext->cbMouseWheel)
				pContext->cbMouseWheel(HIWORD(wParam));
		}
		break;

	case WM_DESTROY:
			PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}
