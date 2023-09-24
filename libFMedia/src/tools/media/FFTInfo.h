#ifndef F_FFTInfo_H
#define F_FFTInfo_H

#include <stdio.h>

struct FFTInfo {
	int32 BufferSize;
	enum {
		complex,
		unified
	} Format;
	float SamplingRate;
	float AnalysisOverlap;
	
	double Frequency( unsigned Index ) {
	    if ( Index >= BufferSize )
	        return 0.0;
	    else if ( Index <= BufferSize/2 )
	        return ((double)Index / (double)BufferSize)*SamplingRate;
	
	    return (-(double)(BufferSize-Index) / (double)BufferSize)*SamplingRate;
	}
	
	void Print() {
		printf("FFT: size %i, %s format, %.1f - %.1f Hz, %i%% Overlap\n",
			BufferSize, Format==unified?"unified":"complex",
			(float)Frequency(1), (float)Frequency((BufferSize/2)-1), AnalysisOverlap*100 );
	}
};

#endif
