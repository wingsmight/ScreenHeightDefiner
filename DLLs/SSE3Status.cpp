#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern "C" __declspec(dllexport) int GetSSE3Status(char* info)
{
	int sse3Status = 0;

	_asm
	{
		mov eax, 0x1
		cpuid
		test edx, 1 << 0
		jz noSSE3
		; SSE3 is available
		mov sse3Status, 1

		noSSE3:
	}

	const char* sse3StatusString = sse3Status ? "yes" : "no";
	wsprintf(info, "%s", sse3StatusString);

	return 0;
}
