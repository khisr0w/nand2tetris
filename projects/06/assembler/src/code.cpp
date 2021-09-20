/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/16/2021 1:25:26 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "code.h"

internal void
InitCodeTranslation(code_state *CodeState, parser_state *ParserState, char *FileName)
{
	CodeState->ParsedData = ParserState;
	CodeState->FileName = FileName;

	if(ParserState->ProgramCount > 0)
	{
		memory_index Size = (16+2) * ParserState->ProgramCount + 1;
		CodeState->BinaryDestSize = Size - 3;
		CodeState->BinaryDest = (char *)PlatformAllocMem(sizeof(char) * Size);
	}
}

inline char *
Dest(char *DestMnemonic)
{
	int32 Index = IndexInArray(DestMnemonic, Dest_Commands);
	return Dest_Jump_Commands_Binary[Index];
}

inline char *
Comp(char *CompMnemonic)
{
	int32 Index = IndexInArray(CompMnemonic, Comp_Commands);
	return Comp_Commands_Binary[Index];
}

inline char *
Jump(char *JumpMnemonic)
{
	int32 Index = IndexInArray(JumpMnemonic, Jump_Commands);
	return Dest_Jump_Commands_Binary[Index];
}

inline char *
ConsumeLine(code_state *CodeState)
{
	char *Base = CodeState->BinaryDest + CodeState->CurrentIndex;
	CodeState->CurrentIndex = CodeState->CurrentIndex + 16;
	CodeState->BinaryDest[CodeState->CurrentIndex++] = '\r';
	CodeState->BinaryDest[CodeState->CurrentIndex++] = '\n';
	for(int32 Index = 0; Index < 16; ++Index) Base[Index] = '0';

	return Base;
}

internal void
CodeTranslate(code_state *CodeState, symbol_state *SymbolState)
{
	Assert(CodeState->BinaryDest);

	parser_state *ParserState = CodeState->ParsedData;
	for(; ParserState->CurrentCommand;
		Advance(ParserState))
	{
		switch(CommandType(ParserState))
		{
			case A_COMMAND:
			{
				char *CharValue = Symbol(ParserState);
				char *Dest = ConsumeLine(CodeState);

				// NOTE(Khisrow): Convert ASCII values of a decimal to its binary in ASCII
				Dest = Dest + 15;
				int32 IntValue;
				if(IsNumber(CharValue)) IntValue = ToInt(CharValue);
				else
				{
					symbol *Symbol = 0;
					if(Symbol = Contains(SymbolState, CharValue)) IntValue = Symbol->Value;
					else
					{
						MakeError(CodeState->Status, "\nERROR: Variable not found!\n");
						return;
					}
				}

				int32 BinLength = 0;
				for(; IntValue != 0;)
				{
					char Char = (IntValue & 0x1) + 48;
					*(Dest--) = Char;

					IntValue = IntValue >> 1;
					++BinLength;
				}
				if(BinLength > 15) 
				{
					MakeError(CodeState->Status, "\nERROR: The number exceed the memory limit of Hack machine\n");
					return;
				}
			} break;

			case C_COMMAND:
			{
				char *DestBinary = Dest(Dest(ParserState));
				char *CompBinary = Comp(Comp(ParserState));
				char *JumpBinary = Jump(Jump(ParserState));

				char *Destination = ConsumeLine(CodeState);
				*(Destination++) = '1';
				*(Destination++) = '1';
				*(Destination++) = '1';
				while(*CompBinary) *(Destination++) = *(CompBinary++);
				while(*DestBinary) *(Destination++) = *(DestBinary++);
				while(*JumpBinary) *(Destination++) = *(JumpBinary++);
			} break;

			default:
			{
				InvalidCodePath;
			}
		}
	}
}

inline void
AddHackExtension(char *FileName)
{
	int32 StrLen = StringLength(FileName);
	FileName[StrLen++] = '.';
	FileName[StrLen++] = 'h';
	FileName[StrLen++] = 'a';
	FileName[StrLen++] = 'c';
	FileName[StrLen++] = 'k';
	FileName[StrLen++] = '\0';
}

#if 0
inline wchar *
CreateWideString(char *String)
{
	Assert(String);
	wchar *Result = 0;
	if(String)
	{
		int32 StrLen = StringLength(String);
		Result = (wchar *)PlatformAllocMem(StrLen*2 + 2);

		for(int32 Index = 0;
			Index < StrLen;
			++Index)
		{
			Result[Index] = (wchar)(String[Index]);
		}
	}

	return Result;
}
#endif

internal void
CodeWriteToDisk(code_state *CodeState)
{
	AddHackExtension(CodeState->FileName);

	HANDLE FileHandle = CreateFileA(CodeState->FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
	if(FileHandle != INVALID_HANDLE_VALUE)
	{
        DWORD BytesWritten;
        if(WriteFile(FileHandle, CodeState->BinaryDest, CodeState->BinaryDestSize, &BytesWritten, 0))
		{
			if(BytesWritten != CodeState->BinaryDestSize)
			{
				MakeError(CodeState->Status, "ERROR: Binary file cannot be written\n");
			}
			CloseHandle(FileHandle);
		}
	}
	else MakeError(CodeState->Status, "ERROR: .hack file cannot be created, check permissions!\n");
}
