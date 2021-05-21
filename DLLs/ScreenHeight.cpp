#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" __declspec(dllexport) int GetScreenHeight(char* info)
{
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	wsprintf(info, "%d", screenHeight);

	return 0;
}
