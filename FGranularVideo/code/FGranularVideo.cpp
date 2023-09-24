#include "FGranularVideo.h"
#include <stdio.h>
#include <ParameterWeb.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#include "FGranuleControl.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf(ADDON_NAME": "); printf
#define FORMAT //printf

#define MAX_FRAMES 25
#define N_GRANULES 5

FGranularVideo::FGranularVideo( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL )
	: FFilter( FGetWildcardVideoFormat(), FGetWildcardVideoFormat(), name, addon ),
	  BMediaNode( name )
	  {
	  
	mGranulationThreshold = NULL;

	nGranules = N_GRANULES;
	mGranules = new FVideoGranule*[nGranules];
	for( int i=0; i<nGranules; i++ ) mGranules[i] = NULL;
	playGranule = -1;
	recordGranule = -1;
	recording = false;
	Recorder = NULL;
	
	thru = true;
	
	mSkip = 0;	

	nPatches=9;
	mPatchVisible = new bool[nPatches];
	for( int i=0; i<nPatches; i++ ) {
		mPatchVisible[i] = true;
	}
	
	mPatchVisible[1] = true;
	mPatchVisible[4] = true;
	mPatchVisible[7] = true;
	
	mPatchArea = new BRect[nPatches];
	mPatchArea[0] = BRect(0,0,107,81);
	mPatchArea[1] = BRect(107,0,214,81);
	mPatchArea[2] = BRect(214,0,320,81);
	
	mPatchArea[3] = BRect(0,81,107,161);
	mPatchArea[4] = BRect(107,81,214,161);
	mPatchArea[5] = BRect(214,81,320,161);
	
	mPatchArea[6] = BRect(0,161,107,240);
	mPatchArea[7] = BRect(107,161,214,240);
	mPatchArea[8] = BRect(214,161,320,240);
	
	mView = new FGranuleControlView( nGranules, this );
}

FGranularVideo::~FGranularVideo() {
	for( int i=0; i<nGranules; i++ ) {
		if( mGranules[i] ) delete mGranules[i];
	}
	delete mGranules;
	
	mView->Close();
}

void FGranularVideo::Setup() {
	// Add your parameters here

	mGranulationThreshold = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "GranulationThreshold", B_GENERIC, "units", .0, 10.0, .01, 6.3 );
	AddParameter( mGranulationThreshold );
	
	FFilter::Setup();
}

bool FGranularVideo::Filter( BBuffer *inBuffer ) {

	int32 *inData = (int32 *) inBuffer->Data();

	//-----------------------
	float granulationThreshold = mGranulationThreshold->Get();

	float difference = 0;
	if( inBuffer->Header()->user_data_type = F_DIFFERENCE_TOTAL ) {
		difference = *(float*)(&inBuffer->Header()->user_data);
	}

	//INFO("diff: %.3f\n", difference );

	if( recording ) {
		if( difference < granulationThreshold ) {
			recording = false;
			INFO("STOP\n");
			if( recFrames < 5 ) {
				delete Recorder;
				Recorder = NULL;
/*				delete mGranules[recordGranule];
				mGranules[recordGranule] = NULL;
				recordGranule--;
*/			} else {
				// replace least used with Recorder 
			//	playGranule = recordGranule;
				
				int oldest = 0;
				bigtime_t age = 0;
				int length;
				for( int i=0; i<nGranules; i++ ) {
					if( !mGranules[i] ) {
						oldest = i;
						age = -2;
					} else {
						if( age == 0 || mGranules[i]->LastPlayed() < age ) {
							age = mGranules[i]->LastPlayed();
							oldest = i;
							length = mGranules[i]->Length();
						} else if( mGranules[i]->LastPlayed() == age ) {
							if( mGranules[i]->Length() > length ) {
								age = mGranules[i]->LastPlayed();
								oldest = i;
								length = mGranules[i]->Length();
							}
						}
					}
				}
				if( mGranules[oldest] ) delete mGranules[oldest];
				mGranules[oldest] = Recorder;
				Recorder = NULL;
				
				mView->Redraw();
			}
		}
	} else {
		if( difference > granulationThreshold ) {
/*			recordGranule++;
			if( recordGranule >= nGranules ) recordGranule = 0;
			if( recordGranule == playGranule ) {
				recordGranule++;
				if( recordGranule >= nGranules ) recordGranule = 0;
			}
*/			recording = true;
//			if( mGranules[ recordGranule ] ) delete mGranules[recordGranule];
//			mGranules[recordGranule] = new FVideoGranule( inBuffer->SizeUsed()/sizeof(int32), MAX_FRAMES );
			Recorder = new FVideoGranule( inBuffer->SizeUsed()/sizeof(int32), MAX_FRAMES );
			
			recFrames = 0;
			INFO("START\n");
			mView->Redraw();
		}
	}

	if( recording ) {
//		mGranules[ recordGranule ]->AddFrame( inData );
		Recorder->AddFrame( inData );
		recFrames++;
	}

	if( mSkip ) {
		RecycleBuffer( inBuffer );
		if( mSkip ) mSkip--;
	} else if( thru ) {
		SendOrRecycleBuffer( inBuffer, mOutput );
	} else if( playGranule >= 0 && mGranules[ playGranule ] ) {
		int32 *frame = mGranules[playGranule]->GetNextFrame( inBuffer->Header()->start_time );
		if( frame ) {
//			memcpy( inData, frame, inBuffer->SizeUsed() );
			Fill( inData, frame );
		} else {
			memset( inData, 0xff, inBuffer->SizeUsed() );
		}
		SendOrRecycleBuffer( inBuffer, mOutput );
	} else {
		memset( inData, 0, inBuffer->SizeUsed() );
		SendOrRecycleBuffer( inBuffer, mOutput );
//		RecycleBuffer( inBuffer );
	}

	//-----------------------
	

	return true;
}

void FGranularVideo::FilterFormatChanged( const media_format& new_format ) {
	// called on connect and when the format changes
}

void FGranularVideo::ParameterChanged( FParameter *param ) {
	// callen whenever a parameter changes
}

void FGranularVideo::Fill( int32 *dst, int32 *src ) {
	int bpp = 4;
	int bpr = 320*bpp;
	for( int p=0; p<nPatches; p++ ) {
		if( mPatchVisible[p] ) {
			for( int y=mPatchArea[p].top; y<mPatchArea[p].bottom; y++ ) {
				int index = ((y*320)+(mPatchArea[p].left));
				memcpy( &dst[ index ], 
						&src[ index ],
						(mPatchArea[p].right - mPatchArea[p].left)*bpp );
			}
		}
	}
}

