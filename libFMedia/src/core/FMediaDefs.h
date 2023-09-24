#ifndef F_MEDIA_DEFS_H
#define F_MEDIA_DEFS_H

#include <MediaDefs.h>

#define B_MEDIA_ANY_TYPE B_MEDIA_UNKNOWN_TYPE

#define F_DEFAULT_BUFFER_SIZE					32768
#define F_INITIAL_LATENCY						20000
#define F_INITIAL_LATENCY_TOLERANCE 			1000
#define F_FASTER_LATENCY_TOLERANCE_FACTOR		0.7
#define F_SLOWER_LATENCY_TOLERANCE_FACTOR		0.1
#define F_MIN_LATENCY_TOLERANCE		 			5000

#define F_META_HEADER_TYPE						'FmtH'
#define F_DIFFERENCE_TOTAL						'FdfT'

#define F_BUFFER_RECYCLED	-0xf3c1


// own media types

enum f_media_type {
	F_MEDIA_FIRST_TYPE = B_MEDIA_FIRST_USER_TYPE + 0xf3c,
	F_MEDIA_FFT_DATA = F_MEDIA_FIRST_TYPE,
	F_MEDIA_CONTROL_DATA
};

// return codes for high-level Filter functions
enum f_filter_status {
	F_NOTHING_DONE = -2,
	F_ERROR = -1,
	F_DONE = 0,
	F_DONE_BUT_DONT_COUNT_LATENCY = 1
};

#endif
