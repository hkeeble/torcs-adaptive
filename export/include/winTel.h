#ifndef _WINTEL_H_
#define _WINTEL_H_

#define WINTEL_IDENT 0

typedef void (*tfWinTelInit)();
typedef void (*tfWinTelAddData)();
typedef void (*tfWinTelUpdate)();

struct DrvProfile;
{
	
	// CONTAINS POINTERS TO DATA BEING WATCHED? OR CONTAINS DATA FROM LAST UPDATE?
}

typedef struct
{
	tfWinTelInit init;
	tfWinTelAddData addData;
	tfWinTelUpdate update;
} tWinTelItf;

#endif // WINTEL_H_