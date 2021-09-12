/*  +======| File Info |===============================================================+
    |                                                                                  |
    |     Subdirectory:  /src                                                          |
    |    Creation date:  9/12/2021 3:13:57 AM                                          |
    |    Last Modified:                                                                |
    |                                                                                  |
    +=====================| Sayed Abid Hashimi, Copyright © All rights reserved |======+  */

#if !defined(COMMON_CPP)

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

#define COMMON_CPP
#endif
