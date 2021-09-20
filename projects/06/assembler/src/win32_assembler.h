/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/6/2021 1:06:58 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(WIN32_ASSEMBLER_H)

inline void *
PlatformAllocMem(memory_index Size, HANDLE *HeapHandle = 0)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size);
}

inline bool32
PlatformFreeMem(void *Memory, HANDLE *HeapHandle = 0)
{
	return HeapFree(GetProcessHeap(), 0, Memory);
}

internal void
PlatformStdOut(char *Text)
{
	WriteConsole(GLOBALConsoleOutputHandle, Text, StringLength(Text), NULL, NULL);
}

#define WIN32_ASSEMBLER_H
#endif
