#ifndef F_PIXEL_H
#define F_PIXEL_H

#include <SupportDefs.h>

struct FPixel {
	uint8 b;
	uint8 g;
	uint8 r;
	uint8 a;
	
	operator int32() { return( *((int32*)this) ); }
};

#endif
