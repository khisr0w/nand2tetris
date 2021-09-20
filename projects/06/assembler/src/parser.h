/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/6/2021 5:09:30 AM                                           |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(PARSER_H)

enum command_type
{
	NUL,
	A_COMMAND,
	C_COMMAND,
	L_COMMAND,
};

char *Comp_Commands[] =
{
	// NOTE(Khisrow): When a = 0
	"0", "1", "-1", "D", "A", "!D", "!A", "-D", "-A", "D+1", "A+1", "D-1", "A-1", "D+A", "D-A", "A-D", "D&A", "D|A",
	// NOTE(Khisrow): When a = 1
	"M", "!M", "-M", "M+1", "M-1", "D+M", "D-M", "M-D", "D&M", "D|M",
};

char *Dest_Commands[] =
{
	"NULL",
	"M",
	"D",
	"MD",
	"A",
	"AM",
	"AD",
	"AMD"
};

char *Jump_Commands[] =
{
	"NULL",
	"JGT",
	"JEQ",
	"JGE",
	"JLT",
	"JNE",
	"JLE",
	"JMP",
};

struct source_stream
{
	char *Base;
	size_t Size;
};

struct command
{
	command_type Type;
	void *Value;

	command *Next;
};

struct c_command_values
{
	char *Comp;
	char *Dest;
	char *Jump;
};

struct parser_state
{
	source_stream *SourceStream;
	char *CurrentPos;
	command *CurrentCommand;

	command *Commands;
	command *LastCommand;
	// NOTE(Khisrow): L_COMMAND doesn't increment the CommandLength
	// as it's a pseudo-command and won't invoke any machine code
	memory_index ProgramCount;

	op_status *Status;
};

#define PARSER_H
#endif
