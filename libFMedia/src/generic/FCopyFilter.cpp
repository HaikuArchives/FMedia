#include "FCopyFilter.h"
#include <stdio.h>
#include <string.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf("FCopyFilter: "); printf
#define WARNING printf("FCopyFilter: "); printf
#define FORMAT printf

FCopyFilter::FCopyFilter( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL )
	: BMediaNode( name ),
	  FControllable( name, requiredFormat.type, requiredFormat.type, addon ),
	  mRequiredFormat( requiredFormat ),
	  mPreferredFormat( preferredFormat ),
	  mOutputFormat( preferredFormat ) {

	mInput = NULL;
	mThruput = mOutput = NULL;

	mBufferManager = new FBufferManager( Name() );
}

FCopyFilter::~FCopyFilter() {
}

void FCopyFilter::Setup() {
	mInput = new FMediaInput( this, "Input", mPreferredFormat, mRequiredFormat );
	AddInput( mInput );
	
	FControllable::Setup();
}

void FCopyFilter::InputConnected( FMediaInput *input, bool connected ) {

	if( connected && input == mInput ) {
		media_format outFormat = mInput->Format();
		if( B_OK == AdaptFormat( &outFormat ) ) {
			mOutput = new FMediaOutput( this, "Output", outFormat, outFormat, F_SIMPLE_BUFFERS, mBufferManager );
			AddOutput( mOutput );

			mThruput = new FMediaOutput( this, "Thruput", mInput->Format(), mInput->Format() );
			AddOutput( mThruput );
			mThruput->Couple( mInput );
		}
	} else if( !connected && input == mInput ) {
		RemoveOutput( mOutput );
		mOutput = NULL;
		RemoveOutput( mThruput );
		mThruput = NULL;
	}
}

bool FCopyFilter::HandleBuffer( BBuffer *thruBuffer, FMediaInput *onInput ) {

	//-----------------------
	size_t sz = FMediaIdealBufferSizeForFormat( &mOutput->Format() );
	FBufferManager *bufferMan = mOutput->GetBufferManager();
	INFO("(FCopyFilter (%s): Requesting buffer size %i from FBufferManager %p\n", Name(), sz, bufferMan );
	BBuffer *outBuffer = bufferMan?bufferMan->GetBuffer( sz ):NULL;
	if( !outBuffer ) {
		WARNING("Didn't get buffer\n");
		SendOrRecycleBuffer( thruBuffer, mThruput, mInput );
		return false;
	} else {
		INFO("%s: Got Buffer\n", Name());
	}
	
	// thruBuffer header -> outBuffer header,
	//! /todo adapt format
	media_header *outHeader = outBuffer->Header();
	media_header *thruHeader = thruBuffer->Header();
	outHeader->start_time = thruHeader->start_time;
	outHeader->user_data_type = thruHeader->user_data_type;
	memcpy( outHeader->user_data, thruHeader->user_data, 64 );	
	
	Filter( thruBuffer, outBuffer );
	
	//-----------------------
		
	return true;
}

status_t FCopyFilter::FormatChanged( FMediaInput *input ) {
	FORMAT("Format Changed\n");
	if( input == mInput ) {
		if( mOutput ) {
			media_format format = input->Format();
			
			// when we handle format
			if( B_OK == AdaptFormat( &format ) ) {
				// set output to adapted....
				ChangeFormat( mOutput, format);
				// and thruput to original format
				if( mThruput )
					ChangeFormat( mThruput, input->Format() );
			}
		}
		return B_OK;
	}
	return B_ERROR;
}

status_t FCopyFilter::AdaptFormat( media_format* inputFormat ) {
	return B_ERROR;
}
