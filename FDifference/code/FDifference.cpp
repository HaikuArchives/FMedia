#include "FDifference.h"
#include <stdio.h>
#include <ParameterWeb.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf(ADDON_NAME": "); printf
#define FORMAT //printf

FDifference::FDifference( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL )
	: FFilter( FGetRGB32VideoFormat(), FGetRGB32VideoFormat(), name, addon ),
	  BMediaNode( name )
	  {
	  holdBuffer = NULL;
}

FDifference::~FDifference() {
}

void FDifference::Setup() {
	mFactor = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Amplification", B_GENERIC, "fold", 1.0, 10.0, 0.1, 1.0);
	AddParameter( mFactor );
	mThreshold = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Threshold", B_GENERIC, "h", 0.0, 255.0, 0.0, 0);
	AddParameter( mThreshold );

	mShow = new FDiscreteParameter( B_MEDIA_RAW_VIDEO, "Show", B_ENABLE, 0);
	AddParameter( mShow );
	mRough = new FDiscreteParameter( B_MEDIA_RAW_VIDEO, "Rough", B_ENABLE, 0);
	AddParameter( mRough );

	FFilter::Setup();
}

bool FDifference::Filter( BBuffer *inBuffer ) {
	if( !holdBuffer ) {
		holdBuffer = inBuffer;
		return false;
	}

	FPixel *inData = (FPixel *)inBuffer->Data();
	FPixel *holdData = (FPixel *)holdBuffer->Data();
	int32 nPixels = (inBuffer->SizeUsed()/sizeof(FPixel));

	//-----------------------
	float Factor = mFactor->Get();
	float Threshold = mThreshold->Get();

	bool show = mShow->Get() > 0;

	int step = 1;
	if( mRough->Get() ) step = 7;

	register float v, in, hold, total = 0;
	for( int32 i=0; i<nPixels; i+=step ) {
		in = inData[i].r;
		hold = holdData[i].r;
		v = ( Factor *  abs( in - hold ) );

		if( v > 0xff ) v = 0xff;
		if( v < Threshold ) v = 0;

		if( show ) {
			holdData[i].r =
			holdData[i].g =
			holdData[i].b = v;
		}
		
		holdData[i].a = v;

		total += v;
	}

	total /= (nPixels/step);
	inBuffer->Header()->user_data_type = F_DIFFERENCE_TOTAL;
	*(float*)(&inBuffer->Header()->user_data) = total;
	
	//-----------------------

	SendOrRecycleBuffer( holdBuffer, mOutput, mInput );
	holdBuffer = inBuffer;
	
	return true;
}
