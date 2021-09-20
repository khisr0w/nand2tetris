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
typedef long long int64;
typedef wchar_t wchar;

#define MAX_STRING 128

HANDLE GLOBALConsoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
HANDLE GLOBALConsoleInputHandle = GetStdHandle(STD_INPUT_HANDLE);

#include "common.cpp"
#include "win32_assembler.h"
op_status GLOBALOpStatus = {};


// NOTE(Khisrow): Unity Build
#include "symbol.cpp"
#include "parser.cpp"
#include "code.cpp"

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

// WARNING (Khisrow): This is faulty TODO must be checked!
struct asm_file_info
{
	char *FileExtension;
	char *FileName;
};
internal asm_file_info
AssemblyFileProperties(char *FileName)
{
	asm_file_info FileProperties = {};
	int32 StrLen = StringLength(FileName) - 1;
	int32 DotIndex;
	for(DotIndex = StrLen;
		DotIndex >= 0;
		--DotIndex)
	{
		if(FileName[DotIndex] == '.')
		{
			FileProperties.FileExtension = FileName+DotIndex+1;
			break;
		}
	}
	int32 AfterLastSlashIndex = 0;
	for(int32 Index = 0;
		Index < StrLen;
		++Index)
	{
		if(FileName[Index] == '\\' || FileName[Index] == '/') AfterLastSlashIndex = Index + 1;
	}

	int32 SubStrLen = DotIndex - AfterLastSlashIndex;
	FileName = FileName + AfterLastSlashIndex;
	// NOTE(Khisrow): Extra 5 bits are allocated for the file name so that it will be
	// used for supplanting the .hack extension upon writing it.
	FileProperties.FileName = (char *)PlatformAllocMem(SubStrLen*sizeof(char)+1+5);
	CopyToString(FileName, FileProperties.FileName, SubStrLen);

	return FileProperties;
}

int main(int argc, char *argv[])
{
	SetConsoleTitle("Nand2Tetris(Hack) Assembler");

	source_stream SourceStream = {};

	parser_state ParserState = {};
	ParserState.Status = &GLOBALOpStatus;

	symbol_state SymbolState = {};
	SymbolState.Status = &GLOBALOpStatus;

	code_state CodeState = {};
	CodeState.Status = &GLOBALOpStatus;
	
	char *FileName = 0;
	if(argc > 1) FileName = argv[1];
#if 1
	else
	{
		PlatformStdOut("Please provide a .asm file for translation\n");
		return 0;
	}
#else
	else FileName = "../Pong.asm";
#endif

	asm_file_info FileProperties = AssemblyFileProperties(FileName);
	if(StringCompare(FileProperties.FileExtension, "asm"))
	{
		SourceStream = Win32GetSource(FileName);
		if(SourceStream.Base)
		{
			InitSymbolTable(&SymbolState, &ParserState.ProgramCount);
			ParserState.SourceStream = &SourceStream;
			Parse(&ParserState, &SymbolState);

			if(*GLOBALOpStatus.Error)
			{
				PlatformStdOut(GLOBALOpStatus.Error);
				return 0;
			}

			InitCodeTranslation(&CodeState, &ParserState, FileProperties.FileName);
			CodeTranslate(&CodeState, &SymbolState);
			if(*GLOBALOpStatus.Error)
			{
				PlatformStdOut(GLOBALOpStatus.Error);
				return 0;
			}

			CodeWriteToDisk(&CodeState);
			if(*GLOBALOpStatus.Error)
			{
				PlatformStdOut(GLOBALOpStatus.Error);
				return 0;
			}
			PlatformStdOut("\nAssembly to binary conversion successful\n");
		}
		else PlatformStdOut("\nERROR: File was not found!\n");
	}
	else PlatformStdOut("\nERROR: Please provide a Hack assembly file with .asm extension\n");

	return 0;
}
