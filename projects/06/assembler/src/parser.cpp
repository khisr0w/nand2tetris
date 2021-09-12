/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/6/2021 5:06:17 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "parser.h"

internal command *
AppendCommand(parser_state *ParserState, command_type Type)
{
	int32 ValueSize = Type == C_COMMAND ? sizeof(c_command_values) : 0;
	command *Command = (command *)PlatformAllocMem(sizeof(command) + ValueSize);
	Assert(Command);
	if(Command)
	{
		if(ParserState->LastCommand) ParserState->LastCommand->Next = Command;
		else ParserState->Commands = Command;
		ParserState->LastCommand = Command;
		Command->Type = Type;
		if(Command->Type == C_COMMAND) Command->Value = (Command+1);
		++ParserState->CommandLength;
	}

	return Command;
}

#if 0
internal bool32
IsAlphaNumeric(char *CurrentPos)
{
	char *Start = CurrentPos;
	while(*Start != ' '  &&
		  *Start != '\r' &&
		  *Start != '\n' &&
		  *Start != '/')
	{
		Start++;
	}
}
#endif

#define IsStringInArray(String, Array) IsStringInArray_(String, Array, ArrayCount(Array))
internal bool32
IsStringInArray_(char *String, char **Array, int32 ArrayLength)
{
	for(int32 Index = 0;
		Index < ArrayLength;
		++Index)
	{
		if(StringCompare(String, Array[Index])) return true;
	}

	return false;
}

internal int32
SpliceString(char *Source, char Delimit, char **Dest1, char **Dest2)
{
	*Dest1 = Source;

	int32 IndexOfSlice = 0;
	for(; *Source;
		++Source, ++IndexOfSlice)
	{
		if(*Source == Delimit)
		{
			*Source = '\0';
			*Dest2 = ++Source;
			break;
		}
	}

	return IndexOfSlice;
}

internal int32
SpliceCodeLine(parser_state *ParserState, char Delimit, char **Dest1, char **Dest2)
{
	char *Source = ParserState->CurrentPos;
	char *InitPointer = Source;

	//NOTE(Khisrow): Find endpoint of the line
	int32 EndLength = 0;
	for(; *Source && (*Source != '\r' && *Source != '\n' && *Source != ' ');
		++Source, ++EndLength)
	{
		if(*Source == '/') if(*Source == '/') break;
	}

	if(EndLength && *Source)
	{
		if(*Source == '\n')
		{
			*Source = '\0';
			++Source;
		}
		else
		{
			*Source = '\0';
			++Source;
			for(; *Source && (*Source != '\r' && *Source != '\n');
				++Source);
			if(*Source == '\r') Source = Source + 2;
			if(*Source == '\n') ++Source;
		}
		ParserState->CurrentPos = Source;
	}

	int32 Index = SpliceString(InitPointer, Delimit, Dest1, Dest2);

	return Index;
}

// NOTE(Khisrow): It will check for the first occurrence of the char
// in a string until the newline. If none found, -1 will be returned.
internal int32
GetIndexOfCharInStringBeforeNL(char *String, char Delimit)
{
	for(int32 Index = 0;
		*String && (*String != '\r' || *String != '\n');
		++String, ++Index)
	{
		if(*String == '/') if(*(String+1) == '/') return -1;
		if((*String) == Delimit) return Index;
	}

	return -1;
}

