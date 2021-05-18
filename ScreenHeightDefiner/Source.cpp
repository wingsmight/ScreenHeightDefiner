#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef int (*ImportFunction)(char*);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadFunc(void*);

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 150;

const int LABEL_ID = 1;
const int HEIGHT_LABEL_ID = 2;

char info[256];
HWND hwnd;
HWND heightLabelHwnd;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdStr, int showCmd)
{
	MSG msg{};
	WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = "ScreenWidthClass";
	wc.lpszMenuName = NULL;
	wc.style = CS_VREDRAW | CS_HREDRAW;

	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	// main window
	HDC screen = GetDC(NULL);
	DWORD windowStyle = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX); //default window style without resizing and maximazing
	int windowPoxX = (GetDeviceCaps(screen, HORZRES) - WINDOW_WIDTH) / 2; //center position of the screen
	int windowPoxY = (GetDeviceCaps(screen, VERTRES) - WINDOW_HEIGHT) / 2;
	hwnd = CreateWindow(wc.lpszClassName, "Screen height definer",
		windowStyle, windowPoxX, windowPoxY, WINDOW_WIDTH, WINDOW_HEIGHT,
		NULL, NULL, wc.hInstance, NULL);

	if (hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

	// label
	DWORD labelStyle = WS_CHILD | WS_VISIBLE | SS_CENTER;
	int labelWidth = WINDOW_WIDTH;
	int labelHeight = 100;
	HWND labelHwnd = CreateWindow("static", "Screen height:",
		labelStyle,
		0, 0, labelWidth, labelHeight,
		hwnd, (HMENU)LABEL_ID, hInstance, NULL);

	HFONT labelFont = CreateFont(25, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(labelHwnd, WM_SETFONT, WPARAM(labelFont), TRUE);

	// height label
	DWORD heightLabelStyle = WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE;
	int heightLabelWidth = WINDOW_WIDTH;
	int heightLabelHeight = WINDOW_HEIGHT;
	heightLabelHwnd = CreateWindow("static", info,
		heightLabelStyle,
		0, 0, heightLabelWidth, heightLabelHeight,
		hwnd, (HMENU)HEIGHT_LABEL_ID, hInstance, NULL);

	HFONT heightLabelFont = CreateFont(50, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(heightLabelHwnd, WM_SETFONT, WPARAM(heightLabelFont), TRUE);

	ShowWindow(hwnd, showCmd);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HANDLE threadHandle;
	DWORD threadId;
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (msg)
	{
		case WM_CREATE:
		{
			threadHandle = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadId);
			WaitForSingleObject(threadHandle, INFINITE);
			CloseHandle(threadHandle);

			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);

			break;
		}
		case WM_PAINT:
		{
			hDC = BeginPaint(hWnd, &paintStruct);

			EndPaint(hWnd, &paintStruct);
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			int id = GetDlgCtrlID((HWND)lParam);
			if (id == HEIGHT_LABEL_ID || id == LABEL_ID)
			{
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(25, 25, 25)); //gray
				return (LRESULT)GetStockObject(NULL_BRUSH);
			}
			break;
		}
		default:
		{
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	return EXIT_SUCCESS;
}

DWORD WINAPI ThreadFunc(void*)
{
	//int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//memset(info, 0, sizeof(info));
	//_itoa_s(screenHeight, info, 10);

	ImportFunction InfoDLL;
	HINSTANCE hinstLib = LoadLibrary(TEXT("ScreenHeightDefinerDLL.dll"));
	InfoDLL = (ImportFunction)GetProcAddress(hinstLib, "Information");
	InfoDLL(info);

	FreeLibrary(hinstLib);

	SetWindowText(heightLabelHwnd, info);

	return EXIT_SUCCESS;
}
