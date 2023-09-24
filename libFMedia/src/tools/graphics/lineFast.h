#ifndef F_lineFast_H
#define F_lineFast_H

#include <SupportDefs.h>

#ifdef __cplusplus
	extern "C" {
#endif

void lineFast(int x0, int y0, int x1, int y1, uint32 *bits, int width, int height, uint32 value);
void lineFastOR(int x0, int y0, int x1, int y1, uint32 *bits, int width, int height, uint32 value);

#ifdef __cplusplus
	} // extern "C"
#endif

#endif