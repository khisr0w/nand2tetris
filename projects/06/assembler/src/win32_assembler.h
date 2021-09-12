/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/6/2021 1:06:58 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(WIN32_ASSEMBLER_H)

#if DEBUG
#define Assert(Expression) if(!(Expression)) {*(int*)0 = 0;}
#define InvalidCodePath Assert(!"InvalidCodePath")
#else
#define Assert(Expression)
#endif

#define ArrayCount(Array) (sizeof((Array)) / sizeof((Array)[0]))

inline uint32
SafeTruncateUInt64 (uint64 Value)
{
	Assert(Value <= 0xFFFFFFFF);
	return (uint32)Value;
}

inline int32
StringLength(char *String)
{
	int Result = 0;

	if(String)
		while(*String++) { ++Result; }
	
	return Result;
}

internal bool32
StringCompare(char * A, char *B)
{
	if(A && B)
	{
		int32 ALen = StringLength(A);
		int32 BLen = StringLength(B);
		if((ALen + BLen > 0) && (ALen != BLen)) { return false; }

		for(int32 Index = 0;
			Index < ALen;
			++Index)
		{
			if(*A++ != *B++) { return false; }
		}

		return true;
	}

	return false;
}

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

struct op_status
{
	char Error[128];
};

#define WIN32_ASSEMBLER_H
#endif
