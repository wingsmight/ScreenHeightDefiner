#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef int (*ImportFunction)(char*);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI ThreadFunc(void*);

char info[256];


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdStr, int showCmd)
{
	MSG msg{};
	HWND hwnd{};
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

	HDC screen = GetDC(NULL);
	DWORD windowStyle = WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX); //default window style without resizing and maximazing
	int windowWidth = 500;
	int windowHeight = 500;
	int windowPoxX = (GetDeviceCaps(screen, HORZRES) - windowWidth) / 2; //center position of the screen
	int windowPoxY = (GetDeviceCaps(screen, VERTRES) - windowHeight) / 2;
	hwnd = CreateWindow(wc.lpszClassName, "Screen height definer",
		windowStyle, windowPoxX, windowPoxY, windowWidth, windowHeight,
		NULL, NULL, wc.hInstance, NULL);

	if (hwnd == INVALID_HANDLE_VALUE)
	{
		return EXIT_FAILURE;
	}

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
			threadHandle = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &threadId);
			WaitForSingleObject(threadHandle, INFINITE);
			CloseHandle(threadHandle);
			break;

		case WM_DESTROY:
			PostQuitMessage(EXIT_SUCCESS);
			break;

		case WM_PAINT:
			hDC = BeginPaint(hWnd, &paintStruct);
			TextOut(hDC, 10, 10, info, strlen(info));
			EndPaint(hWnd, &paintStruct);
			break;

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

DWORD WINAPI ThreadFunc(void*)
{
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	memset(info, 0, sizeof(info));
	_itoa_s(screenHeight, info, 10);

	//ImportFunction InfoDLL;
	//HINSTANCE hinstLib = LoadLibrary(TEXT("info.dll"));
	//InfoDLL = (ImportFunction)GetProcAddress(hinstLib, "Information");
	//InfoDLL(info);

	//FreeLibrary(hinstLib);

	return EXIT_SUCCESS;
}
