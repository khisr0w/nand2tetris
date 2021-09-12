/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /assembler                                                    |
    |    Creation date:  9/6/2021 12:18:06 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

/*
   This file here is the win32 version of the HACK machine assembler
   as studied in the nand2tetris.
 */

#include "windows.h"
#define internal static

typedef int int32;
typedef unsigned char uint8;
typedef unsigned int uint32;
typedef int bool32;
typedef float real32;
typedef unsigned long long memory_index;
typedef unsigned long long uint64;

HANDLE GLOBALConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE GLOBALConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);

#include "win32_assembler.h"
op_status GLOBALOpStatus = {};

#include "common.cpp"

// NOTE(Khisrow): Unity Build
#include "parser.cpp"

internal source_stream
Win32GetSource(char *FileName)
{
	source_stream SourceData = {};

	LARGE_INTEGER FileSize = {};

	HANDLE FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
		if(GetFileSizeEx(FileHandle, &FileSize))
		{
			int32 FileSize32 = SafeTruncateUInt64(FileSize.QuadPart);
			SourceData.Base = (char *)PlatformAllocMem(FileSize32 + 1); // NOTE(Khisrow): 0 at the end
			if(SourceData.Base)
			{
				DWORD BytesRead = 0;
				if(ReadFile(FileHandle, SourceData.Base, FileSize32, &BytesRead, 0) &&
				   (BytesRead == FileSize32))
				{
					SourceData.Size = FileSize32;
					((char *)SourceData.Base)[FileSize32] = '\0';
				}
				else PlatformFreeMem(SourceData.Base);
			}
			else {}//TODO(Khisrow): LOG!!!
		}
		else {}//TODO(Khisrow): LOG!!!
	}
	else {}//TODO(Khisrow): LOG!!!

	CloseHandle(FileHandle);

	return SourceData;
}

int main(int argc, char *argv[])
{
	SetConsoleTitle("Nand2Tetris Assembler");

	source_stream SourceStream = {};
	parser_state ParserState = {};
	ParserState.Status = &GLOBALOpStatus;

	if(argc > 1)
	{
		SourceStream = Win32GetSource(argv[1]);
		ParserState.SourceStream = &SourceStream;
		Parse(&ParserState);
	}
#if 0
	else PlatformStdOut("Please provide a Hack assembly file\n");
#else
	else
	{
		SourceStream = Win32GetSource( "../example.asm");
		ParserState.SourceStream = &SourceStream;
		Parse(&ParserState);

		if(GLOBALOpStatus.Error)
		{
			PlatformStdOut(GLOBALOpStatus.Error);
		}
	}
#endif

	return 0;
}
