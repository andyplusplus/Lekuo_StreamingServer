/*
#include "rsPrintf.h"
*/	


#ifndef __RS_PRINT_TO_DEBUG__
#define __RS_PRINT_TO_DEBUG__

	int rsPrintf (	const char *format,	...	);
	int rsPrintf_Nothing (	const char *format,	...	);
	int printf_Nothing (	const char *format,	...	);
	int rsPrintPacketHead(unsigned char* inBufferLen, unsigned long inBufferSize);

#endif // __RS_PRINT_TO_DEBUG__