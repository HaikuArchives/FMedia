#include "FVideoGranule.h"
#include <stdlib.h>

FVideoGranule::FVideoGranule( size_t frameSize, int maxFrames ) 
	: nFrames( 0 )
	, aFrames( maxFrames )
	, cFrame( 0 )
	, mFrameSize( frameSize )
	{
	
	mFrames = new int32*[aFrames];

	for( int i=0; i<aFrames; i++ ) {
		mFrames[i] = NULL;
	}
	
	forward = true;
	isNew = true;
	lastPlayed = -1;
}

FVideoGranule::~FVideoGranule() {
	for( int i=0; i<nFrames; i++ ) {
		if( mFrames[i] ) delete mFrames[i];
	}
	delete mFrames;	
}

void FVideoGranule::AddFrame( int32 *data ) {
	if( nFrames >= aFrames ) return;
	mFrames[nFrames] = new int32[ mFrameSize ];
	memcpy( mFrames[nFrames], data, mFrameSize*sizeof(int32) );	
	nFrames++;
}