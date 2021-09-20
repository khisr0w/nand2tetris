/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/16/2021 2:09:09 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright � All rights reserved |======+  */

#if !defined(CODE_H)

char *Comp_Commands_Binary[] =
{
	// NOTE(Khisrow): When a = 0
	"0101010", // 0
	"0111111", // 1
	"0111010", // -1
	"0001100", // D
	"0110000", // A
	"0001101", // !D
	"0110001", // !A
	"0001111", // -D
	"0110011", // -A
	"0011111", // D+1
	"0110111", // A+1
	"0001110", // D-1
	"0110010", // A-1
	"0000010", // D+A
	"0010011", // D-A
	"0000111", // A-D
	"0000000", // D&A
	"0010101", // D|A

	// NOTE(Khisrow): When a = 1
	"1110000", // M
	"1110001", // !M
	"1110011", // -M
	"1110111", // M+1
	"1110010", // M-1
	"1000010", // D+M
	"1010011", // D-M
	"1000111", // M-D
	"1000000", // D&M
	"1010101", // D|M
};

char *Dest_Jump_Commands_Binary[] =
{
	"000", // NULL 
	"001", // M   - JGT
	"010", // D   - JEQ
	"011", // MD  - JGE
	"100", // A   - JLT
	"101", // AM  - JNE
	"110", // AD  - JLE
	"111", // AMD - JMP
};

struct code_state
{
	char * FileName;
	parser_state *ParsedData;

	char *BinaryDest;
	memory_index BinaryDestSize;
	int32 CurrentIndex;

	op_status *Status;
};

#define CODE_H
#endif
