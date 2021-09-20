/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/17/2021 5:23:45 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#include "symbol.h"

// TODO(Khisrow): Remove if condition for L_COMMAND from Command allocation instead
// decrement the CommandLength each time you encounter an L_COMMAND inside Symbol
// module.

// TODO(Khisrow): The obligatory "Better Hash Function" comment
// TODO WARNING(Khisrow): Fault-proof this, needs testing!
internal symbol *
GetHashFromSymbolTable(symbol_state *SymbolState, char *Name, bool32 New)
{
	Assert(Name);

	symbol *Result = 0;
	int64 HashValue = 0;
	int64 HashMask = ArrayCount(SymbolState->SymbolTable) - 1;
	int32 StrLen = StringLength(Name);

	int64 Multiplier = (StrLen + (*Name)%124) / 3;
	for(int32 Index = 0;
		Index < StrLen;
		++Index)
	{
		HashValue += Name[Index]*Multiplier;
		Multiplier = (Name[Index]*Multiplier)*137 / 3;
	}
	HashValue &= 0x7FFFFFFFFFFFFFFF;

	int32 HashIndex = HashValue & HashMask;
	Result = SymbolState->SymbolTable + HashIndex;
	do
	{
		if(New && !Result->Name) break;
		if(StringCompare(Result->Name, Name)) break;
		if(New && !Result->Next)
		{
			Result->Next = (symbol *)PlatformAllocMem(sizeof(symbol));
			Result = Result->Next;
			break;
		}
		Result = Result->Next;
	}
	while(Result);

	return Result;
}

internal symbol *
AddEntry(symbol_state *SymbolState, char *Name, int32 Value)
{
	symbol *Symbol = GetHashFromSymbolTable(SymbolState, Name, true);
	if(!Symbol->Name)
	{
		Symbol->Name = Name;
		Symbol->Value = Value;
	}

	return Symbol;
}

inline symbol *
Contains(symbol_state *SymbolState, char *Name)
{
	symbol *Symbol = GetHashFromSymbolTable(SymbolState, Name, false);
	return Symbol;
}

inline int32
GetAddress(symbol_state *SymbolState, char *Name)
{
	symbol *Symbol = Contains(SymbolState, Name);
	return Symbol->Value;
}

internal void
InitSymbolTable(symbol_state *SymbolState, memory_index *ProgramCount)
{
	for(int32 Index = 0;
		Index < ArrayCount(Predefined_Symbols_Name);
		++Index)
	{
		symbol *Symbol = GetHashFromSymbolTable(SymbolState,
												Predefined_Symbols_Name[Index], true);
		Symbol->Name = Predefined_Symbols_Name[Index];
		Symbol->Value = Predefined_Symbols_Value[Index];
	}

	SymbolState->CurrentVarRAM = 16;
	SymbolState->ProgramCount = ProgramCount;
}
