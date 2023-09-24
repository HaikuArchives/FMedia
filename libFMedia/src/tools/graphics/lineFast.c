#include "lineFast.h"

void lineFast(int x0, int y0, int x1, int y1, uint32 *bits, int width, int height, uint32 value) 
{ 
    uint32 pix = value; 
    int dy = y1 - y0; 
    int dx = x1 - x0; 
    int stepx, stepy; 

    if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; } 
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; } 
    dy <<= 1; 
    dx <<= 1; 

    y0 *= width; 
    y1 *= width; 
    bits[x0+y0] = pix; 
    if (dx > dy) { 
        int fraction = dy - (dx >> 1); 
        while (x0 != x1) { 
            if (fraction >= 0) { 
                y0 += stepy; 
                fraction -= dx; 
            } 
            x0 += stepx; 
            fraction += dy; 
            bits[x0+y0] = pix; 
        } 
    } else { 
        int fraction = dx - (dy >> 1); 
        while (y0 != y1) { 
            if (fraction >= 0) { 
                x0 += stepx; 
                fraction -= dy; 
            } 
            y0 += stepy; 
            fraction += dx; 
            bits[x0+y0] = pix; 
        } 
    } 
} 

void lineFastOR(int x0, int y0, int x1, int y1, uint32 *bits, int width, int height, uint32 value) 
{ 
    uint32 pix = value; 
    int dy = y1 - y0; 
    int dx = x1 - x0; 
    int stepx, stepy; 

    if (dy < 0) { dy = -dy;  stepy = -width; } else { stepy = width; } 
    if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; } 
    dy <<= 1; 
    dx <<= 1; 

    y0 *= width; 
    y1 *= width; 
    bits[x0+y0] |= pix; 
    if (dx > dy) { 
        int fraction = dy - (dx >> 1); 
        while (x0 != x1) { 
            if (fraction >= 0) { 
                y0 += stepy; 
                fraction -= dx; 
            } 
            x0 += stepx; 
            fraction += dy; 
            bits[x0+y0] |= pix; 
        } 
    } else { 
        int fraction = dx - (dy >> 1); 
        while (y0 != y1) { 
            if (fraction >= 0) { 
                x0 += stepx; 
                fraction -= dy; 
            } 
            y0 += stepy; 
            fraction += dx; 
            bits[x0+y0] |= pix; 
        } 
    } 
} 