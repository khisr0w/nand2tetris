/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/19/2021 3:13:47 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(SYMBOL_H)

char *Predefined_Symbols_Name[] =
{
	"SP",
	"LCL",
	"ARG",
	"THIS",
	"THAT",
	"R0",
	"R1",
	"R2",
	"R3",
	"R4",
	"R5",
	"R6",
	"R7",
	"R8",
	"R9",
	"R10",
	"R11",
	"R12",
	"R13",
	"R14",
	"R15",
	"SCREEN",
	"KBD",
};

int32 Predefined_Symbols_Value[] =
{
	0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16384, 24576,
};

struct symbol
{
	char *Name;
	int32 Value;

	symbol *Next;
};

struct symbol_state
{
	symbol SymbolTable[2048];

	memory_index CurrentVarRAM;
	memory_index *ProgramCount;

	op_status *Status;
};

#define SYMBOL_H
#endif
