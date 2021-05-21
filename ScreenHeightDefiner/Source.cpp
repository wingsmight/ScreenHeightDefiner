#include <windows.h>
#include <stdio.h>
#include <string.h>


typedef int (*ImportFunction)(char*);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadFunc(void*);
void DrawHeightLabel(HINSTANCE hInstance);
void DrawHeightValue(HINSTANCE hInstance);
void DrawSSE3SupportLabel(HINSTANCE hInstance);
void DrawSSE3SupporValue(HINSTANCE hInstance);


const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 300;

const int LABEL_LABEL_ID = 1;
const int HEIGHT_VALUE_ID = 2;
const int SSE3_SUPPORT_LABEL_ID = 3;
const int SSE3_SUPPORT_VALUE_ID = 4;


char screenHeightString[256];
char sse3SupportString[10];
HWND hwnd;
HWND heightValueHwnd;
HWND sse3SupportValueHwnd;


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

	DrawHeightLabel(hInstance);
	DrawHeightValue(hInstance);

	DrawSSE3SupportLabel(hInstance);
	DrawSSE3SupporValue(hInstance);

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

			SetWindowText(heightValueHwnd, screenHeightString);
			SetWindowText(sse3SupportValueHwnd, sse3SupportString);

			EndPaint(hWnd, &paintStruct);
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			int id = GetDlgCtrlID((HWND)lParam);
			if (id == HEIGHT_VALUE_ID || id == LABEL_LABEL_ID)
			{
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(50, 50, 50)); // gray text
				return (LRESULT)GetStockObject(NULL_BRUSH);
			}
			else if (id == SSE3_SUPPORT_LABEL_ID || id == SSE3_SUPPORT_VALUE_ID)
			{
				HDC hdcStatic = (HDC)wParam;
				SetTextColor(hdcStatic, RGB(255, 255, 255)); // white text
				SetBkMode(hdcStatic, TRANSPARENT); // remove text background
				return (LRESULT)GetStockObject(DKGRAY_BRUSH);
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
	HINSTANCE screenHeightLib = LoadLibrary(TEXT("ScreenHeight.dll"));
	if (screenHeightLib != NULL)
	{
		ImportFunction dllFunc = (ImportFunction)GetProcAddress(screenHeightLib, "GetScreenHeight");
		dllFunc(screenHeightString);

		FreeLibrary(screenHeightLib);
	}
	else
	{
		MessageBox(hwnd, "Cannot load DLL!", "Error!", MB_OK | MB_ICONERROR);

		ExitProcess(EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	HINSTANCE sse3SupportLib = LoadLibrary(TEXT("SSE3Status.dll"));
	if (sse3SupportLib != NULL)
	{
		ImportFunction dllFunc = (ImportFunction)GetProcAddress(sse3SupportLib, "GetSSE3Status");
		dllFunc(sse3SupportString);

		FreeLibrary(sse3SupportLib);
	}
	else
	{
		MessageBox(hwnd, "Cannot load DLL!", "Error!", MB_OK | MB_ICONERROR);

		ExitProcess(EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void DrawHeightLabel(HINSTANCE hInstance)
{
	DWORD style = WS_CHILD | WS_VISIBLE | SS_CENTER;
	int width = WINDOW_WIDTH;
	int height = 100;
	HWND heightLabelHwnd = CreateWindow("static", "Screen height:",
		style,
		0, 0, width, height,
		hwnd, (HMENU)LABEL_LABEL_ID, hInstance, NULL);

	HFONT font = CreateFont(25, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(heightLabelHwnd, WM_SETFONT, WPARAM(font), TRUE);
}
void DrawHeightValue(HINSTANCE hInstance)
{
	DWORD style = WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE;
	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT / 2;
	heightValueHwnd = CreateWindow("static", screenHeightString,
		style,
		0, 0, width, height,
		hwnd, (HMENU)HEIGHT_VALUE_ID, hInstance, NULL);

	HFONT font = CreateFont(50, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(heightValueHwnd, WM_SETFONT, WPARAM(font), TRUE);
}
void DrawSSE3SupportLabel(HINSTANCE hInstance)
{
	int bottomOffset = 28;

	DWORD style = WS_CHILD | WS_VISIBLE | SS_CENTER;
	int posX = 0;
	int posY = WINDOW_HEIGHT / 2 - bottomOffset;
	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT / 2 - bottomOffset;
	HWND sse3SupportLabelHwnd = CreateWindow("static", "SSE3 support:",
		style,
		posX, posY, width, height,
		hwnd, (HMENU)SSE3_SUPPORT_LABEL_ID, hInstance, NULL);

	HFONT font = CreateFont(25, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(sse3SupportLabelHwnd, WM_SETFONT, WPARAM(font), TRUE);
}
void DrawSSE3SupporValue(HINSTANCE hInstance)
{
	int bottomOffset = 28;

	DWORD style = WS_CHILD | WS_VISIBLE | SS_CENTER;
	int posX = 0;
	int posY = WINDOW_HEIGHT * (0.75) - bottomOffset;
	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT / 2 - bottomOffset;
	sse3SupportValueHwnd = CreateWindow("static", "1",
		style,
		posX, posY, width, height,
		hwnd, (HMENU)SSE3_SUPPORT_LABEL_ID, hInstance, NULL);

	HFONT font = CreateFont(50, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	SendMessage(sse3SupportValueHwnd, WM_SETFONT, WPARAM(font), TRUE);
}