internal void
Parse(parser_state *ParserState)
{
	PlatformStdOut(ParserState->SourceStream->Base);
	ParserState->CurrentPos = ParserState->SourceStream->Base;
	int32 Index = GetIndexOfCharInStringBeforeNL(ParserState->CurrentPos, '=');

	while(*ParserState->CurrentPos && !(*ParserState->Status->Error))
	{
		switch(*ParserState->CurrentPos)
		{
			case '/':
			{
				if(*(ParserState->CurrentPos + 1) == '/')
				{
					while(*(++ParserState->CurrentPos) != '\n');
					++ParserState->CurrentPos;
				}
			} break;

			case '@':
			{
				++ParserState->CurrentPos;
				command *Command = AppendCommand(ParserState, A_COMMAND);
				Command->Value = ParserState->CurrentPos;

				while(*ParserState->CurrentPos != ' '  &&
					  *ParserState->CurrentPos != '\r' &&
					  *ParserState->CurrentPos != '\n' &&
					  *ParserState->CurrentPos != '/')
				{
					++ParserState->CurrentPos;
				}

				if(*ParserState->CurrentPos != '\n')
				{
					*ParserState->CurrentPos = '\0';
					while (*(++ParserState->CurrentPos) != '\n');
				}
				else *ParserState->CurrentPos = '\0';
				++ParserState->CurrentPos;
			} break;

			case '(':
			{
				++ParserState->CurrentPos;
				command *Command = AppendCommand(ParserState, L_COMMAND);
				Command->Value = ParserState->CurrentPos;

				while(*(++ParserState->CurrentPos) != ')') ++ParserState->CurrentPos;
				*ParserState->CurrentPos = '\0';
				++ParserState->CurrentPos;
				if(*ParserState->CurrentPos != '\n') while (*(++ParserState->CurrentPos) != '\n');
				++ParserState->CurrentPos;

			} break;

			case ' ': while(*(++ParserState->CurrentPos) == ' '); break;
			case '\r': ParserState->CurrentPos = ParserState->CurrentPos + 2; break;
			case '\n': ++ParserState->CurrentPos; break;
			default:
			{
				char *Dest = 0;
				char *Comp = 0;
				char *Jump = 0;

				int32 EqIdx = SpliceCodeLine(ParserState, '=', &Dest, &Comp);
				// NOTE(Khisrow): In case no '=' was found and all values were assigned
				// to the Dest while Comp becomes null
				if(!Comp)
				{
					Comp = Dest;
					Dest = 0;
				}

				int32 SemiIdx = SpliceString(Comp, ';', &Comp, &Jump);

				if(Comp && IsStringInArray(Comp, Comp_Commands))
				{
					// NOTE(Khisrow): Apart from Comp, either Dest, Jump or both should
					// be in the assembly code.
					bool32 Predicate = false;
					if(Dest && IsStringInArray(Dest, Dest_Commands)) Predicate = true;
					if(Jump && IsStringInArray(Jump, Jump_Commands)) Predicate = true;

					if(Predicate)
					{
						command *Command = AppendCommand(ParserState, C_COMMAND);
						c_command_values *CValues = (c_command_values *)(Command + 1);
						CValues->Comp = Comp;
						CValues->Dest = Dest;
						CValues->Jump = Jump;
					}
					else MakeError(ParserState->Status, "SYNTAX ERROR: Either Dest or Jump command must accompany Comp");
				}
				else MakeError(ParserState->Status, "SYNTAX ERROR: Comp command not found");
			}
		}
	}
}

internal bool32
HasMoreCommands(parser_state *ParserState)
{
	return ParserState->CurrentCommand != ParserState->Commands;
}

internal void
Advance(parser_state *ParserState)
{
	if(ParserState->CurrentCommand) ParserState->CurrentCommand = ParserState->CurrentCommand->Next;
	else ParserState->CurrentCommand = ParserState->Commands;
}

internal command_type
CommandType(parser_state *ParserState)
{
	return ParserState->CurrentCommand->Type;
}

internal char *
Symbol(parser_state *ParserState)
{
	return (char *)ParserState->CurrentCommand->Value;
}

internal char *
Dest(parser_state *ParserState)
{
	return ((c_command_values *)ParserState->CurrentCommand->Value)->Dest;
}

internal char *
Comp(parser_state *ParserState)
{
	return ((c_command_values *)ParserState->CurrentCommand->Value)->Comp;
}

internal char *
Jump(parser_state *ParserState)
{
	return ((c_command_values *)ParserState->CurrentCommand->Value)->Jump;
}
