/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/12/2021 3:13:57 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(COMMON_CPP)

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
	{
		while(*(String++)) ++Result;
	}
	
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

inline void
CopyToString(char *Source, char *Dest, int32 DestListLength)
{
	int32 SourceLength = StringLength(Source);
	int32 DestLength = DestListLength;
	if(DestListLength == -1)
	{
		DestLength = StringLength(Dest);
	}

	for(int32 Index = 0;
		Index < DestListLength;
		++Index)
	{
		*Dest++ = *Source++;
	}
	*Dest = '\0';
}

inline real32
Power(real32 Base, int32 Power)
{
	real32 Result = Base;

	if (Power == 0) return 1;
	if(Base == 0) return 0;

	while(--Power > 0)
	{
		Result *= Base;
	}

	return Result;
}

internal int32
ToInt(char *String)
{
	int32 Result = 0;

	int32 Length = StringLength(String);
	if(Length > 0)
	{
		int32 Value = 0;
		int32 Sign = 1;
		if((int32)(*String - 45) == 0)
		{
			Sign = -1;
			--Length;
			++String;
		}

		for(int32 RaisedAmount = Length - 1;
			RaisedAmount >= 0;
			--RaisedAmount)
		{
			int32 Integer = (int32)((*String++) - 48);
			if((Integer >= 0) && (Integer <= 9))
			{
				Integer *= Power(10, RaisedAmount);
				Value += Integer;
			}
		}

		Value *= Sign;
		Result = Value;
	}

	return Result;
}

struct op_status
{
	char Error[128];
};
#define MakeErrorAndExit(Status, ErrorString) MakeError(Status, ErrorString); return;
internal void
MakeError(op_status *Status, char * ErrorString)
{
	if(*Status->Error) return;

	int32 StringLen = StringLength(ErrorString);
	int32 Index = 0;
	for (; Index < StringLen;
		 ++Index)
	{
		Status->Error[Index] = *(ErrorString + Index);
	}
	Status->Error[Index+1] = '\0';
}

inline int32
IntLength(int32 Value)
{
	int32 Length = 0;
	while(Value)
	{
		int32 Remain = Value % 10;
		Value = (int32)((Value - Remain) * 0.1f);
		++Length;
	}

	return Length;
}

inline char *
ToString(int32 Value, char *String, int32 *StartIndex = 0, char EndChar = '\0')
{
	Assert(String);

	char Reverse[MAX_STRING];
	int32 *StrIndex = 0;
	int32 Temp = 0;

	if(StartIndex) StrIndex = StartIndex;
	else StrIndex = &Temp;

	if(Value == 0)
	{
		String[(*StrIndex)++] = '0';
		String[(*StrIndex)++] = EndChar;
		return String;
	}

	if(Value < 0)
	{
		String[(*StrIndex)++] = '-';
		Value *= -1;
	}

	int32 RevIndex = 0;
	while(Value)
	{
		int32 Remain = Value % 10;
		Reverse[RevIndex++] = Remain + 48;
		Value = (int32)((Value - Remain) * 0.1f);
	}

	for(; RevIndex > 0;) String[(*StrIndex)++] = Reverse[--RevIndex];

	String[(*StrIndex)++] = EndChar;

	return String;
}

#define IndexInArray(String, Array) IndexInArray_(String, Array, ArrayCount(Array))
internal int32
IndexInArray_(char *String, char **Array, int32 ArrayLength)
{
	for(int32 Index = 0;
		Index < ArrayLength;
		++Index)
	{
		if(StringCompare(String, Array[Index])) return Index;
	}

	return -1;
}

#define COMMON_CPP
#endif
