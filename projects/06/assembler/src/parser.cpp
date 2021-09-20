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
		++ParserState->ProgramCount;
	}

	return Command;
}

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
Advance(parser_state *ParserState)
{
	if(ParserState->CurrentCommand) ParserState->CurrentCommand = ParserState->CurrentCommand->Next;
	else ParserState->CurrentCommand = ParserState->Commands;
}

inline bool32
IsVariableName(char *Value)
{
	return (((*Value >= 'a') && (*Value <= 'z'))  ||
			((*Value >= 'A') && (*Value <= 'Z')) ||
			(*Value == '_') || (*Value == '.')  ||
			(*Value == '$') || (*Value == ':'));
}

inline bool32
IsNumber(char *Value)
{
	int32 StrLen = StringLength(Value);
	for(int32 Index = 0;
		Index < StrLen;
		++Index)
	{
		if(!(Value[Index] >= '0' && Value[Index] <= '9')) return false;
	}

	return true;
}

internal void
ParsePseudoCommands(parser_state *ParserState, symbol_state *SymbolState)
{
	char *CurrentPos = ParserState->SourceStream->Base;
	int32 ProgramCount = ParserState->ProgramCount;

	while(*CurrentPos)
	{
		switch(*CurrentPos)
		{
			case '(':
			{
				++CurrentPos;
				char *Name = CurrentPos;
				while(*(++CurrentPos) != ')');
				*CurrentPos++ = '\0';
				if(*CurrentPos != '\n') while (*(++CurrentPos) != '\n');
				++CurrentPos;
				AddEntry(SymbolState, Name, ProgramCount);

			} break;

			case '/':
			{
				if(*(CurrentPos + 1) == '/')
				{
					while(*(++CurrentPos) != '\n');
					++CurrentPos;
				}
			} break;
			case ' ': while(*(++CurrentPos) == ' '); break;
			case '\r': CurrentPos = CurrentPos + 2; break;
			case '\n': ++CurrentPos; break;
			case '@':
			default:
			{
				while(*(++CurrentPos) != '\n');
				++CurrentPos;
				++ProgramCount;
			}
		}
	}
}

internal void
ParseCommandsAndSymbolize(parser_state *ParserState, symbol_state *SymbolState)
{
	// PlatformStdOut(ParserState->SourceStream->Base);
	ParserState->CurrentPos = ParserState->SourceStream->Base;

	while(*ParserState->CurrentPos && !(*ParserState->Status->Error))
	{
		if(ParserState->ProgramCount == 4523)
		{
			int i = 0;
		}
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

				if(IsVariableName((char *)Command->Value))
				{
					if(!Contains(SymbolState, (char *)Command->Value))
					{
						AddEntry(SymbolState, (char *)Command->Value, SymbolState->CurrentVarRAM++);
					}
				}
				else if(!IsNumber((char *)Command->Value))
				{
					MakeError(ParserState->Status, "\nERROR: Invalid variable name, must begin with letters, '.', '_', '$', ':'\n");
				}
				++ParserState->CurrentPos;
			} break;

			case '(':
			{
				while(*(++ParserState->CurrentPos) != '\0');
				++ParserState->CurrentPos;
				if(*ParserState->CurrentPos != '\n') while (*(++ParserState->CurrentPos) != '\n');
				++ParserState->CurrentPos;

			} break;

			case ' ': while(*(++ParserState->CurrentPos) == ' '); break;
			case '\r': ParserState->CurrentPos = ParserState->CurrentPos + 2; break;
			case '\n': ++ParserState->CurrentPos; break;
			default:
			{
				char *Dest = "NULL";
				char *Comp = "NULL";
				char *Jump = "NULL";

				int32 EqIdx = SpliceCodeLine(ParserState, '=', &Dest, &Comp);
				// NOTE(Khisrow): In case no '=' was found and all values were assigned
				// to the Dest while Comp becomes null, the values must be swapped.
				if(StringCompare(Comp, "NULL"))
				{
					Comp = Dest;
					Dest = "NULL";
				}

				int32 SemiIdx = SpliceString(Comp, ';', &Comp, &Jump);

				if(IsStringInArray(Comp, Comp_Commands))
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
					else MakeError(ParserState->Status, "\nSYNTAX ERROR: Either Dest or Jump command must accompany Comp\n");
				}
				else MakeError(ParserState->Status, "\nSYNTAX ERROR: Comp command not found\n");
			}
		}
	}
	Advance(ParserState);
}

inline void
Parse(parser_state *ParserState, symbol_state *SymbolState)
{
	ParsePseudoCommands(ParserState, SymbolState);
	ParseCommandsAndSymbolize(ParserState, SymbolState);
}

internal bool32
HasMoreCommands(parser_state *ParserState)
{
	return ParserState->CurrentCommand != ParserState->Commands;
}

internal command_type
CommandType(parser_state *ParserState)
{
	return ParserState->CurrentCommand->Type;
}

// NOTE(Khisrow): Should be called only if command type is L_COMMAND or A_COMMAND
internal char *
Symbol(parser_state *ParserState)
{
	return (char *)ParserState->CurrentCommand->Value;
}

// NOTE (Khisrow): Dest,Comp, and Jump called only when command type is C_COMMAND
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
